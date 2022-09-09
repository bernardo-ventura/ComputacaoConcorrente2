#include<stdio.h>
#include<pthread.h>

#define NTHREADS 2
#define NVET 10

float vetor[NVET];

//funcao da thread
void * incrementaVetor(void * arg){
    int idThread = * (int *) arg;
    for (int i = idThread; i<=NVET; i+=2){
        vetor[i-1] = vetor[i-1] * 1.1;
    }
    return 0;
}

int main(void) {
    pthread_t tid[NTHREADS]; //identificador da thread no sistema
    int id[NTHREADS]; //identificador local da thread

    for(int i=0;i<NVET;i++){
        vetor[i] = i+1;
    }

    //cria as threads 
    for(int i=1; i<=NTHREADS; i++) {
       id[i-1] = i;
       if (pthread_create(&tid[i-1], NULL, incrementaVetor, (void *)&id[i-1])) 
          printf("ERRO -- pthread_create\n");
    }

    //espera as threads terminarem 
    for(int i=0; i<NTHREADS; i++) {
       if (pthread_join(tid[i], NULL)) 
          printf("ERRO -- pthread_join\n");
    }

     for(int i=0; i<NVET; i++){
        printf("%f\n",vetor[i]);
    }
   

    pthread_exit(NULL);
    return 0;
}