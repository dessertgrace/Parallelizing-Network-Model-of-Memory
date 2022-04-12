#ifndef NS_GLOBALS_HH
#define NS_GLOBALS_HH

#include "Props.hh"
#include <unordered_set>
#include <map>
#include <string>
#include <mpi.h>

using std::string;

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

extern int world_rank; // MPI rank
extern int world_size; // MPI comm size

extern int layer_id;       // which layer (0 -> 3) this rank belongs to
extern int layer_rank;     // rank within the layer
extern int layer_size;     // total number of ranks in layer
extern int total_units_per_layer;
extern uint8_t *global_activations;


extern uint n_units_global;
extern std::map <uint, string> gid_id_map;
extern std::vector<std::string> layer_names;

extern int global_layer_count;

extern MPI_Comm layer_comm;

extern int* counts;
extern int* displacements;

void init_mpi_components();

#endif
