#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> /*dedeclarations of standard UNIX functions (fork(), write() itd.)*/
#include <sys/mman.h>
#include <time.h>

#define N 7 /* sets number of processes*/
#define BUFF_SIZE 8192 /*maximum amount of elements in vector*/

key_t key = 5672, key2 = 5692; /*global keys for creating segments*/

double vector[BUFF_SIZE];/*vector for storing data read from external file*/
int n, SIZE, SIZE2;/*in SIZE are stored ranges, while in SIZE 2 - partial results and one special flag*/

void on_usr1(int pid){}

void sum(int pid) {   

    int i;
    double *result;

    double sum = 0;
    
    int  *range;
    int shmid,shmid2;
    if ((shmid = shmget(key, SIZE2, IPC_CREAT | 0666)) < 0) {/*getting segment from shared memory*/
        perror("shmget");
        exit(1);
    }
    if ((range = shmat(shmid, NULL, 0)) == (int*) -1) {/*attaching got segment*/
        perror("shmat1");
        exit(1);
    }
    if ((shmid2 = shmget(key2, SIZE, IPC_CREAT | 0666)) < 0) {/*similar operations for partial results shared space*/
        perror("shmget");
        exit(1);
    }
    if ((result = shmat(shmid2, NULL, 0)) == (double*) -1) {
        perror("shmat2");
        exit(1);
    }
    int pom = (int) result[N];
    result[N]++;

    for(i = range[2 * pom]; i <= range[(2 * pom) + 1]; i++) {/*here is computing of partial sum*/
        sum += vector[i];
        // printf("%d+", vector[i]);
        // printf("> %d, %d <\n", range[2 * pom], range[2 * pom + 1]);
    }     
    result[pom] = sum;/*...and writing it to the shared memory*/
    shmdt(result);/*these detache shared memory*/
    shmdt(range);
}
int readFile(double *vector, char* fileName)/*reading external file lini-by-line*/
{
    FILE           *in = fopen(fileName, "r");
    int i=0;
    if (in != NULL) {
        int          sum = 0;
        double          x;
        int n=0;
        while (fscanf(in, "%lf", &x) == 1) {
            vector[n]=x;
            n++;
        }
        fclose(in);

        return n;//vector elements number
    } else
        return -1;
}
int* podzial(int range, int n) {/* this functions builds array with special set of ranges*/
    int i;
    int *array = malloc(2*N * sizeof(*array));
    for(i = 0; i <  N; i++){
        if(i != N - 1){
            array[2 * i] = i * range;
            array[2 * i + 1] = i * range + range - 1; 
        }else{
            array[2 * i] = i * range;
            array[2 * i + 1] = n - 1;
        }
    }
    return array;
}
int main(){
    

    pid_t pid;
    double *result;
    int i, *range;
    pid_t *pids = malloc(N * sizeof(*pids));
    n = readFile(vector,"vector.txt"); 
    SIZE = 8 * (N + 1);
    SIZE2 = 8 * N;
    // printf("Moj PID = %d\n", getpid());


    int shmid,shmid2;
    //creating a segment of shared memory
    if ((shmid = shmget(key, SIZE2, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((range = shmat(shmid, NULL, 0)) == (int*) -1) {
        perror("shmat");
        exit(1);
    }
    int* tmp = podzial(n/N, n); 

    memcpy(range, tmp, 2 * N * sizeof(int));

    if ((shmid2 = shmget(key2, SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((result = shmat(shmid2, NULL, 0)) == (double*) -1) {
        perror("shmat");
        exit(1);
    }

    int *tmp2 = malloc(2 * N * sizeof(int));
    for(i = 0; i < 2 * N; i++){
        tmp2[i] = 0;     
    } 

    memcpy(result, tmp2, 2 * N * sizeof(int));
    int s;

    for(i = 0; i < N; i++){
        pid=fork();
        switch(pid){
            case -1:
                fprintf(stderr, "Blad w fork\n");
                return EXIT_FAILURE;
            case 0: /*child process*/                
                printf(" ");                
                sigset_t mask;
                struct sigaction usr1;
                sigemptyset(&mask); /* clear mask */
                usr1.sa_handler = (&sum);
                usr1.sa_mask = mask;
                usr1.sa_flags = SA_SIGINFO;
                sigaction(SIGUSR1, &usr1, NULL);
                pause();/*waiting for a signal*/
                exit(0);
            default:
                pids[i]=pid;/*saving pids of children processes for fater use*/
        }
    }

    sleep(1);/*suspend for a second*/
 
    clock_t time_start, time_end;

    time_start = clock();/*time_start, time_end - measure time of performing computations*/

    for( i = 0; i < N; i++){
       kill(pids[i], SIGUSR1);/*sending signals to the processes*/
    }

    for(i = 0; i < N; i++){
       wait(0);/*waiting, until all processes have completes*/
    }

    time_end = clock();   

    double suma = 0;/*here is start of computing the sum using parralell method (multiple processes)*/
    for(i = 0; i < N; i++){
        suma += result[i];
    }

    printf("N wynosi: %d\n", n);
    /*Uncomment to see "ranges" and "results" shared memory contents*/
    // for(i = 0; i <2 * N; i++){
    //     printf("%lf, ", result[i]);
    // }
    // printf("\n");
    // for(i = 0; i <2 * N; i++){
    //     printf("%d, ", range[i]);
    // }

    shmdt(result);/*detaching shared memory*/
    shmdt(range);


    /*Presenting results*/

    printf("Suma obliczona równolegle wynosi: %lf\n", suma);  
    printf("Czas obliczania równoległego wynosi: %lf\n", (double) (time_end - time_start)/CLOCKS_PER_SEC);

    suma = 0;
    time_start = clock();
    for(i = 0; i < n; i++){
        suma += vector[i];
        //printf("%d, ", vector[i]);
    }
    time_end = clock();  

    printf("Suma klasycznie wynosi: %lf\n", suma);
    printf("Czas obliczania klasycznego wynosi: %lf\n", (double) (time_end - time_start)/CLOCKS_PER_SEC);
    return 0;
}
