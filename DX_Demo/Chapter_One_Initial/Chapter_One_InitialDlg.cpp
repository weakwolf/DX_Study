
// Chapter_One_InitialDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "Chapter_One_Initial.h"
#include "Chapter_One_InitialDlg.h"
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

// CChapterOneInitialDlg 对话框



CChapterOneInitialDlg::CChapterOneInitialDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHAPTER_ONE_INITIAL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nClientWidth = 600;
	m_nClientHeight = 600;
}

CChapterOneInitialDlg::~CChapterOneInitialDlg()
{
	if (m_pD11DeviceContext)
		m_pD11DeviceContext->ClearState();
}

void CChapterOneInitialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChapterOneInitialDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CChapterOneInitialDlg 消息处理程序

BOOL CChapterOneInitialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CRect rect;
	CWnd::FromHandle(this->GetSafeHwnd())->GetWindowRect(&rect);
	::MoveWindow(this->GetSafeHwnd(), rect.left, rect.top, m_nClientWidth, m_nClientHeight, TRUE);

	// TODO: 在此添加额外的初始化代码
	InitD3DReource();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChapterOneInitialDlg::OnPaint()
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
HCURSOR CChapterOneInitialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool CChapterOneInitialDlg::InitD3DReource()
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
	assert(m_4xMsaaQuality > 0);
	m_bEnableMsaa = (m_4xMsaaQuality > 0);

	// 创建DXGI交换链
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

	return true;
}

