#include<mpi.h>
#include<stdio.h>


int main(int args, char* var[]){

    MPI_Init(&args, &var); //Initalizing of MPI environment




    MPI_Finalize(); //Ending of MPI environment

    return 0;
}

