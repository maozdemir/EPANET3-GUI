#ifndef EPANET_GA_H
#define EPANET_GA_H

#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include "structs.h"
#include "struct_atakoy.h"
#include "parseinp.h"
#include "saveinp.h"

class GeneticAlgorithm
{
public:
    int POPULATION_SIZE = 100;
    int CHROMOSOME_LENGTH = 100;
    double MUTATION_RATE = 0.01;

    std::vector<Pipe> PIPE_POPULATION;
    std::mt19937 RNG;
    std::uniform_real_distribution<double> DISTRIBUTION;
    Project PROJECT;

    void set_chromosome_length(int i)
    {
        this->CHROMOSOME_LENGTH = i;
    }

    int get_chromosome_length() {
        return CHROMOSOME_LENGTH;
    }

    int get_population_size() {
        return POPULATION_SIZE;
    }

    void set_project(Project project) {
        this->PROJECT = project;
        this->PIPE_POPULATION = project.pipes;
        this->POPULATION_SIZE = project.pipes.size();
    }

    std::vector<Pipe> get_population() {
        return PIPE_POPULATION;
    }

    void initializePopulation();
    double calculateFitness(std::string inputFile, std::string outputFile);
    void trigger_run(std::string inputFile, std::string outputFile);
    void run();
};

#endif