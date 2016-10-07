/*
 ============================================================================
 Name        : sudoku.c
 Author      : Alexander Duong
 Version     : 1.0
 Description : Program to determine whether the solution to a Sudoku puzzle is valid.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

//Global variable that sets the size of the puzzle
//You can have a 16x16 sudoku puzzle if you want
//Must be a perfect square (9, 16,  25, etc)
#define GRID_SIZE 9
//Returns the maximum number of elements in a given array
#define NUMELEMS(x)	(sizeof(x) / sizeof(x[0]))

//Hard-coded test 9x9 Sudoku solution
int test[GRID_SIZE][GRID_SIZE] = {
		{6,5,3, 1,2,8, 7,9,4},
		{1,7,4, 3,5,9, 6,8,2},
		{9,2,8, 4,6,7, 5,3,1},

		{2,8,6, 5,1,4, 3,7,9},
		{3,9,1, 7,8,2, 4,5,6},
		{5,4,7, 6,9,3, 2,1,8},

		{8,6,5, 2,3,1, 9,4,7},
		{4,1,2, 9,7,5, 8,6,3},
		{7,3,9, 8,4,6, 1,2,5}
};

/* Utility function that prints the contents of an int array
 */
void printArray(int array[], int size) {
	for (int i=0; i<size; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

/* Utility function that prints the contents of a 2D int array
 */
void print2DArray(int array[][GRID_SIZE]) {
	print2DArrayHelper(array, GRID_SIZE);
	printf("\n");
}
void print2DArrayHelper(int array[][GRID_SIZE], int size) {
	for (int i=0; i<size; i++) {
		printArray(array[i], size);
	}
}

/* Comparison function for two integers.
 * Returns the difference of a and b
 */
int compareFunctionAscending(const void *a, const void *b) {
	int *x = (int *)a;
	int *y = (int *)b;
	return *x - *y;
}

/* Checks if a given array is filled with integers 1 -> GRID_SIZE
 * Returns 1 if true
 * Returns 0 if false
 */
int checkArray(int array[GRID_SIZE]) {
	qsort(array, GRID_SIZE, sizeof(*array), compareFunctionAscending);
	for (int i=1; i<=GRID_SIZE; i++) {
		if (array[i-1] != i)
			return 0;
	}
	return 1;
}

/* Checks if a given Sudoku column is complete
 * Returns 1 if true
 * Returns 0 if false
 */
int checkSudokuColumn(int columnIndex, int puzzle[GRID_SIZE][GRID_SIZE]) {
	int column[GRID_SIZE];
	for (int i=0; i<GRID_SIZE; i++) {
		column[i] = puzzle[i][columnIndex];
	}
	return checkArray(column);
}
int checkSudokuColumnTest(int columnIndex) {
	return (void *)checkSudokuColumn(columnIndex, test);
}

/* Checks if a given Sudoku row is complete
 * Returns 1 if true
 * Returns 0 if false
 */
int checkSudokuRow(int rowIndex, int puzzle[GRID_SIZE][GRID_SIZE]) {
	int array[GRID_SIZE];
	for (int i=0; i<GRID_SIZE; i++) {
		array[i] = puzzle[rowIndex][i];
	}
	return checkArray(array);
}
int checkSudokuRowTest(int rowIndex) {
	return (void *)checkSudokuRow(rowIndex, test);
}

/* Checks if a given Sudoku sub-grid is complete
 * Returns 1 if true
 * Returns 0 if false
 */
int checkSudokuSubgrid(int index, int puzzle[GRID_SIZE][GRID_SIZE]) {
	//gets coordinates of the sub-grid
	int xi = (int)(index % (int)sqrt(GRID_SIZE));
	int yi = (int)(index/(int)sqrt(GRID_SIZE));

	int array[GRID_SIZE];
	int arrayIndex = 0;

	for (int y=0; y<sqrt(GRID_SIZE); y++) {
		for (int x=0; x<sqrt(GRID_SIZE); x++) {
			//gets the indexes of the values in the sub-grid
			int valueIndexX = ((int)sqrt(GRID_SIZE) * xi) + x;
			int valueIndexY = ((int)sqrt(GRID_SIZE) * yi) + y;
			array[arrayIndex] = puzzle[valueIndexY][valueIndexX];
			arrayIndex++;
		}
	}
	return checkArray(array);
}
int checkSudokuSubgridTest(int subgridIndex) {
	return (void *)checkSudokuSubgrid(subgridIndex, test);
}

int main(void) {
	printf("CS149 Sudoku from Alexander Duong\n\n");
	print2DArray(test);

	int validRows[GRID_SIZE];
	int validColumns[GRID_SIZE];
	int validSubgrids[GRID_SIZE];

	//create 3 threads per loop. One each to check a row, column, and sub-grid
	//total of 27 threads on a 9x9 sudoku puzzle (Excessive? Yeah, probably haha)
	for (int i=0; i<GRID_SIZE; i++) {
		pthread_t threadr;
		pthread_t threadc;
		pthread_t threads;

		int *resultr;
		int *resultc;
		int *results;

		pthread_create(&threadr, NULL, checkSudokuRowTest, (int)i);
		pthread_join(threadr, &resultr);
		pthread_create(&threadr, NULL, checkSudokuColumnTest, (int)i);
		pthread_join(threadr, &resultc);
		pthread_create(&threadr, NULL, checkSudokuSubgridTest, (int)i);
		pthread_join(threadr, &results);

		validRows[i] = (int)resultr;
		validColumns[i] = (int)resultc;
		validSubgrids[i] = (int)results;
	}

	//checks if there are any incomplete rows, columns or sub-grids
	//and prints out the rows, columns, and sub-grids that are incomplete
	int solved = 1;
	for (int r=0; r<GRID_SIZE; r++) {
		if (validRows[r] == 0) {
			printf("Row %d is wrong\n", r);
			solved = 0;
		}
	}
	for (int c=0; c<GRID_SIZE; c++) {
		if (validColumns[c] == 0) {
			printf("Column %d is wrong\n", c);
			solved = 0;
		}
	}
	for (int s=0; s<GRID_SIZE; s++) {
		if (validSubgrids[s] == 0) {
			int xcoord = (int)(s % (int)sqrt(GRID_SIZE));
			int ycoord = (int)(s/(int)sqrt(GRID_SIZE));
			printf("Sub-grid at (%d, %d) is wrong\n", xcoord, ycoord);
			solved = 0;
		}
	}

	//prints results
	if (solved == 1)
		printf("\nSudoku puzzle is solved! :)\n");
	else
		printf("\nSudoku puzzle is incomplete. :(\n");

	return 0;
}

