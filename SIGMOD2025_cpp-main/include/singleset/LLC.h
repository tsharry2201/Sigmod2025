#ifndef _LLC_H_
#define _LLC_H_

#include <bitset>
#include <random>
#include <cmath>
#include "../utils/MurmurHash3.h"


class AlgorithmLLC {
    public:
    AlgorithmLLC();
    void Record(uint32_t elem, std::bitset<5> &reg);
    uint32_t Query(std::bitset<5> reg);
    uint32_t Estimate(uint32_t var, size_t reg_num);
    void Merge(std::bitset<5> &super_reg, std::bitset<5> &reg);
    uint32_t seed;
    uint32_t sensitivity = 31;
};

#endif