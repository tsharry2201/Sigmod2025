#include "../../include/multiset/CMSkt.h"

template<size_t bit_num, class Algorithm>
CMSkt<bit_num, Algorithm>::CMSkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon):
col_num(row_number), reg_num(reg_number), eps(epsilon)
{
    row_num = 2;
    row_col_reg = row_num * col_num * reg_num;
    row_col = row_num * col_num;
    col_reg = col_num * reg_num;
    regs = new std::bitset<bit_num>[row_col_reg];
    seed_col = new uint32_t[row_num];
    est = new uint32_t[row_col];
    est_noise = new uint32_t[row_col];
    std::random_device rd;
    gen = new std::mt19937(rd());
    seed_reg = (*gen)();
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        seed_col[row_id] = (*gen)();
    }
};

template<size_t bit_num, class Algorithm>
CMSkt<bit_num, Algorithm>::~CMSkt()
{
    delete[] regs;
    delete[] seed_col;
    delete[] est;
    delete[] est_noise;
    delete gen;
};

template<size_t bit_num, class Algorithm>
void CMSkt<bit_num, Algorithm>::Record(uint32_t elem, uint32_t setid)
{
    uint32_t hash_output;
    size_t row_id, col_id, reg_id;
    for (row_id = 0; row_id < row_num; row_id++)
    {
        MurmurHash3_x86_32(&setid, 4, seed_col[row_id], &hash_output);
        col_id = hash_output % col_num;
        MurmurHash3_x86_32(&elem, 4, seed_reg, &hash_output);
        reg_id = hash_output % reg_num;
        algorithm.Record(elem, regs[row_id * col_reg + col_id * reg_num + reg_id]);
    }
    new_record = true;
}

template<size_t bit_num, class Algorithm>
void CMSkt<bit_num, Algorithm>::Query(uint32_t setid)
{
    if (!new_record)
    {
        return;
    }
    new_record = false;
    uint32_t hash_output;
    size_t row_id, col_id, reg_id;
    double Lap_noise = 0.0;
    reg_id = 0;
    for (row_id = 0; row_id < row_num; row_id++)
    {
        for (col_id = 0; col_id < col_num; col_id++)
        {
            var = 0;
            for (size_t i = 0; i < reg_num; i++)
            {
                var += algorithm.Query(regs[reg_id]);
                reg_id++;
            }
            est[row_id * col_num + col_id] = algorithm.Estimate(var, reg_num);
            Lap_noise = GenerateLaplace(0, (double)algorithm.sensitivity/eps, (*gen));
            var_noise = var + Lap_noise;
            est_noise[row_id * col_num + col_id] = algorithm.Estimate(var_noise, reg_num);
        }
    }
}

template<size_t bit_num, class Algorithm>
uint32_t CMSkt<bit_num, Algorithm>::Estimate(uint32_t setid, bool noise)
{
    size_t row_id, col_id;
    uint32_t hash_output;
    uint32_t min_est = 0xFFFFFFFF;
    Query(0);
    if (noise)
    {
        for (row_id = 0; row_id < row_num; row_id++)
        {
            MurmurHash3_x86_32(&setid, 4, seed_col[row_id], &hash_output);
            col_id = hash_output % col_num;
            if (min_est > est_noise[row_id * col_num + col_id])
            {
                min_est = est_noise[row_id * col_num + col_id];
            }
        }
    }
    else
    {
        for (row_id = 0; row_id < row_num; row_id++)
        {
            MurmurHash3_x86_32(&setid, 4, seed_col[row_id], &hash_output);
            col_id = hash_output % col_num;
            if (min_est > est[row_id * col_num + col_id])
            {
                min_est = est[row_id * col_num + col_id];
            }
        }
    }
    return min_est;
}


template class CMSkt<32, AlgorithmFMS>;
template class CMSkt<32, AlgorithmFM>;
template class CMSkt<5, AlgorithmLLC>;
