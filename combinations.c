#include<mpi.h>
#include<math.h>
#include<stdio.h>


void checkCriteria(int localsize,int* localvalues, int* localresults);
void checkLeftAndRightDigit(int size, int* values , int* results);
void checkConsecutiveDuplicates(int size, int* values, int* results);
void checkMoreThanTwoDuplicates(int size, int* values, int* results);
void checkMultiplesOfValues(int size, int* values, int* results);
void checkMultipleOfLength(int size, int* values, int* results);
int extractMostSignificantDigit(int n);
int extractLeastSignificantDigit(int n);
int extractNthDigit(int digit, int value);
int length( int n );


int main(int args, char* inputs[]){
    //if(args < 2 || args > 3){ abort();}
    //if(args == 2);//no digit restrictions
    //if(args == 3); //digit restriction

//"Global" processor variables---------------------------------------------------
    const int root_lowest_password = pow(2, atoi(inputs[0]));
    const int root_highest_password = pow(2, atoi(inputs[1]));
    const int root_arr_size = root_highest_password - root_lowest_password; //Find another way to solve this later.
    
    int processor_count;
    MPI_Comm_size(MPI_COMM_WORLD, &processor_count);


    MPI_Init(&args, &inputs);
   
    int id;       //ID of the process
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

//Individual Processor variables ---------------------------------------------
    int local_arr_size = root_arr_size / processor_count;      //Check later that int vs double wont matter
    int local_values[local_arr_size];   		  	  //Every processor gets floor(n/p) tasks
    int local_results[local_arr_size];
    
    for(int i=0 ; i<local_arr_size; i++ ){  //Later, try to cyclically balance this.
        int local_min_value = root_lowest_password + (id * local_arr_size);
        local_values[i] = local_min_value;
        local_min_value++;
    }
    for(int i=0; i<local_arr_size; i++){ local_results[i] = 1;} //By default everything is a valid entry until checked.


    //Apply multiple checks, which will leave a final state for localresults.
    checkCriteria(local_arr_size, local_values, local_results);


    //Done checking all possible combinations
    int sum_Valid_Passwords = 0;
    for(int i=0; i<local_arr_size; i++){
        if(local_results[i] == 1){
            sum_Valid_Passwords++;
        }
    }

    int finalTotalValidPasswords = 0;
    MPI_Reduce(&sum_Valid_Passwords, &finalTotalValidPasswords, 1, MPI_INT, MPI_SUM, 0 ,MPI_COMM_WORLD); //Reduce the sums
    
    MPI_Finalize(); //Ending of MPI environment

    if(id == 0){
        printf("%d\n", finalTotalValidPasswords);
    }

    return 0;
}


//Restrictions for combinations ===============================================

void checkCriteria(int localsize,int* localvalues, int* localresults){
    checkLeftAndRightDigit(localsize, localvalues, localresults);
    checkConsecutiveDuplicates(localsize, localvalues, localresults);
    checkMoreThanTwoDuplicates(localsize, localvalues, localresults);
    checkMultiplesOfValues(localsize, localvalues, localresults);
    checkMultipleOfLength(localsize, localvalues, localresults);
}

//Takes two arrays of some size, input some array of decimal numbers and it 
//under goes a criteria. If it passes, then it's corresponding results value remains
//1, otherwise, it becomes 0.
void checkLeftAndRightDigit(int size, int* values , int* results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){    //If the entry is still a possible solution

            if(extractMostSignificantDigit(values[i]) == extractLeastSignificantDigit(values[i])){
                results[i] = 0;     //Then it is no longer a possible solution.
            }

        }
    }
}

void checkConsecutiveDuplicates(int size, int* values, int* results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){

            for(int j=0; j<length(values[i])-1; j++){
                if(extractNthDigit(j, values[i]) == extractNthDigit(j+1, values[i])){
                    results[i] = 0;
                    break;
                }
            }

        }
    }
}

void checkMoreThanTwoDuplicates(int size, int* values, int* results){
    int digitcount[10];		//Bucket counting the occurance of each digit
    for(int i=0; i<size; i++){
        if(results[i] == 1){

            for(int j=0; j<10; j++){    //Setting and Refresh your digit count array
                digitcount[j] = 0;
            }
            for(int j=0; j<length(values[i]); j++){
                digitcount[ extractNthDigit(j, values[i])]++;
            }
            for(int j=0; j<10; j++){	//Now, we check the results of the bucket
                if(digitcount[j] > 2){
                    results[i] = 0;
		    break;
                }
            }

        }
    }
}

void checkMultiplesOfValues(int size, int* values, int* results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){
            
            int digitsum = 0;
            for(int j=0; j<length(values[i]); j++){
                digitsum += extractNthDigit(j, values[i]);
            }
            if(digitsum%7 == 0 || digitsum%11 == 0 || digitsum%13 == 0 || digitsum%17 == 0){
                results[i] = 0;
            }

        }
    }

}

void checkMultipleOfLength(int size, int* values, int* results){
    for(int i=0; i<size; i++){
        if(results[i] == 1){

            if(values[i] % length(values[i]) == 0){
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
    int len = length(value);
    value = value/pow(10,len - 1 - digit);
    return value % 10;
}

//Gets the length of an integer

int length( int n ){
    int len = 0;
    while (n>=10){
        n = n/10;
        len++;
    }
    return len;
}
