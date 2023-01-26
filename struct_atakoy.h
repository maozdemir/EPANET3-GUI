#ifndef STRUCT_ATAKOY_H
#define STRUCT_ATAKOY_H

#include <string>


struct Result_Atakoy {
    std::string Time;
    double Inc_FR_;
    double ZB_FR_;
    double ZB_PRV_UPS_PRES_;
    double ZB_PRV_DOWNS_PRES_;
    double ZB_MAX_PRES_;
    double ZB_AVE_PRES_;
    double ZB_MIN_PRES_;
    double Leakage_;
};
#endif