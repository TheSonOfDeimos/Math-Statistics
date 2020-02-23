#ifndef writer_hpp
#define writer_hpp

#include <iostream>
#include <numeric>
#include <array>
#include <cmath>
#include <iomanip>
#include <OpenXLSX/OpenXLSX.h>

#include "model.hpp"

using namespace std;
using namespace OpenXLSX;

class ExelWriter
{
public:
    ExelWriter(const std::string &filepath);
    ~ExelWriter();

    template <std::size_t S, class T, template <class, std::size_t> class G>
    void addResult(G<T, S> &mod);
    void addSummary(int n, double expectation, double dispercion);

    template <std::size_t S>
    void addSequence(int n, const std::array<double, S> &data);

    template <std::size_t S>
    void addSortedSequence(int n, const std::array<double, S> &data);

    template <std::size_t S>
    void addCorrelation(int n, const std::array<double, S> &data);
    void addProbDensity(int n, const std::vector<std::pair<double, double>> &data);

private:
    void initTables();
    XLDocument doc_;
};

//====================== Implementation =========================

ExelWriter::ExelWriter(const std::string &filepath)
{
    doc_.CreateDocument(filepath);
    initTables();
}

ExelWriter::~ExelWriter()
{
    doc_.SaveDocument();
}

template <std::size_t S, class T, template <class, std::size_t> class G>
void ExelWriter::addResult(G<T, S> &mod)
{
    addSummary(S, mod.getExpectation(), mod.getDispersion());
    addSequence(S, mod.getData());
    addSortedSequence(S, mod.getSortedData());
    addCorrelation(S, mod.getCorrelation());
    addProbDensity(S, mod.getProbDensity());
}

void ExelWriter::addSummary(int n, double expectation, double dispercion)
{
    auto wks = doc_.Workbook().Worksheet("Summary");
    wks.Cell(wks.RowCount() + 1, 1).Value() = n;
    wks.Cell(wks.RowCount(), 2).Value() = expectation;
    wks.Cell(wks.RowCount(), 3).Value() = dispercion;
}

template <std::size_t S>
void ExelWriter::addSequence(int n, const std::array<double, S> &data)
{
    auto wks = doc_.Workbook().Worksheet("Data");
    wks.Cell(1, wks.ColumnCount() + 1).Value() = std::string("N = " + std::to_string(S));
    for (auto i = 0; i < data.size(); i++)
    {
        wks.Cell(i + 2, wks.ColumnCount()).Value() = data.at(i);
        wks.Cell(i + 2, 1).Value() = i;
    }
}

template <std::size_t S>
void ExelWriter::addSortedSequence(int n, const std::array<double, S> &data)
{
    auto wks = doc_.Workbook().Worksheet("Sorted Data");
    wks.Cell(1, wks.ColumnCount() + 1).Value() = std::string("N = " + std::to_string(S));
    for (auto i = 0; i < data.size(); i++)
    {
        wks.Cell(i + 2, wks.ColumnCount()).Value() = data.at(i);
        wks.Cell(i + 2, 1).Value() = i;
    }
}

template <std::size_t S>
void ExelWriter::addCorrelation(int n, const std::array<double, S> &data)
{
    auto wks = doc_.Workbook().Worksheet("Correlation");
    wks.Cell(1, wks.ColumnCount() + 1).Value() = std::string("N = " + std::to_string(S));
    for (auto i = 0; i < data.size(); i++)
    {
        wks.Cell(i + 2, wks.ColumnCount()).Value() = data.at(i);
        wks.Cell(i + 2, 1).Value() = i;
    }
}

void ExelWriter::addProbDensity(int n, const std::vector<std::pair<double, double>> &data)
{
    auto wks = doc_.Workbook().Worksheet("Prob Density");
    wks.Cell(1, wks.ColumnCount() + 1).Value() = std::string("N = " + std::to_string(n) + " [STEP]");
    wks.Cell(1, wks.ColumnCount() + 1).Value() = std::string("N = " + std::to_string(n) + " [VALUE]");
    for (auto i = 0; i < data.size(); i++)
    {
        wks.Cell(i + 2, wks.ColumnCount() - 1).Value() = data.at(i).first;
        wks.Cell(i + 2, wks.ColumnCount()).Value() = data.at(i).second;
        wks.Cell(i + 2, 1).Value() = i;
    }
}

void ExelWriter::initTables()
{
    doc_.Workbook().AddWorksheet("Summary");
    auto wks = doc_.Workbook().Worksheet("Summary");
    wks.Cell("A1").Value() = "N";
    wks.Cell("B1").Value() = "Expectation";
    wks.Cell("C1").Value() = "Dispersion";

    doc_.Workbook().AddWorksheet("Data");
    doc_.Workbook().AddWorksheet("Sorted Data");
    doc_.Workbook().AddWorksheet("Correlation");
    doc_.Workbook().AddWorksheet("Prob Density");
}

template <class G>
ExelWriter &operator<<(ExelWriter &wr, G &mod)
{
    wr.addResult(mod);
    return wr;
}

#endif /* writer_hpp */
