/**
 * @file SharedMem.h
 * @author Jeremy Trendoff - 101160306
 * @brief A header file to declare the shared_use_state struct.
 * @date 2021-10-30
 */

#ifndef SHARED_MEM_H
#define SHARED_MEM_H

/* The shared use state struct containing the largest numbers in each row of the matrix and 
    the resulting determinant from the matrix. */
typedef struct shared_use_state {
    int largest_numbers[3];
    int determinant;
} shared_use_state_t;

#endif