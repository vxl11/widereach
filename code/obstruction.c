#include "widereach.h"
#include "helper.h"

void add_variables(glp_prob *p, size_t obstruction_cnt) {
    glp_add_cols(p, obstruction_cnt);
    for (size_t i = 1; i <= obstruction_cnt; i++) {
        int idx = (int) i;
        glp_set_col_kind(p, idx, GLP_CV);
		glp_set_col_bnds(p, idx, GLP_LO, 0., 0.);
        glp_set_obj_coef(p, idx, i <= obstruction_cnt);
    }
}

int *ind_all(size_t obstruction_cnt) {
    int *ind = CALLOC(obstruction_cnt + 1, int);
    for (size_t i = 1; i <= obstruction_cnt; i++) {
            ind[i] = i;
    }
    return ind;
}

double index_to_coordinate(
        size_t coordinate_idx, 
        sample_locator_t *loc, 
        samples_t *samples) {
    return samples->samples[loc->class][loc->index][coordinate_idx];
}

void add_target_constraints(
        glp_prob *p,
        sample_locator_t *target, 
        sample_locator_t *source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    int *ind = ind_all(obstruction_cnt);
    double *val = CALLOC(obstruction_cnt + 1, double);
    size_t dimension = samples->dimension;
    for (size_t coordinate_idx = 0; 
         coordinate_idx < dimension; 
         coordinate_idx++) {
        double source_offset = index_to_coordinate(coordinate_idx, 
                                                   source,
                                                   samples);
        for (size_t obstruction_idx = 1; 
             obstruction_idx <= obstruction_cnt; 
             obstruction_idx++) {
            val[obstruction_idx] = 
                index_to_coordinate(coordinate_idx, 
                                    obstruction[obstruction_idx - 1], 
                                    samples) -
                source_offset;
        }
        int constraint_idx = 1 + (int) coordinate_idx;
        glp_set_mat_row(p, constraint_idx, (int) obstruction_cnt, ind, val);
        double target_coordinate = 
            index_to_coordinate(coordinate_idx, target, samples) - 
            source_offset;
        glp_set_row_bnds(p, constraint_idx, GLP_FX,
                         target_coordinate, target_coordinate);
    }
    free(val);
    free(ind);
}

glp_prob *obstruction_lp(
        sample_locator_t *target, 
        sample_locator_t *source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    glp_prob *p = glp_create_prob();
    glp_set_obj_dir(p, GLP_MAX);
            
    add_variables(p, obstruction_cnt);
            
    glp_add_rows(p, (int) samples->dimension + 1);
    add_target_constraints(p,
                           target, 
                           source, 
                           obstruction_cnt, 
                           obstruction, 
                           samples);
    
    return p;
}


int is_obstructed(
        sample_locator_t *target, 
        sample_locator_t *source,
        size_t obstruction_cnt,
        sample_locator_t **obstruction, 
        samples_t *samples) {
    glp_prob *p = obstruction_lp(target, 
                                 source, 
                                 obstruction_cnt, obstruction, 
                                 samples);
    // glp_write_lp(p, NULL, "tmp.lp");
    int status = glp_simplex(p, NULL);
    int solvable = 
        !status && 
        glp_get_status(p) == GLP_OPT && 
        glp_get_obj_val(p) >= 1.;
    glp_delete_prob(p);
    return solvable; 
}
