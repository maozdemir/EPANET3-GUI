#ifndef STRUCT_ATAKOY_H
#define STRUCT_ATAKOY_H

#include <string>


struct Result_Atakoy {
    std::string Time;
    double Inc_FR_ = 0.0;
    double ZB_FR_ = 0.0;
    double ZB_PRV_UPS_PRES_ = 0.0;
    double ZB_PRV_DOWNS_PRES_ = 0.0;
    double ZB_MAX_PRES_ = 0.0;
    double ZB_AVE_PRES_ = 0.0;
    double ZB_MIN_PRES_ = 0.0;
    double Leakage_ = 0.0;
};
#endif