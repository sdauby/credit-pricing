#pragma once
#include <vector>
#include <string>
#include <ostream>

class Tabulator {
public:
    void addHeader(std::vector<std::string>&& header);
    void startRow();
    void addCell(std::string&& cell);
    void output(std::ostream& os) const;
private:
    std::vector<std::vector<std::string>> headers;
    std::vector<std::vector<std::string>> rows;
    std::vector<size_t> widths;
    size_t headerHeight = 0;
};
