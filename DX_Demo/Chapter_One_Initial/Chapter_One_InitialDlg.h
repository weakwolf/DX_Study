
// Chapter_One_InitialDlg.h: 头文件
//

#pragma once

#include <d3d11_1.h>
#include <wrl/client.h>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

// CChapterOneInitialDlg 对话框
class CChapterOneInitialDlg : public CDialogEx
{
// 构造
public:
	CChapterOneInitialDlg(CWnd* pParent = nullptr);	// 标准构造函数
	~CChapterOneInitialDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHAPTER_ONE_INITIAL_DIALOG };
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
	bool InitD3DReource();

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
	ComPtr<ID3D11Texture2D>			m_pDepthStenciBuffer;		// 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView>	m_pRenderTargerView;		// 渲染目标视图
	ComPtr<ID3D11DepthStencilView>	m_pDepthStencilView;		// 深度模板视图
	D3D11_VIEWPORT					m_screenViewPort;			// 视口

	int m_nClientWidth;		// 视口宽度
	int m_nClientHeight;	// 视口高度

	UINT m_4xMsaaQuality;		// MSAA支持的质量等级
	bool m_bEnableMsaa = false;
};
