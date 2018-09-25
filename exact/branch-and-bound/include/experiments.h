#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <string>

const std::string PATH_INSTANCE_PREFIX = "../../instances/";
const std::string FILE_INSTANCE_SUFFIX = ".dat";
const std::string FILE_OPT_PREFIX =  "[opt]";
const std::string FILE_OPT_SUFFIX =  ".dat";

void read_instance( std::string instance_path );

void runQAP ( std::string instance_path );

#endif