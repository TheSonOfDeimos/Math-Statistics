#ifndef model_hpp
#define model_hpp

#include <iostream>
#include <numeric>
#include <array>
#include <cmath>
#include <vector>
#include <random>

#define MAX_VAL 1
#define MIN_VAL 0

template <class TDistr, class... Args>
class RandomGenerator
{
public:
    RandomGenerator(const Args &... args);
    auto getValue();

private:
    std::random_device rd_;
    std::mt19937 generator_;
    TDistr distr_;
};

template <class G, std::size_t S>
class Model_t
{
public:
    template <class... Args>
    Model_t(const Args &... args);

    void generate();
    double getExpectation();
    double getDispersion();
    const std::array<double, S> &getCorrelation();
    const std::array<double, S> &getData();
    const std::array<double, S> getSortedData();
    const std::vector<std::pair<double, double>> &getProbDensity();

private:
    void countExpectation();
    void countDispersion();
    void countCorrelation();
    void countProbDensity(const int partitions);

    G randGenerator_;
    std::array<double, S> value_arr_;
    std::array<double, S> correlation_coef_arr_;
    std::vector<std::pair<double, double>> prob_density_;
    double expectation_;
    double dispersion_;
};

typedef RandomGenerator<std::uniform_real_distribution<>, int, int> UniformGenerator;
typedef RandomGenerator<std::binomial_distribution<>, int, double> BinomialGenerator;
typedef RandomGenerator<std::geometric_distribution<>, int> GeometricGenerator;
typedef RandomGenerator<std::poisson_distribution<>, int> PoissonGenerator;
typedef RandomGenerator<std::lognormal_distribution<>, double, double> LogonormalGenerator;

//====================== Implementation =========================

template <class TDistr, class... Args>
RandomGenerator<TDistr, Args...>::RandomGenerator(const Args &... args)
    : rd_(),
      generator_(rd_()),
      distr_(args...)
{
}

template <class TDistr, class... Args>
auto RandomGenerator<TDistr, Args...>::getValue()
{
    return distr_(generator_);
}

template <class G, std::size_t S>
template <class... Args>
Model_t<G, S>::Model_t(const Args &... args)
    : randGenerator_(args...)
{
}

template <class G, std::size_t S>
void Model_t<G, S>::generate()
{
    std::srand(unsigned(std::time(0)));
    for (std::size_t i = 0; i < S; i++)
    {
        value_arr_.at(i) = randGenerator_.getValue();
    }

    countExpectation();
    countDispersion();
    countCorrelation();
    countProbDensity(10);
}

template <class G, std::size_t S>
double Model_t<G, S>::getExpectation()
{
    return expectation_;
}

template <class G, std::size_t S>
double Model_t<G, S>::getDispersion()
{
    return dispersion_;
}

template <class G, std::size_t S>
const std::array<double, S> &Model_t<G, S>::getCorrelation()
{
    return correlation_coef_arr_;
}

template <class G, std::size_t S>
const std::array<double, S> &Model_t<G, S>::getData()
{
    return value_arr_;
}

template <class G, std::size_t S>
const std::array<double, S> Model_t<G, S>::getSortedData()
{
    auto arr = value_arr_;
    std::sort(arr.begin(), arr.end());
    return arr;
}

template <class G, std::size_t S>
const std::vector<std::pair<double, double>> &Model_t<G, S>::getProbDensity()
{
    return prob_density_;
}

template <class G, std::size_t S>
void Model_t<G, S>::countExpectation()
{
    expectation_ = 0;
    expectation_ = std::accumulate(value_arr_.begin(), value_arr_.end(), expectation_) / S;
}

template <class G, std::size_t S>
void Model_t<G, S>::countDispersion()
{
    auto functor = [this](double res, double val) {
        return res + std::pow(val - this->expectation_, 2);
    };

    dispersion_ = 0;
    dispersion_ = std::accumulate(value_arr_.begin(), value_arr_.end(), dispersion_, functor) / S;
}

template <class G, std::size_t S>
void Model_t<G, S>::countCorrelation()
{

    for (std::size_t f = 0; f < S; f++)
    {
        double v1 = 0;
        double v2 = 0;
        for (std::size_t i = 0; i < S - f; i++)
        {
            v1 += (value_arr_.at(i) - expectation_) * (value_arr_.at(i + f) - expectation_);
        }

        for (std::size_t i = 0; i < S; i++)
        {
            v2 += pow(value_arr_.at(i) - expectation_, 2);
        }

        correlation_coef_arr_.at(f) = v1 / v2;
    }
}

template <class G, std::size_t S>
void Model_t<G, S>::countProbDensity(const int partitions)
{
    auto sorted_data = this->getSortedData();

    double step = (sorted_data.back() - sorted_data.front()) / partitions;
    double half_step = step / 2.0000;

    for (double h = sorted_data.front() - half_step; h <= sorted_data.back() + half_step; h += step)
    {
        int count = 0;
        for (auto &el : value_arr_)
        {
            if (el >= h - half_step && el < h + half_step)
            {
                count++;
            }
        }
        prob_density_.push_back({h, (count / static_cast<double>(S))});
    }
}

#endif /* model_hpp */
