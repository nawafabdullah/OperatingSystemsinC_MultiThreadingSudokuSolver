// Noah Alharbi
// CSCI360
// Assignement 2 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define PUZZLE_SIZE 9
#define NUMBER_OF_THREADS 27
#define LINE_MAX_LENGTH 100

int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1]; 	// Im memory representation of a sudoku puzzle solution
int status_map[NUMBER_OF_THREADS];       	// Status maps updated by corresponding worker thread

/**
 * Data structure to pass parameters to each worker thread 
 */
typedef struct {
	int thread_no;
	int x;
	int y;
} parameters;

parameters* worker_params[NUMBER_OF_THREADS]; 	// Array of worker thread parameter pointers, main thread passes an appropriate  parameter pointer to a corresponding worker thread
pthread_t workers[NUMBER_OF_THREADS];		// Array of worker threads, main thread needs to wait for all worker threads to finish before it compiles the final result 

void show_puzzle();
// Shows in memory content of a sudoku puzzle solution

void read_from_file(FILE* sudoku_file);
// Reads the solution of a sudoku puzzle line by line from a given FILE pointer
// Parses individual values (separated by comma) from each line and puts it at a corresponding in memory location
// Reports error if reading fails


void* row_worker(void* param);
// Checks whether the row of the sudoku puzzle solution referred by the param contains all the digits from 1 to 9
// Sets the appropriate status value in status_map.

void* col_worker(void* param);
// Checks whether the column of the sudoku puzzle solution referred by the param contains all the digits from 1 to 9
// Sets the appropriate status value in status_map.



void* subgrid_worker(void* param);
// Checks whether the subgrid of the sudoku puzzle solution referred by the param contains all the digits from 1 to 9
// Sets the appropriate status value in status_map.



int main(int argc, char** argv)
{
  FILE* sudoku_file = NULL;
  if (argc < 2)
  {
    printf (" No file name was passed \n");
    printf (" Usage: %s <num> \n", argv[0]);
    printf (" Quitting program \n");
    return 0; 
  }
  sudoku_file = fopen(argv[1], "r"); //fopen takes the file to open as first parm, and the second param specifies the mode "r" in our case for reading
  read_from_file(sudoku_file); //pass the pointer to the function to rean & process file  
  show_puzzle(); // call to function to display puzzle in memory 
  int counter = 0; // keeps track of thread numbers 
  parameters *param = (parameters*)malloc(sizeof(parameters)); // allocate memory space for struct 
  for (int i = 0; i < PUZZLE_SIZE; i++) // loop through creating threadsnand updating columns in col_worker function
  {
      param -> thread_no = counter; // assign new thread num in an increasing order as per counter
      param -> x = i;  // keep updating column
      param -> y = 0;  // let row remain the same 
      worker_params[counter] = param; // assign param to worker 
      printf ("thread number %i has been created and it will check colmn %i \n", i, i);    
      pthread_create (&workers[counter], NULL, col_worker, (void*)worker_params[counter]); //create thread and go to function 
      pthread_join(workers[counter], NULL); // wait for threads at this stage to finish      
      counter++; //increment counter
  }  

  for (int i = 0; i < PUZZLE_SIZE; i++) // loop through creating threads and process in function 
  {
      param -> thread_no = counter; // assign new thread num in an increasing order as per counter
      param -> x = 0; //let column remain the same
      param -> y = i;  // keep updating row to be checked 
      worker_params[counter] = param;  // copy content of param to worker 
      printf ("thread number %i has been created and it will check row %i \n", i + 9, i);    
      pthread_create (&workers[counter], NULL, col_worker, (void*)worker_params[counter]); //create thread and go to function
      pthread_join(workers[counter], NULL); // wait for threads at this level to finish       
      counter++;
  }  

  for (int i = 0; i < PUZZLE_SIZE; i+=3) // loop updating subgrid cordinates 
  {
   for (int j = 0; j < PUZZLE_SIZE; j+=3)// loop updating subgrid cordinates
   {
     param -> thread_no = counter; // assign new thread num in an increasing order as per counter
     param -> x = i; // keep updating col to be checked
     param -> y = j;  // keep updating row to be checked
     worker_params[counter] = param;
     printf ("thread number %i has been created and it will check row %i \n", counter, i);
     pthread_create (&workers[counter], NULL, subgrid_worker, (void*)worker_params[counter]); //create thread and go to function
     pthread_join(workers[counter], NULL); // wait for threads at this level to finish
     counter++;
   }
 } 

    int sum = 0; // initialize sum  
    // Iterate through all elements  
    // and add them to sum  
    for (int i = 0; i < NUMBER_OF_THREADS; i++)  
    {
      sum += status_map[i];
    }

    if (sum == 27) // if sum = 27, then all entries in the status map were 1 and the puzzle is valid
    {
      printf (" ******************************************************************************************* \n ............................. Valid Solution .............................................. \n ******************************************************************************************* \n");
    } else
    {
      printf (" *****************************************************************************************\
** \n ............................. Invalid Solution .............................................. \n ****\
*************************************************************************************** \n");
    }
    
}

void read_from_file(FILE* sudoku_file)
{
  char line_content [LINE_MAX_LENGTH + 1]; //read content until the end of the line 
  char delim[2] = ","; // specifying a delimter that will seperate info.. coma in our case
  int j = 0; //declare variables to be used in loops 
  if (sudoku_file == NULL) //note : pointer will return NULL if the file was not opened succesfully
  {
    printf ("failed to open file..");
    return; 
  }
  while (fgets(line_content, LINE_MAX_LENGTH, sudoku_file) != NULL) // we are using fgets to read and we told it\
 to store the line in array line_content, and we passed it the pointer sudoku_file. it will keep reading until hitti\
ng null
  {
   int  i = 0; 
   char* tokenize = strtok(line_content, delim); //tokenize line seperate by delim
   while (tokenize!= NULL) //loop until end of input storing in puzzle array as per indices [j][i]
  {
    if (i == 0) //first char befor comma
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int 
    }

    if (i == 1) //char after comma 1
    {
      puzzle[j][i] = atoi(tokenize);//atoi converts char to int
    }
    if (i == 2) //char after comma 2
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 3) //char after comma 3
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 4) //char after comma 4
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 5) //char after comma 5
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 6) //char after comma 6
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 7) //char after comma 7
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 8) //char after comma 8
    {
      puzzle[j][i] = atoi(tokenize); //atoi converts char to int
    }
    if (i == 9) //char after comma 19
    {
      puzzle[j][i] = atoi(tokenize);
    }    
    tokenize = strtok(NULL, delim); // to cover last empty line 
    i++; // increment i for next iteration 
    }
   j++; // increment j for next iteration
  }  
}

void show_puzzle()
{

  int i,j; 
  printf ("puzzle will be displayed below.. \n"); 
  for (i = 0; i < PUZZLE_SIZE; i++) //loop through printing elements like a matrix
  {
    for (j = 0; j < PUZZLE_SIZE; j++) 
    {
      printf ("%i \t", puzzle[i][j]);

    }
    printf ("\n");    
  }
}


void* col_worker(void* param)
{
    
  int tracker = 0; // tracker to keep track of found numbers
  int col_num = ((parameters*)param)->x; //recieve column to be checked from main inside the struct
  int tid = ((parameters*)param)->thread_no; //assign tid as per thread number assigned previously in main inside the struct 
  for (int num = 1; num < 10; num++) // loop through checking to find numbers 1 to 9 
  {
      for (int i = 0; i < PUZZLE_SIZE; i++) //loop through each column looking for numbers as per previous loop
      {      

	  if(puzzle[i][col_num] == num) //if number is found update tracker
	  {
	    tracker++; //increment tracker if the number is found
          }                   
        }
    }
        
  if (tracker == 9) // if tracker was increamented to 9, then all numbers 1 - 9 have been found
  {
  	status_map [tid] = 1; // update status map to 1 since all numbers were found in a column
        printf (" number has been found and status map POSITION %i has been updated to 1 \n", tid);
        printf (" STATUS MAP POSITION: %i HAS VALUE %i \n", tid, status_map [tid]);   

  }

  else // if tracker does not equal 9 then a number is missing or repeated 
  {
  	status_map [tid] = 0; // update status map to 0 since NOT all numbers were found in a column or a number was repeated  
        printf (" number has been found and status map POSITION %i has been updated to 0 \n", tid);
        printf (" STATUS MAP POSITION: %i HAS VALUE %i \n", tid, status_map [tid]);   
  }   
  pthread_exit((void*)worker_params[tid]);

}


void* row_worker(void* param)
{
  int tracker = 0;
  int row_num = ((parameters*)param)->y;
  int tid = ((parameters*)param)->thread_no;    
  for (int num = 1; num < 10; num++)
  {
    for (int i = 0; i < PUZZLE_SIZE; i++) 
    {   
      if(puzzle[row_num][i] == num)
      {
	tracker++;
      }                   
    }
  }    
  if (tracker == 9)
  {
    status_map [tid] = 1; 
    printf (" number has been found and status map POSITION %i has been updated to 1 \n", tid);   
  } else 
  {
    status_map [tid] = 0; 
    printf (" number has been found and status map POSITION %i has been updated to 0 \n", tid);   
  }   
  pthread_exit((void*)worker_params[tid]);

}

	
void* subgrid_worker(void* param)
{
    int tracker = 0;
    int col_num = ((parameters*)param)->x;
    int row_num = ((parameters*)param)->y;
    int tid = ((parameters*)param)->thread_no;
    unsigned int i, j;    
    printf("Processing sub-array rows %d-%d, cols %d-%d\n",
           row_num, row_num + 2, col_num, col_num + 2);
    for (int num = 1; num < 10; num++) 
    {
        for (i = row_num; i < row_num + 3; i++)
        {
            for (j = col_num; j < col_num + 3; j++)
	    {
	      if(puzzle[i][col_num] == num) //if number is found update tracker
              {
		tracker++; //increment tracker if the number is found
//                   printf("TRACKER VALUE IS %i \n", tracker);
	      }
	    }
	}
    }

    if (tracker == 9)
    {
        status_map [tid] = 1;
        printf (" number has been found and status map POSITION %i has been updated to 1 \n", tid);
        printf (" STATUS MAP POSITION: %i HAS VALUE %i \n", tid, status_map [tid]);
    }

    else
    {
        status_map [tid] = 0;
        printf (" number has been found and status map POSITION %i has been updated to 0 \n", tid);
        printf (" STATUS MAP POSITION: %i HAS VALUE %i \n", tid, status_map [tid]);
    }

   pthread_exit((void*)worker_params[tid]);

}  
