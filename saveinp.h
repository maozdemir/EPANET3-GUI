#ifndef SAVEINP_H
#define SAVEINP_H
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
    static void saveInputFile(std::string saveFile, Project &project);
private:
    static void generate_coordinates(Node &node);
};
#endif