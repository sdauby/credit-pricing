#include "Core/Tabulator.hpp"

void Tabulator::addHeader(std::vector<std::string>&& header)
{
    auto width = size_t(0);
    for (const auto &s : header) {
        if (const auto w = s.size(); w > width)
            width = w;
    }
    widths.push_back(width);

    if (const auto h = header.size(); h > headerHeight)
        headerHeight = h;

    headers.push_back(std::move(header));
}
    
void Tabulator::startRow()
{
    rows.emplace_back();
}
    
void Tabulator::addCell(std::string&& cell) 
{
    auto& row = rows.back();
    row.push_back(std::move(cell));
    const auto j = row.size() - 1;
    if (const auto w = row[j].size(); w > widths[j])
        widths[j] = w;
}
    
void Tabulator::output(std::ostream& os) const
{
    const auto m = headerHeight;
    const auto n = headers.size();
    const auto m_ = rows.size();

    const auto fill = [&widths=widths] (std::string&& s, size_t j) {
        return s + std::string(widths[j]-s.size(),' ');
    };

    const auto separator = [n] (size_t j) {
        if (j==n-1)
            return '\n';
        else
            return ' ';
    };

    const auto headerCell = [&headers=headers, m] (size_t i, size_t j) {
        const auto& header = headers[j];
        const auto gap = m-header.size();
        if (i<gap)
            return std::string();
        else
            return header[i-gap];
    };

    const auto rulerCell = [&widths=widths] (size_t j) {
        return std::string(widths[j], '-');
    };

    const auto rowCell = [&rows=rows] (size_t i, size_t j) {
        const auto& row = rows[i];
        if (j<row.size())
            return row[j];
        else
            return std::string();
    };

    for (auto i = size_t(0); i<m; ++i) {
        for (auto j = size_t(0); j<n; ++j)
            os << fill( headerCell(i,j), j ) << separator(j);
    }

    for (auto j = size_t(0); j<n; ++j)
        os << rulerCell(j) << separator(j);

    for (auto i = size_t(0); i<m_; ++i) {
        for (auto j = size_t(0); j<n; ++j)
            os << fill( rowCell(i,j), j ) << separator(j);
    }
}
