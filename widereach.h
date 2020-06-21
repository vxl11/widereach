#include <stdlib.h>
#include <glpk.h>

/** Widereach Classification */

/* --------------------------- Samples -------------------------------- */

/* An group of samples */
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

/** @brief Delete the element within the given sample array.
 *
 * It is assumed that all the arrays within the sample
 * have been dynamically allocated.
 * No assumption is made about the allocation of the argument itself.
 *
 * @return the samples */
samples_t *delete_samples(samples_t *);


/** Checks whether the given samples are binary: it contains two classes,
 * the first of which labelled -1 and the other +1 */
int is_binary(samples_t *);


/** Return the total number of samples */
size_t samples_total(samples_t *);

/** Return the number of positives samples in a binary sample set */
size_t positives(samples_t *);

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


/* --------------------- Parameters and Environment ---------------------- */

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
} params_t;

/** Environment */
typedef struct {
	/** Sample set */
	samples_t *samples;
	/** Problem instance parameters */
	params_t *params;
} env_t;

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

/** Returns a new sparse vector representing the right hand side of the 
 * precision constraint */
sparse_vector_t *precision_row(
		/** Sample set */
		samples_t *, 
		/** Precision threshold */
		double theta);


/* ----------------------------- GLPK ----------------------------------- */

/** Return GLPK solver parameters initialized from the given parameters */
glp_iocp *iocp(params_t *);
