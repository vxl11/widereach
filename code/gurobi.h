/* --------------------------- Execution -------------------------------- */

/** Return a Gurobi instance intialized from the given environment */
GRBmodel *gurobi_milp(
  /** Initialization error code (see Gurobi) */
  int *state, 
  /** Instance environment */
  const env_t *);


/** Launch a single experiment 
 
 @return a new array in which 
 the zeroth element is the objective value in the MIP solution at the end of 
 the run,
 and the next elements are the values of the decision variables. */
double *single_gurobi_run(
  /** Seed for the random number generator, or NULL if the drand48 does not
   * need to be reseeded */
  unsigned int *seed, 
  /** Time limit in milliseconds */
  int tm_lim, 
  env_t *);
