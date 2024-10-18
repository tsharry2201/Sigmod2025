#include "../../include/singleset/singleset_test.h"

template <size_t bit_num, class Algorithm>
void singleset_test(uint32_t cardinality)
{
    Algorithm algorithm;
    std::bitset<bit_num> regs[2048];
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t seed = gen();
    for (uint32_t i = 0; i < cardinality; i++)
    {
        uint32_t hash_output;
        MurmurHash3_x86_32(&i, 4, seed, &hash_output);
        uint32_t reg_id = hash_output % 2048;
        algorithm.Record(i, regs[reg_id]);
    }
    uint32_t sum = 0;
    for (int i = 0; i < 2048; i++)
    {
        sum += algorithm.Query(regs[i]);
    }
    printf("sum: %u\n", sum);
  //  printf("est: %u\n", algorithm.Estimate(sum, 2048));
    printf("AARE %f",(float)abs(cardinality-algorithm.Estimate(sum, 2048))/cardinality);
    return;
}


template void singleset_test<32, AlgorithmFM>(uint32_t cardinality);
template void singleset_test<32, AlgorithmFMS>(uint32_t cardinality);
template void singleset_test<5, AlgorithmLLC>(uint32_t cardinality);
