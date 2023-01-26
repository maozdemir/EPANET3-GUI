#ifndef ENUMS_H
#define ENUMS_H

enum class FlowUnit
{
    UNDEFINED,
    CFS,
    GPM,
    MGD,
    IMGD,
    AFD,
    LPS,
    LPM,
    MLD,
    CMH,
    CMD
};

enum class HeadlossModel
{
    UNDEFINED,
    H_W,
    D_W,
    C_M
};


enum ValveType
{
    NONE,
    DPRV,
    PRV,
    PBV,
    PSV,
    FCV,
    TCV,
    GPV,
    CCV
};

enum LineType
{
    PUMP,
    PIPE,
    VALVE
};

enum PMType
{
    FO
};

enum class NodeType
{
    JUNCTION,
    RESERVOIR,
    TANK
};

enum class PipeStatus
{
    OPEN,
    CLOSED
};
#endif