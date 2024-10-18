#include "../../include/singleset/FMS.h"

AlgorithmFMS::AlgorithmFMS()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    seed = gen();
};

void AlgorithmFMS::Record(uint32_t elem, std::bitset<32> &reg)
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

uint32_t AlgorithmFMS::Query(std::bitset<32> reg)
{
    return 32 - reg.count();
}

void AlgorithmFMS::Getpx(size_t reg_num)
{
    for (uint8_t x = 0; x < 31; x++)
    {
        px[x] = 1.0 / std::pow(2, x + 1) / reg_num;
    }
    px[31] = 1.0 / std::pow(2, 31) / reg_num;
}

double AlgorithmFMS::Getev(uint32_t est)
{
    double sum = 0;
    for (uint8_t i = 0; i < 32; i++)
    {
        sum += std::pow(1 - px[i], est);
    }
    return sum;
}

uint32_t AlgorithmFMS::Estimate(uint32_t var, size_t reg_num)
{
    if (last_reg_num != reg_num)
    {
        last_reg_num = reg_num;
        Getpx(reg_num);
    }
    double v = (double)var / reg_num;
    double est[2] = {1, 10};
    double ev[2] = {0};
    ev[0] = Getev(est[0]);
    ev[1] = Getev(est[1]);
    if (v > ev[0])
    {
        return 0;
    }
    while (v < ev[1])
    {
        est[0] = est[1];
        ev[0] = ev[1];
        est[1] *= 10;
        ev[1] = Getev(est[1]);
    }
    while(est[1]-est[0]>3)
    {
        double est_mid = (est[0]+est[1])/2;
        double ev_mid = Getev(est_mid);
        if(v < ev_mid)
        {
            est[0] = est_mid;
        }
        else
        {
            est[1] = est_mid;
        }
    }
    return std::round((est[0]+est[1])/2);
}

void AlgorithmFMS::Merge(std::bitset<32> &super_reg, std::bitset<32> &reg)
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
