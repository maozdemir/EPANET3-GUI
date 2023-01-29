#include <vector>
#include <iostream>
#include "structs.h"
#include "parseinp.h"
#include "epanet-ga.h"
#include "struct_atakoy.h"

int main(int, char **)
{
    Project project;
    Parser::parseInputFile("EPA3-hk-large-peaked-intermediate.inp",project);
    printf("\n");
    printf("Size of nodes: %d\n",project.nodes.size());
    GeneticAlgorithm ga;
    printf("Project nodes in main: %d\n", project.nodes.size());
    ga.set_project(project);
    printf("Chromosome length: %d\n", ga.get_chromosome_length());
    printf("Population size: %d\n", ga.get_population_size());
    ga.run();
    return 0;
}