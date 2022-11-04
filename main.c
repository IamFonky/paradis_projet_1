#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "nqueen_method.h"

int board[20][20];
int N;

bool isSafe(int row, int col);
void printSolution();
int setQueenCol(int row);
void putQueen(int row);
bool testDiagonale(int row, int col);
bool testVerticale(int row, int col);
  
int main(int argc, char *argv[]) 
{      
    N = atoi(argv[1]);

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
        for(int i=0; i<N; i++){
            //printf("test\n");
            printf("%d\n", isSafe(row,i));
            if(isSafe(row,i)){    
                //printf("after: %i \n",row);
                board[row][i] = 1;
                putQueen(row+1);
                break;
            }
        }
    }
    else{
        printf("\n The solution");
        printSolution();

    }
}

bool isSafe(int row, int col)
{
    //int col = setQueenCol(row);
    /*printf("Diago : %d\n", testDiagonale(row,col));
    printf("Verti : %d\n", testVerticale(row,col));*/

    if(testDiagonale(row,col) && testVerticale(row,col)){
        return true;
    }
    else{
        return false;
    }
}

bool testVerticale(int row, int col){

    for(int i=col; i<N; i++){
        if(board[row][i]){
            return false;
        }
    }

    for(int i=col; i>=0; i--){
        if(board[row][i]){
            return false;
        }
    }

    return true;
    
}

bool testDiagonale(int row, int col)
{    
    int i,j;
    //printf("row : %d\n", row);

    for(i=row, j=col; i<N, j<N; i++, j++){
        if(board[i][j]){
            //printf("Test1\n");
            return false;
        }
    }
        
    for(i=row, j=col; i>0, j>=0; i--, j--){
        //printf("i : [%d]\n", i);
        if(board[i][j]){            
            //printf("Test2\n");
            //printf("[%d][%d]\n", i,j);
            //printf("board : %d\n", board[i][j]);
            //printf("\n");
            return false;
        }
    }
    
    for(i=row, j=col; i>0, j<N; i--, j++){
        //printf("i : [%d]\n", i);
        if(board[i][j]){
            //printf("Test3\n");
            //printf("[%d][%d]\n", i,j);
            //printf("board : %d\n", board[i][j]);
            //printf("\n");
            return false;
        }
    }
    for(i=row, j=col; i<N, j>=0; i++, j--){
        if(board[i][j]){
            //printf("Test4\n");
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
            printf("%i", board[i][j]);
        }
        printf("\n");
    }
}