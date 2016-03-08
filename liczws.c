#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> /*deklaracje standardowych funkcji uniksowych (fork(), write() itd.)*/
#include <sys/mman.h>
#include <time.h>

#define N 3
#define BUFF_SIZE 1000
#define SIZE 270

key_t key = 5672, key2 = 5692;
volatile pid_t *pids;
int vector[BUFF_SIZE];
int flag = 0;

void on_usr1(int pid){}
int n;

void sum(int pid) {   

    int i, count = 0;
    int *result;

    int sum = 0;
    
    int  *range;
    int shmid,shmid2;
    if ((shmid = shmget(key, SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((range = shmat(shmid, NULL, 0)) == (int*) -1) {
        perror("shmat1");
        exit(1);
    }
    if ((shmid2 = shmget(key2, SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((result = shmat(shmid2, NULL, 0)) == (int*) -1) {
        perror("shmat2");
        exit(1);
    }
    int pom = result[N];
    result[N]++;

    for(i = range[2 * pom]; i <= range[(2 * pom) + 1]; i++) {
        if(i != 0)
            sum += vector[i];
        printf("%d+", vector[i]);
    }

    printf("> %d, %d <\n", range[2 * pom], range[2 * pom + 1]);
    result[pom] = sum;
}
int readFile(int *vector, char* fileName)//odczyt po linii
{
    FILE           *in = fopen(fileName, "r");
    int i=0;
    if (in != NULL) {
        int          sum = 0;
        int          x;
        int n=0;
        while (fscanf(in, "%d", &x) == 1) {
            vector[n]=x;
            n++;
        }
        fclose(in);

        return n;//ilosc elementow wektora 
    } else
        return -1;
}
int* podzial(int rangse, int n) {
    int i;
    int *array = malloc(2*N * sizeof(*array));
    for(i = 0; i <  N; i++){
        if(i != N - 1){
            array[2 * i] = i * rangse;
            array[2 * i + 1] = i * rangse + rangse - 1; 
        }else{
            array[2 * i] = i * rangse;
            array[2 * i + 1] = n - 1;
        }
    }
    return array;
}
int main(){
    

    pid_t pid;
    int *result;
    int i; int *range;
    pid_t *pidy = malloc(N * sizeof(*pidy));
    n = readFile(vector,"vector.txt"); 
    printf("Moj PID = %d\n", getpid());


    int shmid,shmid2;
    //pamiec wspoldzielona
    if ((shmid = shmget(key, SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmgetxcvz");
        exit(1);
    }
    if ((range = shmat(shmid, NULL, 0)) == (int*) -1) {
        perror("shmat");
        exit(1);
    }
    int* tmp = podzial(n/N, n); 

    memcpy(range, tmp, 2 * N * sizeof(int));

    // for(i = 0; i < 2 * N; i++){
    //     printf("%d-", range[i] );
    // }

    // printf("/n");

    if ((shmid2 = shmget(key2, SIZE, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }
    if ((result = shmat(shmid2, NULL, 0)) == (int*) -1) {
        perror("shmat");
        exit(1);
    }
    //result=malloc(N*sizeof(*result));
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
            case 0: /*proces potomny*/                
                printf(" ");                
                sigset_t mask;
                struct sigaction usr1;
                sigemptyset(&mask); /* Wyczyść maskę */
                usr1.sa_handler = (&sum);
                usr1.sa_mask = mask;
                usr1.sa_flags = SA_SIGINFO;
                sigaction(SIGUSR1, &usr1, NULL);
                pause();
                exit(0);
            default:
                pidy[i]=pid;
        }
    }

    sleep(1);

    for(i = 0; i < N; i++){
        printf("%d\n", pidy[i]);
    }
 
    clock_t time_start, time_end;

    time_start = clock();

    for( i = 0; i < N; i++){
       kill(pidy[i], SIGUSR1);
    }

    // while (-1 == waitpid(pidy[i], &s, 0));
   for(i = 0; i < N; i++){
       wait(0);
   }

    time_end = clock();

    

    double suma = 0;
    for(i = 0; i < N; i++){
        suma += result[i];
    }

    for(i = 0; i <2 * N; i++){
        printf("%d, ", result[i]);
    }
    // printf("\n");
    // for(i = 0; i <2 * N; i++){
    //     printf("%d, ", range[i]);
    // }

    printf("Suma równolegle wynosi: %lf\n", suma);
    shmdt(result);
    suma = 0;

    printf("Czas klasycznego: %lf\n", (double) (time_end - time_start)/CLOCKS_PER_SEC);

    time_start = clock();
    for(i = 1; i < n; i++){
        suma += vector[i];
        printf("%d, ", vector[i]);
    }
    time_end = clock();    
    printf("Suma klasycznie wynosi: %lf\n", suma);
    printf("Czas równoległego: %lf\n", (double) (time_end - time_start)/CLOCKS_PER_SEC);
    return 0;
}
