#ifndef _FM_H_
#define _FM_H_

#include <bitset>
#include <random>
#include <cmath>
#include "../utils/MurmurHash3.h"


class AlgorithmFM {
    public:
    AlgorithmFM();
    void Record(uint32_t elem, std::bitset<32> &reg);
    uint32_t Query(std::bitset<32> reg);
    uint32_t Estimate(uint32_t var, size_t reg_num);
    void Merge(std::bitset<32> &super_reg, std::bitset<32> &reg);
    uint32_t seed;
    uint32_t sensitivity = 31;
};

#endif