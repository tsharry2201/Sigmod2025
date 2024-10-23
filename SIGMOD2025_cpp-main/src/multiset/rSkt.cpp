#include "../../include/multiset/rSkt.h"

template<size_t bit_num, class Algorithm>
rSkt<bit_num, Algorithm>::rSkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon):
row_num(row_number), reg_num(reg_number), eps(epsilon)
{
    regs0 = new std::bitset<bit_num> *[row_num];
    regs1 = new std::bitset<bit_num> *[row_num];
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        regs0[row_id] = new std::bitset<bit_num>[reg_num];
        regs1[row_id] = new std::bitset<bit_num>[reg_num];
    }
    std::random_device rd;
    gen = new std::mt19937(rd());
    seed_row = (*gen)();
    seed_col = (*gen)();
    seed_reg = (*gen)();
    GetM(set_number);
    printf("M: %d, ", M);
};

template<size_t bit_num, class Algorithm>
rSkt<bit_num, Algorithm>::~rSkt()
{
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        delete[] regs0[row_id];
        delete[] regs1[row_id];
    }
    delete[] regs0;
    delete[] regs1;
    delete gen;
};

template<size_t bit_num, class Algorithm>
void rSkt<bit_num, Algorithm>::Record(uint32_t elem, uint32_t setid)
{
    uint32_t hash_input, hash_output;
    size_t row_id, col_id, reg_id;
    MurmurHash3_x86_32(&setid, 4, seed_row, &hash_output);
    row_id = hash_output % row_num;
    MurmurHash3_x86_32(&elem, 4, seed_reg, &hash_output);
    reg_id = hash_output % reg_num;
    hash_input = setid * reg_num + reg_id;
    MurmurHash3_x86_32(&hash_input, 4, seed_col, &hash_output);
    col_id = hash_output % 2;
    if (col_id == 0)
    {
        algorithm.Record(elem, regs0[row_id][reg_id]);
    }
    else
    {
        algorithm.Record(elem, regs1[row_id][reg_id]);
    }
}

template<size_t bit_num, class Algorithm>
void rSkt<bit_num, Algorithm>::Query(uint32_t setid)
{
    uint32_t hash_input, hash_output;
    size_t row_id, col_id, reg_id;
    prim_var = 0;
    comp_var = 0;
    MurmurHash3_x86_32(&setid, 4, seed_row, &hash_output);
    row_id = hash_output % row_num;
    for (size_t reg_id = 0; reg_id < reg_num; reg_id++)
    {
        hash_input = setid * reg_num + reg_id;
        MurmurHash3_x86_32(&hash_input, 4, seed_col, &hash_output);
        col_id = hash_output % 2;
        if (col_id == 0)
        {
            prim_var += algorithm.Query(regs0[row_id][reg_id]);
            comp_var += algorithm.Query(regs1[row_id][reg_id]);
        }
        else
        {
            comp_var += algorithm.Query(regs0[row_id][reg_id]);
            prim_var += algorithm.Query(regs1[row_id][reg_id]);
        }
    }
    return;
}

template<size_t bit_num, class Algorithm>
uint32_t rSkt<bit_num, Algorithm>::Estimate(uint32_t setid, bool noise)
{
    double Lap_noise = 0.0;
    uint32_t prim_est = 0, comp_est = 0;
    Query(setid);
    if (noise)
    {
        Lap_noise = GenerateLaplace(0, (double)algorithm.sensitivity * M/eps, (*gen));
        prim_var += Lap_noise;
        prim_est = algorithm.Estimate(prim_var, reg_num);
        Lap_noise = GenerateLaplace(0, (double)algorithm.sensitivity * M/eps, (*gen));
        comp_var += Lap_noise;
        comp_est = algorithm.Estimate(comp_var, reg_num);
        // printf("estimate_raw: %d\n", estimate_raw);
        if (prim_est > comp_est)
        {
            return prim_est - comp_est;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        prim_est = algorithm.Estimate(prim_var, reg_num);
        comp_est = algorithm.Estimate(comp_var, reg_num);
        // printf("estimate_raw: %d\n", estimate_raw);
        if (prim_est > comp_est)
        {
            return prim_est - comp_est;
        }
        else
        {
            return 0;
        }
    }
}


template<size_t bit_num, class Algorithm>
void rSkt<bit_num, Algorithm>::GetM(uint32_t set_num)
{
    uint32_t *set_cnt, row_id, hash_output;
    set_cnt = new uint32_t[row_num];
    for (uint32_t row_id = 0; row_id < row_num; row_id++)
    {
        set_cnt[row_id] = 0;
    }
    for (uint32_t set_id = 0; set_id < set_num; set_id++)
    {
        MurmurHash3_x86_32(&set_id, 4, seed_row, &hash_output);
        row_id = hash_output % row_num;
        set_cnt[row_id]++;
    }
    for (uint32_t row_id = 0; row_id < row_num; row_id++)
    {
        if (set_cnt[row_id] > M)
        {
            M = set_cnt[row_id];
        }
    }
    delete[] set_cnt;
}

template class rSkt<32, AlgorithmFMS>;
template class rSkt<32, AlgorithmFM>;
template class rSkt<5, AlgorithmLLC>;
