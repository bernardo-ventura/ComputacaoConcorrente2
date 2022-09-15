#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include "timer.h"

float *m3;


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

int main(int argc, char*argv[]) {
   double tempoInitInit, tempoInitFinal, tempoExecInit, tempoExecFinal, tempoFinalInit, tempoFinalFinal, tempoInicial, tempoExec, tempoFinal;
   GET_TIME(tempoInitInit);
    FILE * descritorArquivoM1;
    FILE * descritorArquivoM2;
    FILE * descritorArquivoM3;

    int linhas, colunas; //dimensoes da matriz
    long long int tam;
    float *m1;
    float *m2;
    size_t ret;

    if(argc < 3) {
      fprintf(stderr, "Digite: %s <matriz 1> <matriz 2> <matriz gerada m3> \n", argv[0]);
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

    //M1
   ret = fread(&linhas, sizeof(int), 1, descritorArquivoM1);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas, sizeof(int), 1, descritorArquivoM1);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   tam = linhas * colunas; //calcula a qtde de elementos da matriz

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
   ret = fread(&linhas, sizeof(int), 1, descritorArquivoM2);
   if(!ret) {
      fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
      return 3;
   }
   ret = fread(&colunas, sizeof(int), 1, descritorArquivoM2);
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

   GET_TIME(tempoExecInit);
   multiplicaMatriz(m1,m2,linhas);
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
   // for(int i=0; i<linhas; i++) { 
   //    for(int j=0; j<colunas; j++)
   //      fprintf(stdout, "%.6f ", m3[i*colunas+j]);
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
   ret = fwrite(&linhas, sizeof(int), 1, descritorArquivoM3);
   //escreve os elementos da matriz
   ret = fwrite(&colunas, sizeof(int), 1, descritorArquivoM3);
   ret = fwrite(m3, sizeof(float), tam, descritorArquivoM3);
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
   GET_TIME(tempoFinalFinal);

   tempoInicial = tempoInitFinal - tempoInitInit;
   tempoExec= tempoExecFinal-tempoExecInit;
   tempoFinal = tempoFinalFinal-tempoFinalInit;

   printf("Tempo Inicial: %lf\n",tempoInicial);
   printf("Tempo de Execução: %lf\n",tempoExec);
   printf("Tempo Final: %lf\n",tempoFinal);
   printf("Razão: %lf\n",(( (tempoInicial)+(tempoExec)+(tempoFinal) ) / ( (tempoInicial)+ ((tempoExec)/2) + (tempoFinal) )));

   return 0;

}


