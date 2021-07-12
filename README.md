# renderer

A tiny 3D renderer in OpenGL.

### Features

- Basic
  - Blinn-Phong shading
  - Shadow mapping
  - Skybox
- Advance(in branch `ash`)
  - *SIGGRAPH 2018: Analytic Spherical Harmonic Coefficients for Polygonal Area Lights* 论文复现
  - Precomputed radiance transfer

### Demo

- [Basic](https://github.com/chronoby/renderer/tree/main/demo)
- [Advance](https://github.com/chronoby/renderer/tree/ash/result)

### Dependencies

- GLFW 3.3.3
- GLM
- GLAD
- Dear ImGui
- assimp 5.0.1
- stb
