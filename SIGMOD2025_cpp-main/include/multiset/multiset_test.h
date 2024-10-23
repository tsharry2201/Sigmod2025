#ifndef _MULTISET_TEST_H_
#define _MULTISET_TEST_H_


#include "CMSkt.h"
#include "rSkt.h"
#include "vSkt.h"
#include "AMSkt.h"

template <size_t bit_num, class Algorithm>
double multiset_test(uint32_t set_num, uint32_t *card, size_t reg_num, size_t row_num, bool noise, double epsilon);


#endif
