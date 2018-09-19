#ifndef HUNGARIAN_H
#define HUNGARIAN_H

void add_to_tree(int current, int prev);

void init_labels();

void update_labels();

void augment();

int hungarian_least_cost(int n, int** matrix);

void deallocate_global_variables();

void init_global_variables(int n);

#endif 