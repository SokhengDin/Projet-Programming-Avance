/**
 * @file heat_equation_solver.hpp
 * @brief 1D and 2D heat equation solvers using implicit finite differences.
 *
 * This module provides numerical solvers for the heat equation:
 * @f[
 *   \frac{\partial u}{\partial t} = \alpha \nabla^2 u + \frac{F}{\rho c}
 * @f]
 *
 * where:
 * - u(x,t) is the temperature [K]
 * - α = λ / (ρc) is the thermal diffusivity [m²/s]
 * - λ is the thermal conductivity [W/(m·K)]
 * - ρ is the density [kg/m³]
 * - c is the specific heat capacity [J/(kg·K)]
 * - F is a volumetric heat source [W/m³]
 *
 * Numerical methods:
 * - 1D: Backward Euler implicit scheme solved with Thomas algorithm
 * - 2D: Backward Euler implicit scheme solved with Gauss–Seidel iterations
 *
 * Boundary conditions:
 * - Neumann (zero flux) on left/bottom boundaries
 * - Dirichlet (fixed temperature) on right/top boundaries
 */

#ifndef HEAT_EQUATION_SOLVER_HPP
#define HEAT_EQUATION_SOLVER_HPP

#include "material.hpp"
#include <vector>

namespace ensiie {

/**
 * @class HeatEquationSolver1D
 * @brief Implicit finite difference solver for the 1D heat equation.
 *
 * Solves the heat equation on the domain x ∈ [0, L] using a backward
 * Euler time discretization and centered finite differences in space.
 *
 * The resulting tridiagonal linear system is solved using the Thomas
 * algorithm with O(n) complexity.
 *
 * Boundary conditions:
 * - Neumann condition (∂u/∂x = 0) at x = 0
 * - Dirichlet condition (u = u₀) at x = L
 */
class HeatEquationSolver1D {
private:
    Material mat_;        /**< Material properties (λ, ρ, c) */
    double L_;            /**< Length of the 1D domain */
    double tmax_;         /**< Maximum simulation time */
    double dx_;           /**< Spatial discretization step */
    double dt_;           /**< Time discretization step */
    double u0_kelvin_;    /**< Initial temperature (Kelvin) */
    double t_;            /**< Current simulation time */
    int n_;               /**< Number of grid points */

    std::vector<double> u_; /**< Temperature field */
    std::vector<double> F_; /**< Heat source term */

    /**
     * @brief Initialize the spatial heat source.
     * @param f Source amplitude.
     */
    void init_source(double f);

    /**
     * @brief Solve a tridiagonal linear system using the Thomas algorithm.
     *
     * @param a Sub-diagonal coefficients
     * @param b Main diagonal coefficients
     * @param c Super-diagonal coefficients
     * @param d Right-hand side vector
     * @param x Solution vector
     */
    void solve_tridiagonal(
        const std::vector<double>& a,
        const std::vector<double>& b,
        const std::vector<double>& c,
        std::vector<double>& d,
        std::vector<double>& x
    );

public:
    /**
     * @brief Construct a 1D heat equation solver.
     *
     * @param mat Material properties
     * @param L Length of the domain
     * @param tmax Maximum simulation time
     * @param u0 Initial temperature (°C)
     * @param f Heat source amplitude
     * @param n Number of spatial grid points
     */
    HeatEquationSolver1D(
        const Material& mat,
        double L,
        double tmax,
        double u0,
        double f,
        int n
    );

    /**
     * @brief Advance the solution by one time step.
     * @return false if the final time is reached
     */
    bool step();

    /**
     * @brief Get the current temperature field.
     */
    const std::vector<double>& get_temperature() const { return u_; }

    /**
     * @brief Get the current simulation time.
     */
    double get_time() const { return t_; }

    /**
     * @brief Get the number of grid points.
     */
    int get_n() const { return n_; }

    /**
     * @brief Reset the solver to the initial state (t=0, u=u0)
     */
    void reset();
};


/**
 * @class HeatEquationSolver2D
 * @brief Implicit finite difference solver for the 2D heat equation.
 *
 * Solves the heat equation on a square domain [0, L]² using a five-point
 * stencil and a backward Euler time discretization.
 *
 * The implicit system is solved using Gauss–Seidel iterations.
 *
 * Boundary conditions:
 * - Neumann condition on left and bottom boundaries
 * - Dirichlet condition on right and top boundaries
 */
class HeatEquationSolver2D {
private:
    Material mat_;        /**< Material properties */
    double L_;            /**< Domain size */
    double tmax_;         /**< Maximum simulation time */
    double dx_;           /**< Spatial step */
    double dt_;           /**< Time step */
    double u0_kelvin_;    /**< Initial temperature in Kelvin */
    double t_;            /**< Current time */
    int n_;               /**< Grid points per dimension */

    std::vector<double> u_; /**< Temperature field (row-major) */
    std::vector<double> F_; /**< Heat source */

    /**
     * @brief Convert 2D indices to 1D index.
     */
    int idx(int i, int j) const { return j * n_ + i; }

    /**
     * @brief Initialize the 2D heat source.
     */
    void init_source(double f);

public:
    /**
     * @brief Construct a 2D heat equation solver.
     */
    HeatEquationSolver2D(
        const Material& mat,
        double L,
        double tmax,
        double u0,
        double f,
        int n
    );

    /**
     * @brief Advance one step using Gauss-Seidel iteration
     */
    bool step();

    /**
     * @brief Get temperature at grid point (i,j).
     */
    double get_temperature(int i, int j) const { return u_[idx(i, j)]; }

    /**
     * @brief Get the full temperature field as a 2D array.
     */
    std::vector<std::vector<double>> get_temperature_2d() const;
    double get_time() const { return t_; }
    double get_tmax() const { return tmax_; }
    int get_n() const { return n_; }

    /**
     * @brief Reset the solver to the initial state.
     */
    void reset();
};

} // namespace ensiie

#endif
