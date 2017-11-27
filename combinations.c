/*
Title : combinations.c
Author : Jack Chen
Created on : Oct 11, 2017
Description : The user inputs two integers which correspond to the min and max
  lengths of a combination. The program first determines how many combinations
  it needs to check and cyclic distributes it to each processor. Afterwards, 
  each processor creates a string representation of each value. This is done 
  to make the implmentation of each condition more simple. Each processor applys
  the conditions on the values and keeps a sum of their local valid combinations. 
  In the end, all processors pass theirsum into a final value at id = 0;
Purpose :
  To determine the amount of valid combinations given some length boundries
  and some conditional constraints.
Usage : mpirun -np P combatinions X Y Z
  P is the amount of processors you want your program to run on.
  X is the shortest length your combination can be.
  Y is the longest length your combination can be.
  Z is the digit your combination cannot have (Optional).
Build with : mpicc -o combinations combinations.c 
            or if math library needs to be linked ...
            mpicc -o combinations combinations.c -lm 
*/

#include<mpi.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Checks to see if the arguments being passed into the program is valid. The
//function will terminate the program is it finds an invalid argument.
void errorCheck(int id, int arg_count, char* arguments[]);

//Converts a char to its int literal value.
int chartoi(char c);

//Takes some absolute value of our complete combination pool, determines its
//relative value with respect the starting length 'low, and then creates a
//string of its relative integer value.
char* createStringValue(int absolute_val, int low);

//A function that calls all the other combination criterias. Will return a 1
//if the combination passes every criteria. Else, will return a 0.
int checkConditions(char* combination, int restricted_digit);

//Checks if the combination contains a restricted digit.
int checkRestrictedDigit(char* combination, int restricted_digit);

//Checks if the first and last digit of the combination is the same.
int checkFirstAndLastDigit(char* combination);

//Checks that there are no consecutive duplicate digits.
int checkConsecutiveDuplicates(char* combination);

//Checks that no digit occurs more than twice in a combination.
int checkMoreThanTwoDuplicates(char* combination);

//Checks if the sum of the digits in the combination is not a multiple of
//the following, 7,11,13,17.
int checkMultiplesOfValues(char* combination);

//Checks if the combination is a multiple of its length.
int checkMultipleOfLength(char* combination);


const int DIGIT_BASE = 10;  //We are working with a decimal based system.

int main(int arg_count, char* args[]){
  
  int lowest_comb_len;
  int highest_comb_len;
  int restricted_digit;
  int processor_count;
  int id;

  MPI_Init(&arg_count, &args);

  MPI_Barrier(MPI_COMM_WORLD);
  double start_time = MPI_Wtime();

  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &processor_count);

  //Check if arguments were actually valid before doing actual computations.
  errorCheck(id, arg_count, args);  

  //Preparing variables needed for parallel work.
  lowest_comb_len = atoi(args[1]);
  highest_comb_len = atoi(args[2]);
  if(arg_count == 4) 
    restricted_digit = atoi(args[3]);
  else 
    restricted_digit = -1;  //Doesn't matter, set to any non decimal digit.  

  //Calculating total combinations
  int total_combinations = 0; 
  for(int length=lowest_comb_len; length<=highest_comb_len; length++){
    total_combinations += pow(DIGIT_BASE, length);
  }

  //Start parallel work;
  int valid_combinations = 0;
  for(int i=id;i<total_combinations;i+= processor_count){ //Cyclic load balance
    char* combination = createStringValue(i, lowest_comb_len);
    if(checkConditions(combination, restricted_digit)){
      valid_combinations++;
    }
  }

  //Combining parallel work to determine solution.
  int final_answer;
  MPI_Reduce(&valid_combinations, &final_answer, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  double total_time = MPI_Wtime() - start_time; //Already syncronized after MPI_Reduce

  if(id == 0){
    printf("Total Valid Combinations: %d\n", final_answer);
    printf("Total Run TIme: %f sec(s)\n", total_time);
  }

  MPI_Finalize();

  return 0;
}

//Function Definitions ---------------------------

int checkConditions(char* combination, int restricted_digit){
  if (checkFirstAndLastDigit(combination) == 0) return 0;
  else if (checkConsecutiveDuplicates(combination) == 0) return 0; 
  else if (checkMultipleOfLength(combination) == 0) return 0; 
  else if (checkMultiplesOfValues(combination) == 0) return 0;
  else if (checkMoreThanTwoDuplicates(combination) == 0) return 0;
  else if (checkRestrictedDigit(combination, restricted_digit) == 0) return 0; 
  else return 1; //If checkCondition still hasn't returned 0, then the combination passed.
}

//Checks to see if your combination contains the restricted digit.
int checkRestrictedDigit(char* combination, int restricted_digit){
  int length = strlen(combination);
  for(int i=0; i<length; i++){
    if(chartoi(combination[i]) == restricted_digit) 
      return 0;
  }
  return 1;
}

//Checks if the first and last digit are the same.
int checkFirstAndLastDigit(char* combination){ 
  int length = strlen(combination);
  if(combination[0] == combination[length-1]) 
    return 0;
  else 
    return 1; 
}

//Checks to see if given some digit in your combo, whether the following
//digit is the same.
int checkConsecutiveDuplicates(char* combination){
  int length = strlen(combination);
  for(int i=0; i<length-1; i++){            //No need to check the last element
    if(combination[i] == combination[i+1]) 
      return 0;
  }
  return 1; //If the loop didn't return a false for any element, then it passed.
}

//Checks to see if there are more than 2 occassions of a digit in your combo.
//This is done by having a tally array to count the occurance of each digit.
//The array holds the occurance of each of its corresponding index.
//ex: digit_tally[2] holds the amount of times 2 occurred in the string.
int checkMoreThanTwoDuplicates(char* combination){
  int length = strlen(combination);
  int digit_tally[10];	//An index for each digit in the decimal system.
  for(int i=0; i<10; i++){  //Initalize tally to 0.
    digit_tally[i] = 0;
  }
  for(int i=0; i<length; i++){  //Counting the occurance of each digit
    digit_tally[chartoi(combination[i])]++;
  }
  for(int i=0; i<10; i++){    //Checking the results
    if(digit_tally[i] > 2) 
      return 0;
  }
  return 1; //Passed
}

//Checks to see if the sum of all digits in the combination is a multiple
//of the following integers (7, 11, 13, 17)
int checkMultiplesOfValues(char* combination){
  int length = strlen(combination);
  int sum = 0;
  for(int i=0; i<length; i++){
    sum += chartoi(combination[i]);   //sum of each digit
  }

  if(sum%7 == 0 || sum%11 == 0 || sum%13 == 0 || sum%17 == 0) 
    return 0;
  else 
    return 1;
}

//Checks if the integer value of the combination is a multiple of its length.
int checkMultipleOfLength(char* combination){ 
  if(atoi(combination) % strlen(combination) == 0) 
    return 0;
  else 
    return 1;
}

//Converts a character to an integer.
int chartoi(char c){ 
  char tmp[2];
  tmp[0] = c; tmp[1] = '\0';
  return atoi(tmp);
}

//This is an ugly function, but basically....
//Given some absolute_val starting at length low, this determines
//it's relative value from low and creates a string representation.
//ex: The 0th combination, starting at length 3, is "000";
//ex: The 10th combination, starting at length 1, is "01";
//ex: The 111th combination, starting at length 1, is 001;
char* createStringValue(int absolute_val, int low){ 
  int comb_length = low;
  int offset = pow(DIGIT_BASE, comb_length); 
  while(absolute_val >= offset){      
    offset += pow(DIGIT_BASE, comb_length+1);
    comb_length++;
  }
  offset -= pow(DIGIT_BASE, comb_length);

  int relative_value = absolute_val - offset;          //Determining the value

  char* string = malloc((comb_length+1)*sizeof(char)); //Creating string form.
  sprintf(string, "%0*d",comb_length,relative_value);
  return string;
}


//Checks to see if enough arguments were specified for the program, 
//highest_comb_len is greater than the lowest_comb_len, and checks if the 
//restricted_digit is a decimal digit.
void errorCheck(int id, int arg_count, char* arguments[]){
  if(arg_count < 3){
    if(id ==0)
      printf("Not enough arguments specified. Ending program.\n");
      MPI_Finalize();
      exit(0);
  }
  else if(atoi(arguments[1]) > atoi(arguments[2])){ 
    if(id == 0)
      printf("Invalid combinations lengths. Ending program.\n"); 
    MPI_Finalize();
    exit(0);
  }
  else if(arg_count == 4 && (atoi(arguments[3]) < 0 || atoi(arguments[3]) > 9)){
    if(id == 0){ printf("Invalid restricted digit value. Ending program.\n"); }
    MPI_Finalize();
    exit(0);
  }
  else
    return;
}
