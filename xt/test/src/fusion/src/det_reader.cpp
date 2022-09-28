#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
int main(int argc, char const *argv[])
{
    std::string filename = "detections.csv";
    std::ifstream fp(filename);
    if (!fp.is_open())
    {
        std::cerr << "\x1B[31m"
                  << "Error: couldn't able to open " + filename + " for reading"
                  << "\033[0m" << std::endl;
        return false;
    }
    else
    {
        std::string hline;
        std::getline(fp, hline);
    }
    std::string line, sval;
    double dval;
    while (std::getline(fp, line))
    {
        std::stringstream sline(line);
        std::vector<double> vals;
        while (std::getline(sline, sval, ','))
        {
            dval = std::stod(sval);
            vals.push_back(dval);
        }
        if (9 == vals.size())
        {
        }
    }
    return 0;
}
