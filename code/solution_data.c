#include "widereach.h"
#include "helper.h"

solution_data_t *solution_data_init(size_t size) {
	solution_data_t *data = CALLOC(1, solution_data_t);
	data->rank_significant = 0;
	data->rank = CALLOC(size, int);
    data->branching_node = 0;
    data->integer_solution = NULL;
    data->distance = NULL;
	return data;
}

solution_data_t *delete_solution_data(solution_data_t *data) {
	free(data->rank);
	return data;
}


solution_data_t *append_data(solution_data_t *data, int index) {
	data->rank[(data->rank_significant)++] = index;
	return data;
}

void update_solution(
        solution_data_t *solution_data, 
        double *solution, 
        double value,
        double *distance) {
    double **integer_solution = &(solution_data->integer_solution);
    free(*integer_solution);
    *integer_solution = solution;
    
    solution_data->intopt = value;
    
    double **dist = &(solution_data->distance);
    free(*dist);
    *dist = distance;
}
    
