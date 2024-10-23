#include "../../include/multiset/multiset_test.h"


template <size_t bit_num, class Algorithm>
double multiset_test(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon)
{
    Algorithm algorithm(row_num, reg_num, set_num, epsilon);
    uint32_t aver_abs_error = 0;
    double aver_abs_rel_error = 0.0;
    uint32_t element = 0;

    for (uint32_t i = 0; i < set_num; i++)
    {
        for (uint32_t j = 0; j < card[i]; j++)
        {
            algorithm.Record(element, i);
            element++;
        }
    }
    for (uint32_t i = 0; i < set_num; i++)
    {
        uint32_t estimate = algorithm.Estimate(i, noise);
        aver_abs_error += abs(estimate - card[i]);
        aver_abs_rel_error += (double)abs(estimate - card[i]) / card[i];
        if (i < 0)
        {
            printf("%d, card: %d, estimate: %d, abs_error: %d, rel_error: %f\n", i, card[i], estimate, abs(estimate - card[i]), (double)abs(estimate - card[i]) / card[i]);
        }
    }
    printf("AAE: %d, AARE: %f\n", aver_abs_error / set_num, aver_abs_rel_error / set_num);
    return aver_abs_rel_error / set_num;
}


template double multiset_test<32, vSkt<32, AlgorithmFM>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, vSkt<32, AlgorithmFMS>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<5, vSkt<5, AlgorithmLLC>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, rSkt<32, AlgorithmFM>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, rSkt<32, AlgorithmFMS>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<5, rSkt<5, AlgorithmLLC>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, CMSkt<32, AlgorithmFM>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, CMSkt<32, AlgorithmFMS>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<5, CMSkt<5, AlgorithmLLC>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, mySkt<32, AlgorithmFM>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<32, mySkt<32, AlgorithmFMS>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);
template double multiset_test<5, mySkt<5, AlgorithmLLC>>(uint32_t set_num, uint32_t *card, size_t row_num, size_t reg_num, bool noise, double epsilon);

