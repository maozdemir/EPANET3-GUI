#include "parseinp.h"

void Parser::parseInputFile(std::string inputFile, Title &title, std::vector<Node> &nodes, std::vector<Pipe> &pipe_vector, Project &project)
{
    parsePatterns(project, inputFile);
    std::ifstream file(inputFile);
    std::string line;
    while (getline(file, line))
    {
        Helpers::trim(line);
        // line = line.erase(line.find_last_not_of("\t\n\v\f\r ") + 1);
        if (line == "[TITLE]")
        {
            getline(file, line);
            /*std::istringstream iss(line);
            std::string scenario;
            iss >> scenario;
            title.scenario = scenario.substr(9);

            getline(file, line);
            std::istringstream iss2(line);
            std::string date;
            iss2 >> date;
            title.date = date.substr(5);*/
        }
        if (line == "[JUNCTIONS]")
        {
            printf("FOUND JUNCTIONS\n");
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                Node node;
                node.id = tokens[0];
                node.elevation = std::stod(tokens[1]);
                node.demand = std::stod(tokens[2]);
                try
                {
                    node.pattern = tokens[3];
                }
                catch (std::invalid_argument &)
                {
                    node.pattern = "";
                }
                catch (std::out_of_range &)
                {
                    node.pattern = "";
                }

                node.type = NodeType::JUNCTION;
                nodes.push_back(node);
                project.nodes.push_back(node);
            }
        }
        if (line == "[RESERVOIRS]")
        {
            printf("FOUND RESERVOIRS\n");
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                Node node;
                node.id = tokens[0].c_str();
                node.head = std::stod(tokens[1]);
                printf("TOKEN0: %s, TOKEN1: %s, TOKEN2: %s\n", tokens[0].c_str(), tokens[1].c_str(), tokens[2].c_str());
                try
                {
                    node.pattern = tokens[2];
                }
                catch (std::invalid_argument &)
                {
                    node.pattern = "";
                }
                catch (std::out_of_range &)
                {
                    node.pattern = "";
                }

                node.type = NodeType::RESERVOIR;
                nodes.push_back(node);
                project.nodes.push_back(node);
            }
        }
        if (line == "[COORDINATES]")
        {
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                const char *id = tokens[0].c_str();
                auto it = std::find_if(nodes.begin(), nodes.end(), [&id](const Node &n)
                                       { return n.id == id; });
                if (it != nodes.end())
                {
                    it->x_coord = std::stod(tokens[1]);
                    it->y_coord = std::stod(tokens[2]);
                }
            }
        }
        if (line == "[PIPES]")
        {
            printf("FOUND PIPES\n");
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                const char *start_node = tokens[1].c_str();
                const char *end_node = tokens[2].c_str();

                auto it1 = std::find_if(nodes.begin(), nodes.end(), [&start_node](const Node &n)
                                        { return n.id == start_node; });

                auto it2 = std::find_if(nodes.begin(), nodes.end(), [&end_node](const Node &n)
                                        { return n.id == end_node; });

                if (it1 != nodes.end() && it2 != nodes.end())
                {
                    double length = std::stod(tokens[3]);
                    double diameter = std::stod(tokens[4]);
                    double roughness = std::stod(tokens[5]);
                    double minor_loss = std::stod(tokens[6]);
                    PipeStatus status = (tokens[7] == "Open") ? PipeStatus::OPEN : PipeStatus::CLOSED;
                    LineType line_type = LineType::PIPE;

                    int pipe_id;
                    std::stringstream ss_pipeid(tokens[0]);
                    ss_pipeid >> pipe_id;

                    Pipe pipe(&(*it1), &(*it2), pipe_id, line_type, length, diameter, roughness, minor_loss, status);
                    pipe_vector.push_back(pipe);
                }
            }
        }
        if (line == "[EMITTERS]")
        {
            printf("FOUND EMITTERS\n");
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                const char *node = tokens[0].c_str();

                double coefficient = std::stod(tokens[1]);

                auto it = std::find_if(nodes.begin(), nodes.end(), [&node](const Node &n)
                                       { return n.id == node; });

                if (it != nodes.end())
                {
                    Emitter emitter;
                    emitter.junction = &(*it);
                    emitter.coefficient = coefficient;
                    project.emitters.push_back(emitter);
                }
            }
        }

        if (line == "[DEMANDS]")
        {
            printf("FOUND DEMANDS\n");
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                std::string junction_id = tokens[0];
                double __demand = std::stod(tokens[1]);
                std::string pattern_id = tokens[2];

                auto it_junction = std::find_if(nodes.begin(), nodes.end(), [&junction_id](const Node &n)
                                                { return n.id == junction_id; });

                auto it_pattern = std::find_if(project.patterns.begin(), project.patterns.end(), [&pattern_id](const Pattern &p)
                                               { return p.id == pattern_id; });

                if (it_junction != nodes.end() && it_pattern != project.patterns.end())
                {
                    Demand _demand;
                    _demand.demand = __demand;
                    _demand.pattern = &(*it_pattern);
                    _demand.junction = &(*it_junction);
                    project.demands.push_back(_demand);
                }
            }
            printf("Found %d Demands\n", (int)project.demands.size());
        }
        if (line == "[REPORT]")
        {
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                std::string option = tokens[0];
                std::string value = tokens[1];
            }
        }

        if (line == "[OPTIONS]")
        {
            Options options;
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;
                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                std::string option = tokens[0];
                std::string value = tokens[1];

                HeadlossModel headlossModel;
                FlowUnit flowUnit;
                std::map<std::string, HeadlossModel> headlossModelMap = {
                    {"H-W", HeadlossModel::H_W},
                    {"D-W", HeadlossModel::D_W},
                    {"C-W", HeadlossModel::C_M}};

                if (option == "Headloss_Model")
                {
                    auto it = headlossModelMap.find(tokens[1]);
                    if (it != headlossModelMap.end())
                    {
                        headlossModel = it->second;
                    }
                    else
                    {
                        headlossModel = HeadlossModel::UNDEFINED;
                    }
                    options.headloss_model = headlossModel;
                }
                std::map<std::string, FlowUnit> flowUnitMap = {
                    {"CFS", FlowUnit::CFS},
                    {"GPM", FlowUnit::GPM},
                    {"MGD", FlowUnit::MGD},
                    {"IMGD", FlowUnit::IMGD},
                    {"AFD", FlowUnit::AFD},
                    {"LPS", FlowUnit::LPS},
                    {"LPM", FlowUnit::LPM},
                    {"MLD", FlowUnit::MLD},
                    {"CMH", FlowUnit::CMH},
                    {"CMD", FlowUnit::CMD}};

                if (option == "Flow_Units")
                {
                    auto it = flowUnitMap.find(tokens[1]);
                    if (it != flowUnitMap.end())
                    {
                        flowUnit = it->second;
                    }
                    else
                    {
                        flowUnit = FlowUnit::UNDEFINED;
                    }
                    options.flow_unit = flowUnit;
                }
                if (option == "Specific_Gravity")
                {
                    options.specific_gravity = std::stod(tokens[1].c_str());
                }
                if (option == "Specific_Viscosity")
                {
                    options.specific_viscosity = std::stod(tokens[1].c_str());
                }
                if (option == "Maximum_Trials")
                {
                    options.maximum_trials = std::stoi(tokens[1].c_str());
                }
            }
            project.options = options;
        }
    }
}

void Parser::parsePatterns(Project &project, std::string inputFile)
{

    std::ifstream file(inputFile);
    std::string line;
    while (getline(file, line))
    {
        if (line == "[PATTERNS]")
        {

            printf("FOUND PATTERNS\n");
            while (getline(file, line))
            {
                Helpers::trim(line);
                if (line[0] == ';')
                    continue;
                if (line.empty())
                    break;

                std::istringstream iss(line);
                std::vector<std::string> tokens;
                std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

                std::string pattern_id = tokens[0];

                auto it = std::find_if(project.patterns.begin(), project.patterns.end(), [&pattern_id](const Pattern &n)
                                       { return n.id == pattern_id; });

                if (it != project.patterns.end())
                {
                    for (int token = 1; std::cmp_less(token, tokens.size()); ++token)
                    {
                        try
                        {
                            (*it).multipliers.push_back(std::stod(tokens[token]));
                        }
                        catch (...)
                        {
                        }
                    }
                }
                else
                {
                    std::vector<double> multipliers;
                    for (int token = 1; std::cmp_less(token, tokens.size()); ++token)
                    {
                        try
                        {
                            multipliers.push_back(std::stod(tokens[token]));
                        }
                        catch (...)
                        {
                        }
                    }
                    Pattern pattern;
                    pattern.id = pattern_id;
                    pattern.multipliers = multipliers;
                    project.patterns.push_back(pattern);
                }
            }
            printf("Read %d patterns\n", (int)project.patterns.size());
        }
    }
}