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

void init_global_activations() {
    global_activations = new uint8_t [4 * props.getInt("W") * props.getInt("H")];
}
