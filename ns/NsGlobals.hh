#ifndef NS_GLOBALS_HH
#define NS_GLOBALS_HH

#include "Props.hh"

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
#endif
