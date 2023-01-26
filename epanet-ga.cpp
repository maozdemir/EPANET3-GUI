#include "epanet-ga.h"

void GeneticAlgorithm::initializePopulation()
{
    dist = std::uniform_real_distribution<double>(0.0, 1.0);
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        std::vector<double> individual;
        for (int j = 0; j < CHROMOSOME_LENGTH; j++)
        {
            individual.push_back(dist(rng));
        }
        population.push_back(individual);
    }
}

double GeneticAlgorithm::calculateFitness(std::string inputFile, std::string outputFile)
{
    trigger_run(inputFile, outputFile);
    double fitness = 0;
    return fitness;
}

void GeneticAlgorithm::run()
{
    initializePopulation();
    calculateFitness("resources\\newInputFile.inp", "a.out");
}

void GeneticAlgorithm::trigger_run(std::string inputFile, std::string outputFile)
{
    std::string command = "resources\\run-epanet3.exe " + inputFile + " resources\\report.rpt " + outputFile;
    system(command.c_str());
}