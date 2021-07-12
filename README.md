# Analytic Spherical Harmonic Coefficients for Polygonal Area Lights 论文复现

## 编译

该项目用 Visual Studio 2019 编译，所需的全部外部库头文件在 `include` 下，外部 lib 在 `lib` 下，可直接在 Windows 系统上构建和运行。已放置构建好的可执行文件位于 `src/renderer.exe`

## 实现

该项目实现了以下几个部分：

1. 实现了 PRT 渲染系统的搭建，包括 Lighting function 的 SH系数的计算，Transport function 的 SH系数的计算，着色结果的计算，基本的 OpenGL 渲染管线以及基本的 GUI 和交互
2. 论文的核心部分：Lighting function 的 SH系数，在 Runtime stage 进行，实现于 OpenGL 的 vertex shader。代码位于 `src/shaders/ash_vertex_shader.glsl`
   - 注：论文中的伪代码存在多处错误，包括公式错误和变量未初始化
3. Transport function 的 SH系数的计算(目前实现了Unshadowed Transport Function)，在 Precomputation stage 进行，采用 Monte carlo 积分的方法计算，将求得的系数写入文件中用于 Runtime stage使用。此外提供了一份计算好的 bunny 模型的 transport 系数，位于`tranport_coeff/bunny.txt`

## 运行

程序提供了几个命令行参数用于调整模型，可用的选项有：

1. `-scene1`: bunny 模型，是否有 `-recompute` 参数决定是否重新计算 transport 系数

```
./renderer.exe 默认等价于 ./renderer.exe -scene1 : 使用 bunny 模型，和我提前计算的 transport 系数文件
```

```
./renderer.exe -scene1 -recompute : 使用 bunny 模型，在进入渲染循环前预计算 transport 系数，并写入 tranport_coeff/newcoeff.txt
```

1. `-scene2`: 使用作者提供的模型和 transport 系数，数据位于`assets/model/bin_model`

```
./renderer.exe -scene2
```

3. `-test`: 在 CPU 上进行论文中的 Lighting function 的 SH系数计算，计算某个点在某个多边形区域光上的系数并输出，用于测试和调试

另外程序提供了一些交互：

- 摄像机位置: w, a, s, d
- 摄像机朝向: mouse
- 显示和隐藏光标: e
- 多边形光位置: i, j, k, l, u, o
- 多边形光旋转: r
- 多边形光缩放: n, m

## 结果

`result` 中提供了几个运行结果的演示图

## 论文地址

[Analytic Spherical Harmonic Coefficients for Polygonal Area Lights](https://cseweb.ucsd.edu/~viscomp/projects/ash/)

## 外部库

- glfw, glad, glm
- stb
- assimp
- imgui