#include "widereach.h"
#include "helper.h"

void add_variables(glp_prob *p, size_t var_cnt, size_t obstruction_cnt) {
    glp_add_cols(p, var_cnt);
    for (size_t i = 1; i <= var_cnt; i++) {
        glp_set_col_kind(p, (int) i, GLP_CV);
		glp_set_col_bnds(p, (int) i, GLP_LO, 0., 0.);
        glp_set_obj_coef(p, (int) i, i <= obstruction_cnt);
    }
}

int *ind_all(size_t var_cnt) {
    int *ind = CALLOC(var_cnt, int);
    for (size_t i = 1; i <= var_cnt; i++) {
            ind[i] = i;
    }
    return ind;
}

double index_to_coordinate(
        size_t i, 
        size_t j,
        sample_locator_t **source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    sample_locator_t *loc;
    if (i <= obstruction_cnt) { 
        loc = obstruction[i - 1];
    } else {
        loc = source[i - obstruction_cnt - 1];
    }
    return samples->samples[loc->class][loc->index][j];
}

void add_target_constraints(
        glp_prob *p,
        sample_locator_t *target, 
        size_t var_cnt,
        sample_locator_t **source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    int *ind = ind_all(var_cnt);
    double *val = CALLOC(var_cnt, double);
    size_t dimension = samples->dimension;
    for (size_t j = 0; j < dimension; j++) {
        for (size_t i = 1; i <= var_cnt; i++) {
            val[i] = index_to_coordinate(i, j, 
                                         source, 
                                         obstruction_cnt, obstruction, 
                                         samples);
        }
        int constraint_idx = 1 + (int) j;
        glp_set_mat_row(p, constraint_idx, (int) var_cnt, ind, val);
        double target_coordinate = 
            samples->samples[target->class][target->index][j];
        glp_set_row_bnds(p, constraint_idx, GLP_FX,
                         target_coordinate, target_coordinate);
    }
    free(val);
    free(ind);
}

void add_convexity_constraint(
        glp_prob *p, 
        size_t row_cnt, 
        size_t source_cnt, 
        size_t obstruction_cnt) {
    int *ind = CALLOC(source_cnt, int);
    double *val = CALLOC(source_cnt, double);
    for (size_t i = 1; i <= source_cnt; i++) {
        ind[i] = i + obstruction_cnt;
        val[i] = 1.;
    }
    glp_set_mat_row(p, (int) row_cnt, (int) obstruction_cnt, ind, val);
    glp_set_row_bnds(p, (int) row_cnt, GLP_FX, 1., 1.);
    free(val);
    free(ind);
}


glp_prob *obstruction_lp(
        sample_locator_t *target, 
        size_t source_cnt,
        sample_locator_t **source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    glp_prob *p = glp_create_prob();
    glp_set_obj_dir(p, GLP_MAX);
            
    size_t var_cnt = source_cnt + obstruction_cnt;
    add_variables(p, var_cnt, obstruction_cnt);
            
    size_t row_cnt = samples->dimension + 1;
    glp_add_rows(p, row_cnt);
    add_target_constraints(p,
                           target, 
                           var_cnt, source, obstruction_cnt, obstruction, 
                           samples);
    add_convexity_constraint(p, row_cnt, source_cnt, obstruction_cnt);
    
    return p;
}


int is_obstructed(
        sample_locator_t *target, 
        size_t source_cnt,
        sample_locator_t **source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    glp_prob *p = obstruction_lp(target, 
                                 source_cnt, source, 
                                 obstruction_cnt, obstruction, 
                                 samples);
    // glp_write_lp(p, NULL, "tmp.lp");
    int status = glp_simplex(p, NULL);
    int solvable = !status && glp_get_obj_val(p) >= 1.;
    glp_delete_prob(p);
    return solvable; 
}
