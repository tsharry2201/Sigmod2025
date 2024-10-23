#include "../../include/singleset/FM.h"


AlgorithmFM::AlgorithmFM()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    seed = gen();
};

void AlgorithmFM::Record(uint32_t elem, std::bitset<32> &reg)
{
    uint32_t hash_output;
    MurmurHash3_x86_32(&elem, 4, seed, &hash_output);
    int trailing_zeros = __builtin_ctz(hash_output);
    if (trailing_zeros >= 32)
    {
        reg.set(31);
        return;
    }
    reg.set(trailing_zeros);
    return;
}

uint32_t AlgorithmFM::Query(std::bitset<32> reg)
{
    // find the first bit that is not set
    uint8_t i = 0;
    while (i <= 30 && reg.test(i))
    {
        i++;
    }
    return i;
}

uint32_t AlgorithmFM::Estimate(uint32_t var, size_t reg_num)
{
    double aver_var = (double)var / reg_num;
    return (double)reg_num / 0.77351 * (pow(2.0, aver_var) - pow(2.0, -1.75 * aver_var));
}


void AlgorithmFM::Merge(std::bitset<32> &super_reg, std::bitset<32> &reg)
{
    for (int i = 0; i < 32; i++)
    {
        if (reg.test(i))
        {
            super_reg.set(i);
        }
    }
    return;
}