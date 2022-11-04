#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "nqueen_method.h"

int board[20][20];
int N = 5; //Change here for more Queen
int counterSolution =0;

bool isSafe(int row, int col);
void printSolution();
int setQueenCol(int row);
void putQueen(int row);
bool testDiagonale(int row, int col);
bool testHorizontale(int row, int col);
bool testVerticale(int row, int col);
  
int main(int argc, char *argv[]) 
{      
    //N = atoi(argv[1]); //----> si j'ajoute ça sa ne fonctionne

    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            board[i][j] = 0;
        }
    }
    
    
    putQueen(0);
}

void putQueen(int row)
{
    //printf("before: %i \n",row);
    if(row<N){       
        //printf("N : %i\n", N); 
        for(int i=0; i<N; i++){                        
            if(isSafe(row,i)){
                //printf("[%i][%i] \n",row, i);                
                //printf("after: %i \n",row);
                board[row][i] = 1;
                putQueen(row+1);
                board[row][i] = 0;                
            }
        }
    }
    else{
        counterSolution++;
        printf("\n The solution");
        printSolution();
        printf("Number of solution : %i\n", counterSolution);

    }
}

bool isSafe(int row, int col)
{ 

    if(testDiagonale(row,col) && testVerticale(row,col) && testHorizontale(row,col)){
        return true;
    }
    else{
        return false;
    }
}

bool testHorizontale(int row, int col){

    for(int i=col; i<N; i++){
        if(board[row][i] == 1){
            return false;
        }
    }

    for(int i=col; i>=0; i--){
        if(board[row][i] == 1){
            return false;
        }
    }

    return true;    
}

bool testVerticale(int row, int col){
    for(int i=col; i<N; i++){
        if(board[i][col] == 1){
            return false;
        }
    }

    for(int i=col; i>=0; i--){
        if(board[i][col] == 1){
            return false;
        }
    }

    return true;
}

bool testDiagonale(int row, int col)
{    
    int i,j;

    for(i=row, j=col; i<N, j<N; i++, j++){
        if(board[i][j] == 1){            
            return false;
        }
    }
        
    for(i=row, j=col; i>=0, j>=0; i--, j--){
        if(board[i][j] == 1){                        
            return false;
        }
    }
    
    for(i=row, j=col; i>=0, j<N; i--, j++){
        if(board[i][j] == 1)
        {            
            return false;
        }
    }
    for(i=row, j=col; i<N, j>=0; i++, j--){
        if(board[i][j] == 1)
        {
            return false;
        }
    }    
    return true;
}

int setQueenCol(int row){

    for(int col=0; col<N; col++){
        if(board[row][col]){
            return col;
            break;
        }
    }
}

//

void printSolution(){
    printf("\n");
    for(int i=0; i<N; ++i){
        for(int j=0; j<N; ++j){
            printf(" %i ", board[i][j]);
        }
        printf("\n");
    }
}