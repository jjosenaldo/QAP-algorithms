#ifndef _HUNGARIAN_H_
#define _HUNGARIAN_H_

/**
 * this is an implementation of the Hungarian algorithm
 * for the linear allocation problem. This problem corresponds
 * to finding the perfect matching in a bipartide graph. This
 * code is based on the implementation available on the website
 * <https://www.topcoder.com/community/data-science/data-
 * science-tutorials/assignment-problem-and-hungarian-algorithm/>
 * 
 * For more information on the Hungarian algorithm go to the website
 * <https://codeforces.com/blog/entry/17511>
 */

#include <algorithm>
#include <cstring>
#include <vector>
#include <iostream>

/**
 * Here are the global variables that will be used in the code:
 */

#define max_number_of_vertices 3 // max number of vertices in one part
#define INF 100000000 // just infinity

std::vector<std::vector<int>> matrix_cost = {{1,4,5},{5,7,6},{5,8,8}}; // cost matrix
int facilities, max_match; //assignment of n facilities to n localities
int labels_x[max_number_of_vertices]; // labels of X
int labels_y[max_number_of_vertices]; // Y parts
int xy_match[max_number_of_vertices]; // xy_match[x] - vertex that is matched with x
int yx_match[max_number_of_vertices]; // yx_match[y] - vertex that is matched with y
bool S[max_number_of_vertices], T[max_number_of_vertices]; //sets S and T in algorithm
int slack[max_number_of_vertices]; //as in the algorithm description
int slack_x[max_number_of_vertices]; //slackx[y] such a vertex, that l(slackx[y]) + l(y) - w(slackx[y],y) = slack[y]
int prev[max_number_of_vertices]; //array for memorizing alternating paths

void augment ();
void update_labels();
void add_to_tree (int x, int prev_x);
int hungarian();
void init_labels();



#endif