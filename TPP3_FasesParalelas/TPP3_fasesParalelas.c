#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
 
//#define DEBUG 1            // comentar esta linha quando for medir tempo
#define ARRAY_SIZE 40 //100000     // trabalho final com o valores 10.000, 100.000, 1.000.000
#define delta 250000
 
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
}

void printaVetor(int * vetor2,int tamanho2, int myrank)
{
	printf("\nVetor[%d]: ",myrank);
   for (int i=0 ; i<tamanho2; i++)              /* print unsorted array */
	  printf("[%03d] ", vetor2[i]);                    // sou o raiz, mostro vetor
}


main(int argc, char** argv)
{
	
int my_rank;  /* Identificador do processo */
int proc_n;   /* Número de processos */
int source;   /* Identificador do proc.origem */
int dest,destE,destD,pai;     /* Identificador do proc. destino */
int tag = 50; /* Tag para as mensagens */


MPI_Status status; /* Status de retorno */
 
MPI_Init (&argc , & argv);
 
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &proc_n);

double inteiro[10];
double t1,t2;
t1 = MPI_Wtime();  // inicia a contagem do tempo
inteiro[1]=t1;

//int arvore[proc_n][3];



// num. total =>  an = 1*2^(n-1)
// recebo vetor


	int tam_vetor,n_passos;
	int buffer[2];	
	tam_vetor = ARRAY_SIZE/proc_n;               // defino tamanho inicial do vetor
	int (*vetor) = malloc(tam_vetor * sizeof *vetor);
	inicializa(vetor,tam_vetor,my_rank,proc_n);			 // sou a raiz e portanto gero o vetor - ordem reversa

//printf("Vetor processo %d", my_rank);
//printaVetor(vetor,tam_vetor);

// dividir ou conquistar?

   bs(tam_vetor,vetor);  // conquisto


	destE = my_rank-1;
	destD = my_rank+1;
	n_passos=10;
do{
    // dividir
    // quebrar em duas partes e mandar para os filhos
    // para de enviar quando o número de processos terminar
	if (destD<proc_n){
		// envia para a direita o maior valor		
		MPI_Send (&vetor[tam_vetor-1],1, MPI_INT, destD, 1, MPI_COMM_WORLD);
	}
  
	
	if (my_rank>0){
		//recebe da esquerda o maior valor
		MPI_Recv (&buffer,1, MPI_INT , destE, 1, MPI_COMM_WORLD, &status);
		printf("\n rec[%d]=%d",my_rank,buffer[0]);
		if (buffer[0]>vetor[0]) {
			printf("\nProcesso %d: é maior que o meu menor",my_rank);
			//envia os valores mais baixos para o vizinho da esquerda
			MPI_Send (&vetor[0], 2, MPI_INT, destE, 2, MPI_COMM_WORLD);
		} else MPI_Send (&vetor[0], 2, MPI_INT, destE, 171, MPI_COMM_WORLD);//fake
	}

	MPI_Recv (&buffer[0], 2, MPI_INT , MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	/*printf("\n buffer[%d]=%d",0,buffer[0]);
	printf("\n buffer[%d]=%d",1,buffer[1]);*/
	//TAG de troca de elementos
	if (status.MPI_TAG==2){
		if (status.MPI_SOURCE==destD){// tenho que devolver os maiores
			MPI_Send (&vetor[tam_vetor-2], 2, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);
			vetor[tam_vetor-2]=buffer[0];
			vetor[tam_vetor-1]=buffer[1];
			bs(tam_vetor,vetor); //ordeno novamente
		} else if (status.MPI_SOURCE==destE){// não respondo			
			vetor[0]=buffer[0];
			vetor[1]=buffer[1];
			bs(tam_vetor,vetor); //ordeno novamente
		}
	}
		printaVetor(vetor,tam_vetor,my_rank);			
		n_passos--;
} while(n_passos>0);


    



// mando para o pai

/*if ( my_rank !=0 ){
   //MPI_Send( vetor, pai, tam_vetor);  // tenho pai, retorno vetor ordenado pra ele
   printf("\n Filho %d enviando para o pai %d",my_rank,status.MPI_SOURCE);
   MPI_Send (vetor, tam_vetor, MPI_INT, pai, status.MPI_TAG, MPI_COMM_WORLD);	 
}*/

       // sou o raiz, mostro vetor      
      printaVetor(vetor,tam_vetor,my_rank);

	free(vetor);
	MPI_Finalize();
}
