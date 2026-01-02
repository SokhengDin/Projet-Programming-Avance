# Problems and Solutions

## Problem 1: Numerical Stability of Heat Equation Solver

### Problem Description

The heat equation is given by:

$$\frac{\partial u}{\partial t} = \alpha \nabla^2 u + \frac{F}{\rho c}$$

where:
- $u(x,t)$ is temperature [K]
- $\alpha = \frac{\lambda}{\rho c}$ is thermal diffusivity [m²/s]
- $\lambda$ is thermal conductivity [W/(m·K)]
- $\rho$ is density [kg/m³]
- $c$ is specific heat [J/(kg·K)]
- $F$ is heat source [W/m³]

**The Challenge:**

Explicit schemes (Forward Euler) discretize as:

$$\frac{u_i^{n+1} - u_i^n}{\Delta t} = \alpha \frac{u_{i+1}^n - 2u_i^n + u_{i-1}^n}{\Delta x^2} + \frac{F_i}{\rho c}$$

This gives: $u_i^{n+1} = u_i^n + r(u_{i+1}^n - 2u_i^n + u_{i-1}^n) + \Delta t \frac{F_i}{\rho c}$

where $r = \frac{\alpha \Delta t}{\Delta x^2}$

**Problem:** Theses Explicit schemes are only stable when:

$$r = \frac{\alpha \Delta t}{\Delta x^2} \leq \frac{1}{2}$$

This means that $\Delta t \leq \frac{\Delta x^2}{2\alpha}$

For Copper with $\alpha = 1.14 \times 10^{-4}$ m²/s and $\Delta x = 0.01$ m:

$$\Delta t \leq \frac{(0.01)^2}{2 \times 1.14 \times 10^{-4}} = 0.439 \text{ seconds}$$

To simulate 16 seconds would require **36 time steps minimum**. But for accuracy, we need much smaller $\Delta t$, potentially **thousands of time steps**, making real-time visualization impossible.

### Solution: Implicit Backward Euler Scheme

We use the **Backward Euler (implicit) scheme**:

$$\frac{u_i^{n+1} - u_i^n}{\Delta t} = \alpha \frac{u_{i+1}^{n+1} - 2u_i^{n+1} + u_{i-1}^{n+1}}{\Delta x^2} + \frac{F_i}{\rho c}$$

Rearranging:

$$-r \cdot u_{i-1}^{n+1} + (1 + 2r) \cdot u_i^{n+1} - r \cdot u_{i+1}^{n+1} = u_i^n + \Delta t \frac{F_i}{\rho c}$$

This scheme is **unconditionally stable** for any $\Delta t > 0$, allowing larger time steps while maintaining accuracy.

**Tradeoff:** We must solve a linear system at each time step instead of direct computation.

### Implementation Details

#### 1D Case: Thomas Algorithm

The implicit scheme produces a **tridiagonal system**:

$$\begin{bmatrix}
b_0 & c_0 & 0 & \cdots & 0 \\
a_1 & b_1 & c_1 & \cdots & 0 \\
0 & a_2 & b_2 & \cdots & 0 \\
\vdots & \vdots & \vdots & \ddots & \vdots \\
0 & 0 & 0 & a_{n-1} & b_{n-1}
\end{bmatrix}
\begin{bmatrix}
u_0^{n+1} \\
u_1^{n+1} \\
u_2^{n+1} \\
\vdots \\
u_{n-1}^{n+1}
\end{bmatrix}
=
\begin{bmatrix}
d_0 \\
d_1 \\
d_2 \\
\vdots \\
d_{n-1}
\end{bmatrix}$$

We solve using the **Thomas algorithm** (tridiagonal matrix algorithm) with **O(n) complexity**:

```cpp
void HeatEquationSolver1D::solve_tridiagonal(
    const std::vector<double>& a,  // Lower diagonal
    const std::vector<double>& b,  // Main diagonal
    const std::vector<double>& c,  // Upper diagonal
    std::vector<double>& d,        // Right-hand side
    std::vector<double>& x         // Solution
) {
    int n = b.size();
    std::vector<double> c_star(n);
    std::vector<double> d_star(n);

    // Forward sweep
    c_star[0] = c[0] / b[0];
    d_star[0] = d[0] / b[0];

    for (int i = 1; i < n; i++) {
        double m = 1.0 / (b[i] - a[i] * c_star[i-1]);
        c_star[i] = c[i] * m;
        d_star[i] = (d[i] - a[i] * d_star[i-1]) * m;
    }

    // Back substitution
    x[n-1] = d_star[n-1];
    for (int i = n-2; i >= 0; i--) {
        x[i] = d_star[i] - c_star[i] * x[i+1];
    }
}
```

**File:** `heat_equation_solver.cpp:108-145`

#### 2D Case: Gauss-Seidel Iteration

For 2D, the discretization becomes:

$$\frac{u_{i,j}^{n+1} - u_{i,j}^n}{\Delta t} = \alpha \left( \frac{u_{i+1,j}^{n+1} - 2u_{i,j}^{n+1} + u_{i-1,j}^{n+1}}{\Delta x^2} + \frac{u_{i,j+1}^{n+1} - 2u_{i,j}^{n+1} + u_{i,j-1}^{n+1}}{\Delta y^2} \right) + \frac{F_{i,j}}{\rho c}$$

With $\Delta x = \Delta y$:

$$(1 + 4r) \cdot u_{i,j}^{n+1} - r \cdot (u_{i+1,j}^{n+1} + u_{i-1,j}^{n+1} + u_{i,j+1}^{n+1} + u_{i,j-1}^{n+1}) = u_{i,j}^n + \Delta t \frac{F_{i,j}}{\rho c}$$

This creates a **large sparse linear system**: an $(n^2 \times n^2)$ matrix with only 5 non-zero entries per row (5-point stencil).

**Why not direct solution?**
- Dense direct methods (like Gaussian elimination without exploiting sparsity) would require $O(n^6)$ operations - completely infeasible
- Even sparse direct solvers (like sparse LU factorization) require significant memory and complexity
- For large grids (e.g., $n = 100 \Rightarrow 10,000 \times 10,000$ system), iterative methods are more practical

**Our choice:** We use **Gauss-Seidel iteration**, an iterative method that exploits the sparse structure and updates each point using:

$$u_{i,j}^{new} = \frac{1}{1 + 4r} \left[ u_{i,j}^n + \Delta t \frac{F_{i,j}}{\rho c} + r(u_{i+1,j} + u_{i-1,j} + u_{i,j+1} + u_{i,j-1}) \right]$$

```cpp
bool HeatEquationSolver2D::step() {
    const int max_iter = 100;
    const double tol = 1e-6;

    std::vector<double> u_new = u_;
    double coeff = 1.0 / (1.0 + 4.0 * r);

    for (int iter = 0; iter < max_iter; iter++) {
        double max_diff = 0.0;

        for (int j = 1; j < n_ - 1; j++) {
            for (int i = 1; i < n_ - 1; i++) {
                double rhs = u_[idx(i,j)] + dt_ * F_[idx(i,j)] / (mat_.rho * mat_.c);
                rhs += r * (u_new[idx(i+1,j)] + u_new[idx(i-1,j)] +
                           u_new[idx(i,j+1)] + u_new[idx(i,j-1)]);

                double u_old = u_new[idx(i,j)];
                u_new[idx(i,j)] = coeff * rhs;
                max_diff = std::max(max_diff, std::abs(u_new[idx(i,j)] - u_old));
            }
        }

        if (max_diff < tol) break;
    }

    u_ = u_new;
    t_ += dt_;
    return t_ < tmax_;
}
```

**File:** `heat_equation_solver.cpp:247-278`

---

## Problem 2: Mixed Boundary Conditions Implementation

### Problem Description

The problem specification requires two different types of boundary conditions:

1. **Neumann boundary condition** (zero flux): $\frac{\partial u}{\partial n} = 0$ at $x = 0$ (and $y = 0$ for 2D)
2. **Dirichlet boundary condition** (fixed temperature): $u = u_0$ at $x = L$ (and $y = L$ for 2D)

**Physical Meaning:**
- Neumann BC: Insulated boundary - no heat flow across
- Dirichlet BC: Boundary held at constant temperature (heat sink)

**Challenge:** The derivative in Neumann BC doesn't directly fit into our discretization.

### Solution: First-Order Forward Difference (Neumann BC)

For Neumann BC at $x = 0$, we use a **first-order forward difference** approximation:

$$\frac{\partial u}{\partial x}\Big|_{x=0} \approx \frac{u_1^{n+1} - u_0^{n+1}}{\Delta x} = 0$$

This gives: $u_0^{n+1} = u_1^{n+1}$ (equality condition at boundary)

To implement this in the implicit scheme, we write the heat equation at $i=0$:

$$\frac{u_0^{n+1} - u_0^n}{\Delta t} = \alpha \frac{u_1^{n+1} - u_0^{n+1}}{\Delta x^2} + \frac{F_0}{\rho c}$$

Using $r = \frac{\alpha \Delta t}{\Delta x^2}$:

$$u_0^{n+1} - u_0^n = r(u_1^{n+1} - u_0^{n+1}) + \Delta t \frac{F_0}{\rho c}$$

Rearranging:

$$(1 + r) \cdot u_0^{n+1} - r \cdot u_1^{n+1} = u_0^n + \Delta t \frac{F_0}{\rho c}$$

**Note:** This is a first-order accurate approximation for the Neumann boundary. For higher accuracy, a second-order scheme using ghost points could be used, but first-order is sufficient for this application and simpler to implement.

**Implementation (1D):**

```cpp
bool HeatEquationSolver1D::step() {
    // ... setup vectors a, b, c, d ...

    // Neumann BC at i=0: du/dx = 0
    a[0] = 0.0;
    b[0] = 1.0 + r;
    c[0] = -r;
    d[0] = u_[0] + dt_ * F_[0] / (mat_.rho * mat_.c);

    // Interior points
    for (int i = 1; i < n_ - 1; i++) {
        a[i] = -r;
        b[i] = 1.0 + 2.0 * r;
        c[i] = -r;
        d[i] = u_[i] + dt_ * F_[i] / (mat_.rho * mat_.c);
    }

    // Dirichlet BC at i=n-1: u = u0_kelvin_
    a[n_-1] = 0.0;
    b[n_-1] = 1.0;
    c[n_-1] = 0.0;
    d[n_-1] = u0_kelvin_;

    solve_tridiagonal(a, b, c, d, u_);
    t_ += dt_;
    return t_ < tmax_;
}
```

**File:** `heat_equation_solver.cpp:147-176`

**Implementation (2D):**

For 2D, Neumann BC at $x = 0$ and $y = 0$:

```cpp
// Apply boundary conditions
for (int j = 0; j < n_; j++) {
    // Neumann at x=0: u[-1,j] = u[1,j]
    u_new[idx(0,j)] = u_new[idx(1,j)];

    // Dirichlet at x=L: u[n-1,j] = u0
    u_new[idx(n_-1,j)] = u0_kelvin_;
}

for (int i = 0; i < n_; i++) {
    // Neumann at y=0: u[i,-1] = u[i,1]
    u_new[idx(i,0)] = u_new[idx(i,1)];

    // Dirichlet at y=L: u[i,n-1] = u0
    u_new[idx(i,n_-1)] = u0_kelvin_;
}
```

**File:** `heat_equation_solver.cpp:258-276`

---

## Problem 3: Heat Source Visibility

### Problem Description

The specification defines heat source as:

$$F = t_{max} \cdot f^2$$

where $f = 80°C = 80K$ and $t_{max} = 16s$.

So: $F = 16 \times 80^2 = 102,400$ W/m³

However, the actual heating rate is:

$$\frac{\Delta u}{\Delta t} = \frac{F}{\rho c}$$

For Iron: $\rho c = 7874 \times 440 = 3,464,560$ J/(m³·K)

Heating rate: $\frac{102,400}{3,464,560} \approx 0.0296$ K/s

Over $\Delta t = 0.1$ s: $\Delta u \approx 0.003$ K per time step!

**Problem:** With initial temperature $u_0 = 286.15$ K, a change of 0.003 K is **invisible** - less than 0.001% change. Users see no heat propagation!

### Solution: Non-Physical Source Amplification for Visualization

**Important Note:** The physical model specified in the project uses $F = t_{max} \cdot f^2$, which produces extremely slow, barely visible heating. To enable **interactive real-time visualization** while preserving the solver accuracy, we made a **pragmatic choice**:

We added a **scale factor of 100×** to amplify the heat source **for demonstration purposes only**. This modification **does not reflect the exact physical model** specified, but rather creates a more dramatic scenario that shows heat propagation clearly.

```cpp
void HeatEquationSolver1D::init_source(double f) {
    double f1 = tmax_ * f * f;        // Physical value = 102,400 W/m³
    double f2 = 0.75 * tmax_ * f * f; // Physical value = 76,800 W/m³
    double scale = 100.0;              // Non-physical amplification for visibility

    for (int i = 0; i < n_; i++) {
        double x = i * dx_;
        if (x >= L_ / 10.0 && x <= 2.0 * L_ / 10.0) {
            F_[i] = f1 * scale;  // Visualization: 10,240,000 W/m³
        } else if (x >= 5.0 * L_ / 10.0 && x <= 6.0 * L_ / 10.0) {
            F_[i] = f2 * scale;  // Visualization: 7,680,000 W/m³
        } else {
            F_[i] = 0.0;
        }
    }
}
```

**Result:** Now heating rate is 2.96 K/s for Iron, giving **0.3 K per time step** - easily visible!

**Alternative approach:** Instead of scaling the source, one could visualize the temperature difference $\Delta u = u - u_0$ with auto-ranging. This would preserve physical accuracy while achieving visibility. However, for a demonstration/teaching tool, the amplified source creates more dramatic and pedagogically useful visualizations.

**Files:** `heat_equation_solver.cpp:73-91` (1D), `heat_equation_solver.cpp:197-216` (2D)

---

## Problem 4: Multi-Material Thermal Diffusivity Range

### Problem Description

The four materials have vastly different thermal diffusivities:

| Material | $\alpha$ (m²/s) | Ratio to Copper |
|----------|-----------------|-----------------|
| Copper | $1.14 \times 10^{-4}$ | 1× |
| Iron | $2.31 \times 10^{-5}$ | 0.2× |
| Glass | $5.64 \times 10^{-7}$ | 0.005× |
| Polystyrene | $8.01 \times 10^{-8}$ | 0.0007× |

**Factor difference:** Copper diffuses heat **1,423× faster** than Polystyrene!

**Challenge:** In the same time period:
- Copper shows rapid heat propagation across entire domain
- Polystyrene shows almost no propagation (heat stays localized)

This makes comparison difficult - they need different visualization scales and time frames.

### Solution: 2×2 Grid Mode with Independent Scaling

We implemented **simultaneous multi-solver architecture**:

```cpp
class SDLApp {
private:
    // Grid mode: 4 independent solvers
    std::unique_ptr<ensiie::HeatEquationSolver1D> solvers_1d_[4];
    std::unique_ptr<ensiie::HeatEquationSolver2D> solvers_2d_[4];
    ensiie::Material materials_[4];

    void render_grid() {
        for (int idx = 0; idx < 4; idx++) {
            int row = idx / 2;
            int col = idx % 2;
            int cell_x = col * (win_w / 2);
            int cell_y = row * (win_h / 2);

            // Each cell has independent temperature scaling
            if (sim_type_ == SimType::BAR_1D) {
                auto temps = solvers_1d_[idx]->get_temperature();
                heatmap_->auto_range(temps);  // Auto-scale per material
                heatmap_->draw_1d_cell(temps, info, cell_x, cell_y, win_w/2, win_h/2);
            } else {
                auto temps = solvers_2d_[idx]->get_temperature_2d();
                heatmap_->auto_range_2d(temps);  // Auto-scale per material
                heatmap_->draw_2d_cell(temps, info, cell_x, cell_y, win_w/2, win_h/2);
            }
        }
    }
};
```


**Files:** `sdl_app.cpp:135-157`, `sdl_app.hpp:46-51`

---

## Simulation Results

### 1D Simulations
- **Copper:** Rapid diffusion, smooth wide profiles
- **Iron:** Moderate diffusion, steeper gradients
- **Glass:** Limited diffusion, sharp localized peaks
- **Polystyrene:** Negligible diffusion, heat confined to sources

### 2D Simulations
- **Copper:** Large thermal halos, efficient radial propagation
- **Iron:** Moderate spreading, rounded isotherms
- **Glass:** Highly localized, sharp boundaries
- **Polystyrene:** Strong insulation, minimal propagation

The 2×2 grid mode confirms the three-order-of-magnitude thermal diffusivity differences through observable spatial extent, profile smoothness, and heat flow patterns.

---
