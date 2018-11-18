#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <string>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include "qap.h"
#include "GA_QAP.h"
#include "tabu_search_qap.h"

const std::string PATH_INSTANCE_PREFIX = "../../instances/";
const std::string FILE_INSTANCE_SUFFIX = ".dat";
const std::string FILE_OPT_PREFIX =  "[opt]";
const std::string FILE_OPT_SUFFIX =  ".dat";

void read_instance( std::string instance_name );
void run_genetic_algorithm( std::string instance_name );

#endif