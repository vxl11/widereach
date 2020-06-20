/** Widereach Classification */

/* --------------------------- Samples -------------------------------- */
struct sample_t {
	/** Sample direction: +1 positive, -1 negative */
	int direction;
	/** Dimension of the sample space, and size of the values array */
	int dimension;
	/** Values of the sample coordinates in the given dimensions */
	double *values;
};

/** @brief Delete the given sample array.
 *
 * It is assume that the sample array and the values of each sample
 * have been dynamically allocated */
void delete_samples(size_t, struct sample_t *);

/** Generates random samples in the unit square in the given dimensional 
 * space.
 *
 * @param n Number of samples
 * @param Positives number of positives samples (must be no more than n)
 * @param dimension Dimension of the sample space
 * @return A newly allocated array of samples
 **/
struct sample_t *random_samples(size_t n, int positives, int dimension);


/* --------------------------- Helper methods ---------------------------- */
#define CALLOC(nmemb, type) (type *) calloc(nmemb, sizeof(type))
