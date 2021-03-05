# dx12proj1
first dx12 framework
#d3d12 与d3d11区别：
[d3d12与d3d11区别](https://docs.microsoft.com/en-us/windows/win32/direct3d12/important-changes-from-directx-11-to-directx-12)

#window初始化 
1. 构造WNDCLASSEX
2. 初始化结构体数据， 比如指向窗口过程指针WndProc， 指向窗口过程实例句柄 GetModuleHandle()等
3. 注册window
4. 初始化硬件设置 DEVMODE 屏幕纵横比等
5. 调用CreateWindowEx创建 HWND
6. ShowWindow

#d3d12初始化
- 几个必须包含进来的lib: d3d12.lib dxgi.lib d3dcompiler.lib ：绘制api  硬件显卡相关  shader编译

1. 创建Device 指定feature level
2. create command queue， 用于向gpu传递指令
3. 创建DXGI工厂，获取当前的显卡 显卡输出  IDXGIAdapter adapter IDXGIOutput adapterOutput
4. 创建SwapChain 双缓冲 （一般创建都有两步，先有个desc，初始化后再create相应对应） IDXGISwapChain
5. 创建RenderTargetHandle 初始化双缓冲
6. 创建 CommandList
7. 创建Fence  dx12的fence用来 cpu跟gpu同步，当gpu处理完一组绘制指令的时候cpu可以根据状态提交下一组

#d3d12绘制过程

1. 创建transition barrier 主要用于存储状态之间的切换
2. 清空commandlist allocator, commandlist memory
3. 初始化当前barrier状态，还有下一个, resource barrier用于同步并切换 双缓冲的backbuffer
4. 将Barrier的资源设置到 commnand list， 状态切换顺序 RENDER_TARGET->PRESENT->RENDER_TARGET-> ...
5. 在command queue执行command list 
6. 双缓冲present  
7. m_fence等待 gpu执行完执行下一次

#vertex buffer index buffer 
- vertex buffer 为记录模型所有 顶点信息数组
- index buffer 索引buffer， 用于快速索引访问 vertex buffer中的点

#vertex shader (HLSL语言)
- gpu对 vertex buffer中的点进行变换， 变换到3d空间当中；计算发现

#pixel shader (HLSL语言)
-主要用于多边形着色，颜色，贴图，光照等


#3d基础笔记 

1. vector 
方向向量， 大小+方向， 相同向量（大小跟方向一致）在不同做标记，数值是不一样的，所以需要转换
2. 左右手坐标系
左手坐标系：手掌朝上方平， 手指四指向右指向+x, 四指向上卷曲方向是+y, 拇指方向是+z
右手坐标系：右手手掌朝上方平，手指四指向右指向+x，四指向上卷曲方向是+y，拇指方向是+z
左右手区别就是z方向不一样
3.计算
-vector可以看作方向直接反向反转， x 标量可以看作缩放，不好想象就放到2d坐标系
(待补充)

#d3d关于 buffer的概念讲解
[buffer的概念讲解](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-buffers-intro)
1. Index buffer: Index buffers contain integer offsets into vertex buffers and are used to render primitives more efficiently. 
An index buffer contains a sequential set of 16-bit or 32-bit indices; each index is used to identify a vertex in a vertex buffer. An index buffer can be visualized like the following illustration.
2. const buffer :A constant buffer allows you to efficiently supply shader constants data to the pipeline
3. vertex buffer:A vertex buffer contains the vertex data used to define your geometry. Vertex data includes position coordinates, color data, texture coordinate data, normal data, and so on.