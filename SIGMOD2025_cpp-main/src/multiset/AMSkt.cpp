/*#include "../../include/multiset/mySkt.h"
#include <iostream>
#include <cstdio>
using namespace std;

//vars将reg合并了，要退回用reg计算
template<size_t bit_num, class Algorithm>
mySkt<bit_num, Algorithm>::mySkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon):
row_num(row_number), reg_num(reg_number), eps(epsilon)
{
    reg_pow = log2(reg_num);
    uint32_t set_num_per_row = ceil((double)set_number / row_num);
    var_len_pow = reg_pow - set_num_per_row + 1;
    var_num_pow = set_num_per_row - 1;
  //  var_len = 1 << var_len_pow;
  //  var_num = 1 << var_num_pow;
    regs0 = new std::bitset<bit_num> *[row_num];
    regs1 = new std::bitset<bit_num> *[row_num];
    vars0 = new uint32_t *[row_num];
    vars1 = new uint32_t *[row_num];
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        regs0[row_id] = new std::bitset<bit_num>[reg_num];
        regs1[row_id] = new std::bitset<bit_num>[reg_num];
        vars0[row_id] = new uint32_t[reg_num];
        vars1[row_id] = new uint32_t[reg_num];
    }
    std::random_device rd;
    gen = new std::mt19937(rd());
    seed_reg = (*gen)();
};

template<size_t bit_num, class Algorithm>
mySkt<bit_num, Algorithm>::~mySkt()
{
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        delete[] regs0[row_id];
        delete[] regs1[row_id];
        delete[] vars0[row_id];
        delete[] vars1[row_id];
    }
    delete[] regs0;
    delete[] regs1;
    delete[] vars0;
    delete[] vars1;
    delete gen;
};

template<size_t bit_num, class Algorithm>
void mySkt<bit_num, Algorithm>::Record(uint32_t elem, uint32_t setid)
{
    uint32_t hash_input, hash_output;
    size_t row_id, reg_id, id_in_row;
    size_t var_len4set_pow, var_num4set_pow, var_len4set, var_num4set;
    size_t var_id4set;
    id_in_row = setid / row_num;
    var_len4set_pow = reg_pow - id_in_row;
    var_num4set_pow = id_in_row;
    var_len4set = 1 << var_len4set_pow;
    var_num4set = 1 << var_num4set_pow;

    row_id = setid % row_num;
    MurmurHash3_x86_32(&elem, 4, seed_reg, &hash_output);
    reg_id = hash_output % reg_num;
    var_id4set = reg_id / var_len4set;
    if (hadamardMatrix[setid][reg_id/4] == 1) // 2*k 和2*k+1由k位置决定
    {
        algorithm.Record(elem, regs0[row_id][reg_id]);
    }
    else
    {
        algorithm.Record(elem, regs1[row_id][reg_id]);
    }
   // printf("%d\n",setid);
    new_record = true;
}

template<size_t bit_num, class Algorithm>
void mySkt<bit_num, Algorithm>::Query(bool noise)
{
    if (!new_record)
    {
        return;
    }
    new_record = false;
    uint32_t hash_output;
    size_t row_id, col_id, reg_id, var_id;
    double Lap_noise = 0.0;
    for (row_id = 0; row_id < row_num; row_id++)
    {
        for (var_id = 0; var_id < reg_num; var_id++)
        {
            vars0[row_id][var_id] = 0;
            vars1[row_id][var_id] = 0;
        }
        for (reg_id = 0; reg_id < reg_num; reg_id+=4)
        {
            vars0[row_id][reg_id/4] += algorithm.Query(regs0[row_id][reg_id])+algorithm.Query(regs0[row_id][reg_id+1])+algorithm.Query(regs0[row_id][reg_id+2])+algorithm.Query(regs0[row_id][reg_id+3]);
            vars1[row_id][reg_id/4] += algorithm.Query(regs1[row_id][reg_id])+algorithm.Query(regs1[row_id][reg_id+1])+algorithm.Query(regs1[row_id][reg_id+2])+algorithm.Query(regs1[row_id][reg_id+3]);
        }       
        for (reg_id = 0; reg_id < reg_num/4; reg_id++)
        {
            if (noise)
            {
                vars0[row_id][reg_id] = algorithm.Estimate(vars0[row_id][reg_id] + GenerateLaplace(0, (double)algorithm.sensitivity/eps, (*gen)), 4);//改
                vars1[row_id][reg_id] = algorithm.Estimate(vars1[row_id][reg_id] + GenerateLaplace(0, (double)algorithm.sensitivity/eps, (*gen)), 4);
            }
            else
            {
                vars0[row_id][reg_id] = algorithm.Estimate(vars0[row_id][reg_id], 4);//改
                vars1[row_id][reg_id] = algorithm.Estimate(vars1[row_id][reg_id], 4);
            }
        }
    }
}

template<size_t bit_num, class Algorithm>
uint32_t mySkt<bit_num, Algorithm>::Estimate(uint32_t setid, bool noise)
{
    size_t row_id, reg_id, col_id, id_in_row, var_id;
    size_t var_len4set_pow, var_num4set_pow, var_len4set, var_num4set;
    size_t var_id4set;
    id_in_row = setid / row_num;
    var_len4set_pow = reg_pow - id_in_row;
    var_num4set_pow = id_in_row;
    var_len4set = 1 << var_len4set_pow;
    var_num4set = 1 << var_num4set_pow;
    size_t var_cnt = pow(2, var_num_pow - id_in_row);
    size_t var_cnt_id = 0;
    double Lap_noise = 0.0;
    uint32_t prim_est = 0, comp_est = 0;
    Query(noise);
    row_id = setid % row_num;
    for (uint32_t i = 0; i < reg_num/4; i++) //只用一半
    {
        if (hadamardMatrix[setid][i] == 1)
        {
            prim_est += vars0[row_id][i];
            comp_est += vars1[row_id][i];
          //  if(setid == 1)printf("%d  %d\n",i, vars1[row_id][i]);
        }
        else
        {
            prim_est += vars1[row_id][i];
            comp_est += vars0[row_id][i];
        }
    }
    printf("%d  %d\n",prim_est,comp_est);
   // std::cout <<prim_est - comp_est;
   // std::cout << endl;
    if (prim_est > comp_est)
    {
       
        return prim_est - comp_est;
    }
    else
    {
        return 0;
    }
}


template class mySkt<32, AlgorithmFMS>;
template class mySkt<32, AlgorithmFM>;
template class mySkt<5, AlgorithmLLC>;

*/
#include "../../include/multiset/AMSkt.h"

template<size_t bit_num, class Algorithm>
mySkt<bit_num, Algorithm>::mySkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon):
row_num(row_number), reg_num(reg_number), eps(epsilon)
{
    reg_pow = log2(reg_num);
    uint32_t set_num_per_row = ceil((double)set_number / row_num);
    var_len_pow = reg_pow - set_num_per_row + 1;
    var_num_pow = set_num_per_row - 1;
    var_len = 1 << var_len_pow;
    var_num = 1 << var_num_pow;
    regs0 = new std::bitset<bit_num> *[row_num];
    regs1 = new std::bitset<bit_num> *[row_num];
    vars0 = new uint32_t *[row_num];
    vars1 = new uint32_t *[row_num];
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        regs0[row_id] = new std::bitset<bit_num>[reg_num];
        regs1[row_id] = new std::bitset<bit_num>[reg_num];
        vars0[row_id] = new uint32_t[var_num];
        vars1[row_id] = new uint32_t[var_num];
    }
    std::random_device rd;
    gen = new std::mt19937(rd());
    seed_reg = (*gen)();
};

template<size_t bit_num, class Algorithm>
mySkt<bit_num, Algorithm>::~mySkt()
{
    for (size_t row_id = 0; row_id < row_num; row_id++)
    {
        delete[] regs0[row_id];
        delete[] regs1[row_id];
        delete[] vars0[row_id];
        delete[] vars1[row_id];
    }
    delete[] regs0;
    delete[] regs1;
    delete[] vars0;
    delete[] vars1;
    delete gen;
};

template<size_t bit_num, class Algorithm>
void mySkt<bit_num, Algorithm>::Record(uint32_t elem, uint32_t setid)
{
    uint32_t hash_input, hash_output;
    size_t row_id, reg_id, id_in_row;
    size_t var_len4set_pow, var_num4set_pow, var_len4set, var_num4set;
    size_t var_id4set;
    id_in_row = setid / row_num;
    var_len4set_pow = reg_pow - id_in_row;
    var_num4set_pow = id_in_row;
    var_len4set = 1 << var_len4set_pow;
    var_num4set = 1 << var_num4set_pow;

    row_id = setid % row_num;
    MurmurHash3_x86_32(&elem, 4, seed_reg, &hash_output);
    reg_id = hash_output % reg_num;
    var_id4set = reg_id / var_len4set;
    if (var_id4set % 2 == 0)
    {
        algorithm.Record(elem, regs0[row_id][reg_id]);
    }
    else
    {
        algorithm.Record(elem, regs1[row_id][reg_id]);
    }
    new_record = true;
}

template<size_t bit_num, class Algorithm>
void mySkt<bit_num, Algorithm>::Query(bool noise)
{
    if (!new_record)
    {
        return;
    }
    new_record = false;
    uint32_t hash_output;
    size_t row_id, col_id, reg_id, var_id;
    double Lap_noise = 0.0;
    for (row_id = 0; row_id < row_num; row_id++)
    {
        for (var_id = 0; var_id < var_num; var_id++)
        {
            vars0[row_id][var_id] = 0;
            vars1[row_id][var_id] = 0;
        }
        for (reg_id = 0; reg_id < reg_num; reg_id++)
        {
            var_id = reg_id / var_len;
            vars0[row_id][var_id] += algorithm.Query(regs0[row_id][reg_id]);
            vars1[row_id][var_id] += algorithm.Query(regs1[row_id][reg_id]);
        }
        for (var_id = 0; var_id < var_num; var_id++)
        {
            if (noise)
            {
                vars0[row_id][var_id] = algorithm.Estimate(vars0[row_id][var_id] + GenerateLaplace(0, (double)algorithm.sensitivity/eps, (*gen)), var_len);
                vars1[row_id][var_id] = algorithm.Estimate(vars1[row_id][var_id] + GenerateLaplace(0, (double)algorithm.sensitivity/eps, (*gen)), var_len);
            }
            else
            {
                vars0[row_id][var_id] = algorithm.Estimate(vars0[row_id][var_id], var_len);
                vars1[row_id][var_id] = algorithm.Estimate(vars1[row_id][var_id], var_len);
            }
        }
    }
}

template<size_t bit_num, class Algorithm>
uint32_t mySkt<bit_num, Algorithm>::Estimate(uint32_t setid, bool noise)
{
    size_t row_id, reg_id, col_id, id_in_row, var_id;
    size_t var_len4set_pow, var_num4set_pow, var_len4set, var_num4set;
    size_t var_id4set;
    id_in_row = setid / row_num;
    var_len4set_pow = reg_pow - id_in_row;
    var_num4set_pow = id_in_row;
    var_len4set = 1 << var_len4set_pow;
    var_num4set = 1 << var_num4set_pow;
    size_t var_cnt = pow(2, var_num_pow - id_in_row);
    size_t var_cnt_id = 0;
    double Lap_noise = 0.0;
    uint32_t prim_est = 0, comp_est = 0;
    Query(noise);
    row_id = setid % row_num;
    col_id = 0;
    for (uint32_t i = 0; i < var_num; i++)
    {
        if (col_id == 0)
        {
            prim_est += vars0[row_id][i], var_len;
            comp_est += vars1[row_id][i], var_len;
        }
        else
        {
            prim_est += vars1[row_id][i], var_len;
            comp_est += vars0[row_id][i], var_len;
        }
        var_cnt_id++;
        if (var_cnt_id == var_cnt)
        {
            var_cnt_id = 0;
            col_id = 1 - col_id;
        }
    }
    if (prim_est > comp_est)
    {
        return prim_est - comp_est;
    }
    else
    {
        return 0;
    }
}


template class mySkt<32, AlgorithmFMS>;
template class mySkt<32, AlgorithmFM>;
template class mySkt<5, AlgorithmLLC>;
