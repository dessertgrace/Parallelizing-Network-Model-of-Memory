#ifndef NS_GLOBALS_HH
#define NS_GLOBALS_HH

#include "Props.hh"
#include <unordered_set>

/**
 * Property values, as specified on the command line and/or
 * read from a property file.
 */
extern Props props;

/**
 * The simulation time counter
 */
extern uint  simTime; // hours

/**
 * The size of the simulation time step
 */
extern uint  timeStep; // hours

/**
 * MPI / parallel stuff
 */

extern int rank; // MPI rank
extern int size; // MPI comm size

extern uint n_units_global; // total global number of units (neurons)
extern std::unordered_set <uint> local_gids;

extern int *counts;
extern int *displacements;

extern uint8_t *global_activations;
void init_global_activations();
void synchronize();

#endif
