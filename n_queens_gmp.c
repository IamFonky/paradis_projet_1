#include "n_queens.h"
#include <stdio.h>
#include <gmp.h>
#include <time.h>

// #define NB_QUEENS 8
#define INT_TYPE unsigned long long

mpz_t chessboard;

mpz_t diagonals1[NB_QUEENS][NB_QUEENS];
mpz_t diagonals2[NB_QUEENS][NB_QUEENS];
mpz_t columns[NB_QUEENS];
mpz_t rows[NB_QUEENS];

void makeColumn(mpz_t column, short startPos)
{
    // set column to 0
    mpz_set_ui(column, 0);

    // init and set mask to 1
    mpz_t mask;
    mpz_init(mask);
    mpz_set_ui(mask, 1);

    // offset mask to start column
    mpz_mul_2exp(mask, mask, startPos);

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // add mask to colun
        mpz_add(column, column, mask);

        // left shift mask to next row
        mpz_mul_2exp(mask, mask, NB_QUEENS);
    }

    // clear mask
    mpz_clear(mask);
}

void makeRow(mpz_t row, short startPos)
{
    // set row to 0
    mpz_set_ui(row, 0);

    // init and set mask to 1
    mpz_t mask;
    mpz_init(mask);
    mpz_set_ui(mask, 1);

    // offset mask to start row
    mpz_mul_2exp(mask, mask, (startPos * NB_QUEENS));

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // add mask to row
        mpz_add(row, row, mask);

        // left shift mask to next column
        mpz_mul_2exp(mask, mask, 1);
    }
}

void makeDiagonal1(mpz_t diagonal, short startPos)
{
    // set diagonal to 0
    mpz_set_ui(diagonal, 0);

    // init mask
    mpz_t mask;
    mpz_init(mask);

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // set mask to 1
        mpz_set_ui(mask, 1);
        // left shift mask to next column and row
        mpz_mul_2exp(mask, mask, (i * NB_QUEENS + i));
        // add mask to diagonal
        mpz_add(diagonal, diagonal, mask);
    }

    // init row mask
    mpz_t row_mask;
    mpz_init(row_mask);

    if (startPos >= 0)
    {
        // offset diagonal to startPos
        mpz_mul_2exp(diagonal, diagonal, startPos);

        for (short i = 0; i < startPos; i++)
        {
            // generate a row at pos
            makeRow(row_mask, (NB_QUEENS - 1) - i);

            // invert bits (complement)
            mpz_com(row_mask, row_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, row_mask);
        }
    }
    else
    {
        // offset diagonal to startPos
        mpz_fdiv_q_2exp(diagonal, diagonal, -startPos);

        for (short i = 0; i < -startPos; i++)
        {
            // generate a row at pos
            makeRow(row_mask, i);

            // invert bits (complement)
            mpz_com(row_mask, row_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, row_mask);
        }
    }

    // clear masks
    mpz_clear(mask);
    mpz_clear(row_mask);
}

void makeDiagonal2(mpz_t diagonal, short startPos)
{
    // set diagonal to 0
    mpz_set_ui(diagonal, 0);

    // init mask
    mpz_t mask;
    mpz_init(mask);

    for (short i = 0; i < NB_QUEENS; ++i)
    {
        // set mask to 1
        mpz_set_ui(mask, 1);
        // left shift mask to next column and row
        mpz_mul_2exp(mask, mask, (i * NB_QUEENS + (NB_QUEENS - 1 - i)));
        // add mask to diagonal
        mpz_add(diagonal, diagonal, mask);
    }

    // init column mask
    mpz_t column_mask;
    mpz_init(column_mask);

    if (startPos >= 0)
    {
        // offset diagonal to startPos
        mpz_mul_2exp(diagonal, diagonal, startPos);
        for (short i = 0; i < startPos; i++)
        {
            // generate a row at pos
            makeColumn(column_mask, i);

            // invert bits (complement)
            mpz_com(column_mask, column_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, column_mask);
        }
    }
    else
    {
        // offset diagonal to startPos
        mpz_fdiv_q_2exp(diagonal, diagonal, -startPos);

        for (short i = 0; i < -startPos; i++)
        {
            // generate a row at pos
            makeColumn(column_mask, (NB_QUEENS - 1) - i);

            // invert bits (complement)
            mpz_com(column_mask, column_mask);

            // remove row from the diagonal
            mpz_and(diagonal, diagonal, column_mask);
        }
    }

    // clear masks
    mpz_clear(mask);
    mpz_clear(column_mask);
}

void showChessboard(mpz_t board)
{

    char *displayPatern = "%1d";

    if (NB_QUEENS >= 10)
    {
        displayPatern = "%2d";
    }

    // init and set mask to 1
    mpz_t mask;
    mpz_init(mask);
    mpz_set_ui(mask, 1);

    // init and set checker to 0
    mpz_t checker;
    mpz_init(checker);
    mpz_set_ui(checker, 0);

    printf(" ");
    for (int y = 0; y < (NB_QUEENS); ++y)
    {
        printf(" ");
        printf(displayPatern, y);
    }
    printf("\n");
    for (int y = 0; y < NB_QUEENS; ++y)
    {
        printf(displayPatern, y);
        for (int x = 0; x < NB_QUEENS; ++x)
        {
            printf(" ");

            // and compare mask and board
            mpz_and(checker, board, mask);

            printf(mpz_cmp_ui(checker, 0) ? "!" : ".");

            if (NB_QUEENS >= 10)
            {
                printf(" ");
            }

            // left shift mask to next column
            mpz_mul_2exp(mask, mask, 1);
        }
        printf(" ");
        printf(displayPatern, y);
        printf("\n");
    }
    printf(" ");
    for (int y = 0; y < (NB_QUEENS); ++y)
    {
        printf(" ");
        printf(displayPatern, y);
    }
    printf("\n");

    // clear values
    mpz_clear(mask);
    mpz_clear(checker);
}

/*
    This function will build all the possible lines and put them in arrays so it is
    easy for the user to get which lines goes at witch position

    @param display is a bitmap that tells witch board to show when building
            - 0b0000 : no boards
            - 0b0001 : columns
            - 0b0010 : rows
            - 0b0100 : diagonals 1
            - 0b1000 : diagonals 2
*/
void buildMasks(short display)
{
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        mpz_init(columns[row]);
        makeColumn(columns[row], row);

        if (display & 0b0001)
        {
            printf("Column %d\n", row);
            showChessboard(columns[row]);
        }

        mpz_init(rows[row]);
        makeRow(rows[row], row);

        if (display & 0b0010)
        {
            printf("Row %d\n", row);
            showChessboard(rows[row]);
        }

        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_init(diagonals1[row][column]);
            makeDiagonal1(diagonals1[row][column], column - row);

            if (display & 0b0100)
            {
                printf("Diagonal1 - row : %d - col : %d\n", row, column);
                showChessboard(diagonals1[row][column]);
            }

            mpz_init(diagonals2[row][column]);
            makeDiagonal2(diagonals2[row][column], column + row - NB_QUEENS + 1);

            if (display & 0b1000)
            {
                printf("Diagonal2 - row : %d - col : %d\n", row, column);
                showChessboard(diagonals2[row][column]);
            }
        }
    }
}

void clearMasks()
{
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        mpz_clear(columns[row]);
        mpz_clear(rows[row]);
        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_clear(diagonals1[row][column]);
            mpz_clear(diagonals2[row][column]);
        }
    }
}

short isQueenOnLine(mpz_t board, mpz_t line)
{
    // init and set checker to 0
    mpz_t checker;
    mpz_init(checker);
    mpz_set_ui(checker, 0);

    // compare line and board
    mpz_and(checker, board, line);
    short returnValue = mpz_cmp_ui(checker, 0);

    // clear checker
    mpz_clear(checker);
    return returnValue;
}

short isQueenInColumn(mpz_t board, int column)
{
    return isQueenOnLine(board, columns[column]);
}

short isQueenInRow(mpz_t board, int row)
{
    return isQueenOnLine(board, rows[row]);
}

short isQueenInDiagonal1(mpz_t board, int row, int column)
{
    return isQueenOnLine(board, diagonals1[row][column]);
}

short isQueenInDiagonal2(mpz_t board, int row, int column)
{
    return isQueenOnLine(board, diagonals2[row][column]);
}

short isQueenValid(mpz_t board, int row, int column)
{
    // init and set checker to 0
    mpz_t checker;
    mpz_init(checker);
    mpz_set_ui(checker, 0);

    // compare line and board
    mpz_ior(checker, columns[column], rows[row]);
    mpz_ior(checker, checker, diagonals1[row][column]);
    mpz_ior(checker, checker, diagonals2[row][column]);
    mpz_and(checker, board, checker);
    short returnValue = mpz_cmp_ui(checker, 0);

    // clear checker
    mpz_clear(checker);
    return !returnValue;
}

void addQueenAt(mpz_t board, int row, int column)
{
    mpz_t new_queen;
    mpz_init(new_queen);
    mpz_set_ui(new_queen, 1);

    // left shift queen to position
    mpz_mul_2exp(new_queen, new_queen, (column + row * NB_QUEENS));
    mpz_ior(board, board, new_queen);

    mpz_clear(new_queen);
}

// void testing()
// {

//     mpz_set_ui(chessboard, 7);

//     // show column 3
//     showChessboard(columns[2]);
//     printf("Is queen on column 2?\n");
//     printf(isQueenInColumn(chessboard, 2) ? "YES!\n" : "NO?\n");
//     printf("Is queen on column 4?\n");
//     printf(isQueenInColumn(chessboard, 4) ? "YES!\n" : "NO?\n");

//     // show row 0
//     showChessboard(rows[0]);
//     printf("Is queen on row 0?\n");
//     printf(isQueenInRow(chessboard, 0) ? "YES!\n" : "NO?\n");
//     printf("Is queen on column 4?\n");
//     printf(isQueenInRow(chessboard, 4) ? "YES!\n" : "NO?\n");

//     // show diag1 (7,9)
//     showChessboard(diagonals1[7][9]);
//     printf("Is queen on diag1 (7,9)?\n");
//     printf(isQueenInDiagonal1(chessboard, 7, 9) ? "YES!\n" : "NO?\n");
//     printf("Is queen on diag1 (7,13)?\n");
//     printf(isQueenInDiagonal1(chessboard, 7, 13) ? "YES!\n" : "NO?\n");

//     // show diag1 (7,9)
//     showChessboard(diagonals2[7][9]);
//     printf("Is queen on diag1 (7,9)?\n");
//     printf(isQueenInDiagonal2(chessboard, 7, 9) ? "YES!\n" : "NO?\n");
//     printf("Is queen on diag1 (7,13)?\n");
//     printf(isQueenInDiagonal2(chessboard, 7, 13) ? "YES!\n" : "NO?\n");

//     // test point (9,10)
//     // init and set checker to 0
//     mpz_t checker;
//     mpz_init(checker);
//     mpz_set_ui(checker, 0);

//     // compare line and board
//     mpz_ior(checker, rows[9], columns[10]);
//     mpz_ior(checker, checker, diagonals1[9][10]);
//     mpz_ior(checker, checker, diagonals2[9][10]);

//     showChessboard(checker);
//     printf("Is queen valid on (9,10)?\n");
//     printf(isQueenValid(chessboard, 9, 10) ? "YES!\n" : "NO?\n");
//     printf("Is queen valid on (9,14)?\n");
//     printf(isQueenValid(chessboard, 9, 14) ? "YES!\n" : "NO?\n");

//     mpz_clear(checker);

//     mpz_set_ui(chessboard, 0);
//     showChessboard(chessboard);

//     addQueenAt(chessboard, 1, 0);
//     showChessboard(chessboard);

//     addQueenAt(chessboard, 3, 1);
//     showChessboard(chessboard);

//     addQueenAt(chessboard, 2, 2);
//     showChessboard(chessboard);

//     addQueenAt(chessboard, 0, 3);
//     showChessboard(chessboard);

//     // mpz_t column;
//     // mpz_init(column);
//     // makeColumn(column, 3);
//     // showChessboard(column);
//     // mpz_clear(column);

//     // mpz_t row;
//     // mpz_init(row);
//     // makeRow(row, 3);
//     // showChessboard(row);
//     // mpz_clear(row);

//     // mpz_t diag1;
//     // mpz_init(diag1);
//     // makeDiagonal1(diag1, 0);
//     // showChessboard(diag1);
//     // makeDiagonal1(diag1, -1);
//     // showChessboard(diag1);
//     // makeDiagonal1(diag1, -2);
//     // showChessboard(diag1);
//     // makeDiagonal1(diag1, -3);
//     // showChessboard(diag1);
//     // makeDiagonal1(diag1, -4);
//     // showChessboard(diag1);
//     // makeDiagonal1(diag1, -5);
//     // showChessboard(diag1);
//     // makeDiagonal1(diag1, -6);
//     // showChessboard(diag1);
//     // mpz_clear(diag1);

//     // mpz_t diag2;
//     // mpz_init(diag2);
//     // makeDiagonal2(diag2, 0);
//     // showChessboard(diag2);
//     // makeDiagonal2(diag2, -1);
//     // showChessboard(diag2);
//     // makeDiagonal2(diag2, -2);
//     // showChessboard(diag2);
//     // makeDiagonal2(diag2, -3);
//     // showChessboard(diag2);
//     // makeDiagonal2(diag2, -4);
//     // showChessboard(diag2);
//     // makeDiagonal2(diag2, -5);
//     // showChessboard(diag2);
//     // makeDiagonal2(diag2, -6);
//     // showChessboard(diag2);
//     // mpz_clear(diag2);
// }

long long unsigned nb_solutions = 0;

void checkAllQueensRec(mpz_t board, int row, short verbose)
{
    // printf("nb 1 : %lld\n",mpz_popcount(board));
    // printf("\nRow : %d\n", row);
    // showChessboard(board);
    if (row == NB_QUEENS /*&& mpz_popcount(board) == 4*/)
    {
        // printf("nb 1 : %lld\n", mpz_popcount(board));
        if (verbose)
        {
            showChessboard(board);
        }
        ++nb_solutions;
    }
    else
    {
        for (int col = 0; col < NB_QUEENS; ++col)
        {
            // if (row == 2 && col == 2)
            // {
            //     printf("\n TESTING \n");
            //     printf(isQueenValid(board, row, col) ? "Valid" : "Invalid");
            //     printf("\n");
            //     showChessboard(board);
            // }
            if (isQueenValid(board, row, col))
            {
                mpz_t test_board;
                mpz_init(test_board);
                mpz_set(test_board, board);
                addQueenAt(test_board, row, col);
                checkAllQueensRec(test_board, row + 1, verbose);
            }
        }
    }
}

int main()
{
    buildMasks(0b0000);

    // init chessboard
    mpz_init(chessboard);
    mpz_set_ui(chessboard, 0);
    // showChessboard(chessboard);

    clock_t begin = clock();
    checkAllQueensRec(chessboard, 0, 0);
    double time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;

    printf("Found %llu solutions in %g seconds\n", nb_solutions, time_spent);

    mpz_clear(chessboard);
    clearMasks();
}
