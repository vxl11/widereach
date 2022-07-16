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
