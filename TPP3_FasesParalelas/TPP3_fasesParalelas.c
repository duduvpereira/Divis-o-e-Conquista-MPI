#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 1000000 //100000     // trabalho final com o valores 10.000, 100.000, 1.000.000
//#define delta 250000

/* recebe um ponteiro para um vetor que contem as mensagens recebidas dos filhos e            */
/* intercala estes valores em um terceiro vetor auxiliar. Devolve um ponteiro para este vetor */

int *intercala(int vetor[], int tam)
{
        //int *vetor_auxiliar;
        int i1, i2, i_aux;

        int (*vetor_auxiliar) = malloc(tam * sizeof * vetor_auxiliar);

        i1 = 0;
        i2 = tam / 2;

        for (i_aux = 0; i_aux < tam; i_aux++) {
                if (((vetor[i1] <= vetor[i2]) && (i1 < (tam / 2)))
                    || (i2 == tam))
                        vetor_auxiliar[i_aux] = vetor[i1++];
                else
                        vetor_auxiliar[i_aux] = vetor[i2++];
        }

        return vetor_auxiliar;
}

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

void inicializa(int * vetor,int tamanho,int myrank,int np)
{
    int i;
    for (i=0 ; i<tamanho; i++)              /* init array with worst case for sorting */
        vetor[i] = (tamanho*(np-myrank))-i;
        //vetor[i] = (tamanho*myrank)+i;
}

void printaVetor(int * vetor2,int tamanho2, int myrank)
{
        printf("\n Vetor[%d]: ",myrank);
        int i;
   for (i=0 ; i<tamanho2; i++)              /* print unsorted array */
          printf("[%03d] ", vetor2[i]);                    // sou o raiz, mostro vetor
}


main(int argc, char** argv)
{

int my_rank;  /* Identificador do processo */
int proc_n;   /* N??mero de processos */
int source;   /* Identificador do proc.origem */
int dest,destE,destD,pai;     /* Identificador do proc. destino */
int tag = 50; /* Tag para as mensagens */
int ind_ordenados;


MPI_Status status; /* Status de retorno */

MPI_Init (&argc , & argv);

MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

double inteiro[10];
double t1,t2;
t1 = MPI_Wtime();  // inicia a contagem do tempo
inteiro[1]=t1;

int fila_processos[proc_n];




        int tam_vet_ext;
        int tam_vet_orig;

        tam_vet_orig = ARRAY_SIZE/proc_n;               // defino tamanho inicial do vetor
        tam_vet_ext = tam_vet_orig*1.5;               // defino 20% de buffer
        int (*vetor) = malloc(tam_vet_ext * sizeof *vetor);
        int buffer[1];          //buffer ?? a diferen??a
        int tam_buffer;         //buffer ?? a diferen??a
        tam_buffer=tam_vet_ext-tam_vet_orig;
        inicializa(vetor,tam_vet_orig,my_rank,proc_n);                   // sou a raiz e portanto gero o vetor - ordem reversa



// Ordena




        destE = my_rank-1;
        destD = my_rank+1;

//for(int vezes=0;vezes<2;vezes++){
while(1){

        bs(tam_vet_orig,&vetor[0]);  // ordenar
        //printaVetor(vetor,tam_vet_ext,my_rank);

        //printf("\n vezes=%d",vezes);


        if (destD<proc_n){
                // envia para a direita o maior valor
                MPI_Send (&vetor[tam_vet_orig-1],1, MPI_INT, destD, 1, MPI_COMM_WORLD);
        }


        if (my_rank>0){
                //recebe da esquerda o maior valor
                MPI_Recv (&buffer[0],1, MPI_INT , destE, 1, MPI_COMM_WORLD, &status);
                //printf("\n DEBUG1 My_rank=%d : Mensagem recebida do processo [%d] com Tag=%d e vetor [%d][%d]",my_rank,status.MPI_SOURCE,status.MPI_TAG,buffer[0],buffer[1]);
                //printf("\n rec[%d]=%d",my_rank,buffer[0]);
                if (buffer[0]>vetor[0]) {
                        //printf("\nProcesso %d: ?? maior que o meu menor",my_rank);
                        fila_processos[my_rank]=1;
                } else fila_processos[my_rank]=0;
        }
        int p_raiz;
        for (p_raiz=0;p_raiz<proc_n;p_raiz++)
                MPI_Bcast (&fila_processos[p_raiz],1, MPI_INT, p_raiz, MPI_COMM_WORLD);

        ind_ordenados=1;
        for (p_raiz=0;p_raiz<proc_n;p_raiz++){
                if (fila_processos[p_raiz]==1){
                        ind_ordenados=0;
                        break;
                }
        }

        if (ind_ordenados==1) break;

        //envia menores esquerda
        if (my_rank>0) MPI_Send (&vetor[0], tam_buffer, MPI_INT, destE, 2, MPI_COMM_WORLD);

        //processos das pontas executam um recebimento
        if (destD<proc_n){
                MPI_Recv (&vetor[tam_vet_ext-tam_buffer], tam_buffer, MPI_INT , destD, 2, MPI_COMM_WORLD, &status);
                //Recebeu menores da direita
                //printf("\n DEBUG2 My_rank=%d : Mensagem recebida da DIREITA [%d] com Tag=%d e vetor [%d][%d]",my_rank,destD,2,vetor[tam_vet_ext-2],vetor[tam_vet_ext-1]);
                //printaVetor(vetor,tam_vet_ext,my_rank);
                bs(2*tam_buffer,&vetor[tam_vet_ext-2*tam_buffer]); //ordeno novamente a partir da dist??ncia quadr??tica
                //envia maiores para a direita
                MPI_Send (&vetor[tam_vet_ext-tam_buffer], tam_buffer, MPI_INT, destD, 3, MPI_COMM_WORLD);
                //printaVetor(vetor,tam_vet_ext,my_rank);
        }


        //processos das pontas executam um recebimento
        if(my_rank>0){
                MPI_Recv (&vetor[0], tam_buffer, MPI_INT , destE, 3, MPI_COMM_WORLD, &status);
                //printf("\n DEBUG3 My_rank=%d : Mensagem recebida da ESQUERDA [%d] com Tag=%d e vetor [%d][%d]",my_rank,destE,3,vetor[0],vetor[1]);

                /*printf("\n buffer[%d]=%d",0,buffer[0]);
                printf("\n buffer[%d]=%d",1,buffer[1]);*/

        }





}






// mando para o pai

/*if ( my_rank !=0 ){
   //MPI_Send( vetor, pai, tam_vet_ext);  // tenho pai, retorno vetor ordenado pra ele
   printf("\n Filho %d enviando para o pai %d",my_rank,status.MPI_SOURCE);
   MPI_Send (vetor, tam_vet_ext, MPI_INT, pai, status.MPI_TAG, MPI_COMM_WORLD);
}*/

       // sou o raiz, mostro vetor
       //printf("\n");
       printf("\n DEBUG4 My_rank=%d : min|max [%d][%d]",my_rank,vetor[0],vetor[tam_vet_orig-1]);
    //printaVetor(vetor,tam_vet_orig,my_rank);
        t1=inteiro[1];
      t2 = MPI_Wtime(); // termina a contagem do tempo
      printf(" \nTempo gasto pela mensagem da Raiz at?? o processo %d: %f\n",my_rank, t2-t1);
        free(vetor);
        MPI_Finalize();
}
