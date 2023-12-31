#pragma once

#include "Support/WinInclude.h"
#include "D3D/Context/Context.h"

class Window
{
public:
	bool Init();
	void Update();
	void Present();
	void Shutdown();
	void Resize();
	void SetFullscreen(bool enabled);

	inline bool ShouldClose() const
	{
		return m_shouldClose;
	}

	inline bool ShouldResize() const
	{
		return m_shouldResize;
	}

	static const size_t GetFrameCount()
	{
		return 2;
	}

	inline bool IsFullscreen() const
	{
		return m_isFullscreen;
	}

private:
	static LRESULT CALLBACK OnWindowMessage(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	ATOM m_wndClass = 0;
	HWND m_window = nullptr;
	bool m_shouldClose = false;

	bool m_shouldResize = false;
	const UINT m_defaultWidth = 1280;
	const UINT m_defaultHeight = 720;
	UINT m_width = m_defaultWidth;
	UINT m_height = m_defaultHeight;

	bool m_isFullscreen = false;

	Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;

	// Singleton
public:
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	inline static Window& Get()
	{
		static Window instance;
		return instance;
	}

private:
	Window() = default;
};