/* --------------------------- Samples -------------------------------- */

/** A group of samples */
typedef struct {
	/** Dimension of the sample space, and 
	 * size of the values array of all samples in the array */
	size_t dimension;
	/** Number of sample categories. */
	size_t class_cnt;
	/** Number of samples in each category */
	size_t *count;
	/** Label of the samples in each category */
	int *label;
	/** Sample array. 
	 * The ith array contains count[i] samples in class label[i].
	 * Each sample contains the value along the given dimensions. */
	double ***samples;
} samples_t;


/** Sample locator: specifies a sample in a sample group by class and
 * index within class */
typedef struct {
	/** Class of the index. 
	 * A negative class denotes a non-sample variable */
	int class;
	size_t index;
} sample_locator_t;

/** @brief Delete the element within the given sample array.
 *
 * It is assumed that all the arrays within the sample
 * have been dynamically allocated.
 * No assumption is made about the allocation of the argument itself.
 *
 * @return the samples */
samples_t *delete_samples(samples_t *);

/** Print all samples in the sample set */
void print_samples(samples_t *);

/** Checks whether the given samples are binary: it contains two classes,
 * the first of which labelled -1 and the other +1 */
int is_binary(const samples_t *);

/** Returns the primary value associated with a label. 
 * The primary value is defined as 1 for samples with positive labels
 * and 0 otherwise. */
double primary_value(int label);


/** Return the total number of samples */
size_t samples_total(const samples_t *);

/** Return the number of positive samples in a binary sample set */
size_t positives(const samples_t *);

/** Return the number of negative samples in a binary sample set */
size_t negatives(const samples_t *);

/** Sets the count samples in the given class with the given label
 * to be uniformly distributed over the hypercube */
void set_sample_class(
		samples_t *samples, 
		size_t class, 
		int label, 
		size_t count);

/** Generates random binary samples in the unit square in the given dimension.
 *
 * The drand48(3) functions must have been seeded before invoking this method.
 *
 * @return A newly allocated group of samples
 **/
samples_t *random_samples(
                /** Number of samples */
		size_t count, 
		/** Number of positives samples.
		 * If positives is greater than or equal to count, 
		 * then all points will be positive. */ 
		size_t positives, 
                /** Dimension of the sample space */
		size_t dimension);


/** Returns a new hyperplane defined as a unit vector followed by a bias */
double *random_hyperplane(size_t dimension);

/** Find whether the given sample is on the positive or negative side 
 * of the given hyperplane 
 *
 * @return 1 if the sample is on the positive side, 0 otherwise */
int side(
		/** Locator of the sample of interest */
		sample_locator_t *, 
		/** Sample collection */
		samples_t *, 
		/** Array of size dimension + 1 containing first the 
		 * hyperplane coefficients w1, ... , wd, and then the 
		 * bias c */
		double *hyperplane,
		/** Precision around the hyperplane */
		double precision);

/** Returns the signed distance of the given sample from the given hyperplane.*/
double distance(
		/** Locator of the sample of interest */
		sample_locator_t *, 
		/** Sample collection */
		samples_t *, 
		/** Array of size dimension + 1 containing first the 
		 * hyperplane coefficients w1, ... , wd, and then the 
		 * bias c */
		double *hyperplane,
		/** Precision around the hyperplane */
		double precision);


/** Returns the violation of the given sample from the given hyperplane. 

    The violation is equal to the distance, except that for 
    positive samples it has been multiplied by -1. */
double sample_violation(
		/** Locator of the sample of interest */
		sample_locator_t *, 
		/** Sample collection */
		samples_t *, 
		/** Array of size dimension + 1 containing first the 
		 * hyperplane coefficients w1, ... , wd, and then the 
		 * bias c */
		double *hyperplane,
		/** Precision around the hyperplane */
		double precision);


/** Return the number of samples in the given class that 
 * are on the positive side of the given hyperplane. */
int side_cnt(
        /** Class identifier */
		int class, 
		/** Sample collection */
		samples_t *, 
		/** Array of size dimension + 1 containing first the 
		 * hyperplane coefficients w1, ... , wd, and then the 
		 * bias c */
		double *hyperplane,
		/** Precision around the hyperplane */
		double precision);


/* ---------------------- Sample clusters --------------------------- */

/** Descriptor of a set of clusters of samples */
typedef struct {
    /** Number of clusters */
    size_t cluster_cnt;
    /** Array with the number of points in each of the cluster_cnt clusters */
    size_t *count;
    /** Shift parameters for the clusters (as in random_point_affine) */
    double *shift;
    /** Side parameters for the clusters (as in random_point_affine) */
    double *side;
    /** Dimension of the sample space */
    size_t dimension;
} clusters_info_t;


/** Fills in a cluster information record for a single cluster
 * with zero shift and unit side 
 *
 * @return the cluster information record
 */
clusters_info_t *clusters_info_singleton(
  clusters_info_t *info, 
  size_t count, 
  size_t dimension);

/** @brief Delete the cluster descriptor.
 *
 * It is assumed that all the arrays within the cluster information
 * have been dynamically allocated.
 * No assumption is made about the allocation of the argument itself.
 *
 * @return the cluster descriptor */
clusters_info_t *delete_clusters_info(clusters_info_t *);

/** @return total number of samples across all counters */
size_t clusters_count(clusters_info_t *);

/** Generates random binary samples. Each sample class is described by the 
 * corresponding entry in the cluster information array.
 *
 * The drand48(3) functions must have been seeded before invoking this method.
 *
 * @return A newly allocated group of samples
 **/
samples_t *random_sample_clusters(
  /** Array of two cluster information records. 
   * The dimension of the samples is the largest of the dimension of the 
   * two records. */
  clusters_info_t *);


/* ---------------------- Read Samples ------------------------------ */
/** Allocate and read a single vector. */
double *read_vector(
    /** Input file */
    FILE *, 
    /** Vector dimension */
    size_t dimension);

/** Read all samples from the given class. */
void read_class(
    /** Input file */
    FILE *,
    /** Samples. 
     * count[class] is assumed to have been initialized to the
     * number of samples in the class.
     * The samples[class] field will contain the samples from the
     * file. */
    samples_t *samples, 
    /** Class to be read */
    int class);

/** Read all samples */
void read_classes(
    /** Input file */
    FILE *infile, 
    /** Samples. 
     * count[class] is assumed to have been initialized to the
     * number of samples in the class.
     * The samples field will contain the samples from the file. */
    samples_t *samples);

/** Allocate and read binary samples from the given file */
samples_t *read_binary_samples(FILE *);


/* ---------------------- Simplexes --------------------------------- */

/** Descriptor of a set of simplex clusters */
typedef struct {
  /** Total number of samples */
  size_t count;
  /** Number of positive samples */
  size_t positives; 
  /** Number of clusters among which the positive samples are partitioned
   * (currently supported: 1, 2) */
  size_t cluster_cnt;
  /** Dimension of the sample space */
  size_t dimension;
  /** Simplex side length */
  double side;
} simplex_info_t;

/** Generates random binary samples in the simplex in the given dimension.
 *
 * The drand48(3) functions must have been seeded before invoking this method.
 *
 * @return A newly allocated group of samples
 **/
samples_t *random_simplex_samples(simplex_info_t *);


/* --------------------- Parameters                 ---------------------- */

/** Heuristic method */
typedef
enum {
	/** Round the fractional variable's value to the 
	 * closest feasible integer value. */
	simple, 
	/** Round to the side of hyperplane */
	deep 
} iheur_method_t;


/** Problem instance parameters */
typedef struct {
	/** Problem name */
	char *name;
	/** Verbosity level (GLPK enum) */
	int verbosity;
	/** Precision threshold */
	double theta;
	/** Tolerance for positive samples */
	double epsilon_positive;
	/** Tolerance for negative samples */
	double epsilon_negative;
	/** Tolerance for the precision constraint */
	double epsilon_precision;
	/** Lagrangian multiplier of the precision constraint */
	double lambda;
	/** Violation bound: 
     *  either 0 for unbounded variables (GLP_FR)
     *  or 1 for variable with lower bound (GLP_LO) */
	int violation_type;
	/** Target value of the branching variable */
	double branch_target;
	/** Method to use to round to an integer solution */
	iheur_method_t iheur_method;
    /** Number of initial random trials to find an integer solution */
    int rnd_trials;
    /** Number of random trials at each invocation of iheur */
    int rnd_trials_cont;
} params_t;


/** Return a new parameter set with default values */
params_t *params_default();


/* ---------------------------- Solution Data -------------------------- */

/** Data updated by the callback during the solution of the program */
typedef struct {
	/** Number of elements in rank that are significant */
	size_t rank_significant;
	/** Sorted list of indexes in order of branching priority */
	int *rank;
    /** Last node that branched */
    int branching_node;
    /** GLPK indexes sorted by hyperplane violation */
    int *violation_index;
    /** Best integer solution found by iheur */
    double *integer_solution;
    /** Objective value achieved by the best integer solution */
    double intopt;
} solution_data_t;


/** Allocate and return a new solution data of the given size with no
 * significant element. */
solution_data_t *solution_data_init(size_t);

/** Deallocate and return the elements in the solution data, 
 * but not the solution data. */
solution_data_t *delete_solution_data(solution_data_t *);

/** Append the given index as the last significant element of the solution
 * data */
solution_data_t *append_data(solution_data_t *, int index);

/** Update the integer solution and its objective value with the provided
 * arguments */
void update_solution(
        solution_data_t *,
        /** New solution vector */
        double *solution, 
        /** Objective value of the new solution vector */
        double value,
        /** Indexes sorted by violation */
        int *violation_index);

/* ---------------------------- Environment ----------------------------- */

/** Environment */
typedef struct {
	/** Sample set */
	samples_t *samples;
	/** Problem instance parameters */
	params_t *params;
	/** Solution data */
	solution_data_t *solution_data;
} env_t;

/** Deallocate the samples and the parameters.
 *
 * It assumes that samples and parameters were dynamically allocated */
env_t *delete_env(env_t *);


/* --------------------------- Random number generation ------------------ */

/** Return the square of the length of the 
 * d-dimensional vector given as argument */
double length_squared(
    /** Vector dimention */
    size_t d, 
    /** Vector */
    double *w);

/** Generates two normal random variables and stores the result in the
 * vector passed as an argument. */
void normal_pair(double *w);

/** Multiply a vector by a scalar. */
double multiply_scalar(
    /** Scalar factor */
    double a, 
    /** Size of the vector to be multiplied */
    size_t d, 
    /** Vector to be multiplied */
    double *w);

/** Generate a random point in the unit hypercube of the given dimension.
 * It returns a newly allocated vector. */
double *random_point(size_t dimension);

/** Generate a random point in the hypercube of the give dimension 
 * located from [shift .... shift]^T to [shift+side ... shift+side]^T 
 * (effectively translating the unit hypercube so that the lower left corner
 * is shift away in every direction and scaling it so that each side has 
 * the given length.)
 */
double *random_point_affine(size_t dimension, double shift, double side);

/** Generates a random unit d-dimensional vector. */
void random_unit_vector(
    /** Vector size */
    size_t d, 
    /** Random unit d-dimensional vector, 
     *  stores the result of the computation */
    double *w);

/** Generates a random vector in the volume under the simplex of the given
 * dimension. It returns a newly allocated vector.
 */
double *random_simplex_point(
    /** Simplex side */
    double side,
    /** Vector size */
    size_t dimension);

/** Copy a hyperplane of the given dimension into another one.
 * A hyperplane is defined as dimension+1 vector in which the first
 * dimension component contain a vector perpendicular to the hyperplane
 * and the last component contains the intercept. */
void copy_hyperplane(size_t dimension, double *dest, double *src);

/** Generates a random hyperplane through a random point on the unit 
 * hypersquare. The orthogonal vector part of the hyperplane as unit size.
   Returns a newly allocated hyperplane.  */
double *random_hyperplane(size_t dimension);

/** Generates rnd_trials hyperplanes and returns 
 * one that achieves the highest value of the objective function. 
   The returned vector has been dynamically allocated. 
   If rnd_trials is zero, then NULL is returned. */
double *best_random_hyperplane(
  /** A Boolean variable denoting whether this is the initial (1) or
    * continuing (0) random hyperplane */
  int initial, 
  env_t *);

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
