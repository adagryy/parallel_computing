#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ipc.h>
// #include <sys/shm.h>
#define BUFFOR_SIZE 102

volatile pid_t *pids;

int foo(const char *whoami)
{
        printf("I am a %s.  My pid is:%d  my ppid is %d\n",
                        whoami, getpid(), getppid() );
        return 1;
}

double sum(double* vector, int n) {
        int i;
        double sum = 0.0f;
        for(i=0; i<n; i++) {
                sum += vector[i];
        }
        return sum;
}

void on_usr1(int s){

}

int main(void)
{

        FILE* f = fopen("vector.txt", "r");
        char buffor[BUFFOR_SIZE+1];
        double* vector;
        int n, N = 5;
        int i;

        //int *shm;

        fscanf(f, "%d", &n);
        vector = malloc(sizeof(double) * n);
        printf("Vector has %d elements\n", n);

        int status=0;

        int x = 5;
        sigset_t mask; /* Maska sygnałów */

        // int *pidss = malloc(n * sizeof(pidss));

        // Map space for shared array
          pids = mmap(0, n*sizeof(pid_t), PROT_READ|PROT_WRITE,
                      MAP_SHARED | MAP_ANONYMOUS, -1, 0);
          if (!pids) {
            perror("mmap failed");
            exit(1);
          }
          memset((void *)pids, 0, n*sizeof(pid_t));


        printf("Creating %d children\n", n);
        foo("parent");
        for(i=0;i<n;i++)
        {
                //sleep(1);
                // pid_t pid;
                // pid = fork();

                // if (pid==0) /* only execute this if child */
                // {     
                //         // Konfiguracja obsługi sygnału USR1 
                //         struct sigaction usr1;
                //         sigemptyset(&mask); /* Wyczyść maskę */
                //         usr1.sa_handler = (&on_usr1);
                //         usr1.sa_mask = mask;
                //         usr1.sa_flags = SA_SIGINFO;
                //         sigaction(SIGUSR1, &usr1, NULL);
                //         int p = getpid();
                //         //printf("%d\n", p);
                //         pids[i] = getpid();
                //         exit(0);
                // }
                //wait
                
                        
        }
        //
        for(i=0; i<n; i++) {
               fscanf(f, "%lf", &vector[i]);
        }
        fclose(f);
        int range = n/N;

        int *array = malloc(2 * N * sizeof(int));

        // for(i = 0; i <  N; i++){
        //         if(i != N - 1){
        //                 array[2 * i] = i * range;
        //                 array[2 * i + 1] = i * range + range - 1; 
        //         }else{
        //                 array[2 * i] = i * range;
        //                 array[2 * i + 1] = n - 1;
        //         }
        // }

        int *partial_array = malloc(N * sizeof(int));

        //wait(&status);  /* only the parent waits */

        // for(i = 0; i < n; i++){
        //         printf("Pid: %d\n", pids[i]);
        // }    

        // char c;
        // int shmid;
        // key_t key;
        // char *shm, *s;

        // key = 5678;

        // shmid = shmget(key, n * sizeof(int), IPC_CREAT | 0666);

        // if (shmid < 0) {
        //         perror("shmgetfed");
        //         exit(1);
        // }

        // shm = shmat(shmid, (void *)0, 0);
        // if (shm == (char *)(-1)) {
        //         perror("shmat");
        //         exit(1);
        // }

        // memcpy(shm, "Hello wolrd", 11);

        // if (shmdt(array) == -1) {
        //         perror("shmdt");
        //         exit(1);
        // }

        // if( shmctl(key, IPC_RMID, 0) < 0){
        //         perror("shmctl error");
        //         printf("Shared memory removed\n");
        // }


        for(i = 0; i <  N; i++){
                if(i != N - 1){
                        array[2 * i] = i * range;
                        array[2 * i + 1] = i * range + range - 1; 
                }else{
                        array[2 * i] = i * range;
                        array[2 * i + 1] = n - 1;
                }
        }

        // printf("%d\n", array[1]);

        // printf("v = [ ");
        for(i=0; i<2*N; i++) {
                printf("%d ", array[i]);
        }
        // printf("]\n");

        // printf("Suma elementów w wektorze = %f\n", sum(vector, n));
///////////////////////////////////////////////


        //pause();
        
        x = 100 * x;
        printf("x value in main: %d, pid: %d\n", x, getpid());


        return 0;
}


        // int i;
        // pid_t pid;
        // // printf("I'm a parent process: % d\n", getpid());
        // int s = 0;
        // for(i = 0; i < 1; i++){
        //         pid = fork();
        // }
        // if(pid == 0){
        //         printf("I'm a process: %d\n", getpid());
        //         printf("Tu 2\n");
        // }
        // else    {
        //         int s;
        //         printf("2 I'm a process: %d\n", getpid());
                
        //         printf("%d\n", pid );
        //          printf("Tu 1\n");
        //          printf("Tu 1\n");
        //          printf("Tu 1\n");
        //          printf("Tu 1\n");
        //          printf("Tu 1\n");
        //          printf("Tu 1\n");

        //          //(void)waitpid(pid, &s, 0);
        // }