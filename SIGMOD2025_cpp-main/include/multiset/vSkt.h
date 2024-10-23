#ifndef _VSKT_H_
#define _VSKT_H_

#include <bitset>
#include <random>
#include "../singleset/FM.h"
#include "../singleset/FMS.h"
#include "../singleset/LLC.h"
#include "../utils/Laplace.h"

template <size_t bit_num, class Algorithm>
class vSkt {
    public:
    vSkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon);
    ~vSkt();
    void Record(uint32_t elem, uint32_t setid);
    void Query(uint32_t setid);
    uint32_t Estimate(uint32_t setid, bool noise);
    void GetM(uint32_t set_num);
    
    size_t reg_num = 1024;  // No. of regs a set use
    size_t row_num = 4;  // No. of regs to be chosen for a set
    std::bitset<bit_num> **regs;
    std::bitset<bit_num> *super_regs;
    std::mt19937 *gen;
    uint32_t seed_reg, *seed_row;
    Algorithm algorithm;
    bool new_record = true;
    uint32_t average_record = 0, average_record_noise = 0;
    double eps = 1;
    uint32_t M = 0;
    uint32_t var_sum = 0;
};


#endif