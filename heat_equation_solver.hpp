/**
 * @file heat_equation_solver.hpp
 * @brief 1D and 2D Heat Equation Solvers - Implicit Finite Differences
 *
 * Solves: ∂u/∂t = α∇²u + F/(ρc)
 *
 * Variables:
 *   u(x,t) - temperature [K]
 *   α = λ/(ρc) - thermal diffusivity [m²/s]
 *   λ - conductivity [W/(m·K)]
 *   ρ - density [kg/m³]
 *   c - specific heat [J/(kg·K)]
 *   F - heat source [W/m³]
 *
 * Method: Backward Euler (implicit) scheme
 *   1D: Thomas algorithm O(n)
 *   2D: Gauss-Seidel iteration
 *
 * Boundary Conditions:
 *   Neumann (∂u/∂n=0): x=0 (1D), x=0,y=0 (2D)
 *   Dirichlet (u=u₀): x=L (1D), x=L,y=L (2D)
 */

#ifndef HEAT_EQUATION_SOLVER_HPP
#define HEAT_EQUATION_SOLVER_HPP

#include "material.hpp"
#include <vector>

namespace ensiie {

/**
 * @class HeatEquationSolver1D
 * @brief 1D heat equation solver
 *
 * Domain: x ∈ [0, L]
 * Scheme: -r·u[i-1] + (1+2r)·u[i] - r·u[i+1] = u[i]ⁿ + Δt·F/(ρc)
 * where r = αΔt/Δx²
 */
class HeatEquationSolver1D {
private:
    Material mat_;
    double L_;
    double tmax_;
    double dx_;
    double dt_;
    double u0_kelvin_;
    double t_;
    int n_;

    std::vector<double> u_;
    std::vector<double> F_;

    void init_source(double f);

    void solve_tridiagonal(
        const std::vector<double>& a,
        const std::vector<double>& b,
        const std::vector<double>& c,
        std::vector<double>& d,
        std::vector<double>& x
    );

public:
    HeatEquationSolver1D(
        const Material& mat,
        double L,
        double tmax,
        double u0,
        double f,
        int n
    );

    bool step();
    const std::vector<double>& get_temperature() const { return u_; }
    double get_time() const { return t_; }
    int get_n() const { return n_; }
    void reset();
};


/**
 * @class HeatEquationSolver2D
 * @brief 2D heat equation solver
 *
 * Domain: (x,y) ∈ [0,L]²
 * Scheme: 5-point stencil with Gauss-Seidel iteration
 * (1+4r)·u[i,j] - r·(neighbors) = u[i,j]ⁿ + Δt·F/(ρc)
 */
class HeatEquationSolver2D {
private:
    Material mat_;
    double L_;
    double tmax_;
    double dx_;
    double dt_;
    double u0_kelvin_;
    double t_;
    int n_;

    std::vector<double> u_;
    std::vector<double> F_;

    int idx(int i, int j) const { return j * n_ + i; }
    void init_source(double f);

public:
    HeatEquationSolver2D(
        const Material& mat,
        double L,
        double tmax,
        double u0,
        double f,
        int n
    );

    bool step();
    double get_temperature(int i, int j) const { return u_[idx(i, j)]; }
    std::vector<std::vector<double>> get_temperature_2d() const;
    double get_time() const { return t_; }
    double get_tmax() const { return tmax_; }
    int get_n() const { return n_; }
    void reset();
};

} // namespace ensiie

#endif
