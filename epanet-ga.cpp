#include "epanet-ga.h"

void GeneticAlgorithm::initializePopulation()
{
    DISTRIBUTION = std::uniform_real_distribution<double>(50.0, 150.0);
    Parser::parseResultTxt("res.txt", observed_atakoy_vector);
    FITNESS.reserve(POPULATION_SIZE);
    FITNESS.resize(POPULATION_SIZE);

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        population.push_back(first_individual);
        for (int j = 0; j < CHROMOSOME_LENGTH; j++)
        {
            population[i].pipes[j].roughness = (DISTRIBUTION(RNG));
        }
    }
}

void GeneticAlgorithm::calculateFitness()
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        double fitness = 0;
        double error = 0;
        char resultFile[100] = "";
        sprintf_s(resultFile, "resources\\result_%d.txt", i);
        std::vector<Result_Atakoy> result_atakoy_vector;
        Parser::parseResultTxt(resultFile, result_atakoy_vector);
        // Calculate the MAE between simulated and observed results
        for (int j = 0; j < observed_atakoy_vector.size(); j++)
        {
            error += abs(result_atakoy_vector[j].Inc_FR_ - observed_atakoy_vector[j].Inc_FR_);
        }
        fitness = error / observed_atakoy_vector.size();
        FITNESS[i] = fitness;
        printf("[ -- %d -- ] - CALCULATED FITNESS: %f\n", i, fitness);
    }
}
void GeneticAlgorithm::threader()
{
    std::vector<std::thread> threads;
    printf("Populating treads...\n");
    for (int i = 0; i < POPULATION_SIZE; ++i)
    {
        threads.emplace_back(std::thread(&GeneticAlgorithm::forEachIndividual, this, i));
        printf("Thread %d started.\n", i);
    }
    printf("Total threads: %d\n", threads.size());
    for (auto &th : threads)
        th.join();
}

void GeneticAlgorithm::mutate()
{
    std::normal_distribution<double> mutationDistribution(0, MUTATION_RATE);
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        for (int j = 0; j < CHROMOSOME_LENGTH; j++)
        {
            double randomNum = DISTRIBUTION(RNG);
            if (randomNum < MUTATION_RATE)
            {
                std::uniform_real_distribution<double> roughnessDistribution(50, 150);
                double newRoughness = roughnessDistribution(RNG);
                population[i].pipes[j].roughness = newRoughness;
            }
        }
    }
}

void GeneticAlgorithm::forEachIndividual(int i)
{
    char fileName[100] = "";
    char result[100] = "";
    sprintf_s(fileName, "resources\\ga_inp_%d.inp", i);
    sprintf_s(result, "resources\\result_%d.txt", i);
    Saver::saveInputFile(fileName, population[i]);
    trigger_run(fileName, result);
}

std::vector<Project> GeneticAlgorithm::parent_selection_tournament()
{
    std::vector<Project> parents;

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        int idx1 = rand() % POPULATION_SIZE;
        int idx2 = rand() % POPULATION_SIZE;

        Project parent = FITNESS[idx1] > FITNESS[idx2] ? population[idx1] : population[idx2];

        parents.push_back(parent);
    }

    return parents;
}

/*void GeneticAlgorithm::crossover()
{
    std::uniform_int_distribution<int> distribution(0, CHROMOSOME_LENGTH - 1);
    int crossoverPoint = distribution(RNG);
    for (int i = 0; i < crossoverPoint; i++)
    {
        child1.pipes[i] = parent1.pipes[i];
        child2.pipes[i] = parent2.pipes[i];
    }

    for (int i = crossoverPoint; i < CHROMOSOME_LENGTH; i++)
    {
        child1.pipes[i] = parent2.pipes[i];
        child2.pipes[i] = parent1.pipes[i];
    }
}*/

void GeneticAlgorithm::run()
{
    initializePopulation();
    int num_runs = 0;
    while (num_runs < MAX_RUNS)
    {
        threader();
        printf("Ran for %d/%d runs\n", num_runs + 1, MAX_RUNS);
        calculateFitness();
        num_runs++;
    }
}

void GeneticAlgorithm::trigger_run(std::string inputFile, std::string resultFile)
{
    std::string command = "resources\\run-epanet3.exe " + inputFile + " resources\\report.rpt " + resultFile;
    system(command.c_str());
}