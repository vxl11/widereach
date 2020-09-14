#include <stdlib.h>
#include <stdio.h>
#include <glpk.h>

// #define EXPERIMENTAL

/** Widereach Classification */

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


/* ---------------------- Read Samples ------------------------------ */
/** Read a single vector. */
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


/* ---------------------- Sparse Vectors ---------------------------- */

/** Sparse vector in GLPK format */
typedef struct {
	/** Length of the significant part of ind and val */
	int len;
	/** Additional space at the end of ind and val */
	size_t extra;
	/** Index vector */
	int *ind;
	/** Value vector */
	double *val;
} sparse_vector_t;


/** Allocates a new sparse vector of zero len and the given extras. */
sparse_vector_t *sparse_vector_blank(size_t extra);

/** Deallocates the ind and val vectors in a sparse vector, and 
 * sets the length and extra to zero.
 * It assumes that ind and val were dynamically allocated.
 * 
 * @return the sparse vector */
sparse_vector_t *delete_sparse_vector(sparse_vector_t *);

/** Convert an array of doubles into a new sparse vector. */
sparse_vector_t *to_sparse(
                /** Length of the double array */
		size_t nmemb, 
		double *,
		/** Extra len to be left at the end of the sparse vector */
		size_t extra);

/** Multiple a sparse vector by a scalar.
 *
 * @return The sparse vector after multiplication. */
sparse_vector_t *multiply(sparse_vector_t *, double);

/** Appends an element at the end of the sparse vector. 
 *
 * @return -1 if no extra space is available 
 * (and thus the item was not added) or 
 * the amount of extra space after the insertion. */
int append(sparse_vector_t *, 
	/** Index of the new element.
        It is assumed that this index has not been previously defined. */
	int ind, 
	/** Value to be inserted */
	double val);


/** Creates and returns a sparse vector obtained by retaining only the 
    elements that satisfy the given filter. It assumes that the vector
    is not null. */
sparse_vector_t *filter(
    /** Sparse vector to filter */
    sparse_vector_t *v, 
    /** Predicate that denotes which elements of the sparse vector will be
     * retained. Its argument must be a sparse vector, an index into the 
     * sparse vector, and a pointer to optional information that can be used
     * by the predicate. */
    int (*predicate)(sparse_vector_t *, int, void *),
    /** Additional optional information for potential use by the predicate */
    void *info);

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
	/** Violation bound (either GLP_FR or GLP_LO) */
	int violation_type;
	/** Target value of the branching variable */
	int branch_target;
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

/** Generates a random unit d-dimensional vector. */
void random_unit_vector(
    /** Vector size */
    size_t d, 
    /** Random unit d-dimensional vector, 
     *  stores the result of the computation */
    double *w);

/** Copy a hyperplane of the given dimension into another one.
 * A hyperplane is defined as dimension+1 vector in which the first
 * dimension component contain a vector perpendicular to the hyperplane
 * and the last component contains the intercept. */
void copy_hyperplane(size_t dimension, double *dest, double *src);

/** Generates a random hyperplane through a random point on the unit 
 * hypersquare. The orthogonal vector part of the hyperplane as unit size.
   Returns a newly allocated hyperplane.  */
double *random_hyperplane(size_t dimension);

/** Generates env->rnd_trials hyperplanes and returns 
 * one that achieves the highest value of the objective function. 
   The returned vector has been dynamically allocated. */
double *best_random_hyperplane(
    /** A Boolean variable denoting whether this is the initial (1) or
     * continuing (0) random hyperplane */
    int initial,
    env_t *);

/* ------------------------ Indexing ---------------------------------- */

/*
 * Maps addressing in a binary classification problem instance (env) into 
 * indexing in a GLPK problem (glp_prob) according to the follwoing convetions.
 *
 * *Variables (columns)*
 * - 1 to dimension w
 * - dimension+1 c
 * - dimension+2 to dimension+positives+1 xi
 * - dimension+positives+2 to dimension+samples+1 yj
 * - dimension+samples+2 V
 *
 * *Constrains (rows)*
 * - 1 to positives xi
 * - positives+1 to samples yj
 * - samples+1 V 
 */

/** Return the index corresponding to a sample */
int idx(
	/** 0: return column index, 1: row index */
	int direction, 
	/** Sample class */
    int class, 
	/** Sample index within class */
	size_t sample_index, 
	/** Sample collection */
	samples_t *);


/** Maps the index of a decision variable into a sample locator.
 *
 * In case of error (e.g., the index does not belong to a sample)
 * it returns class = -1. */
sample_locator_t *locator(
		/** Decision variable index */
		int index, 
		/** Sample set */
		samples_t *);

/** Appends an element at the end of the sparse vector. 
 *
 * @return -1 if no extra space is available 
 * (and thus the item was not added) or 
 * the amount of extra space after the insertion. */
int append_locator(sparse_vector_t *, 
	/** Locator of the new element.
        It is assumed that this index has not been previously defined. */
	sample_locator_t *, 
	/** Value to be inserted */
	double val,
    samples_t*);

/** Prints the sample referenced by the locator */
void print_sample(sample_locator_t, samples_t *);

/** Return the minimum or maximum index corresponding to an index of a 
 * given class */
int idx_extreme(
	/** 0: return column index, 1: row index */
	int direction, 
	/** Sample class */
    int class, 
	/** 0: return the minimum index, 1: max */
	int extreme,
	/** Sample collection */
	samples_t *);

/** Return the index corresponding to the violation variable */
int violation_idx(
	/** 0: return column index, 1: row index */
	int direction, 
	/** Sample collection */
	samples_t *);

/** Return the index corresponding to the violation variable */
int violation_idx(
	/** 0: return column index, 1: row index */
	int direction, 
	/** Sample collection */
	samples_t *);

/** Compute the label of the given variable */
int index_label(int i, samples_t *);

/** Fills in a sparse vector in which all decision variables of the given
 * class are assigned the given coefficient 
 *
 * @return the sparse vector */
sparse_vector_t *cover_row(
		/** The sparse vector to be filled */
                sparse_vector_t *,
		/** Class of the decision variables */
                size_t class,
		/** Coefficient of the decision variables */
                double coef,
		/** Sample set */
                samples_t *);

/** Returns a new sparse vector representing the right hand side of the 
 * precision constraint */
sparse_vector_t *precision_row(
		/** Sample set */
		samples_t *, 
		/** Precision threshold */
		double theta);

/** Finds and return the class of the given GLPK decision variable */
int index_to_class(int idx, samples_t *);

/** Create a new solution in which all elements are 1/2 */
double *blank_solution(samples_t *);

/** Convert a hyperplane into an vector of sample distances from the 
 * hyperplane. 
    WARNING: currently unsupported and utested */
void hyperplane_to_distance(
    /** The hyperplane around which the integer solution is to be computed */
    double *hyperplane, 
    /** The distance vector. If the hyperplane is NULL, it is left unchanged. */
    double *distance,
    env_t *);

/** Convert a hyperplane into an integer solution vector 
 * 
 * @return the objective value achieved by the integer solution, or 
            -DBL_MAX if the hyperplane is NULL. */
double hyperplane_to_solution(
    /** The hyperplane around which the integer solution is to be computed */
    double *hyperplane, 
    /** The solution vector, or NULL if the solution is not needed.
     Note that if solution is not null and solution[i] is integer,
     its value is not replaced by the hyperplane side. 
     If the hyperplane is NULL, the solution is left unchanged. */
    double *solution,
    env_t *);

/** Returns an array of GLPK indexes in increasing order of violation
 * from the given hyperplane */
int *sorted_by_violation(double *hyperplane, env_t *);


/* ----------------------------- Paths ---------------------------------- */
/** Check whether the arrays are consistent along the given indexes */
int are_consistent(
    /** An array of indexes along which the next two arrays are supposed
     * to be consistent. The comparison stops at the first zero index. */
    int *index, 
    double *, double *);

/** Similar to are_consistent, except that the first array and the index
 * vectors are replaced by a sparse vector */
int is_path_consistent(sparse_vector_t *, double *);

/** Similar to is_path_consistent, except that it returns the number of 
 * entries that are consistent in the two arguments. */
int consistency_count(sparse_vector_t *, double *);

/* ----------------------------- GLPK ----------------------------------- */

/** Return GLPK solver parameters initialized from the given parameters */
glp_iocp *iocp(const env_t *);

/** Return an GLPK problem intialized from the given environment */
glp_prob *milp(const env_t *);

/** Define the characteristics of the hyperplane variables in 
 *  the given GLPK problem. */
glp_prob *add_hyperplane(glp_prob *, size_t dimension);


/** Data recording the branching decision */
typedef struct {
    /** Flag denoting whether the branch data has been initialized 
        (exit of ibranch) */
    int initialized;
    /** Flag denoting whether the branch data has been pre-initialized
     *  (entry of ibranch) */
    int preinitialized;
    /** Branching variable chosen at this node */
	int branching_variable;
	/** Recommended direction for the branching variable, as a GLP enum */
	int direction;
    /** Number of variables that have been fixed to an integer value
     * in the two classes */
    int class_cnt[2];
    /** Value of the parent's branching variable at this node or,
     * whether the node was branched in the positive or negative direction */
    double branching_value;
    /** A boolean variable to denote whether the node was branched in the
     * primary direction. The primary direction is defined as +1 for positive
     * samples and 0 for negative samples */
    int primary_direction;
    /** Number of variables that have been fixed to the primary
     * integer value in the two classes */
    int directional_cnt[2];
    /** Sum of integer infeasibilities */
    double ii_sum;
    /** Objective value of an integer solution with which this node was 
        evaluated for consistency. */
    double intobj;
    /** A boolean denoting whether the node was consistent with the integer
     * solution */
    int is_consistent;
    /** The index in the violation array where the search terminated
     * last time branching was invoked */
    int violation_rank;
} branch_data_t;

/** Return the branch data of the given node, or NULL if the node has no data */
branch_data_t *branch_data(int node, glp_tree *);

/** Data recoding the identities of the children of the current node */
typedef struct {
    /** Counter of how many children are known */
    int child_cnt;
    /** Node identifiers of the children. If both elements are present,
     * the first denotes the downbrach, the second the upbranch */
    int child[2];
} child_data_t;

/** Add the given node as a child */
int add_child(child_data_t *, int node);

/** Return the direction of the given node as a child of the one
 * whose child data is given:
 * - 0: down branch
 * - 1: up branch
 * - -1: the direction cannot be determined (for example, the node is not a 
 * child, or less than two children are on record for this node) */
int child_direction(child_data_t *, int node);

/** Data regarding lazy constraints and cutting planes. */
typedef struct {
    /** A count of valid inequalities introduced at this node or at one of 
     * its ancestors */
    int inequality_cnt;
    /** Right hand side of the next cutting plane */
    sparse_vector_t *rhs;
    /** Left hand side of the next cutting plane */
    double lhs;
} cuts_data_t;

/** Additional data to be stored in each tree node */
typedef struct {
	/** Flag denoting whether the top level node data has been initialized
     * (branch data has only been initialized to the default state */
	int initialized;
    /** Branching data */
    branch_data_t branch_data;
    /** Node indexes of the two children on the down- and up-branch */
    child_data_t child_data;
    /** Data regarding lazy constraints and cutting planes */
    cuts_data_t cuts_data;
    /** A Boolean denoting 
     * whether iheur has already been invoked on this node */
    int iheur;
} node_data_t;

/** Initialize and return the node data, except branch data,
 * at the given node. If initialize_data has already been invoked
 for this node, it does nothing. */
node_data_t *initialize_data(int node, glp_tree *, samples_t *);

/** Return the node data of the given node, or 
 * NULL if the node is the root 
 
    @pre node is a valid non-zero node identifier */
node_data_t *parent_data(int node, glp_tree *);

/** Computes and returns a boolean denoting whether the node was branched
 * in the primary direction. If unknown, return -1. */
int is_direction_primary(
    /** Index of the current node */
    int node, 
    /** A Boolean to determine whetehr the value of the relaxation
     * at the current node (which may or may not be the argument)
     * should be used */
    int relaxation,
    glp_tree *, samples_t *); 

/** Traverse the tree from the current node to the root, while printing
 * node data for all initialized nodes */
void traverse(
		/** Current solution, to be printed during the traversal.
		 * If NULL, the solution is not printed. */
		double *solution, 
		glp_tree *, env_t *);

/** Return the set of variables along which it was branched from the root
 * to this node. The returned value is ordered from the leaf to the root
 * and terminates when a zero is reached. */
int *branching_variables(int node, glp_tree *, samples_t *);

/** Returns the path from the given node to the root as a sparse vector
 * containing the branching variables along the paths and their values.
    It returns NULL if the node is invalid or if its branch data have
    not been preinitialized. */
sparse_vector_t *path(int node, glp_tree *);


/** Node signature: parameters that affect the comparison between nodes */
typedef struct {
    /** Level of the node in the branch-and-cut tree */
    int level;
    /** Denote whether the node branched 
     * in the primary direction (1) or not (0), where 
     the primary direction is defined as 1 for positive samples and 
     0 for negative samples */
    int primary;
    /** Local bound at the current node */
    double bound;
    /** Sum of integer infeasibilities in the parent's node */
    double ii_sum;
    /** Sequence number of the node: higher sequence numbers were generated
     * later */
    int seqno;
    /** A Boolean denoting whether this node is the primary child of the
     * last branching node */
    int last_primary;
    /** The directional count of the negative samples at the parent node */
    int directional;
} node_signature_t;

/** Set signature fields to the given values */
void set_signature(
    /** Signature whose values need to be set */
    node_signature_t *, 
    /** Level new value */
    int level,
    /** A boolean value denoting whether the node branched in the primary 
     * direction */
    int primary,
    /** Local bound */
    double bound,
    /** Parent's sum of integer infeasibilities */
    double ii_sum,
    /** Sequential number new value */
    int seqno,
    /** A flag to denote whether this node is the primary of the last */
    int last_primary,
    /** The directional count of negative samples at the parent */
    int directional);

/** Copy a signature */
void copy_signature(
    /** Destination signature */
    node_signature_t *dest, 
    /** Source signature */
    const node_signature_t *src);

/** Compare two signatures. 
 * 
 * @return Comparison value, defined as in qsort(3). */
int compare_signature(const node_signature_t *, const node_signature_t *);


/** Return a new array containing the value of the decision variables in
 * the solution of the given problem. */
double *solution_values(int node, glp_prob *);


/* -------------------------- Obstruction ------------------------------- */
/** Returns whether the given target is obstructed from the sources by the
 * obstruction */
int is_obstructed(
    /** Sample to check for reachability from the sources */
    sample_locator_t *target, 
    /** Source samples */
    sample_locator_t *source,
    /** Number of obstructing samples */
    size_t obstruction_cnt,
    /** Obstructing samples */
    sample_locator_t **obstruction, 
    samples_t *);


/* -------------------------- Consistency ------------------------------- */
/** Initialize a consistency problem of the given dimension */
glp_prob *init_consistency_problem(size_t dimension);

/** Append a sample to the given consistency problem.
 * 
 * @return The consistency problem. */
glp_prob *append_sample(
    /** Consistency problem */
    glp_prob *p, 
    /** Locator of the sample to append */
    sample_locator_t *loc, 
    env_t *);

/** Returns whether the given sample is consistent with the rest of the 
 * consistency problem. The consistency problem is not modified by this call. */
int is_interdicted(
    /** Consistency problem */
    glp_prob *p, 
    /** Locator of the sample to test */
    sample_locator_t *loc, 
    env_t *);


/* -------------------------- Cutting planes ---------------------------- */
/** Creates a cutting plane from a path of constraining samples and 
 * a vector of interdicted samples */
void interdiction_cut(
    /** Samples that were branched in the primary direction */
    sparse_vector_t *pth, 
    /** Samples that are interdicted by the path */
    sparse_vector_t *interdicted,
    /** Right hand side of the resulting cutting plane */
    sparse_vector_t **rhs,
    /** Left hand side of the resulting cutting plane */
    double *lhs);

/* -------------------------- Callback ---------------------------------- */

/** Callback entry point */
void callback(glp_tree *, void *);

/** Find an integer solution compatible with 
 * the current solution of the relaxation */
void iheur(glp_tree *, env_t *);

/** Decide on a variable to branch. */
void ibranch(glp_tree *, env_t *);

/** Select the next problem to expand. */
void iselect(glp_tree *, env_t *);

/** Add lazy constraints */
void irowgen(glp_tree *, env_t *);

/** Add cutting planes */
void icutgen(glp_tree *, env_t *);

/** Collect information when a better integer solution is found */
void ibingo(glp_tree *, env_t *);
