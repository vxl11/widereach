#include <stdlib.h>
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

#include "widereach.h"
#include "helper.h"

int init_samples() {
	srand48(20200620);
	return 0;
}

extern double **random_points(size_t count, size_t dimension);
extern void set_sample_class(samples_t *samples, size_t class, int label, 
                             size_t count);


int test_accumulator(
    samples_t *samples, 
    sample_locator_t locator, 
    void *accumulation, 
    void *aux) {
  double *result = (double *) accumulation;
  *result += samples->samples[locator.class][locator.index][0];
  return 0;
}

void test_samples() {
  // Test random_points
    double **points = random_points(3, 2);
    for (size_t i = 0; i < 3; i++) {
      for (size_t j = 0; j < 2; j++) {
          CU_ASSERT(points[i][j] >= 0.);
          CU_ASSERT(points[i][j] <= 1.);
      }
      free(points[i]);
    }
    free(points);
    
    // Test set_sample_class
    samples_t *samples = CALLOC(1, samples_t);
    samples->dimension = 2;
	samples->class_cnt = 1;
	samples->label = CALLOC(1, int);
	samples->count = CALLOC(1, size_t);
	samples->samples = CALLOC(1, double **);
    set_sample_class(samples, 0, -1, 2); 
    CU_ASSERT_EQUAL(samples->label[0], -1);
    CU_ASSERT_EQUAL(samples->count[0], 2);
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            CU_ASSERT(samples->samples[0][i][j] >= 0.);
            CU_ASSERT(samples->samples[0][i][j] <= 1.);
        }
    }
    free(delete_samples(samples));
    
	samples = random_samples(5, 3, 2);
	CU_ASSERT(is_binary(samples));
	CU_ASSERT_EQUAL(samples->dimension, 2);
	CU_ASSERT_EQUAL(samples_total(samples), 5);
	CU_ASSERT_EQUAL(positives(samples), 3);
	CU_ASSERT_EQUAL(negatives(samples), 2);
	for (size_t k = 0; k < 2; k++) {
		for (size_t i = 0; i < samples->count[k]; i++) {
			for (size_t j = 0; j < 2; j++) {
				CU_ASSERT(samples->samples[k][i][j] >= 0.);
				CU_ASSERT(samples->samples[k][i][j] <= 1.);
			}
		}
	}
	
	CU_ASSERT_DOUBLE_EQUAL(primary_value(1), 1., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(primary_value(0), 0., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(primary_value(-1), 0., 1e-12);

	double hyperplane[] = { 1., 1., .5};
	sample_locator_t loc = { 1, 0 };
	samples->samples[1][0][0] = 1.+2e-3;
	samples->samples[1][0][1] = -.5;
    CU_ASSERT_DOUBLE_EQUAL(distance(&loc, samples, hyperplane, 1e-3), 
                           1e-3, 1e-6);
    CU_ASSERT_DOUBLE_EQUAL(sample_violation(&loc, samples, hyperplane, 1e-3), 
                           -1e-3, 1e-6);
	CU_ASSERT(side(&loc, samples, hyperplane, 1e-3));
    loc.index = 1;
	samples->samples[1][1][0] = 1.;
	samples->samples[1][1][1] = -.5;
    CU_ASSERT_DOUBLE_EQUAL(distance(&loc, samples, hyperplane, 1e-3), 
                           -1e-3, 1e-6);
	CU_ASSERT(!side(&loc, samples, hyperplane, 1e-3));
    loc.index = 2;
	samples->samples[1][2][0] = 1.-1e-3;
	samples->samples[1][2][1] = -.5;
    CU_ASSERT_DOUBLE_EQUAL(distance(&loc, samples, hyperplane, 1e-3), 
                           -2e-3, 1e-6);
	CU_ASSERT(!side(&loc, samples, hyperplane, 1e-3));
    CU_ASSERT_EQUAL(side_cnt(1, samples, hyperplane, 1e-3), 1);

	loc.class = 0;
    loc.index = 0;
	samples->samples[0][0][0] = 1.+1e-3;
	samples->samples[0][0][1] = -.5;
	CU_ASSERT(side(&loc, samples, hyperplane, 1e-3));
	samples->samples[0][0][0] = 1.;
	samples->samples[0][0][1] = -.5;
	CU_ASSERT(side(&loc, samples, hyperplane, 1e-3));
	samples->samples[0][0][0] = 1.-2e-3;
	samples->samples[0][0][1] = -.5;
	CU_ASSERT(!side(&loc, samples, hyperplane, 1e-3));
    
    samples->samples[0][1][0] = 0.;
    double result = 0;
    CU_ASSERT_EQUAL(reduce(samples, (void *) &result, test_accumulator, NULL), 
                    0);
    CU_ASSERT_DOUBLE_EQUAL(result, 1.+2e-3 + 1. + 1.-1e-3 + 1.-2e-3, 1e-6);
    
	free(delete_samples(samples));
    
    double *rnd_plane = random_hyperplane(2);
    free(rnd_plane);
}

int is_even(sparse_vector_t *v, int i, void *info) {
    int x = floor(v->val[i]);
    return !(x % 2);
}

void test_sparse_vector() {
	double ary[] = { 3., 2., 1. };
	sparse_vector_t *v = to_sparse(3, ary, 2);
	for (int i = 1; i <= 3; i++) {
        CU_ASSERT_EQUAL(v->ind[i], i);
        CU_ASSERT_DOUBLE_EQUAL(v->val[i], (double) (4 - i), 1e-12);
	}
	
	sparse_vector_t *u = filter(v, is_even, NULL);
    CU_ASSERT_EQUAL(u->len, 1);
    CU_ASSERT_EQUAL(u->ind[1], 2);
    CU_ASSERT_DOUBLE_EQUAL(u->val[1], 2., 1e-12);
    free(delete_sparse_vector(u));

	multiply(v, -1.);
	for (int i = 1; i <= 3; i++) {
        CU_ASSERT_EQUAL(v->ind[i], i);
        CU_ASSERT_DOUBLE_EQUAL(v->val[i], (double) -(4 - i), 1e-12);
	}

	CU_ASSERT_EQUAL(append(v, 6, 6.), 1);
    CU_ASSERT_EQUAL(v->ind[4], 6);
	CU_ASSERT_DOUBLE_EQUAL(v->val[4], 6., 1e-12);
    
	free(delete_sparse_vector(v));

	v = sparse_vector_blank(3);
    CU_ASSERT_EQUAL(v->len, 0);
    CU_ASSERT_EQUAL(v->extra, 3);
	free(delete_sparse_vector(v));
}

void test_env() {
	env_t env;
	env.params = params_default();
	env.samples = random_samples(5, 3, 2);
	env.solution_data = solution_data_init(5);
	delete_env(&env);
}


struct distance_record {
    int index;
    double directional_distance;
};
extern int dist_cmp(const void *, const void *);
extern unsigned int count_solution(double *solution, int index_max);

void test_indexing() {
	samples_t *samples = random_samples(5, 3, 2);
	CU_ASSERT_EQUAL(idx(0, 1, 1, samples), 5);
	CU_ASSERT_EQUAL(idx(1, 1, 1, samples), 2);
	CU_ASSERT_EQUAL(idx(0, 0, 1, samples), 8);
	CU_ASSERT_EQUAL(idx(1, 0, 1, samples), 5);
	CU_ASSERT_EQUAL(index_to_class(5, samples), 1);
	CU_ASSERT_EQUAL(violation_idx(0, samples), 9);
	CU_ASSERT_EQUAL(violation_idx(1, samples), 6);
	CU_ASSERT_EQUAL(idx_extreme(0, 1, 0, samples), 4);
	CU_ASSERT_EQUAL(idx_extreme(0, 1, 1, samples), 6);
	CU_ASSERT_EQUAL(idx_extreme(0, 0, 0, samples), 7);
	CU_ASSERT_EQUAL(idx_extreme(0, 0, 1, samples), 8);
	CU_ASSERT_EQUAL(idx_extreme(1, 1, 0, samples), 1);
	CU_ASSERT_EQUAL(idx_extreme(1, 1, 1, samples), 3);
	CU_ASSERT_EQUAL(idx_extreme(1, 0, 0, samples), 4);
	CU_ASSERT_EQUAL(idx_extreme(1, 0, 1, samples), 5);
    CU_ASSERT_EQUAL(index_to_class(5, samples), 1);
    CU_ASSERT_EQUAL(index_label(5, samples), 1);
    CU_ASSERT_EQUAL(index_to_class(8, samples), 0);
    CU_ASSERT_EQUAL(index_label(8, samples), -1);

    sparse_vector_t *sparse = sparse_vector_blank(3);
	sample_locator_t *loc;
	loc = locator(5, samples);
	CU_ASSERT_EQUAL(loc->class, 1);
	CU_ASSERT_EQUAL(loc->index, 1);
    append_locator(sparse, loc, 1., samples);
	free(loc);
	loc = locator(8, samples);
	CU_ASSERT_EQUAL(loc->class, 0);
	CU_ASSERT_EQUAL(loc->index, 1);
	free(loc);
	loc = locator(3, samples);
	CU_ASSERT_EQUAL(loc->class, -1);
	free(loc);
	loc = locator(9, samples);
	CU_ASSERT_EQUAL(loc->class, -1);
	free(loc);
	loc = locator(7, samples);
	CU_ASSERT_EQUAL(loc->class, 0);
	CU_ASSERT_EQUAL(loc->index, 0);
    append_locator(sparse, loc, 2., samples);
	free(loc);
    CU_ASSERT_EQUAL(sparse->len, 2);
    CU_ASSERT_EQUAL(sparse->ind[1], 5);
    CU_ASSERT_DOUBLE_EQUAL(sparse->val[1], 1., 1e-12);
    CU_ASSERT_EQUAL(sparse->ind[2], 7);
    CU_ASSERT_DOUBLE_EQUAL(sparse->val[2], 2., 1e-12);
    free(delete_sparse_vector(sparse));
    
    double hyperplane[3] = { 1., 1., -1. };
    env_t env;
    env.samples = samples;
    env.params = params_default();
    double value = hyperplane_to_solution_parts(hyperplane, NULL, 
                                                env.params, samples);
    CU_ASSERT_DOUBLE_EQUAL(value, 3., 1e-9);
    value = hyperplane_to_solution(hyperplane, NULL, &env);
    CU_ASSERT_DOUBLE_EQUAL(value, 3., 1e-9);
    
    double solution[] = { 0., 0., 0., 0., 1., 0., 0., 1., 1., -1. }; 
    CU_ASSERT_EQUAL(count_solution(solution + 4, 3), 1);
    CU_ASSERT_EQUAL(count_solution(solution + 7, 2), 2);
    CU_ASSERT_EQUAL(reach(solution, samples), 1);
    CU_ASSERT_EQUAL(false_positives(solution, samples), 2);
    CU_ASSERT_DOUBLE_EQUAL(precision(solution, samples), 1./ 3., 1e-9);
    
    // double *distance = blank_solution(samples);
    // hyperplane_to_distance(hyperplane, distance, &env);
    // CU_ASSERT_DOUBLE_EQUAL(value, 3., 1e-9);
    // free(distance);
    
    struct distance_record r[2];
    r[0].index = 0;
    r[1].index = 1;
    r[0].directional_distance = 1.;
    r[1].directional_distance = 2.;
    CU_ASSERT(dist_cmp(r, r + 1) < 0);
    r[1].directional_distance = 1.;
    CU_ASSERT(dist_cmp(r, r + 1) == 0);
    r[1].directional_distance = 0.;
    CU_ASSERT(dist_cmp(r, r + 1) > 0);
    int *ary = sorted_by_violation(hyperplane, &env);
    params_t *params = env.params;
    double precision[] = { params->epsilon_negative, params->epsilon_positive };
    loc = locator(ary[0], samples);
    double violation0 = 
        fabs(distance(loc, samples, hyperplane, precision[loc->class]));
    free(loc);
    loc = locator(ary[1], samples);
    free(ary);
    double violation1 = 
        fabs(distance(loc, samples, hyperplane, precision[loc->class]));
    free(loc);
    CU_ASSERT(violation0 <= violation1);
    
	sparse_vector_t *v = precision_row(samples, .7);
	CU_ASSERT_EQUAL(v->len, 6);
	// Negatives
	CU_ASSERT_EQUAL(v->ind[1], 7);
	CU_ASSERT_DOUBLE_EQUAL(v->val[1], .7, 1e-12);
	CU_ASSERT_EQUAL(v->ind[2], 8);
	// Positives
	for (int i = 3; i <= 5; i++) {
		CU_ASSERT_EQUAL(v->ind[i], i + 1);
	        CU_ASSERT_DOUBLE_EQUAL(v->val[i], -.3, 1e-12);
	}
	CU_ASSERT_EQUAL(v->ind[6], 9);
	CU_ASSERT_DOUBLE_EQUAL(v->val[6], -1., 1e-12);
	free(delete_sparse_vector(v));

	v = sparse_vector_blank(5);
	cover_row(v, 1, 1., samples);
	CU_ASSERT_EQUAL(v->ind[2], 5);
	CU_ASSERT_DOUBLE_EQUAL(v->val[2], 1., 1e-12);
	free(delete_sparse_vector(v));
    free(delete_samples(samples));
    free(env.params);
}  

void test_glpk() {
	env_t env;
	env.params = params_default();
	env.solution_data = solution_data_init(5);
	env.samples = random_samples(5, 3, 2);
	CU_ASSERT_PTR_NOT_NULL(milp(&env));
	delete_env(&env);
}


extern double rounded_positive(sample_locator_t *, double , double *, env_t *);
extern double rounded_negative(sample_locator_t *, double , double *, env_t *);
extern double rounded(sample_locator_t *, double solution, double *plane, 
		double *X, double *Y, env_t *);
extern double iheur_round(
		int i, double solution, double *hyperplane,
		double *X, double *Y, env_t *);
extern double iheur_violation(double X, double Y, params_t *);

void test_iheur() {
	env_t env;
	samples_t *samples = random_samples(5, 3, 2);
	env.samples = samples;
	env.params = params_default();
	double X = 0.;
	double Y = 0.;
	CU_ASSERT_DOUBLE_EQUAL(iheur_round(3, 3.14, NULL, &X, &Y, &env), 
			3.14, 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(X, 0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(Y, 0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(iheur_round(4, 0.14, NULL, &X, &Y, &env), 
			0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(X, 0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(Y, 0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(iheur_round(4, 1., NULL, &X, &Y, &env), 
			1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(X, 1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(Y, 0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(iheur_round(8, 0.14, NULL, &X, &Y, &env), 
			1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(X, 1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(Y, 1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(iheur_round(4, 0., NULL, &X, &Y, &env), 
			0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(X, 1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(Y, 1., 1e-12);

	params_t *params = params_default();
        CU_ASSERT_DOUBLE_EQUAL(iheur_violation(X, Y, params), 15e-4, 1e-9);
        CU_ASSERT_DOUBLE_EQUAL(iheur_violation(10., 0., params), 0., 1e-9);

	env.params->iheur_method = deep;
	X = 0.;
	Y = 0.;
	double plane[] = { 1., 1., .5 };
	sample_locator_t loc = { 1, 0 };
	samples->samples[1][0][0] = 1.;
	samples->samples[1][0][1] = -.5;
	CU_ASSERT_DOUBLE_EQUAL(rounded_positive(&loc, .14, plane, &env), 
			0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(rounded(&loc, .14, plane, &X, &Y, &env),
			0., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(X, 0., 1e-12);
	loc.class = 0;
	samples->samples[0][0][0] = 1.;
	samples->samples[0][0][1] = -.5;
	CU_ASSERT_DOUBLE_EQUAL(rounded_negative(&loc, .14, plane, &env),
			1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(rounded(&loc, .14, plane, &X, &Y, &env),
			1., 1e-12);
	CU_ASSERT_DOUBLE_EQUAL(Y, 1., 1e-12);

	free(delete_samples(samples));
    free(env.params);
	free(params);
}


void test_solution_data() {
	solution_data_t *data = solution_data_init(5);
	CU_ASSERT_EQUAL(data->rank_significant, 0);
	append_data(data, 4);
	CU_ASSERT_EQUAL(data->rank_significant, 1);
	CU_ASSERT_EQUAL(data->rank[0], 4);
    update_solution(data, NULL, 0., NULL);
    CU_ASSERT_EQUAL(data->intopt, 0.);
    CU_ASSERT_PTR_NULL(data->integer_solution);
    update_solution(data, NULL, 1., NULL);
    CU_ASSERT_EQUAL(data->intopt, 1.);
	free(delete_solution_data(data));
}


void test_signature() {
    node_signature_t a;
    set_signature(&a, 2, 1, 2., .1, 3, 0, 0);
    CU_ASSERT_EQUAL(a.level, 2);
    CU_ASSERT_EQUAL(a.primary, 1);
    CU_ASSERT_DOUBLE_EQUAL(a.bound, 2., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(a.ii_sum, .1, 1e-12);
    CU_ASSERT_EQUAL(a.seqno, 3);
    CU_ASSERT_EQUAL(a.last_primary, 0);
    /* Compare signature cannnot be tested yet, because it is experimental
     * and thus still changing all the time
    node_signature_t b;
    copy_signature(&b, &a);
    CU_ASSERT_EQUAL(compare_signature(&a, &b), 0);
    set_signature(&b, 1, 1, 1., 3);
    CU_ASSERT(compare_signature(&a, &b) > 0);
    set_signature(&b, 2, 0, 1., 5);
    CU_ASSERT(compare_signature(&a, &b) > 0);
    set_signature(&b, 2, 1, 1., 4);
    CU_ASSERT(compare_signature(&a, &b) < 0); */
}

void test_children() {
    child_data_t data;
    data.child_cnt = 0;
    CU_ASSERT_EQUAL(add_child(&data, 2), 1);
    CU_ASSERT_EQUAL(add_child(&data, 2), -1);
    CU_ASSERT_EQUAL(child_direction(&data, 2), -1);
    CU_ASSERT_EQUAL(data.child_cnt, 1);
    CU_ASSERT_EQUAL(data.child[0], 2);
    CU_ASSERT_EQUAL(add_child(&data, 1), 1);
    CU_ASSERT_EQUAL(data.child[0], 2);
    CU_ASSERT_EQUAL(data.child[1], 1);
    CU_ASSERT_EQUAL(child_direction(&data, 2), 0);
    CU_ASSERT_EQUAL(child_direction(&data, 3), -1);
}

void test_paths() {
    int index[3];
    memset(index, 0, sizeof(index));
    double a[5] = { .1, .2, .3, .4, .5 };
    double b[5] = { 1., .2, 3., .4, .5 };
    CU_ASSERT(are_consistent(index, a, b));
    index[0] = 2;
    CU_ASSERT_FALSE(are_consistent(index, a, b));
    index[0] = 1; 
    index[1] = 4;
    CU_ASSERT(are_consistent(index, a, b));
    
    sparse_vector_t *path = sparse_vector_blank(4);
    append(path, 3, .4);
    CU_ASSERT(is_path_consistent(path, a));
    append(path, 4, .5);
    CU_ASSERT(is_path_consistent(path, a));
    CU_ASSERT_EQUAL(consistency_count(path, a), 2);
    append(path, 1, .1);
    CU_ASSERT_FALSE(is_path_consistent(path, a));
    CU_ASSERT_EQUAL(consistency_count(path, a), 2);
    free(delete_sparse_vector(path));
}

extern int doublecmp(const void *, const void *);
extern void cumulative2density(
    double side,
    size_t dimension,
    double *cumulative, 
    double *density);
extern int has_zero(size_t, double *);

void test_random() {
    double a[2];
    a[1] = 0.;
    a[0] = -1;
    CU_ASSERT_EQUAL(doublecmp(a, a + 1), -1);
    a[0] = 0.;
    CU_ASSERT_EQUAL(doublecmp(a, a + 1), 0);
    a[0] = 1.;
    CU_ASSERT_EQUAL(doublecmp(a, a + 1), 1);
  
    double w[] = { 0., 0. };
    CU_ASSERT_DOUBLE_EQUAL(length_squared(2, w), 0., 1e-9);
    w[0] = -1.;
    CU_ASSERT_DOUBLE_EQUAL(length_squared(2, w), 1., 1e-9);
    w[1] = 1.;
    CU_ASSERT_DOUBLE_EQUAL(length_squared(2, w), 2., 1e-6);
    multiply_scalar(2., 2, w);
    CU_ASSERT_DOUBLE_EQUAL(length_squared(2, w), 8., 1e-6);
    
    srand48(20200718110752);
    double x[7];
    random_unit_vector(7, x);
    CU_ASSERT_DOUBLE_EQUAL(length_squared(7, x), 1., 1e-6);
    
    double *pt = random_point(2);
    CU_ASSERT(0. <= pt[0] && pt[0] < 1.);
    CU_ASSERT(0. <= pt[1] && pt[1] < 1.);
    free(pt);
    double cumulative = 0.;
    for (size_t t = 0; t < 30; t++) {
      pt = random_point(2);
      cumulative += pt[0];
      free(pt);
    }
    CU_ASSERT(cumulative > 0.);
    
    pt = random_point_affine(2, .9, .1);
    CU_ASSERT(.9 <= pt[0] && pt[0] < 1.);
    CU_ASSERT(.9 <= pt[1] && pt[1] < 1.);
    free(pt);
    
    pt = CALLOC(3, double);
    double *cumul = CALLOC(3, double);
    cumul[0] = .3;
    cumul[1] = .8;
    cumul[2] = .9;
    cumulative2density(1., 3, cumul, pt);
    CU_ASSERT_DOUBLE_EQUAL(pt[0], .3, 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(pt[1], .5, 1e-9);
    CU_ASSERT_DOUBLE_EQUAL(pt[2], .1, 1e-9);
    CU_ASSERT_FALSE(has_zero(3, pt));
    pt[1] = 0.;
    CU_ASSERT(has_zero(3, pt));
    free(cumul);
    free(pt);
    
    pt = random_simplex_point(.5, 3);
    CU_ASSERT_FALSE(has_zero(3, pt));
    CU_ASSERT(pt[0] + pt[1] + pt[2] < .5);
    free(pt);
}

void test_hyperplane() {
    double w[3] = {.5, .3, -.1 };
    double w1[3];
    copy_hyperplane(2, w1, w);
    for (size_t i = 0; i <= 2; i++) {
        CU_ASSERT_DOUBLE_EQUAL(w1[i], w[i], 1e-12);
    }
    
    free(random_hyperplane(16));
    
    env_t env;
    env.params = params_default();
    env.params->rnd_trials = 1;
	env.samples = random_samples(5, 3, 2);
	env.solution_data = solution_data_init(5);
    free(best_random_hyperplane(0, &env));
    free(delete_solution_data(env.solution_data));
    free(delete_samples(env.samples));
    free(env.params);
}

void test_cuts() {
    sparse_vector_t *pth = sparse_vector_blank(3);
    append(pth, 3, 1.);
    append(pth, 1, 0.);
    append(pth, 4, 1.);
    sparse_vector_t *interdicted = sparse_vector_blank(2);
    append(interdicted, 2, 1.);
    append(interdicted, 5, -1.);
    sparse_vector_t *rhs = NULL;
    double lhs;
    int initialized = 0;
    interdiction_cut(pth, interdicted, &initialized, &rhs, &lhs);
    CU_ASSERT_EQUAL(initialized, 1);
    free(delete_sparse_vector(pth));
    free(delete_sparse_vector(interdicted));
    CU_ASSERT_EQUAL(rhs->len, 5);
    CU_ASSERT_EQUAL(rhs->ind[1], 2);
    CU_ASSERT_DOUBLE_EQUAL(rhs->val[1], 1., 1e-12);
    CU_ASSERT_EQUAL(rhs->ind[2], 5);
    CU_ASSERT_DOUBLE_EQUAL(rhs->val[2], -1., 1e-12);
    CU_ASSERT_EQUAL(rhs->ind[3], 3);
    CU_ASSERT_DOUBLE_EQUAL(rhs->val[3], 2., 1e-12);
    CU_ASSERT_EQUAL(lhs, 3);
    free(delete_sparse_vector(rhs));
}

#define INSIZE 1024

void test_read_samples() {
    char instring[INSIZE];
    FILE *infile = fmemopen(instring, 0, "r");
    free(read_vector(infile, 0));
    fclose(infile);
    
    int inptr = snprintf(instring, INSIZE, "%g %g %g\n", 1., 2., 3.);
    infile = fmemopen(instring, INSIZE, "r");
    double *sample = read_vector(infile, 3);
    CU_ASSERT_DOUBLE_EQUAL(sample[0], 1., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(sample[1], 2., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(sample[2], 3., 1e-12);
    free(sample);
    fclose(infile);
    
    samples_t *samples = CALLOC(1, samples_t);
    samples->class_cnt = 2;
    samples->dimension = 3;
    size_t *count = samples->count = CALLOC(2, size_t);    
    count[0] = 0;
    count[1] = 2;
    samples->samples = CALLOC(2, double **);
    inptr += snprintf(instring + inptr, INSIZE - inptr, 
                      "%g %g %g\n", 4., 5., 6.);
    infile = fmemopen(instring, INSIZE, "r");
    read_class(infile, samples, 1);
    double **s = samples->samples[1];
    CU_ASSERT_DOUBLE_EQUAL(s[0][0], 1., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(s[1][1], 5., 1e-12);
    CU_ASSERT_DOUBLE_EQUAL(s[1][2], 6., 1e-12);
    free(s[0]);
    free(s[1]);
    free(s);
    free(samples->samples);
    free(samples->count);
    fclose(infile);

    samples->class_cnt = 2;
    samples->label = NULL;
    count = samples->count = CALLOC(2, size_t); 
    count[0] = 0;
    count[1] = 2;
    samples->samples = CALLOC(2, double **);
    infile = fmemopen(instring, INSIZE, "r");
    read_classes(infile, samples);
    s = samples->samples[1];
    CU_ASSERT_DOUBLE_EQUAL(s[0][1], 2., 1e-12);
    fclose(infile);
    free(delete_samples(samples));
    
    inptr = snprintf(instring, INSIZE, "%u %u %u\n", 3, 0, 2);
    inptr += snprintf(instring + inptr, INSIZE - inptr, 
                      "%g %g %g\n", 1., 2., 3.);
    inptr += snprintf(instring + inptr, INSIZE - inptr, 
                      "%g %g %g\n", 4., 5., 6.);
    infile = fmemopen(instring, INSIZE, "r");
    samples = read_binary_samples(infile);
    CU_ASSERT_EQUAL(samples->class_cnt, 2);
    CU_ASSERT_DOUBLE_EQUAL(samples->samples[1][0][1], 2., 1e-12);
    fclose(infile);
    free(delete_samples(samples));
}

extern double **random_point_cluster(size_t count, size_t dimension, 
                                     double shift, double side,
                                     double **samples);
extern size_t clusters_count(clusters_info_t *);
extern double **random_point_clusters(clusters_info_t *info);
extern void set_sample_class_clusters(samples_t *, size_t class, int label, 
		clusters_info_t *);

void free_points(double **points, size_t count) {
  for (size_t i = 0; i < count; i++) {
    free(points[i]);
  }
}

void validate_points(double **points, size_t count, double min, double max) {
  for (size_t i = 0; i < count; i++) {
    for (size_t j = 0; j < 2; j++) {
      CU_ASSERT(points[i][j] >= min);
      CU_ASSERT(points[i][j] <= max);
    }
  }
}

void test_clusters() {
  // Test random_point_cluster
  double **points = CALLOC(3, double *);
  double **points_ptr = random_point_cluster(3, 2, .9, .1, points);
  CU_ASSERT_EQUAL(points_ptr, points + 3);
  validate_points(points, 3, .9, 1.);
  free_points(points, 3);
  free(points);
  
  // Test cluster count and delete
  clusters_info_t *info = CALLOC(1, clusters_info_t);
  info->cluster_cnt = 2;
  info->count = CALLOC(2, size_t); 
  info->count[0] = 3;
  info->count[1] = 2;
  CU_ASSERT_EQUAL(clusters_count(info), 5);
  
  // Test random point clusters
  info->dimension = 2;
  info->shift = CALLOC(2, double);
  info->side = CALLOC(2, double);
  info->shift[0] = 0.;
  info->side[0] = 1.;
  info->shift[1] = .9;
  info->side[1] = .1;
  points = random_point_clusters(info);
  validate_points(points, 3, 0., 1.);
  validate_points(points + 3, 2, .9, 1.);
  free_points(points, 5);
  free(points);
  
  // Test set sample class clusters
  samples_t *samples = CALLOC(1, samples_t);
  samples->dimension = 2;
  samples->class_cnt = 2;
  samples->label = CALLOC(2, int);
  samples->count = CALLOC(2, size_t);
  samples->samples = CALLOC(2, double **);
  clusters_info_t *info_negative = 
    clusters_info_singleton(CALLOC(1, clusters_info_t), 2, 2);
  set_sample_class_clusters(samples, 0, -1, info_negative);
  set_sample_class_clusters(samples, 1, 1, info);
  validate_points(samples->samples[0], 2, 0., 1.);
  points = samples->samples[1];
  validate_points(points, 3, 0., 1.);
  validate_points(points + 3, 2, .9, 1.);
  free(delete_samples(samples));
  free(delete_clusters_info(info));
  free(delete_clusters_info(info_negative));
  
  // Test random sample clusters
  clusters_info_t clusters[2];
  clusters_info_singleton(clusters, 2, 2);
  info = clusters + 1;
  info->dimension = 1;
  info->cluster_cnt = 2;
  info->count = CALLOC(2, size_t); 
  info->count[0] = 3;
  info->count[1] = 2;
  info->shift = CALLOC(2, double);
  info->side = CALLOC(2, double);
  info->shift[0] = 0.;
  info->side[0] = 1.;
  info->shift[1] = .9;
  info->side[1] = .1;
  samples = random_sample_clusters(clusters);
  CU_ASSERT_EQUAL(samples->dimension, 2);
  CU_ASSERT_EQUAL(samples->label[0], -1);
  CU_ASSERT_EQUAL(samples->count[0], 2);
  validate_points(samples->samples[0], 2, 0., 1.);
  points = samples->samples[1];
  validate_points(points, 3, 0., 1.);
  validate_points(points + 3, 2, .9, 1.);
  free(delete_samples(samples));
  delete_clusters_info(clusters);
  delete_clusters_info(info);
}

extern double ticks2threshold(unsigned int);
extern unsigned int threshold2ticks(double);
extern void advance_max(
    double *obj_max, 
    double obj, 
    unsigned int *theta_max, 
    unsigned int theta);

void test_exec() {
  srand48(195583786);
  
  int n = 16;
  samples_t *samples = random_samples(n, n / 2, 2);
  
  env_t env;
  env.samples = samples;
  params_t *parms = env.params = params_default();
  parms->theta = 0.8;
  parms->branch_target = 0.0;
  parms->iheur_method = deep;
  parms->lambda = 10 * (n + 1); 
  parms->rnd_trials = 100;
  parms->rnd_trials_cont = 0;
  
  double *result = single_run(NULL, 120000, &env);
  CU_PASS("single run");

  free(delete_samples(samples));
  free(parms);
  free(result);
}

extern void mirror_sample(size_t dimension, double *sample);
extern double **random_simplex_points(size_t count, simplex_info_t *);
extern void set_sample_class_simplex(
		samples_t *samples, 
		size_t class, 
		int label, 
        size_t count,
		simplex_info_t *);

void test_simplex() {
  simplex_info_t info = { 
      .count = 7, 
      .positives = 5, 
      .cluster_cnt = 1, 
      .dimension = 2, 
      .side = .1 };
      
  // Test mirror
  double p[] = { .1, .2 };
  mirror_sample(2, p);
  CU_ASSERT_DOUBLE_EQUAL(p[0], .9, 1e-9);
  CU_ASSERT_DOUBLE_EQUAL(p[1], .8, 1e-9);
  
  // Test random_points
  double **points = random_simplex_points(7, &info);
  double norm;
  for (size_t i = 0; i < 3; i++) {
    norm = 0.;
    for (size_t j = 0; j < 2; j++) {
      CU_ASSERT(points[i][j] >= 0.);
      norm += points[i][j];
    }
    CU_ASSERT(norm < .1);
    free(points[i]);
  }
  for (size_t i = 3; i < 7; i++) {
    free(points[i]);
  }
  
  free(points);
  info.cluster_cnt = 2;
  points = random_simplex_points(16, &info);
  for (size_t i = 4; i < 8; i++) {
    norm = 0.;
    for (size_t j = 0; j < 2; j++) {
      CU_ASSERT(points[i][j] <= 1.);
      norm += 1. - points[i][j];
    }
    CU_ASSERT(norm < .1);
  }
  for (size_t i = 0; i < 16; i++) {
    free(points[i]);
  }
  info.cluster_cnt = 1;
  free(points);
    
    // Test set_sample_class_simplex
    samples_t *samples = CALLOC(1, samples_t);
    samples->dimension = 2;
	samples->class_cnt = 1;
	samples->label = CALLOC(1, int);
	samples->count = CALLOC(1, size_t);
	samples->samples = CALLOC(1, double **);
    set_sample_class_simplex(samples, 0, -1, 5, &info); 
    CU_ASSERT_EQUAL(samples->label[0], -1);
    CU_ASSERT_EQUAL(samples->count[0], 5);        
    norm = 0.;
    for (size_t j = 0; j < 2; j++) {
      norm += samples->samples[0][1][j];
    }
    CU_ASSERT(norm <= .1);
    free(delete_samples(samples));
    
    samples = random_simplex_samples(&info);
	CU_ASSERT_EQUAL(samples->dimension, 2);
	CU_ASSERT_EQUAL(samples_total(samples), 7);
	CU_ASSERT_EQUAL(positives(samples), 5);
	CU_ASSERT_EQUAL(negatives(samples), 2);
    for (size_t i = 0; i < 2; i++) {
      norm = 0.;
      for (size_t j = 0; j < 2; j++) {
        norm += samples->samples[1][i][j];
      }
      CU_ASSERT(norm <= .1);
    }
    free(delete_samples(samples));
}


void test_labels() {
  CU_ASSERT_EQUAL(label_to_varname(1), 'x');
  CU_ASSERT_EQUAL(label_to_varname(0), 'y');
  CU_ASSERT_DOUBLE_EQUAL(label_to_obj(1), 1., 1e-12);
  CU_ASSERT_DOUBLE_EQUAL(label_to_obj(0), 0., 1e-12);
  
  params_t *params = params_default();
  CU_ASSERT_DOUBLE_EQUAL(label_to_bound(1, params), 
                         1. - params->epsilon_positive, 1e-12);
  CU_ASSERT_DOUBLE_EQUAL(label_to_bound(0, params), 
                         - params->epsilon_negative, 1e-12);
  free(params);
}

GRBmodel *init_gurobi_model(const env_t *);
int add_gurobi_hyperplane(GRBmodel *, size_t);
int add_gurobi_sample_var(GRBmodel *, int label, char *name);
int add_gurobi_sample_constr(
    GRBmodel *, sample_locator_t, int label, char *name, const env_t *);
int add_gurobi_sample(GRBmodel *, sample_locator_t, const env_t *);
void gurobi_indices(sparse_vector_t *);
int gurobi_accumulator(samples_t *, sample_locator_t, void *model, void *env);
int add_gurobi_samples(GRBmodel *, const env_t *);
int add_gurobi_precision(GRBmodel *, const env_t *);
void test_gurobi() {
  env_t env;
  env.params = params_default();
  env.solution_data = solution_data_init(5);
  env.samples = random_samples(5, 3, 2);
  GRBmodel *model = init_gurobi_model(&env);
  CU_ASSERT_PTR_NOT_NULL(model);
  
  CU_ASSERT_EQUAL(add_gurobi_hyperplane(model, 2), 0);
  CU_ASSERT_PTR_NOT_NULL(model);
  CU_ASSERT_EQUAL(GRBupdatemodel(model), 0);
  int varnumP;
  CU_ASSERT_EQUAL(GRBgetvarbyname(model, "w1", &varnumP), 0);
  CU_ASSERT_EQUAL(varnumP, 0);
  
  sparse_vector_t *v = sparse_vector_blank(2);
  append(v, 2, -1.);
  append(v, 4, 1.);
  gurobi_indices(v);
  CU_ASSERT_EQUAL(v->ind[1], 1);
  CU_ASSERT_EQUAL(v->ind[2], 3);
  free(delete_sparse_vector(v));
  
  sample_locator_t *locator = CALLOC(1, sample_locator_t);;
  locator->class = 1;
  locator->index = 0;
  CU_ASSERT_EQUAL(add_gurobi_sample(model, *locator, &env), 0);
  CU_ASSERT_EQUAL(GRBupdatemodel(model), 0);
  CU_ASSERT_EQUAL(GRBgetvarbyname(model, "x1", &varnumP), 0);
  CU_ASSERT_EQUAL(varnumP, 3);
  double valP;
  CU_ASSERT_EQUAL(GRBgetcoeff(model, 0, 3, &valP), 0);
  CU_ASSERT_DOUBLE_EQUAL(valP, 1., 1e-12);
  CU_ASSERT_EQUAL(GRBgetdblattr(model, "MaxRHS", &valP), 0);
  CU_ASSERT_DOUBLE_EQUAL(valP, 1.-env.params->epsilon_positive, 1e-9);
  
  locator->class = 1;
  locator->index = 1;
  int label = env.samples->label[locator->class];
  CU_ASSERT_EQUAL(add_gurobi_sample_var(model, label, "x2"), 0);
  CU_ASSERT_PTR_NOT_NULL(model);
  CU_ASSERT_EQUAL(GRBupdatemodel(model), 0);
  CU_ASSERT_EQUAL(GRBgetvarbyname(model, "x2", &varnumP), 0);
  CU_ASSERT_EQUAL(varnumP, 4);
  
  CU_ASSERT_EQUAL(add_gurobi_sample_constr(model, *locator, label, "x2", &env), 
                  0);
  CU_ASSERT_EQUAL(GRBupdatemodel(model), 0);
  CU_ASSERT_EQUAL(GRBgetcoeff(model, 1, 4, &valP), 0);
  CU_ASSERT_DOUBLE_EQUAL(valP, 1., 1e-12);
  CU_ASSERT_EQUAL(GRBgetdblattr(model, "MaxRHS", &valP), 0);
  CU_ASSERT_DOUBLE_EQUAL(valP, 1.-env.params->epsilon_positive, 1e-9);
  
  locator->class = 1;
  locator->index = 2;
  label = env.samples->label[locator->class];
  CU_ASSERT_EQUAL(add_gurobi_sample(model, *locator, &env), 0);
  locator->class = 0;
  locator->index = 0;
  CU_ASSERT_EQUAL(gurobi_accumulator(env.samples, *locator, model, &env), 0);
  CU_ASSERT_EQUAL(GRBupdatemodel(model), 0);
  CU_ASSERT_EQUAL(GRBgetcoeff(model, 3, 6, &valP), 0);
  CU_ASSERT_DOUBLE_EQUAL(valP, -1., 1e-12);

  CU_ASSERT_EQUAL(GRBfreemodel(model), 0);
  
  GRBmodel *m = init_gurobi_model(&env);
  add_gurobi_hyperplane(m, 2);
  add_gurobi_samples(m, &env);
  CU_ASSERT_EQUAL(add_gurobi_precision(m, &env), 0);
  CU_ASSERT_EQUAL(GRBupdatemodel(m), 0);
  CU_ASSERT_EQUAL(GRBgetcoeff(m, 5, 8, &valP), 0);
  CU_ASSERT_DOUBLE_EQUAL(valP, -1., 1e-12);

  GRBwrite(m, "tmp.lp");
  CU_ASSERT_EQUAL(GRBfreemodel(m), 0);
  
  free(locator);
  delete_env(&env);
}


int main() {
	if (CU_initialize_registry() != CUE_SUCCESS) {
		return EXIT_FAILURE;
	}

	// Samples
	CU_pSuite samples = CU_add_suite("samples", init_samples, NULL);
	CU_add_test(samples, "generic samples", test_samples);

	// Sparse vectors
	CU_pSuite sparse = CU_add_suite("sparse vector", NULL, NULL);
	CU_add_test(sparse, "generic sparse vector", test_sparse_vector);
	
	// Environment
	CU_pSuite env = CU_add_suite("environment", init_samples, NULL);
	CU_add_test(env, "environment", test_env);

	// Indexing
	CU_pSuite indexing = CU_add_suite("indexing", init_samples, NULL);
	CU_add_test(indexing, "indexing", test_indexing);

	// GLPK
	CU_pSuite glpk = CU_add_suite("glpk", init_samples, NULL);
	CU_add_test(glpk, "glpk", test_glpk);

	// IHeur
	CU_pSuite iheur = CU_add_suite("iheur", init_samples, NULL);
	CU_add_test(iheur, "iheur", test_iheur);

	// Solution data
	CU_pSuite solution = CU_add_suite("solution data", init_samples, NULL);
	CU_add_test(solution, "solution data", test_solution_data);

    // Node signature
	CU_pSuite signature = CU_add_suite("node signature", NULL, NULL);
	CU_add_test(signature, "node signature", test_signature);
    
    // Children
	CU_pSuite children = CU_add_suite("children", NULL, NULL);
	CU_add_test(children, "children", test_children);

    // Paths
	CU_pSuite paths = CU_add_suite("paths", NULL, NULL);
	CU_add_test(paths, "paths", test_paths);
    
    // Random
	CU_pSuite random = CU_add_suite("random", NULL, NULL);
	CU_add_test(random, "random", test_random);
    
    // Hyperplane
	CU_pSuite hyperplane = CU_add_suite("hyperplane", init_samples, NULL);
	CU_add_test(hyperplane, "hyperplane", test_hyperplane);
    
    // Cuts
    CU_pSuite cuts = CU_add_suite("cuts", NULL, NULL);
	CU_add_test(cuts, "cuts", test_cuts);
    
    // Read samples
    CU_pSuite read_samples = CU_add_suite("readsamples", NULL, NULL);
	CU_add_test(read_samples, "readsamples", test_read_samples);
    
    // Clusters
    CU_pSuite clusters = CU_add_suite("clusters", NULL, NULL);
	CU_add_test(clusters, "clusters", test_clusters);
    
    // Simplex
    CU_pSuite simplex = CU_add_suite("simplex", NULL, NULL);
	CU_add_test(simplex, "simplex", test_simplex);

    // Execution support
    CU_pSuite exec = CU_add_suite("execution", NULL, NULL);
	CU_add_test(exec, "execution", test_exec);
    
    // Label conversion
    CU_pSuite labels = CU_add_suite("labels", NULL, NULL);
	CU_add_test(labels, "labels", test_labels);
    
    // Gurobi
	CU_pSuite gurobi = CU_add_suite("gurobi", init_samples, NULL);
	CU_add_test(gurobi, "gurobi", test_gurobi);

	// Run tests
	CU_basic_run_tests();

	CU_cleanup_registry();
	return EXIT_SUCCESS;
}
