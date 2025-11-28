#ifndef HEAT_EQUATION_CPP
#define HEAT_EQUATION_CPP
#include <iostream>
#include <vector>
#include <memory>
#include <exception>

using namespace std;

class HeatEquation 
{
    private: 
        double _lambda;
        double _rho;
        double _constant_c;

        // For iteration
        int _index_k;

        // state t, x, in 2D
        vector<double> state_t_x = {0.0, 0.0}; 

    public:
        HeatEquation(double lambda, double rho, double constant_c);
        ~HeatEquation();

        // init value
        void write_equation() const;

        // insert numeric value
        void insert_variables();

        // solve equation
        double solve_heat_equation() const;

        // run simulation
        void run_simulation();
};

#endif