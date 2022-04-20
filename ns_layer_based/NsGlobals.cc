#include "NsGlobals.hh"
#include "NsSystem.hh"
#include <iostream>
#include <map>
#include <mpi.h>

using std::string;


/**
 * Definition of global variables declared in Globals.hh
 */
Props props;
uint  simTime;
uint  timeStep;

/**
 * MPI / parallel stuff
 */

int world_rank;
int world_size;

int global_layer_count = 0;
uint n_units_global = 0;
int total_units_per_layer;

MPI_Comm layer_comm;

int layer_id;       // which layer (0 -> 3) this rank belongs to
int layer_rank;     // rank within the layer
int layer_size;     // total number of ranks in layer

int *counts;
int *displacements;
//uint8_t *global_activations;

std::map <uint, string> gid_id_map;
std::vector<std::string> layer_names;


void init_counts_displacements() {
    counts = new int [layer_size];
    displacements = new int [layer_size];
    displacements[0] = 0;
    total_units_per_layer = props.getInt("W") * props.getInt("H");
    int count = total_units_per_layer / layer_size;
    int remainder = total_units_per_layer % layer_size;
    for (int i = 0; i < layer_size; i++) {
        if (i < remainder) {
            counts[i] = count + 1;
        } else {
            counts[i] = count;
        }
        if (i > 0) {
            displacements[i] = displacements[i - 1] + counts[i - 1];
        }
    }
}


bool needs_layer_activations(int layerID) {
    switch (layerID) {
        case 0:
            return true;
        case 1:
            return true;
        case 2:
            if (layer_id == 0 || layer_id == 1 || layer_id == 2) {
                return true;
            } else {
                return false;
            }
        case 3:
            if (layer_id == 0 || layer_id == 1 || layer_id == 3) {
                return true;
            } else {
                return false;
            }
        default:
            return true;
    }
}


void init_mpi_components() {
    layer_id = world_rank % 4;
    MPI_Comm_split(MPI_COMM_WORLD, layer_id, world_rank, &layer_comm);
    MPI_Comm_rank(layer_comm, &layer_rank);
    MPI_Comm_size(layer_comm, &layer_size);
    init_counts_displacements();
    //global_activations = new uint8_t [total_units_per_layer * world_size];
}