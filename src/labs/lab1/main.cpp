#include <iostream>
#include <numeric>
#include <array>
#include <cmath>
#include <iomanip>
#include <OpenXLSX/OpenXLSX.h>

#include "writer.hpp"

void lab1()
{
    Model_t<UniformGenerator, 10> m1(0, 1);
    Model_t<UniformGenerator, 100> m2(0, 1);
    Model_t<UniformGenerator, 1000> m3(0, 1);
    Model_t<UniformGenerator, 10000> m4(0, 1);
    
    m1.generate();
    m2.generate();
    m3.generate();
    m4.generate();

    ExelWriter exel("./lab1.xlsx");
    exel << m1 << m2 << m3 << m4;
}

template<std::size_t S, class Gen, class ... Args>
void gen_tester(std::string file_path, const Args& ... args)
{
    Model_t<Gen, S> m1(args ...);
    Model_t<Gen, S> m2(args ...);
    Model_t<Gen, S> m3(args ...);
    
    m1.generate();
    m2.generate();
    m3.generate();
    
    ExelWriter exel(file_path);
    exel << m1 << m2 << m3;
    
}

void lab2()
{
    gen_tester<1000, UniformGenerator>("./lab2_uniform_gen.xlsx", 0, 1);
    gen_tester<1000, BinomialGenerator>("./lab2_binomial_gen.xlsx", 40, 0.5);
    gen_tester<1000, GeometricGenerator>("./lab2_geometric_gen.xlsx", 0.5);
    gen_tester<1000, PoissonGenerator>("./lab2_poisson_gen.xlsx", 4);
}

void lab3()
{
    gen_tester<10000, UniformGenerator>("./lab3_uniform_gen.xlsx", 2, 10);
    gen_tester<10000, NormalGenerator>("./lab3_normal_gen.xlsx", -2.0, 2.5);
    gen_tester<10000, ExponentialGenerator>("./lab3_exponential_gen.xlsx", 1.5);
    gen_tester<10000, ChiSquareGenerator>("./lab3_chi_square_gen.xlsx", 3.0);
    gen_tester<10000, StudentGenerator>("./lab3_student_gen.xlsx", 5.0);
    
}

int main(int argc, const char *argv[])
{
    
    lab3();
    return 0;
}

