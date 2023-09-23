#include "OtherFunction.h"

std::string formatDoubleToDecimalPlaces(double number, int precision)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(precision) << number;
    return oss.str();
}