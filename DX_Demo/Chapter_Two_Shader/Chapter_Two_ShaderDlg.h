
// Chapter_Two_ShaderDlg.h: 头文件
//

#pragma once

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// CChapterTwoShaderDlg 对话框
class CChapterTwoShaderDlg : public CDialogEx
{
	// 顶点描述
	struct VertexPosColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
		static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	};
	// 常量缓冲区对应的结构体
	struct ConstantBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX proj;
	};

// 构造
public:
	CChapterTwoShaderDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CChapterTwoShaderDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAPTER_TWO_SHADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	bool	InitD3D();			// D3D初始化
	bool	InitEffect();		// 创建顶点着色器，像素着色器
	bool	InitReSource();		// 创建顶点缓冲区，并绑定所有资源到渲染管线
	void	DrawScene();		// 绘制每一帧
	void	UpdateScene();		// 更新世界矩阵
	bool	CreateRenderTargetViewAndDepthStencilView();

private:
	void	InitTimer();
	float	AspectRatio() const;   // 获取屏幕宽高比

	// D3D相关成员
private:
	// D3D11(以后我们使用D3D11的API来写代码)
	ComPtr<ID3D11Device>		m_pD11Device;			// D3D11设备
	ComPtr<ID3D11DeviceContext> m_pD11DeviceContext;	// D3D11设备上下文
	ComPtr<IDXGISwapChain>		m_pSwapChian;			// D3D11交换链

	// D3D11.1
	ComPtr<ID3D11Device1>			m_pD11Device1;			// D3D11.1设备
	ComPtr<ID3D11DeviceContext1>	m_pD11DeviceContext1;	// D3D11.1设备上下文
	ComPtr<IDXGISwapChain1>			m_pSwapChain1;			// D3D11.1交换链

	// 常用资源
	ComPtr<ID3D11Texture2D>			m_pDepthStencilBuffer;		// 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView>	m_pRenderTargetView;		// 渲染目标视图
	ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView;		// 深度模板视图
	D3D11_VIEWPORT					m_screenViewPort;			// 视口

	UINT m_nClientWidth;	// 视口宽度
	UINT m_nClientHeight;	// 视口高度

	UINT m_4xMsaaQuality;		// MSAA支持的质量等级
	bool m_bEnableMsaa = false;

	// 顶点缓冲区，顶点着色器，像素着色器等资源
private:
	ComPtr<ID3D11InputLayout>	m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer>		m_pVerxtexBuffer;	// 顶点缓冲区
	ComPtr<ID3D11Buffer>		m_pIndexBuffer;		// 索引缓冲区
	ComPtr<ID3D11Buffer>		m_pConstBuffer;		// 常量缓冲区
	ComPtr<ID3D11VertexShader>	m_pVertexShader;	// 顶点着色器
	ComPtr<ID3D11PixelShader>	m_pPixelShader;		// 像素着色器

private:
	ConstantBuffer	m_constBuffer;	// 用于修改常量缓冲区的变量

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
