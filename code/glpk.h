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
     If the hyperplane is NULL, the solution is left unchanged. 
     Solution indexing is in GLPK style, with solution[0] unused. */
    double *solution,
    env_t *);

/** Same as hyperplane_to_solution, but takes only the parameters and samples
 * rather than the full environment. */
double hyperplane_to_solution_parts(
  double *, double *, params_t *, samples_t *);

/** @return reach of the given integer solution for the given samples */
unsigned int reach(double *solution, samples_t *);

/** @return number of false positives in the given integer solution 
 *for the given samples */
unsigned int false_positives(double *solution, samples_t *);

/** @return precision of the given integer solution for the given samples */
double precision(double *solution, samples_t *);

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
