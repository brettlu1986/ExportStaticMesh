# d3d12 与d3d11区别：
[d3d12与d3d11区别](https://docs.microsoft.com/en-us/windows/win32/direct3d12/important-changes-from-directx-11-to-directx-12)

# window初始化 
1. 构造WNDCLASSEX
2. 初始化结构体数据， 比如指向窗口过程指针WndProc， 指向窗口过程实例句柄 GetModuleHandle()等
3. 注册window
4. 初始化硬件设置 DEVMODE 屏幕纵横比等
5. 调用CreateWindowEx创建 HWND
6. ShowWindow

# d3d12初始化
- 几个必须包含进来的lib: d3d12.lib dxgi.lib d3dcompiler.lib ：绘制api  硬件显卡相关  shader编译

1. 通过D3D12Factory枚举显卡adapter, 创建DXGI工厂，获取当前的显卡 显卡输出  IDXGIAdapter adapter IDXGIOutput adapterOutput
2. 创建Device 指定feature level
3. create command queue， 用于向gpu传递指令，create commant list, commant allocator, 用于存储绘制指令向 command queue提交
4. 创建SwapChain 双缓冲 （一般创建都有两步，先有个desc，初始化后再create相应对应） IDXGISwapChain
5. 创建DescriptionHeap (RtvHeapdesc->RtvHeap ReaderTargetView; DsvHeapDesc->DsvHeap Deapth Stencil View; CbvHeapDesc->CbvHeap)  
6. 创建Root Signature, 根签名用来描述 shader函数的参数情况的，确定当前绘制管线的 输入参数详情
7. Create Constant Buffer , 映射ConstantBuffer数据传输指针，用于Update传递参数，比如 MVP矩阵
8. Load Shader, 创建Vertext Format对齐顶点信息
9. 创建VertexBuffer， VertexBufferUpload，将VertexBuffer上传到UploadBuffer,用于 GPU快速读取
10. 创建IndexBuffer, IndexBufferUpload 同理
11. 创建Fence  dx12的fence用来 cpu跟gpu同步，当gpu处理完一组绘制指令的时候cpu可以根据状态提交下一组

# d3d12绘制过程
1. Reset CommandAllocation , Reset CommandList, 进入Recording绘制指令状态
2. 设置ViewPort ScissorRect
3. 初始化当前barrier状态，还有下一个, resource barrier用于同步并切换，双缓冲的backbuffer，状态切换顺序 RENDER_TARGET->PRESENT->RENDER_TARGET-> ...
4. Set RetnHandle, DsvHandle, 设置 DescripterHeap为 CbvHeap
5. Set VertexBUffer, IndexBuffer
6. 在command queue执行command list 
7. set RootSignature, 按Index进行绘制  DrawIndexdInstanced()
8. 双缓冲present  
9. m_fence等待 gpu执行完执行下一次

# d3d12调试注意点:
1. 新建工程win32工程往往只是包含了include 路径， lib link可能会有link不进来的情况，保险一点最好 包含进来    

```
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
```
2. 关闭Windows窗口的时候如果没有正常释放ComPtr引用计数，会报ReportLiveObjects的warning，但是往往看不到是哪些object没有解引用，
可以在destroy或者析构中加入如下代码，这样就可以打印出object的名字
```
#include <dxgidebug.h>

#if defined(_DEBUG)
	{
		ComPtr<IDXGIDebug1> DxgiDebug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&DxgiDebug))))
		{
			DxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}
	}
#endif
```
3. 对于D3D object可以使用NAME_D3D12_OBJECT(object)来把名字注册到debug layer当中，这样当出现绘制报错，或者绘制命令含有已被引用的object的时候，控制台报错就可以看到是哪个名字的object的问题
```
NAME_D3D12_OBJECT(Device);
NAME_D3D12_OBJECT(Fence);
```

# vertex buffer index buffer 
- vertex buffer 为记录模型所有 顶点信息数组
- index buffer 索引buffer， 用于快速索引访问 vertex buffer中的点

# vertex shader (HLSL语言)
- gpu对 vertex buffer中的点进行变换， 变换到3d空间当中

# pixel shader (HLSL语言)
-主要用于多边形着色，颜色，贴图，光照等


# 3d基础笔记 

1. vector 
方向向量， 大小+方向， 相同向量（大小跟方向一致）在不同坐标系，数值是不一样的，所以需要转换
2. 左右手坐标系
左手坐标系：手掌朝上方平， 手指四指向右指向+x, 四指向上卷曲方向是+y, 拇指方向是+z
右手坐标系：右手手掌朝上方平，手指四指向右指向+x，四指向上卷曲方向是+y，拇指方向是+z
左右手区别就是z方向不一样
3. [行列式基本计算规则](https://www.math10.com/en/algebra/matrices/determinant.html)
4. [矩阵 平移 旋转 缩放变换推导](https://zhuanlan.zhihu.com/p/104331790)
5. 计算
-vector可以看作方向直接反向反转， x 标量可以看作缩放，不好想象就放到2d坐标系

6. [模型变换参考视频，注重推导过程值得一看](https://www.bilibili.com/video/av6731067/?p=1)
(待补充)

# d3d关于 buffer的概念讲解
[buffer的概念讲解](https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-buffers-intro)
1. Index buffer: Index buffers contain integer offsets into vertex buffers and are used to render primitives more efficiently. 
An index buffer contains a sequential set of 16-bit or 32-bit indices; each index is used to identify a vertex in a vertex buffer. An index buffer can be visualized like the following illustration.
2. const buffer :A constant buffer allows you to efficiently supply shader constants data to the pipeline
3. vertex buffer:A vertex buffer contains the vertex data used to define your geometry. Vertex data includes position coordinates, color data, texture coordinate data, normal data, and so on.

# shader 传参数
1. [shader参数传递](https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-shaders-and-shader-resources)

# d3d coder 
1. [参考 d3d sample，龙书上的，很不错](https://github.com/d3dcoder/d3d12book/blob/master/Common/MathHelper.h)

# d3d texture convert
1. [tga convert dds](https://www.aconvert.com/image/tga-to-dds/)

# 问题汇总：

1. DXGIFactory 提供一些访问显卡的函数，枚举显卡
2. Adapter  WarpAdapter怎么理解  *****
3. D3DDevice Adapter跟Device该怎么去理解抽象 ？ Device是电脑cpu 程序？ *****
4. CommandQueue 向GPU 显卡输送命令队列
5. CommandList 存放绘制指令用
6. CommandAllocator 给CommandList分配空间
7. SwapChain 交换链 buffer里存放当前的 RenderTarget内容
8. DescriptorHeap (rtv dsv cbv) DescriptorHeap主要用来描述各种 buffer的详细信息，地址等，用于创建buffer [decripter heap说的比较清楚](https://blog.csdn.net/u013412391/article/details/110679071)
9. Fence  负责调度 CPU与GPU的协调执行，确保 当前CPU指令输送完毕之后，再执行下一轮 CPU命令
10. Root Signature  根签名用来描述 shader函数的参数情况的，确定当前绘制管线的 输入参数详情
11. Pipline State Object  不太理解 管线对象？ *****
12. RenderTargetBuffer rtv desriptor 创建 RenderTargetBuffer, RenderTargetBuffer获取句柄和起始指针 还有 SwapChain来创建 RenderTargetView 
13. IndexBuffer  顶点buffer
14. DepthStentilBuffer 深度 这个还不太理解 ******
15. RenderTargetView 双缓冲 buffer ， 跟 SwapChain里的buffer相对应。  如果是三缓冲， 是不是 RenderTarget就是3？？ ****
16. IndexBufferView
17. ResourceBarrier  用于设置当前 pipeline state object的渲染状态 ？？
18. 矩阵 MVP矩阵的计算 需要了解一下
19. shader register(b0) 跟 RootSignature里的 Init关系

# directx 多线程性能分析
[多线程性能](https://software.intel.com/content/www/cn/zh/develop/articles/understanding-directx-multithreaded-rendering-performance-by-experiments.html)
	
	


