#include "../../include/singleset/LLC.h"

AlgorithmLLC::AlgorithmLLC()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    seed = gen();
};

void AlgorithmLLC::Record(uint32_t elem, std::bitset<5> &reg)
{
    uint32_t hash_output;
    MurmurHash3_x86_32(&elem, 4, seed, &hash_output);
    int trailing_zeros = __builtin_ctz(hash_output) + 1;
    if (trailing_zeros >= 32)
    {
        reg = 31;
        return;
    }
    if (trailing_zeros > reg.to_ulong())
    {
        reg = trailing_zeros;
        return;
    }
}

uint32_t AlgorithmLLC::Query(std::bitset<5> reg)
{
    return reg.to_ulong();
}

uint32_t AlgorithmLLC::Estimate(uint32_t var, size_t reg_num)
{
    double aver_var = (double)var / reg_num;
    return 0.39701 * (double)reg_num * (pow(2.0, aver_var) - pow(2.0, -1.75 * aver_var));
}


void AlgorithmLLC::Merge(std::bitset<5> &super_reg, std::bitset<5> &reg)
{
    if (reg.to_ulong() > super_reg.to_ulong())
    {
        super_reg = reg;
    }
}
