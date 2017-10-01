#include<mpi.h>
#include<stdio.h>


int main(int args, char* inputs[]){

    const int lowest_password = pow(2, inputs[0]);
    const int highest_password = pow(2, inputs[1]);
    const int arr_size = highest_password - lowest_password; //Find another way to solve this later.

    const int processor_count;
    MPI_Comm_size(MPI_COMM_WORLD, processor_count);



    MPI_Init(&args, &var); //Initalizing of MPI environment
   
    int rank;       //ID of the process
    MPI_Comm_rank(MPI_COMM_WORLD, rank);

    int localsize = arr_size / processor_count;         //Check later that int vs double wont matter
    int localvalues[localsize];          //Every processor gets totalpasswords/processorcount to work with
    int localresults[localsize];
    
    for( ){} //Later, try to cyclically balance this.
    for(int i=0; i<localsize; i++){ localresults[i] = 1;} //By default everything is a valid entry until checked.



    

    MPI_Finalize(); //Ending of MPI environment

    return 0;
}

//Runtime checks
bool checkArgumentConditions(int args, char* inputs[]){
    if(args == 2){ //Then there are no restricted digit.

    }
    elseif(args == 3){ //There are resitrctied digits

    }
}


//Restrictions

void checkLeftAndRightDigit(int[] values , int[] results){


}

void checkConsecutiveDuplicates(int[] values, int[] results){


}

void checkMoreThanTwoDuplicates(int[] values, int[] results){


}

void checkMultiplesOfValues(int[] values, int[] results){

}

void checkMultipleOfLength(int[] values, int[] results){

}

