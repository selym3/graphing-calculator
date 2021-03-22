#ifndef __ENGINE_HPP__
#define __ENGINE_HPP__

#include <cmath>
#include <string>

#include <vector>

class Engine
{

public:
    Engine(const std::string& input) {}

    double operator()(double x) 
    {
        return std::tanh(x);
    }

};

#endif