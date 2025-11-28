#include "heat_equation.hpp"


HeatEquation::HeatEquation(
    double lambda
    , double rho
    , double constant_c
): _lambda(lambda), _rho(rho), _constant_c(constant_c)
{

}

HeatEquation::~HeatEquation() {
    
};