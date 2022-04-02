#include "NsGlobals.hh"
#include <iostream>
#include <unordered_set>
#include <mpi.h>


/**
 * Definition of global variables declared in Globals.hh
 */
Props props;
uint  simTime;
uint  timeStep;

/**
 * MPI / parallel stuff
 */

int rank;
int size;
uint n_units_global = 0;

std::unordered_set <uint> local_gids;

uint8_t *global_activations;
int *counts;
int *displacements;
int *recvcounts;
int max_count;

MPI_Datatype strided, resizestrided;


void init_global_counts_displacements() {
    counts = new int [size];
    displacements = new int [size];
    recvcounts = new int [size];

    int total_count = 4 * props.getInt("W") * props.getInt("H");
    int count = total_count / size;
    int remainder = total_count % size;

    if (remainder != 0) {
        max_count = count + 1;
    } else {
        max_count = count;
    }

    if (rank < remainder) count = count + 1;

    for (int i=0; i<size; i++) {
        counts[i] = count;
        recvcounts[i] = 1;
        displacements[i] = i;
    }
}


void build_mpi_components() {
    MPI_Type_vector(max_count, 1, size, MPI_UINT8_T, &strided);
    MPI_Type_create_resized(strided, 0, sizeof(uint8_t), &resizestrided);
    MPI_Type_commit(&resizestrided);
}


void init_global_activations() {
    init_global_counts_displacements();
    global_activations = new uint8_t [max_count * size];
    build_mpi_components();
}


void synchronize() {
    MPI_Allgatherv(MPI_IN_PLACE, 1, resizestrided,
                   global_activations, recvcounts, displacements,
                   resizestrided, MPI_COMM_WORLD);
}