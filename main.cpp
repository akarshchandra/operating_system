//Description:
/*
 * This program takes a Sudoku puzzle solution as an input and then determines whether
 * the puzzle solution is valid. This validation is done using single thread and 27 threads.
 * 27 threads are assigned as  follows:
 * 9 for each 3x3 subsection, 9 for the 9 columns, and 9 for the 9 rows.
 * Each thread returns a integer value of 1 indicating that
 * the corresponding region in the puzzle they were responsible for is valid.
 * The program then waits for all threads to complete their execution and
 * checks if the return values of all the threads have been set to 1.
 * If yes, the solution is valid. If not, solution is invalid.
 * 
 * This Program also takes multiple sudoku puzzle solution as an input and then determines whether
 * all the solutions are valid .This validation is done using single threads and 10 threads
 * where each of the 10  threads are assigned each of 10 boards 
 * in the single thread approach the function checks and returns false if any of the condition is not met 
 * in multi thread approach a vector of size 10 is set in the coreesponding index if that solution is invalid 
 * 
 * This program also displays the total time taken for validation.
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <chrono>
#include <vector>
#include <typeinfo>
#include <string>

using namespace std;
using namespace std::chrono;
using std::vector;

const int number_of_threads=27;
const int rows=9;
const int columns=9;
int result_final[number_of_threads]={};
vector<bool> res_mul(10,false);

//the sudoku puzzle used for testing validation 
int sudoku[9][9]= 
            {
                {6, 2, 4, 5, 3, 9, 1, 8, 7},
                {5, 1, 9, 7, 2, 8, 6, 3, 4},
                {8, 3, 7, 6, 1, 4, 2, 9, 5},
                {1, 4, 3, 8, 6, 5, 7, 2, 9},
                {9, 5, 8, 2, 4, 7, 3, 6, 1},
                {7, 6, 2, 3, 9, 1, 4, 5, 8},
                {3, 7, 1, 9, 5, 6, 8, 4, 2},
                {4, 9, 6, 1, 8, 2, 5, 7, 3},
                {2, 8, 5, 4, 7, 3, 9, 1, 6}
        };

struct arguments
{
// initial row
int row;
//initial column
int column;
//the pointer to the sudoku board 
int (*sud_ptr)[9];
};

//Prototype of single thread single sudoku validation functions

bool verifyGrid();
bool verifyColumn();
bool verifyRow();
bool verifySudokuBySingleThread();

////Prototype of 27 thread single sudoku validation functions

void *verify_grid(void *parameter);
void *verify_row(void *params);
void *verify_col(void *params);


//Prototype of helper functions to  validate multiple sudoku solutions using multiple threads 

void copy2darrays(int params[9][9],int params1[9][9]);
bool verifyGrid_multiple(int sudd[][9]);
bool verifyColumn_multiple(int sudd[][9]);
bool verifyRow_multiple(int sudd[][9]);
bool verifySudokuBySingleThread_multiple(int sudd[][9]);
void *verify_multiple_multithread(void * parameter);


//containers for solving mutiple sudoku solutions using multiple threads  

class sud_container
{
    //used to encapsulate  2d arrays so that they can be stored and accessed via a std vector
    public:
    sud_container(int b[][9])
    {
        // copy the 2d array to data member 
        for(int i=0;i<9;i++)
        {
            for(int j=0;j<9;j++)
            {
                this->arr[i][j]=b[i][j];
            }
        }

    }
    //2d array public data member  
    int arr[9][9];

};


struct multiple_thread_obj
{
    // structure to encapsulate 2d array and iterator info  to be passed to pthread create  
    multiple_thread_obj(int par[][9],int j)
    {
    copy2darrays(a,par);
    b=j;
    }
    //public data member containing 2d array and an integer 
    int a[9][9];
    int b;

};





//Multiple sudoku solutions 


int sudoku1[9][9]=
            {
                {6, 2, 4, 5, 3, 9, 1, 8, 7},
                {5, 1, 9, 7, 2, 8, 6, 3, 4},
                {8, 3, 7, 6, 1, 4, 2, 9, 5},
                {1, 4, 3, 8, 6, 5, 7, 2, 9},
                {9, 5, 8, 2, 4, 7, 3, 6, 1},
                {7, 6, 2, 3, 9, 1, 4, 5, 8},
                {3, 7, 1, 9, 5, 6, 8, 4, 2},
                {4, 9, 6, 1, 8, 2, 5, 7, 3},
                {2, 8, 5, 4, 7, 3, 9, 1, 6}

            };

int sudoku2[9][9]=
            {
                {4, 3, 5, 2, 6, 9, 7, 8, 1},
                {6, 8, 2, 5, 7, 1, 4, 9, 3},
                {1, 9, 7, 8, 3, 4, 5, 6, 2},
                {8, 2, 6, 1, 9, 5, 3, 4, 7},
                {3, 7, 4, 6, 8, 2, 9, 1, 5},
                {9, 5, 1, 7, 4, 3, 6, 2, 8},
                {5, 1, 9, 3, 2, 6, 8, 7, 4},
                {2, 4, 8, 9, 5, 7, 1, 3, 6},
                {7, 6, 3, 4, 1, 8, 2, 5, 9}

            };

int sudoku3[9][9]=
            {
                {1, 5, 2, 4, 8, 9, 3, 7, 6},
                {7, 3, 9, 2, 5, 6, 8, 4, 1},
                {4, 6, 8, 3, 7, 1, 2, 9, 5},
                {3, 8, 7, 1, 2, 4, 6, 5, 9},
                {5, 9, 1, 7, 6, 3, 4, 2, 8},
                {2, 4, 6, 8, 9, 5, 7, 1, 3},
                {9, 1, 4, 6, 3, 7, 5, 8, 2},
                {6, 2, 5, 9, 4, 8, 1, 3, 7},
                {8, 7, 3, 5, 1, 2, 9, 6, 4}

            };

int sudoku4[9][9]=
            {
                {1, 2, 3, 6, 7, 8, 9, 4, 5},
                {5, 8, 4, 2, 3, 9, 7, 6, 1},
                {9, 6, 7, 1, 4, 5, 3, 2, 8},
                {3, 7, 2, 4, 6, 1, 5, 8, 9},
                {6, 9, 1, 5, 8, 3, 2, 7, 4},
                {4, 5, 8, 7, 9, 2, 6, 1, 3},
                {8, 3, 6, 9, 2, 4, 1, 5, 7},
                {2, 1, 9, 8, 5, 7, 4, 3, 6},
                {7, 4, 5, 3, 1, 6, 8, 9, 2}

            };
int sudoku5[9][9]=
            {
                {5, 8, 1, 6, 7, 2, 4, 3, 9},
                {7, 9, 2, 8, 4, 3, 6, 5, 1},
                {3, 6, 4, 5, 9, 1, 7, 8, 2},
                {4, 3, 8, 9, 5, 7, 2, 1, 6},
                {2, 5, 6, 1, 8, 4, 9, 7, 3},
                {1, 7, 9, 3, 2, 6, 8, 4, 5},
                {8, 4, 5, 2, 1, 9, 3, 6, 7},
                {9, 1, 3, 7, 6, 8, 5, 2, 4},
                {6, 2, 7, 4, 3, 5, 1, 9, 8}

            };
int sudoku6[9][9]=
            {
                {2, 7, 6, 3, 1, 4, 9, 5, 8},
                {8, 5, 4, 9, 6, 2, 7, 1, 3},
                {9, 1, 3, 8, 7, 5, 2, 6, 4},
                {4, 6, 8, 1, 2, 7, 3, 9, 5},
                {5, 9, 7, 4, 3, 8, 6, 2, 1},
                {1, 3, 2, 5, 9, 6, 4, 8, 7},
                {3, 2, 5, 7, 8, 9, 1, 4, 6},
                {6, 4, 1, 2, 5, 3, 8, 7, 9},
                {7, 8, 9, 6, 4, 1, 5, 3, 2}

            };
int sudoku7[9][9]=
            {
                {1, 2, 6, 4, 3, 7, 9, 5, 8},
                {8, 9, 5, 6, 2, 1, 4, 7, 3},
                {3, 7, 4, 9, 8, 5, 1, 2, 6},
                {4, 5, 7, 1, 9, 3, 8, 6, 2},
                {9, 8, 3, 2, 4, 6, 5, 1, 7},
                {6, 1, 2, 5, 7, 8, 3, 9, 4},
                {2, 6, 9, 3, 1, 4, 7, 8, 5},
                {5, 4, 8, 7, 6, 9, 2, 3, 1},
                {7, 3, 1, 8, 5, 2, 6, 4, 9}

            };
int sudoku8[9][9]=
            {
                {1, 7, 2, 5, 4, 9, 6, 8, 3},
                {6, 4, 5, 8, 7, 3, 2, 1, 9},
                {3, 8, 9, 2, 6, 1, 7, 4, 5},
                {4, 9, 6, 3, 2, 7, 8, 5, 1},
                {8, 1, 3, 4, 5, 6, 9, 7, 2},
                {2, 5, 7, 1, 9, 8, 4, 3, 6},
                {9, 6, 4, 7, 1, 5, 3, 2, 8},
                {7, 3, 1, 6, 8, 2, 5, 9, 4},
                {5, 2, 8, 9, 3, 4, 1, 6, 7}

            };

int sudoku9[9][9]=
            {
                {7, 2, 6, 4, 9, 3, 8, 1, 5},
                {3, 1, 5, 7, 2, 8, 9, 4, 6},
                {4, 8, 9, 6, 5, 1, 2, 3, 7},
                {8, 5, 2, 1, 4, 7, 6, 9, 3},
                {6, 7, 3, 9, 8, 5, 1, 2, 4},
                {9, 4, 1, 3, 6, 2, 7, 5, 8},
                {1, 9, 4, 8, 3, 6, 5, 7, 2},
                {5, 6, 7, 2, 1, 4, 3, 8, 9},
                {2, 3, 8, 5, 7, 9, 4, 6, 1}

            };
int sudoku10[9][9]=
            {
                {6, 5, 9, 3, 1, 4, 2, 8, 7},
                {1, 8, 7, 6, 5, 2, 4, 3, 9},
                {2, 3, 4, 8, 9, 7, 5, 1, 6},
                {4, 2, 6, 1, 3, 5, 9, 7, 8},
                {8, 7, 1, 9, 4, 6, 3, 5, 2},
                {5, 9, 3, 2, 7, 8, 6, 4, 1},
                {3, 1, 2, 5, 8, 9, 7, 6, 4},
                {7, 6, 5, 4, 2, 1, 8, 9, 3},
                {9, 4, 8, 7, 6, 3, 1, 2, 5}

            };



int main(void)
{
    //starting time for single thread execution
    steady_clock::time_point start_time_single_thread = steady_clock::now();
    if(verifySudokuBySingleThread())
    {
        cout<<endl;
        cout<<endl;
        std::cout<<"1)sudoku board is valid"<<std::endl;
    }
    else
    {
        cout<<endl;
        cout<<endl;
        std::cout<<"1)Sudoku board is not valid"<<std::endl;
    }

    // calculating the elapsed time in microseconds.
    steady_clock::time_point end_time_single_thread = steady_clock::now();
    long elapsed_time_single_thread = duration_cast<microseconds>(end_time_single_thread - start_time_single_thread).count();
    cout<<endl;
    cout << endl << "2)Total time for evaluating a single sudoku solution using single thread: " << elapsed_time_single_thread << " Micro seconds" <<endl;
    cout<<"================================================================================================================="<<endl;
    cout<<endl;
    cout<<endl;




    // using 27  threads to solve a single sudoku board 
    
    pthread_t threads[number_of_threads];

    //start time for validating the solution with 27 threads

    steady_clock::time_point starttime_threadmethod = steady_clock::now();
    int thread_index = 0;

    // create 27 threads 9 for 3*3 grids ,9 for columns ,9 for rows
    //pthread_create(pthread_t * thread, pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);

    for (int k=0;k<9;k++)
    {   //3*3 grid validation
        for(int l=0;l<9;l++)
        {
            if(k%3==0 && l%3==0)
            {
                arguments *sud_grid=new arguments;
                sud_grid->row=k;
                sud_grid->column=l;
                sud_grid->sud_ptr=sudoku;
                pthread_create(&threads[thread_index++], NULL, verify_grid, sud_grid);
            }
            if(l==0)
            {   // row validation 
                arguments *sud_row=new arguments;
                sud_row->row=k;
                sud_row->column=l;
                sud_row->sud_ptr=sudoku;
                pthread_create(&threads[thread_index++], NULL, verify_row, sud_row);
            }
            if(k==0)
            {   //column validation
                arguments *sud_col=new arguments;
                sud_col->row=k;
                sud_col->column=l;
                sud_col->sud_ptr=sudoku;
                pthread_create(&threads[thread_index++], NULL, verify_col, sud_col);
            }
        }
    }

    //waiting for all threads to finish execution
    for(int i=0;i<27;i++)
    {
        pthread_join(threads[i], NULL);
    }

    // if any  elements of  result_final contains 0 then the solution is invalid
    int cond=0;
    for(int i=0;i<27;i++)
    {
        if(result_final[i]==0)
        {
            cout<<"3) Sudoku board is not valid"<<endl;
            steady_clock::time_point endtime_threadmethod = steady_clock::now();
            //calculating time spent in microseconds
            long total_time = duration_cast<microseconds>(endtime_threadmethod - starttime_threadmethod).count();
            cout<<endl;
            cout<<endl;
            cout <<"4) Total time to evaluate a single sudoku board using 27 threads: " << total_time << "Micro seconds" << endl;
            cout<<"=============================================================================="<<endl;
            cond=1;
            break;

        }
    }
    if(cond==0)
    {
        cout<<"3) Sudoku board is valid"<<endl;
        steady_clock::time_point endtime_threadmethod = steady_clock::now();
        //calculating time spent in microseconds
        long total_time = duration_cast<microseconds>(endtime_threadmethod - starttime_threadmethod).count();
        cout<<endl;
        cout<<endl;
        cout << "4) Total time to evaluate a single sudoku board using 27 threads: " << total_time << "Micro seconds" << endl;
        cout<<"================================================================================"<<endl;
    }
    
    
    cout<<endl;
    cout<<endl;
    cout<<"5) ";

    //multiple boards or validating multiple sudoku solutions

    //creating a vector to store multiple sudoku boards
    // creating objects of sud_containers to store in the vector so that we are able to iterate and access each board one after other

    vector<sud_container> v_sud;
    sud_container *s1=new sud_container(sudoku1);
    v_sud.push_back(*s1);
    sud_container *s2=new sud_container(sudoku2);
    v_sud.push_back(*s2);
    sud_container *s3=new sud_container(sudoku3);
    v_sud.push_back(*s3);
    sud_container *s4=new sud_container(sudoku4);
    v_sud.push_back(*s4);
    sud_container *s5=new sud_container(sudoku5);
    v_sud.push_back(*s5);
    sud_container *s6=new sud_container(sudoku6);
    v_sud.push_back(*s6);
    sud_container *s7=new sud_container(sudoku7);
    v_sud.push_back(*s7);
    sud_container *s8=new sud_container(sudoku8);
    v_sud.push_back(*s8);
    sud_container *s9=new sud_container(sudoku9);
    v_sud.push_back(*s9);
    sud_container *s10=new sud_container(sudoku10);
    v_sud.push_back(*s10);


   //multiple sudoku execution single thread
   // strating time single time multiple sudoku validations
   steady_clock::time_point start_time_single_thread_multiple = steady_clock::now();

   for(int i=0;i<v_sud.size();i++)
   {
    bool result=verifySudokuBySingleThread_multiple(v_sud[i].arr);
     if(result)
    {
        std::cout<<"Sudoku "<<(i+1)<<" is valid | ";
    }
    else
    {
        std::cout<<"Sudoku "<<(i+1)<<" is not valid | ";
    }
       
   }
   // calculate total time spent to  validate 10 solutions in a single thread

   steady_clock::time_point end_time_single_thread_multiple = steady_clock::now();
   long elapsed_time_single_thread_multiple = duration_cast<microseconds>(end_time_single_thread_multiple - start_time_single_thread_multiple).count();
   cout<<endl;
   cout<<endl;
   cout<< "6) Total time taken to evaluate 10 sudoku boards using single thread: " << elapsed_time_single_thread_multiple << " Micro seconds" << endl;
   cout<<"=================================================================================="<<endl;
   cout<<endl;
   cout<<endl;

   //validate 10 sudoku soutions using 10 threads 

   int thread_index_multiple=0;
   int number_of_thread_multiple=10;
   vector<bool> res(10,false);

   pthread_t threads_multiple[number_of_thread_multiple];

   //start clock 10 sudoku soutions using 10 threads 

   steady_clock::time_point starttime_threadmethod_multiple = steady_clock::now();
   
   for(int i=0;i<10;i++)
   {
    multiple_thread_obj *mbo=new multiple_thread_obj(v_sud[i].arr,i);
    pthread_create(&threads_multiple[thread_index_multiple++], NULL, verify_multiple_multithread,mbo);
   }

    // wait for all 10 threads to complete
    for(int i=0;i<10;i++)
    {
        pthread_join(threads_multiple[i], NULL);
    }

    // calculate time taken to  validate 10 sudoku solutions using 10 threads

    steady_clock::time_point endtime_threadmethod_multiple = steady_clock::now();
    long total_time_multiple = duration_cast<microseconds>(endtime_threadmethod_multiple - starttime_threadmethod_multiple).count();
    cout<<endl;
    cout<< "7) Total time taken to solve 10 sudoku boards using 10 threads  : " << total_time_multiple << " Micro seconds" << endl;
    cout<<"============================================================================================"<<endl;
    cout<<endl;
    cout<<endl;

    // iterate res_mul vector if any element is 1 the sudoku corresponding to the index of that element is invalid
    for(int i=0;i<res_mul.size();i++)
    {
        if(res_mul[i]==0)
        {
           cout<<"sudoku "<<(i+1)<<" is valid | ";
        }
        else if(res_mul[i]==1)
        {
            cout<<"sudoku "<<(i+1)<<" is not valid | ";
        }
    }

   

}

// function definitions 
/*used for single thread solution validation
 * verifies if each grid  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the grid, the corresponding index's value in numbers is set to true.
 * If a value in numbers vector is false then that value is either absent or present twice.
 * This false value indicates that the given solution is not valid.
 * If all the values are true then solution is valid
 *
 * @return  bool result of grid validation
 */
bool verifyGrid(){
    for (int i = 0; i < 3; ++i){
        for (int j = 0; j < 3; ++j){
            int grid_row = 3 * i;
            int grid_column = 3 * j;
            vector<bool> numbers(rows, false);
            for(int r=grid_row; r < grid_row+3; r++){
                for(int c=grid_column; c < grid_column+3; c++){
                    numbers[sudoku[r][c]-1] = !numbers[sudoku[r][c]-1];
                }
            }
            for(auto i : numbers){
                if(!i){
                    return false;
                }
            }
        }
    }
    return true;
}

/*used for single thread solution validation
 * verifies if each column  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the column, the corresponding index's value in numbers is set to true.
 * If a value in numbers vector is false then that value is either absent or present twice.
 * This false value indicates that the given solution is not valid.
 * If all the values are true then solution is valid
 *
 * @return  bool result of column validation
 */
bool verifyColumn(){
    for(int i=0; i<rows; i++){
        vector<bool> numbers(rows, false);
        for(int j=0; j<columns; j++){
            numbers[sudoku[j][i]-1] = !numbers[sudoku[j][i]-1];
        }
        for(auto i : numbers){
            if(!i){
                return false;
            }
        }
    }
    return true;
};

/*used for single thread solution validation
 * verifies if each row  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the row, the corresponding index's value in numbers is set to true.
 * If a value in numbers vector is false then that value is either absent or present twice.
 * This false value indicates that the given solution is not valid.
 * If all the values are true then solution is valid
 *
 * @return  bool result of row validation
 */
bool verifyRow(){
    for(int i=0; i<rows; i++){
        vector<bool> numbers(rows, false);
        for(int j=0; j<columns; j++){
                numbers[sudoku[i][j]-1] = !numbers[sudoku[i][j]-1];
        }
        for(auto i : numbers){
            if(!i){
                return false;
            }
        }
    }
    return true;
}

/*For single thread solution validation
 * verifies the solution based on row,column and grid
 *
 * @return  bool result of solution validation
 */
bool verifySudokuBySingleThread(){
    if (!verifyRow()){
        return false;
    }
    if (!verifyColumn()){
        return false;
    }
    if (!verifyGrid()) {
        return false;
    }
    return true;
}




/* used for multi thread single solution validation
 * verifies if each grid  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the grid, the corresponding index's value in numbers is set to true.
 * If a value in index corresponding to the current value is already true then that value is present twice.
 * This indicates that the given solution is not valid.
 * If all the values are true then solution is valid
 * @param   void * parameters (pointer).
 * 
 */

void *verify_grid(void *parameter)
{
    arguments *sud=(arguments*) parameter;
    int row=sud->row;
    int col=sud->column;
    vector<bool> numbers(10,false);
    for (int i = row; i < row + 3; ++i)
    {
        for(int j=col;j<col+3;++j)
        {
            int ind=sud->sud_ptr[i][j];
            if (numbers[ind]==true)
            {
            pthread_exit(NULL);
            }
            else
            {
            numbers[ind]=!numbers[ind];
            }
        }
    }

    // If the execution has reached this point, then the 3x3 sub-grid is valid.
    result_final[row + col/3] = 1; // Maps the 3X3 sub-grid to an index in the first 9 indices of the result array
    pthread_exit(NULL);

}



/* used for multi thread single solution validation
 * verifies if each row  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the grid, the corresponding index's value in numbers is set to true.
 * If a value in index corresponding to the current value is already true then that value is present twice.
 * This indicates that the given solution is not valid.
 * If all the values are true then solution is valid
 * @param   void * params (pointer).
 * 
 */


void *verify_row(void *params)
{
    arguments *sud=(arguments*) params;
    int row=sud->row;
    vector<bool> numbers(10,false);
    for (int j=0;j<9;j++)
    {
        int ind=sud->sud_ptr[row][j];
        if (numbers[ind]==true)
        {
            pthread_exit(NULL);
        }
        else
        {
            numbers[ind]=!numbers[ind];
        }
    }

    result_final[9 + row] = 1;
    pthread_exit(NULL);
}

/* used for multi thread single solution validation
 * verifies if each column  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the grid, the corresponding index's value in numbers is set to true.
 * If a value in number corresponding to the current value is already true then that value is present twice.
 * This indicates that the given solution is not valid.
 * If all the values are true then solution is valid
 * @param   void * params (pointer).
 * 
 */

void *verify_col(void *params)
{
    arguments *sud=(arguments*) params;
    int col=sud->column;
    vector<bool> numbers(10,false);
    for (int i = 0; i < 9; i++)
    {
        int ind=sud->sud_ptr[i][col];
        if (numbers[ind]==true)
        {
            pthread_exit(NULL);
        }
        else
        {
            numbers[ind]=!numbers[ind];
        }
    }
    // If the execution has reached this point, then the column is valid.
    result_final[18 + col] = 1; // Maps the column to an index in the third set of 9 indices of the result array
    pthread_exit(NULL);

}


//functions to solve multiple sudoku solutions

/*
 * copies 2d array source and destination both passed as parameters
 * mested for loops used for deep copy of each element
 * @param  int 2d array destination, int 2d array of sudoku board source  .
 */

void copy2darrays(int params[9][9],int params1[9][9])
{
    for(int i=0;i<9;i++)
    {
        for(int j=0;j<9;j++)
        {
            params[i][j]=params1[i][j];
        }
    }
}


/* Used for both single and multi thread multi solution validation
 * Checks if a grid of size 3x3 contains all numbers from 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the grid, the corresponding index in numbers is set to true.
 * If the value in numbers is false implies that the value is repeating or the value is not preesent.
 * This indicates theat the grid is invalid
 * @param   int  2d array of the sudoku solution
 * @return  bool result of grid validation
 */

bool verifyGrid_multiple(int sudd[][9]){
    for (int i = 0; i < 3; ++i){
        for (int j = 0; j < 3; ++j){
            int grid_row = 3 * i;
            int grid_column = 3 * j;
            vector<bool> numbers(rows, false);
            for(int r=grid_row; r < grid_row+3; r++){
                for(int c=grid_column; c < grid_column+3; c++){
                    numbers[sudd[r][c]-1] = !numbers[sudd[r][c]-1];
                }
            }
            for(auto i : numbers){
                if(!i){
                    return false;
                }
            }
        }
    }
    return true;
} 




/* Used for both single and multi thread multi solution validation
 * Checks if each column contains all digits from 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the column, the corresponding index in numbers is set to true.
 * If the value in numbers is false implies that the value is repeating or the value is not preesent.
 * This indicates theat the column is invalid
 * @param   int  2d array of the sudoku solution
 * @return  bool result of grid validation
 */


bool verifyColumn_multiple(int sudd[][9]){
    for(int i=0; i<9; i++){
        vector<bool> numbers(rows, false);
        for(int j=0; j<9; j++){
            numbers[sudd[j][i]-1] = !numbers[sudd[j][i]-1];
        }
        for(auto i : numbers){
            if(!i){
                return false;
            }
        }
    }
    return true;
};


/* used for both single and multi thread  multiple solution validation
 * Checks if each row  contains all the digits 1-9.
 * A boolean vector numbers is initialized to false.
 * For every value in the row, the corresponding index in numbers is set to true.
 * If the value in numbers is false implies that the value is repeating or the value is not preesent.
 * This means the row is  not valid
 * @param   int  2d array of the sudoku solution
 * @return  bool result of row validation
 */


bool verifyRow_multiple(int sudd[][9]){
    for(int i=0; i<rows; i++){
        vector<bool> numbers(rows, false);
        for(int j=0; j<columns; j++){
                numbers[sudd[i][j]-1] = !numbers[sudd[i][j]-1];
        }
        for(auto i : numbers){
            if(!i){
                return false;
            }
        }
    }
    return true;
}

/* For single thread multiple solution validation
 * verifies the solution based on row,column and grid
 *
 * @return  bool result of solution validation
 */


bool verifySudokuBySingleThread_multiple(int sudd[][9]){
    if (!verifyRow_multiple(sudd)){
        return false;
    }
    if (!verifyColumn_multiple(sudd)){
        return false;
    }
    if (!verifyGrid_multiple(sudd)) {
        return false;
    }
    
    return true;
}

/* For multiple thread multiple solution validation
 * verifies the solution based on row,column and grid
 * sets the global vector res_mul based on the  result of solution validation
 */

void *verify_multiple_multithread(void * parameter)
{
    
    multiple_thread_obj *mto = (multiple_thread_obj*) parameter;
    int k= mto->b;

    if (!verifyRow_multiple(mto->a)){
        res_mul[k]=1;
    }
    if (!verifyColumn_multiple(mto->a)){
       
        res_mul[k]=1;
    }
    if (!verifyGrid_multiple(mto->a)) {
          res_mul[k]=1;
        
    }
    

    pthread_exit(NULL);
}
