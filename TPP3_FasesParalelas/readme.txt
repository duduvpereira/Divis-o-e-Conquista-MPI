Compilação em máquina local:  mpicc file.c -o file_exec

Execução em máquina local:      mpirun -np 1 ./file_exec (onde np é o número de processos que serão criados)




Alguns colegas relataram dificuldades para executar um programa MPI com mais processos que núcleos da máquina física (parâmetro np > available cores). 

Caso a versão de MPI que  está sendo usada acuse este erro, é possível usar o parâmetro abaixo para contornar esta limitação:

--oversubscribe

Ex:

Em uma máquina com 4 cores:

mpirun --oversubscribe -np 8 hello
