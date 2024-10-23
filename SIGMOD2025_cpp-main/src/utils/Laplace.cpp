#include "../../include/utils/Laplace.h"


double GenerateLaplace(double mu, double b, std::mt19937& gen) {
    std::uniform_real_distribution<double> uni(-1.0, 1.0);
    std::exponential_distribution<double> exp(1.0/b);

    double u = uni(gen);
    double e = exp(gen);

    if (u < 0.0) {
        return mu - e;
    } else {
        return mu + e;
    }
}
