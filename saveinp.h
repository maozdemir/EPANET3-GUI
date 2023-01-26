
#include "structs.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>


class Saver
{
public:
    static void saveInputFile(std::string saveFile, Title &title, std::vector<Node> &nodes,std::vector<Pipe> &line_arg, Project &project);
private:
    static void generate_coordinates(Node &node);
};