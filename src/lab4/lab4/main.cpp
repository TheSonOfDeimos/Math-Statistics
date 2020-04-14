#include <iostream>
#include <array>
#include <cmath>
#include <vector>
#include <random>
#include <thread>

#include <boost/asio/io_service.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

// Variant 10
#define M               5
#define BLOCK_COUNT     13
#define BLOCK_MAP       {2, 2, 2, 3, 4}
#define MIN_L           1
#define MAX_L           5

#define LAMBDA_1        40 * pow(10, -6)
#define LAMBDA_2        10 * pow(10, -6)
#define LAMBDA_3        80 * pow(10, -6)
#define LAMBDA_4        30 * pow(10, -6)
#define LAMBDA_5        20 * pow(10, -6)
#define LAMBDA          {LAMBDA_1, LAMBDA_2, LAMBDA_3, LAMBDA_4, LAMBDA_5}

#define T               8760
#define P0              0.995
#define EPS             0.01

#define THREAD_COUNT    4

bool LFSR(std::vector<double> x)
{
    bool result = (((x.at(0) > T) & (x.at(1) > T)) | ((x.at(2) > T) & (x.at(3) > T))) &
                  ((x.at(4) > T) & (x.at(5) > T)) &
                  ((x.at(6) > T) | (x.at(7) > T) | (x.at(8) > T)) &
                  (((x.at(9) > T) | (x.at(10) > T)) & ((x.at(11) > T) | (x.at(12) > T)));
    return result;
}

static double N()
{
    return 5.41028 * (P0 * (1 - P0)) / std::pow(10, -6);
}



double run(const std::array<std::size_t, M>& L, const std::array<std::size_t, M>& n, const std::array<double, M>& lambda)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    
    std::size_t d = 0;
    for (std::size_t k = 0; k < N(); k++)
    {
        std::vector<double> x;
        for (std::size_t i = 0; i < M; i++)
        {
            std::vector<double> t;
            for (std::size_t j = 0; j < n.at(i); j++)
            {
                t.push_back( -log(dis(gen)) / lambda.at(i));
            }
            
            for (std::size_t j = 0; j < L.at(i); j++)
            {
                auto min = std::min_element(t.begin(), t.end());
                *min = *min - log(dis(gen)) / lambda.at(i);
            }
            
            for(std::size_t j = 0; j < t.size(); j++)
            {
                x.push_back(t.at(j));
            }
        }
        
        d += !LFSR(x);
    }
    
    
    double P = 1 - d / N();
    return P;
}

void task(const std::array<std::size_t, M>& L, const std::array<std::size_t, M>& n, const std::array<double, M>& lambda)
{
    double p = run(L, n, lambda);
    if (p > P0) {
        std::cout << "L = {" << L.at(0) << ", " << L.at(1) << ", " << L.at(2) << ", " << L.at(3) << "} P = " << p << "\n";
    }
}

int main(int argc, const char * argv[]) {

    boost::asio::io_service ioService;
    boost::thread_group threadpool;
    boost::asio::io_service::work work(ioService);
    
    for ( int i = 0; i < THREAD_COUNT; ++i)
    {
        threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));
    }

    const std::array<std::size_t, M> n = BLOCK_MAP;
    const std::array<double, M> lambda = LAMBDA;
    
    for (std::size_t l1 = MIN_L; l1 <= MAX_L; l1++)
    {
        for (std::size_t l2 = MIN_L; l2 <= MAX_L; l2++)
        {
            for (std::size_t l3 = MIN_L; l3 <= MAX_L; l3++)
            {
                for (std::size_t l4 = MIN_L; l4 <= MAX_L; l4++)
                {
                    for (std::size_t l5 = MIN_L; l5 <= MAX_L; l5++)
                    {
                        const std::array<std::size_t, M> L = {l1, l2, l3, l4, l5};
                        ioService.post(boost::bind(&task, L, n, lambda));
                    }
                }
            }
        }
    }
    
    
    threadpool.join_all();
    std::cout << "Finish";
    ioService.stop();
    
    
    return 0;
}
