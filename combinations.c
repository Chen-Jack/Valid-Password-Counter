#include<mpi.h>
#include<stdio.h>


int main(int args, char* inputs[]){
    if(args < 2 || args > 3){ abort();}
    if(args == 2);//no digit restrictions
    if(args == 3); //digit restriction


    const int lowest_password = pow(2, inputs[0]);
    const int highest_password = pow(2, inputs[1]);
    const int arr_size = highest_password - lowest_password; //Find another way to solve this later.

    const int processor_count;
    MPI_Comm_size(MPI_COMM_WORLD, processor_count);


    MPI_Init(&args, &var); //"Local" MPI environment--------------------------------
   
    int rank;       //ID of the process
    MPI_Comm_rank(MPI_COMM_WORLD, rank);

    int localsize = arr_size / processor_count;         //Check later that int vs double wont matter
    int localvalues[localsize];          //Every processor gets totalpasswords/processorcount to work with
    int localresults[localsize];
    
    for(int i=0 ; i<localsize; i++ ){  //Later, try to cyclically balance this.
        int localmin = lowest_password + (rank * localsize);
        localvalues[i] = localmin;
        localmin++;
    }
    for(int i=0; i<localsize; i++){ localresults[i] = 1;} //By default everything is a valid entry until checked.


    //Apply multiple checks, which will leave a final state for localresults.
    checkLeftAndRightDigit(&localsize, &localvalues, &localresults);
    checkConsecutiveDuplicates(&localsize, &localvalues, &localresults);
    checkMoreThanTwoDuplicates(&localsize, &localvalues, &localresults);
    checkMultiplesOfValues(&localsize, &localvalues, &localresults);
    checkMultipleOfLength(&localsize, &localvalues, &localresults);

    //Done checking all possible combinations

    int sumLocalValidPasswords = 0;
    for(int i=0; i<localsize; i++){
        if(localresults[i] = 1){
            sumValidPasswords++;
        }
    }

    int finalTotalValidPasswords = 0;
    MPI_Reduce(&sum, &finalTotalValidPasswords, 1, MPI_INT, MPI_SUM, 0 ,MPI_COMM_WORLD); //Reduce the sums
    
    MPI_Finalize(); //Ending of MPI environment

    if(rank == 0){
        printf("%d\n", finalTotalValidPasswords);
    }

    return 0;
}


//Restrictions for combinations ===============================================

//Takes two arrays of some size, input some array of decimal numbers and it 
//under goes a criteria. If it passes, then it's corresponding results value remains
//1, otherwise, it becomes 0.
void checkLeftAndRightDigit(int size, int[] values , int[] results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){    //If the entry is still a possible solution

            if(extractMostSignificantDigit(values[i]) != extractLeastSignificantDigit(values[i])){
                results[i] = 0;     //Then it is no longer a possible solution.
            }

        }
    }
}

void checkConsecutiveDuplicates(int arrsize, int[] values, int[] results){
    for(int i=0; i<arrsize; i++){
        if(results[i] == 1){

            for(int j=0; i<numberLength(values[i])-1; j++){
                if(extractNthDigit(j) == extractNthDigit(j+1)){
                    results[i] = 0;
                    break;
                }
            }

        }
    }
}

void checkMoreThanTwoDuplicates(int size, int[] values, int[] results){
    int digitcount[10];
    for(int i=0; i<size; i++){
        if(results[i] == 1){

            for(int j=0; j<10; j++){    //Refresh your digit count array
                digitcount[j] = 0;
            }
            for(int j=0; j<numberLength(values[i]); j++){
                digitcount[ extractNthDigit(j, values[i])] ++;
            }
            for(int j=0; j<10; j++){
                if(digitcount[i] > 2){
                    results[i] = 0;
                }
            }

        }
    }
}

void checkMultiplesOfValues(int size, int[] values, int[] results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){
            
            int digitsum = 0;
            for(int j=0; j<numberLength(values[i]); i++){
                sum += extractNthDigit(j, values[i]);
            }
            if(digitsum%7 ==0 || digitsum%11 == 0 || digitsum%13 == 0 || digitsum%17 == 0){
                results[i] = 0;
            }

        }
    }

}

void checkMultipleOfLength(int size, int[] values, int[] results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){

            if(values[i] % numberLength(values[i]) == 0){
                results[i] = 0;
            }

        }
    }
}

//Helper functions===================================

//Extracts the most significant digit
int extractMostSignificantDigit(int n){
    while (n >= 10){ 
        n = n / 10;
    }
    return n;
}

//Extracts the least significant digit
int extractLeastSignificantDigit(int n){
    return n % 10;
}

//The 0th digit is the most significant digit
int extractNthDigit(int digit, int value){
    int length = numberLength(value);
    n = n/pow(10,length - 1 - digit);
    return n % 10;
}

int numberLength( int n ){
    int length = 0;
    while (n>=10){
        n = n/10;
        length++;
    }
    return length;
}