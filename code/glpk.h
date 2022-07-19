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
    /** Flag denoting whether the cuts data has been initialized */
    int initialized;
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
     * (branch data has only been initialized to the default state) */
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
 * the solution of the given problem, 
 * indexed from 1 to the number of variables. 
 * The 0th entry is unused.
 */
double *solution_values(glp_prob *);

/** Return a new array containing the value of the decision variables in
 * the MIP solution of the given problem.
 * The format of the return value is the same as in solution_values.
 */
double *solution_values_mip(glp_prob *);


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
    /** A Boolean flag denoting whether the cuts data has been initialized */
    int *initalized,
    /** Right hand side of the resulting cutting plane. 
     * The argument *rhs must be null. */
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
