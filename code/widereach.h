/** Widereach Classification */

#include <stdlib.h>
#include <stdio.h>
#include <glpk.h>

// #define EXPERIMENTAL

#include "general.h"
#include "glpk.h"

/* --------------------------- Execution -------------------------------- */

/** Launch a single experiment 
 
 @return a new array in which 
 the zeroth element is the objective value in the MIP solution at the end of 
 the run,
 and the next elements are the values of the decision variables. */
double *single_run(
  /** Seed for the random number generator, or NULL if the drand48 does not
   * need to be reseeded */
  unsigned int *seed, 
  /** Time limit in milliseconds */
  int tm_lim, 
  env_t *);

/** Binary search on theta to find the approximately largest value of 
 * feasible theta and sets it in the env.
 * 
 * @return the theta threshold
 */
double precision_threshold(
  /** Seed, as in single run */
  unsigned int *seed, 
  env_t *);

/** Scan through all values of theta to find the largest objective value 
 * 
 * @return the theta threshold
 */
double precision_scan(
  /** Seed, as in single run */
  unsigned int *seed, 
  env_t *);



