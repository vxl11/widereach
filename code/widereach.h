#include <stdlib.h>
#include <glpk.h>

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


/** Sample locator: specifies a sample in a sample group but call and
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


/* ----------------------------- GLPK ----------------------------------- */

/** Return GLPK solver parameters initialized from the given parameters */
glp_iocp *iocp(const env_t *);

/** Return an GLPK problem intialized from the given environment */
glp_prob *milp(const env_t *);


/** Data recording the branching decision */
typedef struct {
    /** Branching variable chosen at this node */
	int branching_variable;
	/** Recommended direction for the branching variable */
	int direction;
    /** Number of variables that have been fixed to an integer value
     * in the two classes */
    int class_cnt[2];
    /** Sum of integer infeasibilities */
    double ii_sum;
} branch_data_t;

/** Additional data to be stored in each tree node */
typedef struct {
	/** Flag denoting whether the node data has been initialized:
     - 0 not initialized
     - 1 initialized except branch data
     - 2 initialized, including branch data */
	int initialized;
    /** Branching data */
    branch_data_t branch_data;
    /** A boolean variable to denote whether the node was branched in the
     * primary direction. The primary direction is defined as +1 for positive
     * samples and 0 for negative samples */
    int primary_direction;
    /** Number of variables that have been fixed to the primary
     * integer value in the two classes */
    int directional_cnt[2];
    /* Node indexes of the two children on the down- and up-branch
        (currently unused) */
    // int branch[2];
} node_data_t;

/** Initialize and return the node data at the given node. */
node_data_t *initialize_data(int node, glp_tree *, samples_t *);

/** Return the node data of the given node, or 
 * NULL if the node is the root 
 
    @pre node is a valid non-zero node identifier */
node_data_t *parent_data(int node, glp_tree *);

/** Computes and returns a boolean denoting whether the node was branched
 * in the primary direction. */
int is_direction_primary(
    /** Index of the current node */
    int node, 
    glp_tree *, samples_t *); 

/** Traverse the tree from the current node to the root, while printing
 * node data for all initialized nodes */
void traverse(
		/** Current solution, to be printed during the traversal.
		 * If NULL, the solution is not printed. */
		double *solution, 
		glp_tree *, env_t *);


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
    int seqno);

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

/** Calculate and set the node data for the current node */
void irowgen(glp_tree *, env_t *);

/** Collect information when a better integer solution is found */
void ibingo(glp_tree *, env_t *);
