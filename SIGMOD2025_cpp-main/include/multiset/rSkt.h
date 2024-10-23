#ifndef _RSKT_H_
#define _RSKT_H_

#include <bitset>
#include <random>
#include "../singleset/FM.h"
#include "../singleset/FMS.h"
#include "../singleset/LLC.h"
#include "../utils/Laplace.h"

template <size_t bit_num, class Algorithm>
class rSkt {
    public:
    rSkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon);
    ~rSkt();
    void Record(uint32_t elem, uint32_t setid);
    void Query(uint32_t setid);
    uint32_t Estimate(uint32_t setid, bool noise);
    void GetM(uint32_t set_num);
    
    size_t reg_num = 1024;  // No. of regs a set use
    size_t row_num = 4;  // No. of regs to be chosen for a set
    std::bitset<bit_num> **regs0, **regs1;
    std::mt19937 *gen;
    uint32_t seed_col, seed_reg, seed_row;
    Algorithm algorithm;
    double eps = 1;
    uint32_t M = 0;
    uint32_t prim_var, comp_var;
};


#endif