#include "epanet-ga.h"

void GeneticAlgorithm::initialize_population()
{
    DISTRIBUTION_ROUGHNESS = std::uniform_real_distribution<double>(0.0, 150.0);
    DISTRIBUTION_EMITTER = std::uniform_real_distribution<double>(0.0, 0.001);
    Parser::parseResultTxt("res.txt", observed_atakoy_vector);

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        population.push_back(first_individual);
        for (int j = 0; j < CHROMOSOME_LENGTH_PIPES; j++)
        {
            population[i].project.pipes[j].roughness = (DISTRIBUTION_ROUGHNESS(RNG));
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_EMITTERS; j++)
        {
            population[i].project.emitters[j].coefficient = (DISTRIBUTION_EMITTER(RNG));
        }
    }
}

void GeneticAlgorithm::calculate_fitness()
{
    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        double fitness = 0;
        double error = 0;
        char resultFile[100] = "";
        sprintf_s(resultFile, "resources\\result_%d.txt", i);
        std::vector<Result_Atakoy> result_atakoy_vector;
        Parser::parseResultTxt(resultFile, result_atakoy_vector);
        for (int j = 0; j < observed_atakoy_vector.size(); j++)
        {
            error += abs(result_atakoy_vector[j].Inc_FR_ - observed_atakoy_vector[j].Inc_FR_);
        }
        fitness = error / observed_atakoy_vector.size();
        fitness = 1 / error;
        population[i].fitness = fitness;
        printf("[ -- %d -- ] - CALCULATED FITNESS: %f\n", i, fitness);
    }
}
void GeneticAlgorithm::threader()
{
    std::vector<std::thread> threads;
    printf("Populating treads...\n");
    for (int i = 0; i < POPULATION_SIZE; ++i)
    {
        threads.emplace_back(std::thread(&GeneticAlgorithm::for_each_individual, this, i));
        printf("Thread %d started.\n", i);
    }
    printf("Total threads: %d\n", threads.size());
    for (auto &th : threads)
        th.join();
}

void GeneticAlgorithm::mutate()
{
    for (int i = 0; i < population.size(); i++)
    {
        for (int j = 0; j < CHROMOSOME_LENGTH_PIPES; j++)
        {
            double randomNum = DISTRIBUTION_MUTATION(RNG);
            if (randomNum < MUTATION_RATE)
            {
                double newRoughness = DISTRIBUTION_ROUGHNESS(RNG);
                population[i].project.pipes[j].roughness = newRoughness;
            }
        }
        for (int j = 0; j < CHROMOSOME_LENGTH_EMITTERS; j++)
        {
            double randomNum = DISTRIBUTION_MUTATION(RNG);
            if (randomNum < MUTATION_RATE)
            {
                double newEmitterCoeff = DISTRIBUTION_EMITTER(RNG);
                population[i].project.emitters[j].coefficient = newEmitterCoeff;
            }
        }
    }
}

void GeneticAlgorithm::for_each_individual(int i)
{
    char fileName[100] = "";
    char result[100] = "";
    sprintf_s(fileName, "resources\\ga_inp_%d.inp", i);
    sprintf_s(result, "resources\\result_%d.txt", i);
    Saver::saveInputFile(fileName, population[i].project);
    trigger_run(fileName, result);
}

std::vector<Individual> GeneticAlgorithm::parent_selection_tournament()
{
    std::vector<Individual> parents;

    for (int i = 0; i < POPULATION_SIZE; i++)
    {
        int idx1 = rand() % POPULATION_SIZE;
        int idx2 = rand() % POPULATION_SIZE;

        Individual parent = population[idx1].fitness > population[idx2].fitness ? population[idx1] : population[idx2];

        parents.push_back(parent);
    }

    return parents;
}

Individual GeneticAlgorithm::crossover(Individual parent1, Individual parent2)
{
    std::uniform_int_distribution<int> distribution_pipes(0, CHROMOSOME_LENGTH_PIPES - 1);
    std::uniform_int_distribution<int> distribution_emitters(0, CHROMOSOME_LENGTH_EMITTERS - 1);
    int crossoverPoint = distribution_pipes(RNG);
    int crossoverPoint2 = distribution_emitters(RNG);
    Individual child = parent1;
    std::vector<Individual> children;
    for (int j = 0; j < crossoverPoint; j++)
    {
        child.project.pipes[j] = parent1.project.pipes[j];
    }
    for (int j = crossoverPoint; j < CHROMOSOME_LENGTH_PIPES; j++)
    {
        child.project.pipes[j] = parent2.project.pipes[j];
    }
    for (int j = 0; j < crossoverPoint2; j++)
    {
        child.project.emitters[j] = parent1.project.emitters[j];
    }
    for (int j = crossoverPoint2; j < CHROMOSOME_LENGTH_EMITTERS; j++)
    {
        child.project.emitters[j] = parent2.project.emitters[j];
    }
    return child;
}

void GeneticAlgorithm::run()
{
    initialize_population();
    int num_runs = 0;
    while (num_runs < MAX_RUNS)
    {
        auto start = std::chrono::high_resolution_clock::now();

        threader();
        calculate_fitness();
        printf("Ran for %d/%d runs\n", num_runs + 1, MAX_RUNS);

        auto end = std::chrono::high_resolution_clock::now();

        log_results("results.txt", num_runs, population, end - start);
        // system("pause");
        std::vector<Individual> parents = parent_selection_tournament();
        printf("Parents: %d\n", parents.size());
        elitism();
        // breed_from_elites();
        // mutate();
        POPULATION_SIZE = population.size();
        num_runs++;
    }
}

void GeneticAlgorithm::trigger_run(std::string inputFile, std::string resultFile)
{
    std::string command = "resources\\run-epanet3.exe " + inputFile + " resources\\report.rpt " + resultFile;
    system(command.c_str());
}

void GeneticAlgorithm::elitism()
{
    // Sort the current population by fitness score in descending order
    std::sort(population.begin(), population.end(), [](const Individual &p1, const Individual &p2)
              { return p1.fitness > p2.fitness; });
    BEST_FITNESS = population[0].fitness;

    // Select the top X elite individuals and add them to a new population
    std::vector<Individual> newPopulation;
    int eliteCount = 2;
    std::vector<Individual> parents;
    std::vector<Individual> elites;
    for (int i = 0; i < eliteCount; i++)
    {
        elites.push_back(population.at(0));
    }
    for (int i = 0; i < POPULATION_SIZE / 2; i++)
    {
        parents.push_back(population.at(i));
    }
    newPopulation = elites;

    while (newPopulation.size() < POPULATION_SIZE - eliteCount)
    {
        // Select two parents for crossover
        int idx1 = rand() % parents.size();
        int idx2 = rand() % parents.size();
        Individual parent1 = parents[idx1];
        Individual parent2 = parents[idx2];

        // Perform crossover to create an offspring
        Individual offspring = crossover(parent1, parent2);

        // Add the offspring to the new population
        newPopulation.push_back(offspring);
    }
    population = newPopulation;
    adaptiveMutation();
    for (int i = 0; i < eliteCount; i++)
    {
        population.push_back(elites.at(i));
    }
}

double calculate_stddev(const std::vector<Individual> &population)
{
    double sum = 0;
    double mean;
    double variance = 0;
    double stdDev;
    for (int i = 0; i < population.size(); i++)
    {
        sum += population[i].fitness;
    }
    mean = sum / population.size();
    for (int i = 0; i < population.size(); i++)
    {
        variance += pow(population[i].fitness - mean, 2);
    }
    variance = variance / population.size();
    stdDev = sqrt(variance);
    return stdDev;
}

void GeneticAlgorithm::adaptiveMutation()
{
    double stdDev = calculate_stddev(population);
    double new_mutation_rate = MUTATION_RATE;
    MUTATION_RATE += (stdDev * ADAPTATION_FACTOR);
    MUTATION_RATE = std::max(MIN_MUTATION_RATE, std::min(MUTATION_RATE, MAX_MUTATION_RATE));
    mutate();
}

void GeneticAlgorithm::log_results(std::string log_file, int run, std::vector<Individual> population, std::chrono::duration<long long, std::nano> runtime)
{
    std::ofstream outputFile(log_file, std::ios::app);
    outputFile << "[RUN " << run << "]" << std::endl;
    for (int i = 0; i < population.size(); i++)
    {
        outputFile << std::left << std::setw(20) << i
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << population[i].fitness
                   << std::endl;
    }
    std::sort(population.begin(), population.end(), [](const Individual &p1, const Individual &p2)
              { return p1.fitness > p2.fitness; });
    outputFile << "The best individual had "
               << std::setprecision(10) << population[0].fitness << " fitness."
               << std::endl;
    outputFile << "This run took "
               << std::setprecision(10) << std::chrono::duration_cast<std::chrono::microseconds>(runtime).count() << " microseconds.";
    outputFile << "("
               << std::setprecision(10) << std::chrono::duration_cast<std::chrono::seconds>(runtime).count() << " seconds)"
               << std::endl;
    outputFile << "Mutation rate was: "
               << std::setprecision(10) << MUTATION_RATE << " seconds)"
               << std::endl;

    outputFile << std::endl;
    outputFile << std::endl;
    outputFile << std::endl;
}