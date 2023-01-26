#ifndef EPANET_GA_H
#define EPANET_GA_H

#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "structs.h"

class GeneticAlgorithm
{
public:
    int POPULATION_SIZE = 100;
    int CHROMOSOME_LENGTH = 100;
    double MUTATION_RATE = 0.01;

    std::vector<std::vector<double>> population;
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;

    void set_chromosome_length(int i)
    {
        CHROMOSOME_LENGTH = i;
    }
    void set_population_size(int i)
    {
        POPULATION_SIZE = i;
    }

    int get_chromosome_length() {
        return CHROMOSOME_LENGTH;
    }

    void initializePopulation();
    double calculateFitness(std::string inputFile, std::string outputFile);
    void trigger_run(std::string inputFile, std::string outputFile);
    void run();
};

#endif