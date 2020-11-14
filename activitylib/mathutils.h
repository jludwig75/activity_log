#pragma once

#include <cmath>


template<typename ValueType>
ValueType radians(ValueType v)
{
    return (v * M_PI) / 180.0;
}
