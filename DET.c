/**
 * @file Assignment1.c
 * @author Jeremy Trendoff - 101160306
 * @brief A program to calculate the determinants of a matrix using three processes.
 * @date 2021-10-30
 */

// Include statements for shared memory, gettimeofday, and basic program functionality
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/time.h>

// Include statement for the shared memory state struct.
#include "SharedMem.h"

// Used for the time calculation.
#define MICRO_SEC_IN_SEC 1000000

// Get the largest number in a 3x3 matrix row.
int get_largest_number_in_matrix_row(int matrix[3][3], int row)
{
	int largest_number = matrix[row][0];

	for (int i = 0; i < 3; i++)
	{
		if (largest_number < matrix[row][i])
		{
			largest_number = matrix[row][i];
		}
	}

	return largest_number;
}

// Get the largest number in an array of length 3.
int get_largest_number(int arr[3])
{
	int largest_number = arr[0];

	for (int i = 1; i < 3; i++)
	{
		if (largest_number < arr[i]) 
		{
			largest_number = arr[i];
		}
	}

	return largest_number;
}

// Print a 3x3 matrix.
void print_matrix(int input_matrix[3][3])
{
	printf("The input matrix used:\n");
	printf("{%d, %d, %d}\n", input_matrix[0][0], input_matrix[0][1], input_matrix[0][2]);
	printf("{%d, %d, %d}\n", input_matrix[1][0], input_matrix[1][1], input_matrix[1][2]);
	printf("{%d, %d, %d}\n", input_matrix[2][0], input_matrix[2][1], input_matrix[2][2]);
}

int main() {
	// The shared use state.
	shared_use_state_t *shared_stuff;
	void *shared_memory = (void *)0;	
	int shmid;

	// The input matrix conataining the test data.
	int input_matrix[3][3] = {
		{20, 20, 50},
		{10, 6, 70},
		{40, 3, 2}
	};

	// PID's for the processes.
	pid_t pid1, pid2, pid3;

    // Creating shared memory ID and attaching shared memory segment to child process 
	shmid = shmget((key_t)1234, sizeof(shared_use_state_t), 0666 | IPC_CREAT);
	if (shmid == -1) 
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}

	shared_memory = shmat(shmid, (void *)0, 0);
	if (shared_memory == (void *)-1) 
	{
	    fprintf(stderr, "shmat failed\n");
	    exit(EXIT_FAILURE);
	}	
	shared_stuff = (shared_use_state_t *)shared_memory;

	// Initializing gettimeofday() calculator and starting the counter 
	struct timeval start, end;
	gettimeofday(&start, NULL);

	/* Create and execute the three child processes. */

	pid1 = fork();
	switch(pid1) {
		case -1:
			perror("fork #1 failed");
			exit(EXIT_FAILURE);
		case 0:
			shared_stuff->determinant += input_matrix[0][0] * ((input_matrix[1][1] * input_matrix[2][2]) - (input_matrix[1][2] * input_matrix[2][1]));
			shared_stuff->largest_numbers[0] = get_largest_number_in_matrix_row(input_matrix, 0);
			exit(EXIT_SUCCESS);
	}
	pid2 = fork();
	switch(pid2) {
		case -1:
			perror("fork #2 failed");
			exit(EXIT_FAILURE);
		case 0:
			shared_stuff->determinant += input_matrix[0][1] * ((input_matrix[1][2] * input_matrix[2][0]) - (input_matrix[1][0] * input_matrix[2][2]));
			shared_stuff->largest_numbers[1] = get_largest_number_in_matrix_row(input_matrix, 1);
			exit(EXIT_SUCCESS);
	}
	pid3 = fork();
	switch(pid3) {
		case -1:
			perror("fork #3 failed");
			exit(EXIT_FAILURE);
		case 0:
			shared_stuff->determinant += input_matrix[0][2] * ((input_matrix[1][0] * input_matrix[2][1]) - (input_matrix[1][1] * input_matrix[2][0]));
			shared_stuff->largest_numbers[2] = get_largest_number_in_matrix_row(input_matrix, 2);
			exit(EXIT_SUCCESS);
	}
	
	// Wait for child processes to complete.
	waitpid(pid1);
	waitpid(pid2);
	waitpid(pid3);

	// Ending the elapsed time counter 
	gettimeofday(&end, NULL);

	// Print out the results.
	print_matrix(input_matrix);
    printf("\nThe resulting determinant of the matrix is: %d\n", shared_stuff->determinant);
	printf("The largest number in the matrix is: %d\n", get_largest_number(shared_stuff->largest_numbers));

	// Detaching shared memory from parent process and destroy shared memory segment 
	shared_stuff = (shared_use_state_t *)shared_memory;
	if (shmdt(shared_memory) == -1) {
		fprintf(stderr, "shmdt failed\n");		
		exit(EXIT_FAILURE);
	}

	if (shmctl(shmid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}

	// Print elapsed time of performing all the operations 
	printf("Elapsed Time: %ld micro sec\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec)));

	exit(EXIT_SUCCESS);
}