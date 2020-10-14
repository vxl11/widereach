#include "widereach.h"

void add_interdiction(glp_tree *t) {
    int curr_node = glp_ios_curr_node(t);
    node_data_t *data = glp_ios_node_data(t, curr_node);
    cuts_data_t *cuts_data = &(data->cuts_data);
    sparse_vector_t *rhs = cuts_data->rhs;
    if (cuts_data->rhs != NULL) {
        /* glp_ios_add_row(t, NULL, 0, 0, 
                        rhs->len, rhs->ind, rhs->val, GLP_UP, cuts_data->lhs); */
        free(delete_sparse_vector(rhs));
        cuts_data->rhs = NULL;
    }
}

void icutgen(glp_tree *t, env_t *env) {
    // glp_printf("ICUTGEN\n");
    add_interdiction(t);
}
