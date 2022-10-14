/* Disciplina: Computacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Descricao: implementa  o problema dos leitores/escritores usando variaveis de condicao da biblioteca Pthread
*/

#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define T1 2 //numero de threads T1
#define T2 2 //numero de threads T2
#define T3 2 //numero de threads T3

//variaveis do problema
int leit=0; //contador de threads lendo
int escr=0; //contador de threads escrevendo
int variavel = 0;

//variaveis para sincronizacao
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;

//entrada leitura
void InicLeit (int id, int t) {
   pthread_mutex_lock(&mutex);
   printf("Thread[%d] de t%d quer ler\n", id, t);
   while(escr > 0) {
     printf("Thread[%d] de t%d bloqueou\n", id,t);
     pthread_cond_wait(&cond_leit, &mutex);
     printf("Thread[%d] de t%d desbloqueou\n", id,t);
   }
   leit++;
   pthread_mutex_unlock(&mutex);
}

//saida leitura
void FimLeit (int id,int t) {
   pthread_mutex_lock(&mutex);
   printf("Thread[%d] de t%d terminou de ler\n", id,t);
   leit--;
   if(leit==0) pthread_cond_signal(&cond_escr);
   pthread_mutex_unlock(&mutex);
}

//entrada escrita
void InicEscr (int id,int t) {
   pthread_mutex_lock(&mutex);
   printf("Thread[%d] de t%d quer escrever\n", id,t);
   while((leit>0) || (escr>0)) {
     printf("Thread[%d] de t%d bloqueou\n", id,t);
     pthread_cond_wait(&cond_escr, &mutex);
     printf("Thread[%d] de t%d desbloqueou\n", id,t);
   }
   escr++;
   pthread_mutex_unlock(&mutex);
}

//saida escrita
void FimEscr (int id, int t) {
   pthread_mutex_lock(&mutex);
   printf("Thread[%d] de t%d terminou de escrever\n", id,t);
   escr--;
   pthread_cond_signal(&cond_escr);
   pthread_cond_broadcast(&cond_leit);
   pthread_mutex_unlock(&mutex);
}

 //thread t1
void * t1 (void * arg) {
  int id = *(int *) arg;
  InicEscr(id,1);
  variavel++;
  FimEscr(id,1);
  pthread_exit(NULL);
}

//thread t2
void * t2 (void * arg) {
  int id = *(int *) arg;

  InicLeit(id,2);
  if(variavel % 2 == 0){
    printf("Valor da variável = %d, par\n", variavel);
  }else{
    printf("Valor da variável = %d, ímpar\n", variavel);
  }
  FimLeit(id,2);
  pthread_exit(NULL);
}

//thread t3
void * t3(void * arg) {
  int id = *(int *) arg;
  int bobeira = 0;
  
    InicLeit(id,3);
    printf("Valor da variável = %d\n",variavel);
    FimLeit(id,3);

    for (int i=0;i<=10000;i++){
      bobeira++;
    }

    InicEscr(id,3);
    variavel = id;
    FimEscr(id,3);
    
  pthread_exit(NULL);
}

//funcao principal
int main(void) {
  //identificadores das threads
  pthread_t tid[T1+T2+T3];
  int id[T1+T2+T3];

  //inicializa as variaveis de sincronizacao
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond_leit, NULL);
  pthread_cond_init(&cond_escr, NULL);

  //cria as threads t1
  for(int i=0; i<T1; i++) {
    id[i] = i+1;
    if(pthread_create(&tid[i], NULL, t1, (void *) &id[i])) exit(-1);
  } 
  
  //cria as threads t2
  for(int i=0; i<T2; i++) {
    id[i+T1] = i+1+T1;
    if(pthread_create(&tid[i+T1], NULL, t2, (void *) &id[i+T1])) exit(-1);
  }

  //cria as threads t3
  for(int i=0; i<T3; i++) {
    id[i+T1+T2] = i+1+T1+T2;
    if(pthread_create(&tid[i+T1+T2], NULL, t3, (void *) &id[i+T1+T2])) exit(-1);
  }

  // pthread join
    for(int i=0; i<T1+T2+T3; i++) {
        pthread_join(tid[i], NULL);
    }

  pthread_exit(NULL);
  return 0;
}
