#include "Support/WinInclude.h"
#include "Support/Window.h"
#include "Debug/DebugLayer.h"
#include "D3D/Context/Context.h"

int main()
{
	DebugLayer::Get().Init();

	if (Context::Get().Init() && Window::Get().Init())
	{
		// Window::Get().SetFullscreen(true);

		// Main Loop
		while (!Window::Get().ShouldClose())
		{
			// Process pending window message
			Window::Get().Update();

			// Handle resizing
			if (Window::Get().ShouldResize())
			{
				Context::Get().Flush(Window::GetFrameCount());
				Window::Get().Resize();
			}

			// Begin drawing
			auto* cmdList = Context::Get().InitCommandList();

			// TODO: Draw

			// Finish drawing and present
			Context::Get().ExecuteCommandList();
			Window::Get().Present();
		}

		// Flushing
		Context::Get().Flush(Window::GetFrameCount());

		Window::Get().Shutdown();
		Context::Get().Shutdown();
	}

	DebugLayer::Get().Shutdown();
}