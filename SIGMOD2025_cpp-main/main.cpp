#include <fstream>
#include <iostream>
#include <vector>
#include "./include/singleset/singleset_test.h"
#include "./include/multiset/multiset_test.h"
#include "./include/utils/hadamardMatrix.h"

// 定义全局的哈达马矩阵
std::vector<std::vector<int>> hadamardMatrix;
int set_map[10000];
// 生成哈达马矩阵的函数
std::vector<std::vector<int>> generateHadamardMatrix(int n) {
    std::vector<std::vector<int>> H(n, std::vector<int>(n, 1));

    for (int k = 1; k < n; k *= 2) {
        for (int i = 0; i < k; ++i) {
            for (int j = 0; j < k; ++j) {
                H[i + k][j] = H[i][j];
                H[i][j + k] = H[i][j];
                H[i + k][j + k] = -H[i][j];
            }
        }
    }

    return H;
}

int main(){
    
    std::random_device rd;
    std::mt19937 gen(rd());
    uint32_t card_diff, max_card, *card, card_sum;
    double AARE[12];

    uint32_t times_num = 10;
    bool noise = 1;
    bool save2file = true;
    //singleset_test<32, AlgorithmFM>(1000000000);
    //singleset_test<32, AlgorithmFMS>(10000);
    //singleset_test<5, AlgorithmLLC>(10000);
    uint32_t ans = 0;
    uint32_t set_num= 1000,aver_card = 1000000;
    size_t reg_num = 1024, row_num = 1024;
    double epsilon = 1;
    hadamardMatrix = generateHadamardMatrix(reg_num/4);
    for (size_t row_num = 128; row_num <= 4096; row_num *= 2)
    {
        for (uint32_t set_num = 1000; set_num <= 3000; set_num += 2000)
        {
            for (uint32_t aver_card = 10000; aver_card <= 10000; aver_card *= 10)
            {
                for (size_t reg_num = 256; reg_num <= 256; reg_num *= 2)
                {
                    for (size_t row_num = 256; row_num <= 4096; row_num *= 2)
                    {
                    for (double epsilon = 0.1; epsilon <= 1; epsilon += 0.1)
                    {
                        for (int i = 0; i < 12; i++)
                        {
                            AARE[i] = 0;
                        }
                        std::cout << "---------------------------------------------------------------------------------------------------" << std::endl;
                        std::cout << "set_num:" << set_num << " aver_card:" << aver_card <<  " reg_num:" << reg_num << " row_num:" << row_num << " noise:"<< noise << " eps:" << epsilon << std::endl;
                        for (size_t times_id = 0; times_id < times_num; times_id++)
                        {
                            card_diff = aver_card / 2;
                            max_card = aver_card + card_diff;
                            std::uniform_int_distribution<uint32_t> dis(card_diff, max_card);
                            card = new uint32_t[set_num];
                            card_sum = 0;
                            for (uint32_t i = 0; i < set_num; i++)
                            {
                                card[i] = dis(gen);
                                card_sum += card[i];
                            }
                            std::cout << "card_sum:" << card_sum << std::endl;                         
                            std::cout << "vSkt, FM, ";
                            AARE[0] += multiset_test<32, vSkt<32, AlgorithmFM>>(set_num, card, row_num, reg_num, noise, epsilon);
                            std::cout << "vSkt, FMS, ";
                            AARE[1] += multiset_test<32, vSkt<32, AlgorithmFMS>>(set_num, card, row_num, reg_num, noise, epsilon);
                            std::cout << "vSkt, LLC, ";
                            AARE[2] += multiset_test<5, vSkt<5, AlgorithmLLC>>(set_num, card, (size_t)(row_num), reg_num, noise, epsilon);
                            std::cout << "rSkt, FM, ";
                            AARE[3] += multiset_test<32, rSkt<32, AlgorithmFM>>(set_num, card, row_num , reg_num, noise, epsilon);
                            std::cout << "rSkt, FMS, ";
                            AARE[4] += multiset_test<32, rSkt<32, AlgorithmFMS>>(set_num, card, row_num , reg_num, noise, epsilon);
                            std::cout << "rSkt, LLC, ";
                            AARE[5] += multiset_test<5, rSkt<5, AlgorithmLLC>>(set_num, card,(size_t)(row_num), reg_num, noise, epsilon);
                            std::cout << "CMSkt, FM, ";
                            AARE[6] += multiset_test<32, CMSkt<32, AlgorithmFM>>(set_num, card, row_num , reg_num, noise, epsilon);
                            std::cout << "CMSkt, FMS, ";
                            AARE[7] += multiset_test<32, CMSkt<32, AlgorithmFMS>>(set_num, card, row_num, reg_num, noise, epsilon);
                            std::cout << "CMSkt, LLC, ";
                            AARE[8] += multiset_test<5, CMSkt<5, AlgorithmLLC>>(set_num, card, (size_t)(row_num), reg_num, noise, epsilon);
                            std::cout << "mySkt, FM, ";
                            AARE[9] += multiset_test<32, mySkt<32, AlgorithmFM>>(set_num, card, row_num , reg_num, noise, epsilon);
                            std::cout << "mySkt, FMS, ";
                            AARE[10] += multiset_test<32, mySkt<32, AlgorithmFMS>>(set_num, card, row_num, reg_num, noise, epsilon);
                            std::cout << "mySkt, LLC, ";
                            AARE[11] += multiset_test<5, mySkt<5, AlgorithmLLC>>(set_num, card, (size_t)(row_num), reg_num, noise, epsilon);
                            
                            delete[] card;
                        }
                        std::cout << "vSkt, FM, AARE:" << AARE[0] / times_num << std::endl;
                        std::cout << "vSkt, LLC, AARE:" << AARE[2] / times_num << std::endl;
                        std::cout << "rSkt, FM, AARE:" << AARE[3] / times_num << std::endl;
                        std::cout << "rSkt, FMS, AARE:" << AARE[4] / times_num << std::endl;
                        std::cout << "rSkt, LLC, AARE:" << AARE[5] / times_num << std::endl;
                        std::cout << "CMSkt, FM, AARE:" << AARE[6] / times_num << std::endl;
                        std::cout << "CMSkt, FMS, AARE:" << AARE[7] / times_num << std::endl;
                        std::cout << "CMSkt, LLC, AARE:" << AARE[8] / times_num << std::endl;
                        std::cout << "mySkt, FM, AARE:" << AARE[9] / times_num << std::endl;
                        std::cout << "mySkt, FMS, AARE:" << AARE[10] / times_num << std::endl;
                        std::cout << "mySkt, LLC, AARE:" << AARE[11] / times_num << std::endl;
                     }
                    }
                }
            }
        }                        
    }
    return 0;
}
