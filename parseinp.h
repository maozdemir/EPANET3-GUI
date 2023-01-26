
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
    static void parseInputFile(std::string inputFile, Title &title, std::vector<Node> &nodes, std::vector<Pipe> &line_arg, Project &project);

private:
    static void parsePatterns(Project &project, std::string inputFile);
};