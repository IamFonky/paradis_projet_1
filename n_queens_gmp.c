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

// the merge columns, rows and diagonals in a single int
mpz_t shots[NB_QUEENS][NB_QUEENS];

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

    // clear masks
    mpz_clear(mask);
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
*/
void buildMasks()
{
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        mpz_init(columns[row]);
        makeColumn(columns[row], row);

#if BUILD_SHOW_COLUMNS > 0
        printf("Column %d\n", row);
        showChessboard(columns[row]);
#endif

        mpz_init(rows[row]);
        makeRow(rows[row], row);

#if BUILD_SHOW_ROWS > 0
        printf("Row %d\n", row);
        showChessboard(rows[row]);
#endif

        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_init(diagonals1[row][column]);
            makeDiagonal1(diagonals1[row][column], column - row);

#if BUILD_SHOW_DIAGONALS_1 > 0
            printf("Diagonal1 - row : %d - col : %d\n", row, column);
            showChessboard(diagonals1[row][column]);
#endif

            mpz_init(diagonals2[row][column]);
            makeDiagonal2(diagonals2[row][column], column + row - NB_QUEENS + 1);

#if BUILD_SHOW_DIAGONALS_2 > 0
            printf("Diagonal2 - row : %d - col : %d\n", row, column);
            showChessboard(diagonals2[row][column]);
#endif
        }
    }

    // pretty redondant build but we dont care as the time to make it is insignificant
    for (short row = 0; row < NB_QUEENS; ++row)
    {
        for (short column = 0; column < NB_QUEENS; ++column)
        {
            mpz_init(shots[row][column]);
            mpz_set_ui(shots[row][column], 0);

            // compare line and board
            mpz_ior(shots[row][column], columns[column], rows[row]);
            mpz_ior(shots[row][column], shots[row][column], diagonals1[row][column]);
            mpz_ior(shots[row][column], shots[row][column], diagonals2[row][column]);

#if BUILD_SHOW_SHOTS > 0
            printf("All - row : %d - col : %d\n", row, column);
            showChessboard(shots[row][column]);
#endif
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
            mpz_clear(shots[row][column]);
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
    mpz_and(checker, board, shots[row][column]);
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

long long unsigned nb_solutions = 0;

void checkAllQueensRec(mpz_t board, int row)
{
    if (row == NB_QUEENS)
    {
#if RUN_SHOW_SOLUTIONS > 0
        showChessboard(board);
#endif
        ++nb_solutions;
    }
    else
    {
        for (int col = 0; col < NB_QUEENS; ++col)
        {
            if (isQueenValid(board, row, col))
            {
                // create new board that is a copy of the board
                mpz_t test_board;
                mpz_init(test_board);
                mpz_set(test_board, board);

                // put the queen at position (row,col)
                addQueenAt(test_board, row, col);

                // recurse with the new board
                checkAllQueensRec(test_board, row + 1);
            }
        }
    }
}

int stack_max = 0;
unsigned long long allocated_ints = 0;

void checkAllQueensIt()
{
    int index_stack = 0;

    // Creating and prealocating boards
    mpz_t boards[INT_SIZE];
    for (int i = 0; i < INT_SIZE; ++i)
    {
        mpz_init(boards[i]);
        mpz_set_ui(boards[i], 0);
    }
    mpz_t current_board;
    mpz_init(current_board);

    int rows[INT_SIZE];
    rows[0] = 0;

    while (index_stack >= 0)
    {
        stack_max = (index_stack > stack_max) * index_stack + (index_stack <= stack_max) * stack_max;

        mpz_set(current_board, boards[index_stack]);
        int old_stack = index_stack;
        int row = rows[index_stack];
        --index_stack;
#if RUN_SHOW_STACK > 0
        printf("Stack : %d\n", index_stack);
#endif

        if (row == NB_QUEENS)
        {
#if RUN_SHOW_SOLUTIONS > 0
            showChessboard(current_board);
#endif

            ++nb_solutions;

#if RUN_SHOW_SOLUTIONS_EVOLUTION > 0
            printf("Nb solutions : %llu\n", nb_solutions);
#endif
        }
        else
        {
            for (int col = 0; col < NB_QUEENS; ++col)
            {
                if (isQueenValid(current_board, row, col))
                {
                    ++index_stack;
#if RUN_SHOW_STACK > 0
                    printf("Stack : %d\n", index_stack);
#endif
#if RUN_SHOW_STACK_EVOLUTION > 0
                    if (index_stack > stack_max)
                    {
                        printf("New stack max : %d\n", index_stack);
                    }
#endif
                    mpz_set(boards[index_stack], current_board);
                    // put the queen at position (row,col)
                    addQueenAt(boards[index_stack], row, col);

                    rows[index_stack] = row + 1;
                }
            }
        }
        // mpz_clear(current_board);
    }

    // clearing boards
    for (int i = 0; i < INT_SIZE; ++i)
    {
        mpz_clear(boards[i]);
    }
    mpz_clear(current_board);
}

int main()
{
    printf("%d queen problem\n", NB_QUEENS);
    clock_t begin_build = clock();
    buildMasks(0b00000);
    double time_spent = (double)(clock() - begin_build) / CLOCKS_PER_SEC;
    printf("Time to buid %g seconds\n", time_spent);

    // init chessboard
    mpz_init(chessboard);
    mpz_set_ui(chessboard, 0);
    // showChessboard(chessboard);

    clock_t begin_queens = clock();
    // checkAllQueensRec(chessboard, 0, 0);
    // time_spent = (double)(clock() - begin_queens) / CLOCKS_PER_SEC;
    // printf("Recursive : Found %llu solutions in %g seconds\n", nb_solutions, time_spent);

    begin_queens = clock();
    nb_solutions = 0;
    checkAllQueensIt(0);
    time_spent = (double)(clock() - begin_queens) / CLOCKS_PER_SEC;
    printf("Iterative : Found %llu solutions in %g seconds\n", nb_solutions, time_spent);
    printf("Biggest stack : %d\n", stack_max);
    mpz_clear(chessboard);
    clearMasks();

    time_spent = (double)(clock() - begin_build) / CLOCKS_PER_SEC;
    printf("Total time to complete %g seconds\n", time_spent);
}
