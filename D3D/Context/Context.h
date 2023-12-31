#pragma once

#include "Support/WinInclude.h"

class Context
{
	public:
		bool Init();
		void Shutdown();

		void SignalAndWait();
		ID3D12GraphicsCommandList6* InitCommandList();
		void ExecuteCommandList();

		inline void Flush(size_t count)
		{
			for (size_t i = 0; i < count; i++)
				SignalAndWait();
		}
		inline Microsoft::WRL::ComPtr<IDXGIFactory7>& GetFactory()
		{
			return m_dxgiFactory;
		}
		inline Microsoft::WRL::ComPtr<ID3D12Device8>& GetDevice()
		{
			return m_device;
		}
		inline Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCommandQueue()
		{
			return m_cmdQueue;
		}

	private:
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_dxgiFactory;

		Microsoft::WRL::ComPtr<ID3D12Device8> m_device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_cmdQueue;
		
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_cmdAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_cmdList;

		Microsoft::WRL::ComPtr<ID3D12Fence1> m_fence;
		UINT64 m_fenceValue = 0;
		HANDLE m_fenceEvent = nullptr;

	// Singleton
	public:
		Context(const Context&) = delete;
		Context& operator=(const Context&) = delete;

		inline static Context& Get()
		{
			static Context instance;
			return instance;
		}

	private:
		Context() = default;
};