#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NTHREADS  4

pthread_mutex_t mutex;
pthread_cond_t cond1, cond2;

int contador = 0;

void * thread1 (void * arg){

    pthread_mutex_lock(&mutex);
    
    while(contador != 2){
        pthread_cond_wait(&cond2, &mutex);
    }
    printf("Volte sempre!\n");

    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

void * thread2 (void * arg){
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond1, &mutex);

    printf("Fique a vontade.\n");
    contador ++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond2);
    pthread_exit(NULL);
}

void * thread3 (void * arg){
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond1, &mutex);

    printf("Sente-se por favor.\n");
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond2);
    pthread_exit(NULL);
}

void * thread4 (void * arg){
    //pthread_mutex_lock(&mutex);

    printf("Seja bem-vindo!\n");
    pthread_cond_broadcast(&cond1);
    //pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NTHREADS];

    //inicializa mutex e cond
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init (&cond1, NULL);
    pthread_cond_init (&cond2, NULL);
    //pthread_cond_init (&cond4, NULL);

    //cria threads
    pthread_create(&threads[0], NULL, thread1, NULL);
    pthread_create(&threads[1], NULL, thread2, NULL);
    pthread_create(&threads[2], NULL, thread3, NULL);
    pthread_create(&threads[3], NULL, thread4, NULL);

    
    /* Espera todas as threads completarem */
    for (int i = 0; i < NTHREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    /* Desaloca variaveis e termina */
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
   // pthread_cond_destroy(&cond4);

}



//     if(id == 1){
//         printf("Volte sempre!");
//     }
//     else if(id ==2){
//         printf("Fique a vontade.");
//     }
//     else if(id == 3) {
//         printf("Sente-se por favor.");
//     }
//     else if(id == 4) {
//         printf("Seja bem-vindo!");
//     }


