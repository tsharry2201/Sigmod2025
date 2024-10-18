#include "../../include/multiset/vSkt.h"

template<size_t bit_num, class Algorithm>
vSkt<bit_num, Algorithm>::vSkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon):
row_num(row_number), reg_num(reg_number), eps(epsilon)
{
    regs = new std::bitset<bit_num> *[reg_num];
    for (size_t i = 0; i < reg_num; i++)
    {
        regs[i] = new std::bitset<bit_num>[row_num];
    }
    super_regs = new std::bitset<bit_num>[reg_num];
    seed_row = new uint32_t[reg_num];

    std::random_device rd;
    gen = new std::mt19937(rd());
    seed_reg = (*gen)();
    for (size_t i = 0; i < reg_num; i++)
    {
        seed_row[i] = (*gen)();
    }
    GetM(set_number);
    printf("M: %d, ", M);
};

template<size_t bit_num, class Algorithm>
vSkt<bit_num, Algorithm>::~vSkt()
{
    for (size_t i = 0; i < reg_num; i++)
    {
        delete[] regs[i];
    }
    delete[] regs;
    delete[] super_regs;
    delete[] seed_row;
    delete gen;
};

template<size_t bit_num, class Algorithm>
void vSkt<bit_num, Algorithm>::Record(uint32_t elem, uint32_t setid)
{
    uint32_t hash_output;
    size_t reg_id, row_id;
    MurmurHash3_x86_32(&elem, 4, seed_reg, &hash_output);
    reg_id = hash_output % reg_num;
    MurmurHash3_x86_32(&setid, 4, seed_row[reg_id], &hash_output);
    row_id = hash_output % row_num;
    algorithm.Record(elem, regs[reg_id][row_id]);
    if (new_record)
    {
        return;
    }
    else
    {
        new_record = true;
    }
}

template<size_t bit_num, class Algorithm>
void vSkt<bit_num, Algorithm>::Query(uint32_t setid)
{
    uint32_t hash_output;
    size_t row_id;
    var_sum = 0;
    for (size_t i = 0; i < reg_num; i++)
    {
        MurmurHash3_x86_32(&setid, 4, seed_row[i], &hash_output);
        row_id = hash_output % row_num;
        var_sum += algorithm.Query(regs[i][row_id]);
    }
}

template<size_t bit_num, class Algorithm>
uint32_t vSkt<bit_num, Algorithm>::Estimate(uint32_t setid, bool noise)
{
    double Lap_noise = 0.0;
    uint32_t estimate_raw = 0;
    if (new_record)
    {
        var_sum = 0;
        new_record = false;
        for (size_t i = 0; i < reg_num; i++)
        {
            for (size_t j = 0; j < row_num; j++)
            {
                // algorithm.Merge(super_regs[i], regs[i][j]);
                var_sum += algorithm.Query(regs[i][j]);
            }
            // var_sum += algorithm.Query(super_regs[i]);
        }
        Lap_noise = GenerateLaplace(0, (double)algorithm.sensitivity * M/eps, (*gen));
        // average_record = algorithm.Estimate(var_sum, reg_num) / row_num;
        // average_record_noise = algorithm.Estimate(var_sum + Lap_noise, reg_num) / row_num;
        average_record = algorithm.Estimate(var_sum, reg_num * row_num) / row_num;
        average_record_noise = algorithm.Estimate(var_sum + Lap_noise, reg_num * row_num) / row_num;
        // printf("average_record: %d, average_record_noise: %d, ", average_record, average_record_noise);
    }
    Query(setid);
    if (noise)
    {
        Lap_noise = GenerateLaplace(0, (double)algorithm.sensitivity * M/eps, (*gen));
        var_sum += Lap_noise;
        estimate_raw = algorithm.Estimate(var_sum, reg_num);
        // printf("estimate_raw: %d\n", estimate_raw);
        if (estimate_raw > average_record_noise)
        {
            return estimate_raw - average_record_noise;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        estimate_raw = algorithm.Estimate(var_sum, reg_num);
        // printf("estimate_raw: %d\n", estimate_raw);
        if (estimate_raw > average_record)
        {
            return estimate_raw - average_record;
        }
        else
        {
            return 0;
        }
    }
}


template<size_t bit_num, class Algorithm>
void vSkt<bit_num, Algorithm>::GetM(uint32_t set_num)
{
    uint32_t **set_cnt, row_id, hash_output;
    set_cnt = new uint32_t *[reg_num];
    for (uint32_t reg_id = 0; reg_id < reg_num; reg_id++)
    {
        set_cnt[reg_id] = new uint32_t[row_num];
        for (uint32_t row_id = 0; row_id < row_num; row_id++)
        {
            set_cnt[reg_id][row_id] = 0;
        }
    }
    for (uint32_t set_id = 0; set_id < set_num; set_id++)
    {
        for (uint32_t reg_id = 0; reg_id < reg_num; reg_id++)
        {
            MurmurHash3_x86_32(&set_id, 4, seed_row[reg_id], &hash_output);
            row_id = hash_output % row_num;
            set_cnt[reg_id][row_id]++;
        }
    }
    for (uint32_t reg_id = 0; reg_id < reg_num; reg_id++)
    {
        for (uint32_t row_id = 0; row_id < row_num; row_id++)
        {
            if (set_cnt[reg_id][row_id] > M)
            {
                M = set_cnt[reg_id][row_id];
            }
        }
    }
    for (size_t i = 0; i < reg_num; i++)
    {
        delete[] set_cnt[i];
    }
    delete[] set_cnt;
}

template class vSkt<32, AlgorithmFMS>;
template class vSkt<32, AlgorithmFM>;
template class vSkt<5, AlgorithmLLC>;
