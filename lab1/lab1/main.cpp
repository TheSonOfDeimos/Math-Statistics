#include <iostream>
#include <numeric>
#include <array>
#include <cmath>
#include <iomanip>
#include <OpenXLSX/OpenXLSX.h>

#include "writer.hpp"

int main(int argc, const char *argv[])
{
    const std::size_t S = 10000;
    Model_t<UniformGenerator, S> m1(0, 1);
    Model_t<BinomialGenerator, S> m2(10, 0.5);
    Model_t<LogonormalGenerator, S> m3(1.6, 0.25);
    m1.generate();
    m2.generate();
    m3.generate();

    ExelWriter exel("./lab2_P2.xlsx");
    exel << m1 << m2 << m3;
    return 0;
}
