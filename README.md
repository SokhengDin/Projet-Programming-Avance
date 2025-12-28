# Heat Equation Simulation

A C++ project that simulates heat diffusion in 1D (bar) and 2D (plate) using the finite difference method. The simulation uses SDL2 for real-time visualization with a heatmap display.

## Mathematical Formulation

### Heat Equation

The heat equation describes how temperature $u$ evolves over time in a material:

$$\frac{\partial u}{\partial t} = \frac{\lambda}{\rho c} \Delta u + \frac{F}{\rho c}$$

Where:
- $u(t, x)$ : Temperature in Kelvin at time $t$ and position $x$
- $\lambda$ : Thermal conductivity (W/(mK))
- $\rho$ : Density (kg/m^3)
- $c$ : Specific heat capacity (J/(kgK))
- $F$ : Heat source term
- $\Delta$ : Laplacian operator

### 1D Case (Bar)

$$\frac{\partial u}{\partial t} = \alpha \frac{\partial^2 u}{\partial x^2} + \frac{F}{\rho c}$$

With $\alpha = \frac{\lambda}{\rho c}$ (thermal diffusivity).

**Boundary Conditions:**
- Neumann at $x = 0$: $\frac{\partial u}{\partial x}(t, 0) = 0$ (insulated)
- Dirichlet at $x = L$: $u(t, L) = u_0$ (fixed temperature)

**Heat Sources:**

$$F(x) = \begin{cases} t_{\max} f^2 & \text{if } x \in \left[\frac{L}{10}, \frac{2L}{10}\right] \\ \frac{3}{4} t_{\max} f^2 & \text{if } x \in \left[\frac{5L}{10}, \frac{6L}{10}\right] \\ 0 & \text{otherwise} \end{cases}$$

### 2D Case (Plate)

$$\frac{\partial u}{\partial t} = \alpha \left( \frac{\partial^2 u}{\partial x^2} + \frac{\partial^2 u}{\partial y^2} \right) + \frac{F}{\rho c}$$

**Boundary Conditions:**
- Neumann at $x = 0$ and $y = 0$: $\frac{\partial u}{\partial n} = 0$
- Dirichlet at $x = L$ and $y = L$: $u = u_0$

**Heat Sources:** Four symmetric regions at:
- $\left[\frac{L}{6}, \frac{2L}{6}\right] \times \left[\frac{L}{6}, \frac{2L}{6}\right]$
- $\left[\frac{4L}{6}, \frac{5L}{6}\right] \times \left[\frac{L}{6}, \frac{2L}{6}\right]$
- $\left[\frac{L}{6}, \frac{2L}{6}\right] \times \left[\frac{4L}{6}, \frac{5L}{6}\right]$
- $\left[\frac{4L}{6}, \frac{5L}{6}\right] \times \left[\frac{4L}{6}, \frac{5L}{6}\right]$

Each with $F = t_{\max} f^2$.

### Numerical Method

**Implicit Finite Difference Scheme (Backward Euler):**

For 1D, discretizing with spatial step $\Delta x$ and time step $\Delta t$:

$$\frac{u_i^{n+1} - u_i^n}{\Delta t} = \alpha \frac{u_{i-1}^{n+1} - 2u_i^{n+1} + u_{i+1}^{n+1}}{\Delta x^2} + \frac{F_i}{\rho c}$$

Rearranging with $r = \frac{\alpha \Delta t}{\Delta x^2}$:

$$-r \cdot u_{i-1}^{n+1} + (1 + 2r) \cdot u_i^{n+1} - r \cdot u_{i+1}^{n+1} = u_i^n + \frac{\Delta t}{\rho c} F_i$$

This forms a **tridiagonal system** $A \mathbf{u}^{n+1} = \mathbf{b}$ solved by the **Thomas algorithm** in $O(n)$.

For 2D, the implicit scheme leads to a larger sparse system solved iteratively using **Gauss-Seidel iteration**.

### Material Properties

| Material | $\lambda$ (W/(m·K)) | $\rho$ (kg/m³) | $c$ (J/(kg·K)) |
|----------|---------------------|----------------|----------------|
| Copper | 389 | 8940 | 380 |
| Iron | 80.2 | 7874 | 440 |
| Glass | 1.2 | 2530 | 840 |
| Polystyrene | 0.1 | 1040 | 1200 |

### Simulation Parameters

| Parameter | Value |
|-----------|-------|
| $L$ | 1 m |
| $t_{\max}$ | 16 s |
| $u_0$ | 13°C (286.15 K) |
| $f$ | 80°C |
| Grid points (1D) | 1001 |
| Grid points (2D) | 101 × 101 |

## Features

- 1D heat diffusion simulation (bar)
- 2D heat diffusion simulation (plate)
- Multiple material properties (Copper, Iron, Glass, Polystyrene)
- Real-time visualization with color-coded heatmap
- Interactive material and simulation type selection

## Prerequisites

- C++17 compatible compiler
- Meson build system (>= 0.50.0)
- Ninja build tool
- SDL2 library

### Installing Dependencies

**macOS:**
```bash
brew install meson ninja sdl2
```

**Ubuntu/Debian:**
```bash
sudo apt-get install meson ninja-build libsdl2-dev
```

**Arch Linux:**
```bash
sudo pacman -S meson ninja sdl2
```

## Building the Project

### Using Meson

1. Clone or navigate to the project directory

2. Setup the build directory:
```bash
meson setup builddir
```

3. Compile the project:
```bash
meson compile -C builddir
```

### Alternative: Direct g++ compilation
```bash
g++ -g -Wall -Wextra -o prog *.cpp $(pkg-config --cflags --libs sdl2)
```

## Generating Documentation

Generate Doxygen documentation:
```bash
doxygen doc/Doxyfile
```
Open `html/index.html` in a browser to view the documentation.

## Running the Simulation

After building, run the executable:
```bash
./builddir/bin/heat_equation
```

You will be prompted to:
1. Choose simulation type (1D Bar or 2D Plate)
2. Select a material (Copper, Iron, Glass, or Polystyrene)

The simulation window will open showing the heat diffusion visualization.

## Project Structure
```
.
├── bin/                    # Main application
│   ├── main.cpp            # Entry point
│   └── meson.build
├── lib/                    # Library code
│   ├── heat/               # Heat equation solvers
│   │   ├── heat_equation_solver_1d.cpp/.hpp  # 1D solver (Thomas algorithm)
│   │   ├── heat_equation_solver_2d.cpp/.hpp  # 2D solver (Gauss-Seidel)
│   │   ├── material.hpp   # Material properties
│   │   └── meson.build
│   └── sdl/               # SDL2 wrapper classes
│       ├── sdl_core.cpp/.hpp      # SDL initialization/cleanup
│       ├── sdl_window.cpp/.hpp    # Window management
│       ├── sdl_heatmap.cpp/.hpp   # Heatmap rendering & visualization
│       ├── sdl_font.cpp/.hpp      # TTF font rendering
│       ├── sdl_app.cpp/.hpp       # Main application loop & UI
│       └── meson.build
├── test/                  # Unit tests
├── doc/                   # Documentation (Doxygen)
├── subprojects/           # External dependencies (SDL2, SDL2_ttf)
└── meson.build            # Root build configuration
```

## Controls

**Keyboard:**
- `ESC` - Quit the simulation
- `SPACE` - Pause/Resume simulation
- `R` - Reset simulation
- `+/-` - Adjust simulation speed

**Control Panel (right side):**
- Speed slider - Adjust simulation speed (0.5x to 4x)
- Play/Pause button - Start/stop the simulation
- Reset button - Reset to initial conditions
- Menu button - Return to main menu

## License

This project is part of an academic assignment at ENSIIE.

## References

- Tridiagonal Matrix Algorithm (Thomas Algorithm)  
  https://en.wikipedia.org/wiki/Tridiagonal_matrix_algorithm

- Gauss–Seidel Method  
  https://en.wikipedia.org/wiki/Gauss%E2%80%93Seidel_method

- Finite Difference Method  
  https://en.wikipedia.org/wiki/Finite_difference_method
