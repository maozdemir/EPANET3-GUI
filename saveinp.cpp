#include "saveinp.h"

void Saver::saveInputFile(std::string saveFile, Project &project)
{
    std::ofstream outputFile(saveFile);

    outputFile << "[TITLE]" << std::endl;
    outputFile << "Scenario: " << project.title.scenario << std::endl;
    outputFile << "Date: " << project.title.date << std::endl;
    outputFile << std::endl;

    outputFile << "[JUNCTIONS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Elev"
               << std::left << std::setw(20) << "Demand"
               << std::left << std::setw(20) << "Pattern" << std::endl;

    for (auto &node : project.nodes)
    {
        if (node.type == NodeType::JUNCTION)
        {
            outputFile << std::left << std::setw(20) << node.id
                       << std::left << std::setw(20) << std::fixed << std::setprecision(10) << node.elevation
                       << std::left << std::setw(20) << std::fixed << std::setprecision(10) << node.demand
                       << std::left << std::setw(20) << node.pattern << ";" << std::endl;
        }
    }
    outputFile << std::endl;

    outputFile << "[RESERVOIRS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Head"
               << std::left << std::setw(20) << "Pattern" << std::endl;
    for (auto &node : project.nodes)
    {
        if (node.type == NodeType::RESERVOIR)
        {
            outputFile << std::left << std::setw(20) << node.id
                       << std::left << std::setw(20) << std::fixed << std::setprecision(10) << node.head
                       << std::left << std::setw(20) << node.pattern << ";" << std::endl;
        }
    }
    outputFile << std::endl;

    outputFile << "[TANKS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Head"
               << std::left << std::setw(20) << "Pattern" << std::endl;
    for (auto &node : project.nodes)
    {
        if (node.type == NodeType::TANK)
        {
            outputFile << std::left << std::setw(20) << node.id
                       << std::left << std::setw(20) << std::fixed << std::setprecision(10) << node.head
                       << std::left << std::setw(20) << node.pattern << ";" << std::endl;
        }
    }
    outputFile << std::endl;

    outputFile << "[PIPES]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Node1"
               << std::left << std::setw(20) << "Node2"
               << std::left << std::setw(20) << "Length"
               << std::left << std::setw(20) << "Diameter"
               << std::left << std::setw(20) << "Roughness"
               << std::left << std::setw(20) << "MinorLoss"
               << std::left << std::setw(20) << "Status" << std::endl;
    for (auto &pipe : project.pipes)
    {
        outputFile << std::left << std::setw(20) << pipe.id
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << pipe.start->id
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << pipe.end->id
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << pipe.length
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << pipe.diameter
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << pipe.roughness
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << pipe.minor_loss
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << ((pipe.pipe_status == PipeStatus::OPEN) ? "Open" : "Closed") << ";"
                   << std::endl;
    }
    outputFile << std::endl;

    outputFile << "[PUMPS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Node1"
               << std::left << std::setw(20) << "Node2"
               << std::left << std::setw(20) << "Parameters" << std::endl;
    outputFile << std::endl;

    outputFile << "[VALVES]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Node1"
               << std::left << std::setw(20) << "Node2"
               << std::left << std::setw(20) << "Diameter"
               << std::left << std::setw(20) << "Type"
               << std::left << std::setw(20) << "Setting"
               << std::left << std::setw(20) << "MinorLoss" << std::endl;

    outputFile << std::left << std::setw(20) << "1167080"
               << std::left << std::setw(20) << "1167080-A"
               << std::left << std::setw(20) << "1167080-B"
               << std::left << std::setw(20) << "600"
               << std::left << std::setw(20) << "DPRV"
               << std::left << std::setw(20) << "FO"
               << std::left << std::setw(20) << "44"
               << std::endl;

    outputFile << std::left << std::setw(20) << "1167139"
               << std::left << std::setw(20) << "1167139-A"
               << std::left << std::setw(20) << "1167139-B"
               << std::left << std::setw(20) << "400"
               << std::left << std::setw(20) << "DPRV"
               << std::left << std::setw(20) << "FO"
               << std::left << std::setw(20) << "42"
               << std::endl;

    outputFile << std::left << std::setw(20) << "1167144"
               << std::left << std::setw(20) << "1167144-A"
               << std::left << std::setw(20) << "1167144-B"
               << std::left << std::setw(20) << "400"
               << std::left << std::setw(20) << "PRV"
               << std::left << std::setw(20) << "1000"
               << std::left << std::setw(20) << "0"
               << std::endl;
    outputFile << std::endl;

    outputFile << "[TAGS]" << std::endl;
    outputFile << std::endl;

    // !!! DEMANDS HERE !!!
    outputFile << "[DEMANDS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Junction"
               << std::left << std::setw(20) << "Demand"
               << std::left << std::setw(20) << "Pattern"
               << std::left << std::setw(20) << "Category";
    outputFile << std::endl;

    for (auto &demand : project.demands)
    {
        outputFile << std::left << std::setw(20) << demand.junction->id
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << demand.demand
                   << std::left << std::setw(20) << demand.pattern->id
                   << std::left << std::setw(20) << demand.category
                   << std::endl;
    }
    outputFile << std::endl;

    // !!! STATUS HERE !!!

    outputFile << "[PATTERNS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "Status/Setting";
    outputFile << std::left << std::setw(20) << "1167144"
               << std::left << std::setw(20) << "Open";
    outputFile << std::endl;

    // !!! PATTERNS HERE !!!

    outputFile << "[PATTERNS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Node"
               << std::left << std::setw(20) << "Multipliers";

    {
        size_t elems_per_row = 6;
        for (size_t i = 0; i < project.patterns.size(); i++)
        {
            double max_rows = ceil(project.patterns[i].multipliers.size() / double(elems_per_row));
            // printf("max_rows = %f\n", max_rows);
            for (size_t j = 0; j < max_rows; j++)
            {
                outputFile << std::endl
                           << std::left << std::setw(20) << project.patterns[i].id;
                for (size_t k = j * elems_per_row; k < ((j + 1) * elems_per_row) && k < project.patterns[i].multipliers.size(); k++)
                {
                    outputFile << std::left << std::setw(20) << project.patterns[i].multipliers[k];
                }
            }
        }
    }
    outputFile << std::endl;
    outputFile << std::endl;

    // !!! LEAKAGE HERE !!!

    outputFile << "[CURVES]" << std::endl;
    outputFile << std::left << std::setw(20) << ";ID"
               << std::left << std::setw(20) << "X-Value"
               << std::left << std::setw(20) << "Y-Value" << std::endl;

    // !!! CURVES HERE !!!

    outputFile << "[CONTROLS]" << std::endl;
    outputFile << std::endl;

    outputFile << "[RULES]" << std::endl;
    outputFile << std::endl;

    outputFile << "[ENERGY]" << std::endl;
    outputFile << std::left << std::setw(20) << "Global Efficiency"
               << std::left << std::setw(20) << "75" << std::endl;
    outputFile << std::left << std::setw(20) << "Global Price"
               << std::left << std::setw(20) << "0" << std::endl;
    outputFile << std::left << std::setw(20) << "Demand Charge"
               << std::left << std::setw(20) << "0" << std::endl;
    outputFile << std::endl;

    outputFile << "[EMITTERS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Junction"
               << std::left << std::setw(20) << "Coefficient"
               << std::endl;

    for (auto &emitter : project.emitters)
    {
        outputFile << std::left << std::setw(20) << emitter.junction->id
                   << std::left << std::setw(20) << std::fixed << std::setprecision(10) << emitter.coefficient
                   << std::endl;
    }
    outputFile << std::endl;

    outputFile << "[QUALITY]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Node"
               << std::left << std::setw(20) << "InitQual" << std::endl;
    outputFile << std::endl;

    outputFile << "[SOURCES]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Node"
               << std::left << std::setw(20) << "Type"
               << std::left << std::setw(20) << "Quality"
               << std::left << std::setw(20) << "Pattern" << std::endl;
    outputFile << std::endl;

    outputFile << "[REACTIONS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Type"
               << std::left << std::setw(20) << "Pipe/Tank"
               << std::left << std::setw(20) << "Coefficient" << std::endl;
    outputFile << std::endl;

    outputFile << "[REACTIONS]" << std::endl;
    outputFile << std::left << std::setw(30) << "Order Bulk"
               << std::left << std::setw(30) << "1" << std::endl;
    outputFile << std::left << std::setw(30) << "Order Tank"
               << std::left << std::setw(30) << "1" << std::endl;
    outputFile << std::left << std::setw(30) << "Order Wall"
               << std::left << std::setw(30) << "1" << std::endl;
    outputFile << std::left << std::setw(30) << "Global Bulk"
               << std::left << std::setw(30) << "0" << std::endl;
    outputFile << std::left << std::setw(30) << "Global Wall"
               << std::left << std::setw(30) << "0" << std::endl;
    outputFile << std::left << std::setw(30) << "Limiting Potential"
               << std::left << std::setw(30) << "0" << std::endl;
    outputFile << std::left << std::setw(30) << "Roughness Correlation"
               << std::left << std::setw(30) << "0" << std::endl;
    outputFile << std::endl;

    outputFile << "[MIXING]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Tank"
               << std::left << std::setw(20) << "Model" << std::endl;
    outputFile << std::endl;

    outputFile << "[TIMES]" << std::endl;
    outputFile << std::left << std::setw(20) << "Duration"
               << std::left << std::setw(20) << "24:00" << std::endl;
    outputFile << std::left << std::setw(20) << "Hydraulic Timestep"
               << std::left << std::setw(20) << "0:15" << std::endl;
    outputFile << std::left << std::setw(20) << "Quality Timestep"
               << std::left << std::setw(20) << "0:00" << std::endl;
    outputFile << std::left << std::setw(20) << "Pattern Timestep"
               << std::left << std::setw(20) << "0:15" << std::endl;
    outputFile << std::left << std::setw(20) << "Pattern Start"
               << std::left << std::setw(20) << "0:00" << std::endl;
    outputFile << std::left << std::setw(20) << "Report Timestep"
               << std::left << std::setw(20) << "1:00" << std::endl;
    outputFile << std::left << std::setw(20) << "Report Start"
               << std::left << std::setw(20) << "0:00" << std::endl;
    outputFile << std::left << std::setw(20) << "Start ClockTime"
               << std::left << std::setw(20) << "12 am" << std::endl;
    outputFile << std::left << std::setw(20) << "Statistic"
               << std::left << std::setw(20) << "NONE" << std::endl;
    outputFile << std::endl;

    outputFile << "[REPORT]" << std::endl;
    outputFile << std::left << std::setw(20) << "Status"
               << std::left << std::setw(20) << "No"
               << std::endl;
    outputFile << std::left << std::setw(20) << ";Trials"
               << std::left << std::setw(20) << "Yes"
               << std::endl;
    outputFile << std::left << std::setw(20) << "Summary"
               << std::left << std::setw(20) << "No"
               << std::endl;
    outputFile << std::left << std::setw(20) << "Page"
               << std::left << std::setw(20) << "0"
               << std::endl;
    outputFile << std::left << std::setw(20) << ";Nodes"
               << std::left << std::setw(20) << "All"
               << std::endl;
    outputFile << std::left << std::setw(20) << ";Links"
               << std::left << std::setw(20) << "All"
               << std::endl;
    outputFile << std::endl;

    outputFile << "[OPTIONS]" << std::endl;
    outputFile << std::left << std::setw(30) << "Flow_Units"
               << std::left << std::setw(30) << "LPS"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Pressure_Units"
               << std::left << std::setw(30) << "METERS"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Headloss_Model"
               << std::left << std::setw(30) << "H-W"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Specific_Gravity"
               << std::left << std::setw(30) << "0.998"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Specific_Viscosity"
               << std::left << std::setw(30) << "0.982157860636364"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Maximum_Trials"
               << std::left << std::setw(30) << "100"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Relative_Accuracy"
               << std::left << std::setw(30) << "0.001"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Unbalanced"
               << std::left << std::setw(30) << "Continue"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Demand_Pattern"
               << std::left << std::setw(30) << "1"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Demand_Multiplier"
               << std::left << std::setw(30) << "1.0"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Quality_Model"
               << std::left << std::setw(30) << "None mg/L"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Specific_Diffusivity"
               << std::left << std::setw(30) << "1"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Quality_Tolerance"
               << std::left << std::setw(30) << "0.001"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Head_Tolerance"
               << std::left << std::setw(30) << "0.001"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Flow_Tolerance"
               << std::left << std::setw(30) << "0.001"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Flow_Change_Limit"
               << std::left << std::setw(30) << "0"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Step_Sizing"
               << std::left << std::setw(30) << "FULL"
               << std::endl;
    outputFile << std::left << std::setw(30) << "VALVE_REP_TYPE"
               << std::left << std::setw(30) << "Cd"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Demand_Model"
               << std::left << std::setw(30) << "POWER"
               << std::endl;
    outputFile << std::left << std::setw(30) << "LEAKAGE_MODEL"
               << std::left << std::setw(30) << "FAVAD"
               << std::endl;
    outputFile << std::left << std::setw(30) << "MINIMUM_PRESSURE"
               << std::left << std::setw(30) << "0"
               << std::endl;
    outputFile << std::left << std::setw(30) << "SERVICE_PRESSURE"
               << std::left << std::setw(30) << "0.1"
               << std::endl;
    outputFile << std::left << std::setw(30) << "PRESSURE_EXPONENT"
               << std::left << std::setw(30) << "0.5"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Emitter_Exponent"
               << std::left << std::setw(30) << "1.18"
               << std::endl;
    outputFile << std::left << std::setw(30) << "LEAKAGE_COEFF1"
               << std::left << std::setw(30) << "0"
               << std::endl;
    outputFile << std::left << std::setw(30) << "LEAKAGE_COEFF2"
               << std::left << std::setw(30) << "0"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Hyd_Solver"
               << std::left << std::setw(30) << "RWCGGA"
               << std::endl;
    outputFile << std::left << std::setw(30) << "Time_Weight"
               << std::left << std::setw(30) << "0.5"
               << std::endl;
    outputFile << std::left << std::setw(30) << "TEMP_DISC_PARA"
               << std::left << std::setw(30) << "1"
               << std::endl;
    outputFile << std::endl;

    outputFile << "[COORDINATES]" << std::endl;
    outputFile << std::left << std::setw(20) << ";Node"
               << std::left << std::setw(20) << "X-Coord"
               << std::left << std::setw(20) << "Y-Coord"
               << std::endl;

    for (auto &node : project.nodes)
    {

        outputFile << std::left << std::setw(20) << node.id
                   << std::left << std::setw(20) << std::fixed << std::setprecision(3) << node.x_coord
                   << std::left << std::setw(20) << std::fixed << std::setprecision(3) << node.y_coord
                   << std::endl;
    }
    outputFile << std::endl;

    // !!! VERTICES HERE !!! - not important tho.
    outputFile << "[VERTICES]" << std::endl;
    outputFile << std::endl;

    outputFile
        << "[LABELS]" << std::endl;
    outputFile << std::left << std::setw(20) << ";X-Coord"
               << std::left << std::setw(20) << "Y-Coord"
               << std::left << std::setw(20) << "Label & Anchor Node"
               << std::endl;
    outputFile << std::endl;

    outputFile << "[BACKDROP]" << std::endl;
    outputFile << std::left << std::setw(20) << "DIMENSIONS"
               << std::left << std::setw(20) << "402027.617"
               << std::left << std::setw(20) << "4538122.089"
               << std::left << std::setw(20) << "405357.875"
               << std::left << std::setw(20) << "4541011.644"
               << std::endl;
    outputFile << std::left << std::setw(20) << "UNITS"
               << std::left << std::setw(20) << "None"
               << std::endl;
    outputFile << std::left << std::setw(20) << "FILE"
               << std::left << std::setw(20) << ""
               << std::endl;
    outputFile << std::left << std::setw(20) << "OFFSET"
               << std::left << std::setw(20) << "0.00"
               << std::left << std::setw(20) << "0.00"
               << std::endl;
    outputFile << std::endl;

    outputFile << "[END]" << std::endl;

    outputFile.close();
}