/** Widereach Classification */

/* --------------------------- Samples -------------------------------- */
/** A single sample */
struct sample_t {
	/** Sample direction: +1 positive, -1 negative */
	int direction;
	/** Values of the sample coordinates in the given dimensions */
	double *values;
};


/** An array of samples */
struct samples_t {
	/** Number of samples */
	size_t count;
	/** Dimension of the sample space, and 
	 * size of the values array of all samples in the array */
	size_t dimension;
	/** Sample array */
	struct sample_t *samples;
};

/** @brief Delete the given sample array.
 *
 * It is assume that the sample array and the values of each sample
 * have been dynamically allocated */
void delete_samples(struct samples_t *);

/** Generates random samples in the unit square in the given dimensional 
 * space.
 *
 * The drand48(3) functions must have been seeded before invoking this method.
 *
 * @return A newly allocated array of samples
 **/
struct samples_t *random_samples(
                /** Number of samples */
		size_t count, 
		/** Number of positives samples.
		 * If positives is greater than count, then all points 
		 * will be positive. */ 
		size_t positives, 
                /** Dimension of the sample space */
		size_t dimension);


/* --------------------------- Helper methods ---------------------------- */
#define CALLOC(nmemb, type) (type *) calloc(nmemb, sizeof(type))
