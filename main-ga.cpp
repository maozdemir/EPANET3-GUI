#include <iostream>
#include "parseinp.h"
#include "epanet-ga.h"

int main(int, char **)
{
    Title title;
    std::vector<Node> nodes;
    std::vector<Pipe> pipes;
    Project project;
    Parser::parseInputFile("EPA3-hk-large-peaked-intermediate.inp",title,nodes,pipes,project);
    printf("\n");
    printf("Size of nodes: %d\n",nodes.size());
    GeneticAlgorithm ga;
    ga.set_chromosome_length(pipes.size());
    printf("Chromosome length: %d\n", ga.get_chromosome_length());
    system("pause");
    return 0;
}