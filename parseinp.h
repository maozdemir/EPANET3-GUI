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
#include <iterator>
#include "helpers.h"

class Parser
{
public:
    static void parseInputFile(std::string inputFile, Project &project);
    static void parsePatterns(Project &project, std::string inputFile);
    static void parseDemands(Project &project, std::string inputFile);
    static void parseNodes(Project &project, std::string inputFile);
    // !!! IMPROPER, PURPOSE SPECIFIC FUNCTION  BEGINS  !!!
    static bool parseResultTxt(std::string resultFile, std::vector<Result_Atakoy> &result_atakoy_vector);
    // !!! IMPROPER, PURPOSE SPECIFIC FUNCTION ENDS     !!!
private:
};
#endif