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
    ga.set_chromosome_length(1);
    printf("Chromosome length: %d\n", ga.get_chromosome_length());
    printf("Population size: %d\n", ga.get_population_size());
    std::vector<Result_Atakoy> result_atakoy_vector;
    Parser::parseResultTxt("ak-Sonuc.txt", result_atakoy_vector);
    printf("Size of result: %d\n", result_atakoy_vector.size());
    ga.run();
    /*std::vector<Pipe> pipes_after_initialization = ga.get_population();
    for (int i = 0; i < pipes_after_initialization.size(); i++) {
        printf("[%d] Original roughness: %f && Modified roughness: %f\n", i, pipes[i].roughness, pipes_after_initialization[i].roughness);
    }*/
    //system("pause");
    return 0;
}