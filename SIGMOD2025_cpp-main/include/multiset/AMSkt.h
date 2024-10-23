#ifndef _MYSKT_H_
#define _MYSKT_H_

#include <bitset>
#include <random>
#include "../singleset/FM.h"
#include "../singleset/FMS.h"
#include "../singleset/LLC.h"
#include "../utils/Laplace.h"
#include <vector>
#include "../utils/hadamardMatrix.h"  // 包含全局变量声明的头文件

template <size_t bit_num, class Algorithm>
class mySkt {
    public:
    mySkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon);
    ~mySkt();
    void Record(uint32_t elem, uint32_t setid);
    void Query(bool noise);
    uint32_t Estimate(uint32_t setid, bool noise);
    
    size_t reg_num = 1024;  // No. of regs a set use
    size_t reg_pow = 10;
    size_t row_num = 4;  // No. of regs to be chosen for a set
    size_t var_len_pow = 10;
    size_t var_len = 1024;
    size_t var_num_pow = 0;
    size_t var_num = 1;
    std::bitset<bit_num> **regs0, **regs1;
    uint32_t **vars0, **vars1;
    std::mt19937 *gen;
    uint32_t seed_reg;
    Algorithm algorithm;
    double eps = 1;
    uint32_t M = 0;
    bool new_record = false;
};


#endif