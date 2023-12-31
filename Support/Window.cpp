#include "Window.h"

bool Window::Init()
{
	// Window Class
	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_OWNDC;
	wcex.lpfnWndProc = &Window::OnWindowMessage;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandleW(nullptr);
	wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D3D12ExWndCls";
	wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	m_wndClass = RegisterClassExW(&wcex);

	if (m_wndClass == 0)
	{
		return false;
	}

	// Place window on current screen
	POINT pos{ 0,0 };
	GetCursorPos(&pos);
	HMONITOR monitor = MonitorFromPoint(pos, MONITOR_DEFAULTTOPRIMARY);
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(monitorInfo);

	GetMonitorInfo(monitor, &monitorInfo);

	// Window
	m_window = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW,
		(LPCWSTR)m_wndClass,
		L"D3D12 Window",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		monitorInfo.rcWork.left + 100, monitorInfo.rcWork.top + 100,
		m_defaultWidth, m_defaultHeight,
		nullptr,
		nullptr,
		wcex.hInstance,
		nullptr
	);

	if (m_window == nullptr)
	{
		return false;
	}

	// Describe SwapChain
	DXGI_SWAP_CHAIN_DESC1 scd{};
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd{};
	scd.Width = m_defaultWidth;
	scd.Height = m_defaultHeight;
	scd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.Stereo = false;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = GetFrameCount();
	scd.Scaling = DXGI_SCALING_STRETCH;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	scfd.Windowed = true;

	// Create SwapChain
	auto& factory = Context::Get().GetFactory();
	factory->CreateSwapChainForHwnd(Context::Get().GetCommandQueue().Get(),
		m_window,
		&scd,
		&scfd,
		nullptr,
		m_swapChain.GetAddressOf()
		);

	return true;
}

void Window::Update() 
{
	MSG msg;
	while (PeekMessageW(&msg, m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void Window::Present()
{
	m_swapChain->Present(1, 0);
}

void Window::Shutdown()
{
	m_swapChain.Reset();

	if (m_window)
	{
		DestroyWindow(m_window);
	}

	if (m_wndClass)
	{
		UnregisterClassW((LPCWSTR)m_wndClass, GetModuleHandleW(nullptr));
	}
}

void Window::Resize()
{
	RECT cr;
	if (GetClientRect(m_window, &cr))
	{
		m_width = cr.right - cr.left;
		m_height = cr.bottom - cr.top;

		m_swapChain->ResizeBuffers(GetFrameCount(),
			m_width,
			m_height,
			DXGI_FORMAT_UNKNOWN,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
		);
		m_shouldResize = false;
	}
}

void Window::SetFullscreen(bool enabled)
{
	// Update window styling
	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	DWORD exStyle = WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW;
	if (enabled)
	{
		style = WS_POPUP | WS_VISIBLE;
		exStyle = WS_EX_APPWINDOW;
	}

	SetWindowLongW(m_window, GWL_STYLE, style);
	SetWindowLongW(m_window, GWL_EXSTYLE, exStyle);

	HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
	MONITORINFO monitorInfo{};
	monitorInfo.cbSize = sizeof(monitorInfo);
	GetMonitorInfoW(monitor, &monitorInfo);

	// Adjust window size
	if (enabled)
	{
			SetWindowPos(m_window,
				nullptr,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_NOZORDER
			);
	}
	else 
	{
		// Maximize window
		// ShowWindow(m_window, SW_MAXIMIZE);
		SetWindowPos(m_window,
			nullptr,
			monitorInfo.rcMonitor.left + 100,
			monitorInfo.rcMonitor.top + 100,
			m_defaultWidth,
			m_defaultHeight,
			SWP_NOZORDER
		);
	}

	m_isFullscreen = enabled;
}

LRESULT CALLBACK Window::OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			switch (wParam)
			{
				case VK_F11:
					Get().SetFullscreen(!Get().IsFullscreen());
					break;
				case VK_ESCAPE:
					if (Get().IsFullscreen())
					{
						Get().SetFullscreen(false);
					}
					break;
			}
			break;

		case WM_SIZE:
			if (lParam && (HIWORD(lParam) != Get().m_height || LOWORD(lParam) != Get().m_width))
			{
				Get().m_shouldResize = true;
			}
			break;

		case WM_CLOSE:
			Get().m_shouldClose = true;
			break;
	}

	return DefWindowProc(wnd, msg, wParam, lParam);
}
