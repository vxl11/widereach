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
	int len;
	int *ind;
	double *val
} sparse_vector_t;

/** Deallocates the ind and val vectors in a sparse vector.
 * It assumes that ind and val were dynamically allocated.
 * 
 * @return the sparse vector */
void delete_sparse_vector(sparse_vector_t *);

/** Convert an array of doubles into a new sparse vector */
sparse_vector_t *to_sparse(double *);
