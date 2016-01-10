#pragma once
#include "stdafx.h"
class GraphicsDevice
{
public:
	static const int FrameCount = 2;
	static void InitializePipeline();
	static void Render();
	static void FSM();

private:
	static ComPtr<ID3D12Device> m_device;
	static ComPtr<ID3D12CommandQueue> m_commandQueue;
	static ComPtr<IDXGISwapChain3> m_swapChain;
	static ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
	static ComPtr<ID3D12Resource> m_rtvs[FrameCount];
	static ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	static UINT m_rtvDescSize;
	static UINT m_frameIndex;
};

