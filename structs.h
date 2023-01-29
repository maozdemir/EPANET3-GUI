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
    long double x_coord;
    long double y_coord;
    char *description;
    char *tag;
    long double elevation;
    NodeType type;
    long double demand;
    std::string pattern;
    long double head;
    long double init_level;
    long double min_level;
    long double max_level;
    long double diameter;
    long double min_vol;
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
    std::vector<long double> multipliers;
};

struct Pipe
{

    Node *start;
    Node *end;
    int id;
    LineType lineType;

    long double length;
    long double diameter;
    long double roughness;
    long double minor_loss;
    PipeStatus pipe_status;
    Pipe(Node *start, Node *end, const int &id, const LineType &lineType, const long double &length, const long double &diameter, const long double &roughness, const long double &minor_loss, const PipeStatus &pipe_status)
        : start(start), end(end), id(id), lineType(lineType), length(length), diameter(diameter), roughness(roughness), minor_loss(minor_loss), pipe_status(pipe_status)
    {
    }

    long double getAngle() const
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
    long double diameter;
    ValveType type;
    long double outlet_pressure;
};

struct Tag
{
};

struct Demand
{
    Node *junction;
    long double demand;
    Pattern *pattern;
    std::string category;
};

struct Emitter {
    Node *junction;
    long double coefficient;
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
    long double specific_gravity = 1.0;
    long double specific_viscosity = 1.0;
    int maximum_trials = 1;
    long double accuracy = 0.001;
};

struct Curve
{
    int id;
    long double x_value;
    long double y_value;
};

struct Control
{
};

struct Leakage
{
    int link_id;
    long double coefficient_1;
    long double coefficient_2;
};

struct Rule
{
};
struct Energy
{
    long double global_efficiency;
    long double global_price;
    long double demand_charge;
};

struct Quality
{
    Node *node;
    long double initial_quality;
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
    long double coefficient;
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
    std::vector<Pipe> pipes;
};

#endif