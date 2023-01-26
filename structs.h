#ifndef STRUCTS_H
#define STRUCTS_H

#include "enums.h"
#include "imgui.h"
#include <cmath>
#include <string>
#include <vector>

struct Title
{
    std::string scenario;
    std::string date;
};

struct Node
{
    std::string id;
    double x_coord;
    double y_coord;
    char *description;
    char *tag;
    double elevation;
    NodeType type;
    double demand;
    std::string pattern;
    double head;
    double init_level;
    double min_level;
    double max_level;
    double diameter;
    double min_vol;
    std::string vol_curve;
    std::string overflow;
};

struct Annotation
{
    ImVec2 position;
    std::string text;

    Annotation(const ImVec2 &position, const std::string &text)
        : position(position), text(text)
    {
    }
};

struct Pattern
{
    std::string id;
    std::vector<double> multipliers;
};

struct Pipe
{

    Node *start;
    Node *end;
    int id;
    LineType lineType;

    double length;
    double diameter;
    double roughness;
    double minor_loss;
    PipeStatus pipe_status;
    Pipe(Node *start, Node *end, const int &id, const LineType &lineType, const double &length, const double &diameter, const double &roughness, const double &minor_loss, const PipeStatus &pipe_status)
        : start(start), end(end), id(id), lineType(lineType), length(length), diameter(diameter), roughness(roughness), minor_loss(minor_loss), pipe_status(pipe_status)
    {
    }

    double getAngle() const
    {
        return atan2(end->y_coord - start->y_coord, end->x_coord - start->x_coord);
    }

    ImVec2 getCenterPoint() const
    {
        return ImVec2((float)(start->x_coord + end->x_coord) / 2, (float)(start->y_coord + end->y_coord) / 2);
    }
};

struct Pump
{
    int id;
    Node *start;
    Node *end;
    std::string parameters;
};

struct Valve
{
    int id;
    Node *start;
    Node *end;
    double diameter;
    ValveType type;
    double outlet_pressure;
};

struct Tag
{
};

struct Demand
{
    Node *junction;
    double demand;
    Pattern *pattern;
    std::string category;
};

struct Emitter {
    Node *junction;
    double coefficient;
};

struct Status
{
    int id;
    Status *status;
};

struct Options
{
    FlowUnit flow_unit = FlowUnit::UNDEFINED;
    HeadlossModel headloss_model = HeadlossModel::UNDEFINED;
    double specific_gravity = 1.0;
    double specific_viscosity = 1.0;
    int maximum_trials = 1;
    double accuracy = 0.001;
};

struct Curve
{
    int id;
    double x_value;
    double y_value;
};

struct Control
{
};

struct Leakage
{
    int link_id;
    double coefficient_1;
    double coefficient_2;
};

struct Rule
{
};
struct Energy
{
    double global_efficiency;
    double global_price;
    double demand_charge;
};

struct Quality
{
    Node *node;
    double initial_quality;
};

struct Source
{
    Node *node;
    std::string type;
    Quality *quality;
    Pattern *pattern;
};

struct Reaction
{
    std::string type;
    std::string pipe_or_tank;
    double coefficient;
};

struct Reaction_2
{
    int order_bulk;
    int order_tank;
    int order_wall;
    int global_bulk;
    int global_wall;
    int limiting_potential;
    int roughness_correction;
};

// ? WIP
struct Project
{
    Title title;
    Options options;
    std::vector<Node> nodes;
    std::vector<Pattern> patterns;
    std::vector <Emitter> emitters;
    std::vector<Demand> demands;
};

#endif