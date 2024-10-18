#ifndef _FMS_H_
#define _FMS_H_

#include <bitset>
#include <random>
#include <cmath>
#include "../utils/MurmurHash3.h"


class AlgorithmFMS {
    public:
    AlgorithmFMS();
    void Record(uint32_t elem, std::bitset<32> &reg);
    uint32_t Query(std::bitset<32> reg);
    uint32_t Estimate(uint32_t var, size_t reg_num);
    void Merge(std::bitset<32> &super_reg, std::bitset<32> &reg);
    void Getpx(size_t reg_num);
    double Getev(uint32_t est);
    uint32_t seed;
    uint32_t last_reg_num = 0;
    double px[32];
    uint32_t sensitivity = 1;
};

#endif
