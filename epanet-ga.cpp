#include "epanet-ga.h"

void GeneticAlgorithm::initializePopulation()
{
    DISTRIBUTION = std::uniform_real_distribution<double>(50.0, 150.0);
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < CHROMOSOME_LENGTH; j++)
        {
            PIPE_POPULATION[i].roughness = (DISTRIBUTION(RNG));
        }
    }
}

double GeneticAlgorithm::calculateFitness(std::string inputFile, std::string outputFile)
{
    double fitness = 0;
    std::vector<Result_Atakoy> result_atakoy_vector;
    Parser::parseResultTxt("ak-Sonuc.txt", result_atakoy_vector);
    return fitness;
}

void GeneticAlgorithm::run()
{
    initializePopulation();
    PROJECT.pipes = PIPE_POPULATION;
    printf("Project nodes: %d\n", PROJECT.nodes.size());
    Saver::saveInputFile("resources\\ga_inp.inp", PROJECT);
    trigger_run("resources\\ga_inp.inp", "a.out");
    calculateFitness("resources\\ga_inp.inp", "a.out");
}

void GeneticAlgorithm::trigger_run(std::string inputFile, std::string outputFile)
{
    std::string command = "resources\\run-epanet3.exe " + inputFile + " resources\\report.rpt " + outputFile;
    system(command.c_str());
}