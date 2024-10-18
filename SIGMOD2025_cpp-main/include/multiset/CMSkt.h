#ifndef _CMSKT_H_
#define _CMSKT_H_

#include <bitset>
#include <random>
#include "../singleset/FM.h"
#include "../singleset/FMS.h"
#include "../singleset/LLC.h"
#include "../utils/Laplace.h"

template <size_t bit_num, class Algorithm>
class CMSkt {
    public:
    CMSkt(size_t row_number, size_t reg_number, uint32_t set_number, double epsilon);
    ~CMSkt();
    void Record(uint32_t elem, uint32_t setid);
    void Query(uint32_t setid);
    uint32_t Estimate(uint32_t setid, bool noise);
    
    size_t reg_num = 1024;  // No. of regs per sketch
    size_t row_num = 2;     // No. of sketches per set
    size_t col_num = 4;     // No. of sketches to choose per row
    size_t row_col_reg = 0, row_col = 0, col_reg = 0;
    std::bitset<bit_num> *regs;
    std::mt19937 *gen;
    uint32_t seed_reg, *seed_col;
    uint32_t var, var_noise, *est, *est_noise;
    Algorithm algorithm;
    double eps = 1;
    bool new_record = false;
};


#endif