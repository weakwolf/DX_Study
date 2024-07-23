
// Chapter_Two_ShaderDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Chapter_Two_Shader.h"
#include "Chapter_Two_ShaderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
// 最下面两个lib在windows上的真实名字叫做d3dcompiler和WinMM
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")

// CChapterTwoShaderDlg 对话框


const D3D11_INPUT_ELEMENT_DESC CChapterTwoShaderDlg::VertexPosColor::inputLayout[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

CChapterTwoShaderDlg::CChapterTwoShaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAPTER_TWO_SHADER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nClientWidth = 600;
	m_nClientHeight = 600;

	ZeroMemory(&m_screenViewPort, sizeof(D3D11_VIEWPORT));
}

CChapterTwoShaderDlg::~CChapterTwoShaderDlg()
{
	if (m_pD11DeviceContext)
		m_pD11DeviceContext->ClearState();
}

void CChapterTwoShaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChapterTwoShaderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CChapterTwoShaderDlg 消息处理程序

BOOL CChapterTwoShaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	InitD3D();
	InitEffect();
	InitReSource();
	InitTimer();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChapterTwoShaderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChapterTwoShaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CChapterTwoShaderDlg::InitD3D()
{
	HRESULT hr = S_OK;

	// 创建D3D设备和D3D设备上下文
	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	// 驱动类型数组
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,	// 硬件驱动
		D3D_DRIVER_TYPE_WARP,		// WARP驱动
		D3D_DRIVER_TYPE_REFERENCE,	// 软件驱动
	};
	UINT nNumDriverType = ARRAYSIZE(driverTypes);
	// 特性等级数组
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	UINT nNumFeartureLevel = ARRAYSIZE(featureLevels);
	D3D_FEATURE_LEVEL featureLevel;
	D3D_DRIVER_TYPE driverType;

	for (UINT nDriverIndex = 0; nDriverIndex < nNumDriverType; ++nDriverIndex)
	{
		driverType = driverTypes[nDriverIndex];
		hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, featureLevels, nNumFeartureLevel,
			D3D11_SDK_VERSION, m_pD11Device.GetAddressOf(), &featureLevel, m_pD11DeviceContext.GetAddressOf());
		if (E_INVALIDARG == hr)
		{
			// 返回E_INVALIDARG代表API不承认系统支持d3d11.1特性等级，需要尝试更低的特性等级
			hr = D3D11CreateDevice(nullptr, driverType, nullptr, createDeviceFlags, &featureLevels[1], nNumFeartureLevel - 1,
				D3D11_SDK_VERSION, m_pD11Device.GetAddressOf(), &featureLevel, m_pD11DeviceContext.GetAddressOf());
		}
		if (SUCCEEDED(hr))
			break;
	}

	// 检测D3D设备是否创建成功
	if (FAILED(hr))
	{
		MessageBox(_T("D3D11CreateDevice failed!!!"));
		return false;
	}
	// 检查创建的设备的特性等级
	if (D3D_FEATURE_LEVEL_11_1 != featureLevel && D3D_FEATURE_LEVEL_11_0 != featureLevel)
	{
		MessageBox(_T("Direct3D Feature Level 11 unsupported."));
		return false;
	}

	// 检测当前特性等级支持的4倍多重采样的质量等级
	// @see https://learn.microsoft.com/zh-cn/windows/win32/api/d3d11/nf-d3d11-id3d11device-checkmultisamplequalitylevels
	m_pD11Device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_4xMsaaQuality);
	ASSERT(m_4xMsaaQuality > 0);
	m_bEnableMsaa = (m_4xMsaaQuality > 0);

	// 创建DXGI交换链
	// 注：实际上有两种交换链翻转模型，这里采用的是教程中的方式，也就是第一种
	ComPtr<IDXGIDevice> pDxgiDevice = nullptr;
	ComPtr<IDXGIAdapter> pDxgiAdapter = nullptr;
	ComPtr<IDXGIFactory1> pDxgiFactory1 = nullptr;
	ComPtr<IDXGIFactory2> pDxgiFactory2 = nullptr;
	// 首先创建IDXGIFactory
	HR(m_pD11Device.As(&pDxgiDevice));
	HR(pDxgiDevice->GetAdapter(&pDxgiAdapter));
	HR(pDxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(pDxgiFactory1.GetAddressOf())));
	// 查看该对象是否包含IDXGIFactory2的接口
	if (pDxgiFactory1.As(&pDxgiFactory2))
	{
		m_pD11Device.As(&m_pD11Device1);
		m_pD11DeviceContext.As(&m_pD11DeviceContext1);
		// 填充各种结构体以创建交换链
		DXGI_SWAP_CHAIN_DESC1 sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Width = m_nClientWidth;
		sd.Height = m_nClientHeight;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// 是否开启四倍多重采样
		if (m_bEnableMsaa)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
		fd.RefreshRate.Numerator = 60;
		fd.RefreshRate.Denominator = 1;
		fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fd.Windowed = TRUE;
		// 为当前窗口创建交换链
		HR(pDxgiFactory2->CreateSwapChainForHwnd(m_pD11Device1.Get(), this->GetSafeHwnd(), &sd, &fd, nullptr, &m_pSwapChain1));
		HR(m_pSwapChain1.As(&m_pSwapChian));
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferDesc.Width = m_nClientWidth;
		sd.BufferDesc.Height = m_nClientHeight;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		if (m_bEnableMsaa)
		{
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_4xMsaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.BufferCount = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = this->GetSafeHwnd();
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;
		HR(pDxgiFactory1->CreateSwapChain(m_pD11Device.Get(), &sd, m_pSwapChian.GetAddressOf()));
	}
	// 禁用Alt+Enter进入全屏
	pDxgiFactory1->MakeWindowAssociation(this->GetSafeHwnd(), DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);

	// 设置调试对象名
	D3D11SetDebugObjectName(m_pD11DeviceContext.Get(), "ImmediateContext");
	DXGISetDebugObjectName(m_pSwapChian.Get(), "SwapChain");

	// 创建渲染目标视图，深度/模板缓冲区
	CreateRenderTargetViewAndDepthStencilView();

	return true;
}

bool CChapterTwoShaderDlg::InitEffect()
{
	// 创建顶点着色器
	ComPtr<ID3DBlob> pBlob;
	HR(CreateShaderFromFile(_T("HLSL\\Triangle_VS.cso"), _T("HLSL\\Triangle_VS.hlsl"), "VS", "vs_5_0", pBlob.ReleaseAndGetAddressOf()));
	HR(m_pD11Device->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建输入顶点布局并绑定到顶点着色器
	HR(m_pD11Device->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(_T("HLSL\\Triangle_PS.cso"), _T("HLSL\\Triangle_PS.hlsl"), "PS", "ps_5_0", pBlob.ReleaseAndGetAddressOf()));
	HR(m_pD11Device->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}

bool CChapterTwoShaderDlg::InitReSource()
{
	// 创建顶点缓冲区，这里绘制一个三角形
	VertexPosColor vertices[] = 
	{
		{DirectX::XMFLOAT3(0.0f,0.5f,0.5f),DirectX::XMFLOAT4(0.0f,1.0f,0.0f,1.0f)},
		{DirectX::XMFLOAT3(0.5f,-0.5f,0.5f),DirectX::XMFLOAT4(0.0f,0.0f,1.0f,1.0f)},
		{DirectX::XMFLOAT3(-0.5f,-0.5f,0.5f),DirectX::XMFLOAT4(1.0f,0.0f,0.0f,1.0f)}
	};
	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(vertices);
	vbd.CPUAccessFlags = 0;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HR(m_pD11Device->CreateBuffer(&vbd, &initData, m_pVerxtexBuffer.GetAddressOf()));

	// 给渲染管线各个阶段绑定号所需资源

	// 绑定顶点缓冲区
	UINT nStride = sizeof(VertexPosColor);	//每一个顶点之间的跨越字节数
	UINT nOffset = 0;						// 起始偏移量
	// 这里的最后两个参数是在告诉渲染管线一次读取的顶点数据的大小
	// 而我们在前面设置的定点布局描述的是一个顶点数据的含义
	m_pD11DeviceContext->IASetVertexBuffers(0, 1, m_pVerxtexBuffer.GetAddressOf(), &nStride, &nOffset);
	
	// 设置图元类型--三角形
	m_pD11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// 设置顶点布局
	m_pD11DeviceContext->IASetInputLayout(m_pVertexLayout.Get());

	// 设置顶点着色器，像素着色器
	m_pD11DeviceContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pD11DeviceContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);

	return true;
}

bool CChapterTwoShaderDlg::CreateRenderTargetViewAndDepthStencilView()
{
	// 释放渲染管线输出用到的相关资源
	ASSERT(m_pD11DeviceContext);
	ASSERT(m_pD11Device);
	ASSERT(m_pSwapChian);
	if (m_pD11Device1 != nullptr)
	{
		ASSERT(m_pD11DeviceContext1);
		ASSERT(m_pD11Device1);
		ASSERT(m_pSwapChain1);
	}
	m_pRenderTargetView.Reset();
	m_pDepthStencilView.Reset();
	m_pDepthStencilBuffer.Reset();

	// 重设交换链并且重新创建渲染目标视图
	// 注：渲染目标视图和深度/模板视图，视口不同，后两者可以重新设置，而前者需要重置缓冲区大小
	ComPtr<ID3D11Texture2D> pBackBuffer;
	HR(m_pSwapChian->ResizeBuffers(1, m_nClientWidth, m_nClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	HR(m_pSwapChian->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf())));
	HR(m_pD11Device->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf()));
	pBackBuffer.Reset();

	// 创建深度/模板缓冲区
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = m_nClientWidth;
	depthStencilDesc.Height = m_nClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	if (m_bEnableMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;
	HR(m_pD11Device->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf()));

	// 创建深度/模板视图
	HR(m_pD11Device->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf()));

	// 为渲染管线的输出合并设置渲染目标
	m_pD11DeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	// 设置视口，也就是指定将视图输出到窗口的具体位置
	m_screenViewPort.TopLeftX = 0;
	m_screenViewPort.TopLeftY = 0;
	m_screenViewPort.Width = m_nClientWidth;
	m_screenViewPort.Height = m_nClientHeight;
	m_screenViewPort.MinDepth = 0.0f;
	m_screenViewPort.MaxDepth = 1.0f;
	m_pD11DeviceContext->RSSetViewports(1, &m_screenViewPort);

	return true;
}

void CChapterTwoShaderDlg::DrawScene()
{
	ASSERT(m_pD11Device);
	ASSERT(m_pD11DeviceContext);
	// 清空的颜色
	static float red[4] = { 0.0f,0.0f,0.0f,1.0f }; // RGBA,0 ~ 255
	// 清空渲染目标视图
	m_pD11DeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), red);
	m_pD11DeviceContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制三角形
	m_pD11DeviceContext->Draw(3, 0);
	// 上屏
	HR(m_pSwapChian->Present(0, 0));
}

void CChapterTwoShaderDlg::InitTimer()
{
	SetTimer(TIMER_ID, INTERVAL, nullptr);
}



void CChapterTwoShaderDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case TIMER_ID:
		DrawScene();

		break;
	default:
		return;
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CChapterTwoShaderDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_pD11Device)
	{
		CreateRenderTargetViewAndDepthStencilView();
	}
}
