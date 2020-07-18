/* --------------------------- Helper methods ---------------------------- */
#define CALLOC(nmemb, type) (type *) calloc(nmemb, sizeof(type))

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
double multiply_basic(
    /** Scalar factor */
    double a, 
    /** Size of the vector to be multiplied */
    size_t d, 
    /** Vector to be multiplied */
    double *w);

/** Generates a random unit d-dimensional vector. */
void random_unit_vector(
    /** Vector size */
    size_t d, 
    /** Random unit d-dimensional vector, 
     * stores the result of the computation */
    double *w);


