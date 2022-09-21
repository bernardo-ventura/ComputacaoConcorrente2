#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include "timer.h"
#include <unistd.h>

float *m1;
float *m2;
float *m3;
float *resultadoConc;
int linhas1, colunas1, linhas2, colunas2;
double tempoConcInit, tempoConcFinal, tempoExecConc;
double tempoCriacaoThreads, tempoThreadsInit, tempoThreadsFinal, tempoFinalConc;
int nthreads;


typedef struct{
   int id; // id do elemento que a thread vai processar
   // int tam;
   int l1; // número de linhas da matriz 1
   int l2; // número de linhas da matriz 2
   int c1; // número de colunas da matriz 1
   int c2; // número de colunas da matriz 2
}tArgs;

// funcao sequencial
float *multiplicaMatriz(float *m1, float *m2, int n){
    int i, j, k;
    m3 = (float*) malloc(sizeof(float) * n * n);
    if(!m3) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return NULL;
   }
   for (i=0; i < n; i++){
      for (j=0;j<n; j++){
         for(k=0; k<n; k++){
            m3[i*n + j] += m1[i*n + k] * m2[k*n + j];
         }
      }
   }
   return m3;
}

//funcao concorrente


void * tarefa(void * arg){
   GET_TIME(tempoConcInit);
   tArgs *args = (tArgs*) arg;
   
   resultadoConc = (float*) malloc(sizeof(float) * args->c1 * args->l1);
   if(!resultadoConc) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return NULL;
   }

   if(args->c1 != args->l2){
      return NULL;
   }

   printf("%d, %d, %d, %d\n", args->l1, args->c1, args->l2, args->c2);

   for (int i = args->id; i < args->l1; i += nthreads){
      for (int j = 0; j < args->c1; j++){
         for (int k = 0; k < args->c1; k++){
            resultadoConc[i * (args->l1) + j] += m1[i * (args->l1) + k] * m2[k * (args->c2) + j];
         }
      }
   }
   GET_TIME(tempoConcFinal);
   pthread_exit(NULL);
}


int main(int argc, char*argv[]) {
   double tempoInitInit, tempoInitFinal, tempoExecInit, tempoExecFinal, tempoFinalInit, tempoFinalFinal, tempoInicial, tempoExec, tempoFinal;
   
   GET_TIME(tempoInitInit);
    FILE * descritorArquivoM1;
    FILE * descritorArquivoM2;
    FILE * descritorArquivoM3;
    FILE * descritorArquivoResultadoConcorrente;

     //dimensoes da matriz
    long long int tam;
    
    size_t ret;

    pthread_t *tid;
    tArgs *args;

    if(argc < 5) {
      fprintf(stderr, "Digite: %s <matriz 1> <matriz 2> <matriz sequencial> <matriz concorrente> <numero de threads> \n", argv[0]);
      return 1;
   }

   descritorArquivoM1 = fopen(argv[1], "rb");
   if(!descritorArquivoM1) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

    descritorArquivoM2 = fopen(argv[2], "rb");
   if(!descritorArquivoM2) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 2;
   }

    //abre o arquivo para escrita binaria
   descritorArquivoM3 = fopen(argv[3], "wb");
   if(!descritorArquivoM3) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 3;
   }

   descritorArquivoResultadoConcorrente = fopen(argv[4], "wb");
   if(!descritorArquivoResultadoConcorrente) {
      fprintf(stderr, "Erro de abertura do arquivo\n");
      return 4;
   }

   nthreads = atoi(argv[5]);

    //M1
   ret = fread(&linhas1, sizeof(int), 1, descritorArquivoM1);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas1, sizeof(int), 1, descritorArquivoM1);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   tam = linhas1 * colunas1; //calcula a qtde de elementos da matriz

    //aloca memoria para a matriz
   m1 = (float*) malloc(sizeof(float) * tam);
   if(!m1) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(m1, sizeof(float), tam, descritorArquivoM1);
   if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }
   
   //M2
   ret = fread(&linhas2, sizeof(int), 1, descritorArquivoM2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas2, sizeof(int), 1, descritorArquivoM2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }

    //aloca memoria para a matriz
   m2 = (float*) malloc(sizeof(float) * tam);
   if(!m2) {
      fprintf(stderr, "Erro de alocao da memoria da matriz\n");
      return 3;
   }

   //carrega a matriz de elementos do tipo float do arquivo
   ret = fread(m2, sizeof(float), tam, descritorArquivoM2);
   if(ret < tam) {
      fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
      return 4;
   }
   GET_TIME(tempoInitFinal);

   GET_TIME(tempoThreadsInit);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO -- malloc"); return 2;}

   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO -- malloc"); return 2;}

   //criacao das threads
   for (int i =0; i<nthreads; i++){
      (args+i) -> id = i;
      (args+i) -> l1 = linhas1;
      (args+i) -> l2 = linhas2;
      (args+i) -> c1 = colunas1;
      (args+i) -> c2 = colunas2;

      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO -- pthread_create"); return 3;
      }
   }

   //espera o termino das threads
   for(int i=0; i<nthreads; i++) {
        pthread_join(*(tid+i),NULL);
   }
   GET_TIME(tempoThreadsFinal);

   GET_TIME(tempoExecInit);
   multiplicaMatriz(m1,m2,linhas1);
   GET_TIME(tempoExecFinal);

   //print resultado para verificar corretude
   // printf("--------------------\n");
   // for(int i=0; i<linhas; i++) { 
   //    for(int j=0; j<colunas; j++)
   //      fprintf(stdout, "%.6f ", m1[i*colunas+j]);
   //    fprintf(stdout, "\n");
   // }
   // printf("--------------------\n");

   // printf("--------------------\n");
   // for(int i=0; i<linhas; i++) { 
   //    for(int j=0; j<colunas; j++)
   //      fprintf(stdout, "%.6f ", m2[i*colunas+j]);
   //    fprintf(stdout, "\n");
   // }
   // printf("--------------------\n");

   // printf("--------------------\n");
   // for(int i=0; i<linhas1; i++) { 
   //    for(int j=0; j<colunas1; j++)
   //      fprintf(stdout, "%.6f ", m3[i*colunas1+j]);
   //    fprintf(stdout, "\n");
   // }
   // printf("--------------------\n");

   // printf("--------------------\n");
   // for(int i=0; i<linhas1; i++) { 
   //    for(int j=0; j<colunas1; j++)
   //      fprintf(stdout, "%.6f ", resultadoConc[i*colunas1+j]);
   //    fprintf(stdout, "\n");
   // }
   // printf("--------------------\n");

//   //imprimir na saida padrao a matriz gerada
//    #ifdef TEXTO
//    for(int i=0; i<linhas; i++) {
//       for(int j=0; j<colunas; j++)
//         fprintf(stdout, "%.6f ", matriz[i*colunas+j]);
//       fprintf(stdout, "\n");
//    }
//    #endif

   GET_TIME(tempoFinalInit);
   //escreve numero de linhas e de colunas
   ret = fwrite(&linhas1, sizeof(int), 1, descritorArquivoM3);
   //escreve os elementos da matriz
   ret = fwrite(&colunas1, sizeof(int), 1, descritorArquivoM3);
   ret = fwrite(m3, sizeof(float), tam, descritorArquivoM3);
   if(ret < tam) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }

   ret = fwrite(&linhas1, sizeof(int), 1, descritorArquivoResultadoConcorrente);
   //escreve os elementos da matriz
   ret = fwrite(&colunas1, sizeof(int), 1, descritorArquivoResultadoConcorrente);
   ret = fwrite(resultadoConc, sizeof(float), tam, descritorArquivoResultadoConcorrente);
   if(ret < tam) {
      fprintf(stderr, "Erro de escrita no  arquivo\n");
      return 4;
   }

   //libera espaço
   fclose(descritorArquivoM1);
   free(m1);
   fclose(descritorArquivoM2);
   free(m2);
   fclose(descritorArquivoM3);
   free(m3);
   fclose(descritorArquivoResultadoConcorrente);
   GET_TIME(tempoFinalFinal);

   tempoInicial = tempoInitFinal - tempoInitInit;
   tempoExec= tempoExecFinal-tempoExecInit;
   tempoFinal = tempoFinalFinal-tempoFinalInit;
   tempoExecConc = tempoConcFinal - tempoConcInit;
   tempoCriacaoThreads = tempoThreadsFinal - tempoThreadsInit;
   tempoFinalConc = tempoExecConc + tempoCriacaoThreads;

   printf("Tempo Inicial: %lf\n",tempoInicial);
   printf("Tempo de Execução Sequencial: %lf\n",tempoExec);
   printf("Tempo de Execução Concorrente: %lf\n",tempoFinalConc);
   printf("Tempo Final: %lf\n",tempoFinal);

   printf("Razão: %lf\n",(( (tempoInicial)+(tempoExec)+(tempoFinal) ) / ( (tempoInicial)+ (tempoFinalConc) + (tempoFinal) )));

   return 0;

}


