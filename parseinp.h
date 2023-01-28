#ifndef PARSEINP_H
#define PARSEINP_H
#include "struct_atakoy.h"
#include "structs.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include "helpers.h"

class Parser
{
public:
    static void parseInputFile(std::string inputFile, Project &project);
    // !!! IMPROPER, PURPOSE SPECIFIC FUNCTION  BEGINS  !!!
    static void parseResultTxt(std::string resultFile, std::vector<Result_Atakoy> &result_atakoy_vector);
    // !!! IMPROPER, PURPOSE SPECIFIC FUNCTION ENDS     !!!
private:
    static void parsePatterns(Project &project, std::string inputFile);
};
#endif