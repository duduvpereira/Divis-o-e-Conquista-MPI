#include <string.h>
#include <stdio.h>
#include "mpi.h"
 
//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 100000     // trabalho final com o valores 10.000, 100.000, 1.000.000
#define ARRAY_NUMBER 1000     // número de arrays
 
/* recebe um ponteiro para um vetor que contem as mensagens recebidas dos filhos e            */
/* intercala estes valores em um terceiro vetor auxiliar. Devolve um ponteiro para este vetor */          */
 
int *intercala(int vetor[], int tam)
{
	int *vetor_auxiliar;
	int i1, i2, i_aux;

	vetor_auxiliar = (int *)malloc(sizeof(int) * tam);

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

void inicializa()
{
    int i;
    for (i=0 ; i<ARRAY_SIZE; i++)              /* init array with worst case for sorting */
        vetor[i] = ARRAY_SIZE-i;
}


//int (*saco)[ARRAY_NUMBER] = malloc(ARRAY_SIZE * sizeof *saco);

int vetor[ARRAY_SIZE];
int tam_vetor;

main(int argc, char** argv)
{
int my_rank;  /* Identificador do processo */
int proc_n;   /* Número de processos */
int source;   /* Identificador do proc.origem */
int dest;     /* Identificador do proc. destino */
int tag = 50; /* Tag para as mensagens */


MPI_Status status; /* Status de retorno */
 
MPI_Init (&argc , & argv);
 
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

double inteiro[10];
double t1,t2;
t1 = MPI_Wtime();  // inicia a contagem do tempo
inteiro[1]=t1;


// recebo vetor

if ( my_rank != 0 )
   {
   MPI_Recv (vetor, pai);                       // não sou a raiz, tenho pai
   MPI_Get_count(&Status, MPI_INT, &tam_vetor);  // descubro tamanho da mensagem recebida
   }
else
   {
   tam_vetor = ARRAY_SIZE;               // defino tamanho inicial do vetor
   Inicializa();			 // sou a raiz e portanto gero o vetor - ordem reversa
   }

// dividir ou conquistar?

if ( tam_vetor <= delta )
   bs(ARRAY_SIZE,vetor);  // conquisto
else
       {
    // dividir
    // quebrar em duas partes e mandar para os filhos

    MPI_Send ( &vetor[0], filho esquerda, tam_vetor/2 );  // mando metade inicial do vetor
    MPI_Send ( &vetor[tam_vetor/2], filho direita , tam_vetor/2 );  // mando metade final

    // receber dos filhos

    MPI_Recv ( &vetor[0], filho esquerda);            
    MPI_Recv ( &vetor[tam_vetor/2], filho direita);   

    // intercalo vetor inteiro
 
    vetor = intercala(vetor, ARRAY_SIZE);//ATENÇÃO
    }

// mando para o pai

if ( my_rank !=0 )
   MPI_Send( vetor, pai, tam_vetor);  // tenho pai, retorno vetor ordenado pra ele
else{
   printf("\nVetor: ");
   for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
      printf("[%03d] ", vetor[i]);                    // sou o raiz, mostro vetor
}
MPI_Finalize();
}
