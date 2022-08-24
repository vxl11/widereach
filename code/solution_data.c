#include "widereach.h"
#include "helper.h"

solution_data_t *solution_data_init(size_t size) {
	solution_data_t *data = CALLOC(1, solution_data_t);
	data->rank_significant = 0;
	data->rank = CALLOC(size, int);
    data->branching_node = 0;
    data->integer_solution = NULL;
    data->violation_index = NULL;
	return data;
}

solution_data_t *delete_solution_data(solution_data_t *data) {
	free(data->rank);
    free(data->integer_solution);
    free(data->violation_index);
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
        int *violation_index) {
    double **integer_solution = &(solution_data->integer_solution);
    free(*integer_solution);
    *integer_solution = solution;
    
    solution_data->intopt = value;
    
    int **dist = &(solution_data->violation_index);
    free(*dist);
    *dist = violation_index;
}
    
