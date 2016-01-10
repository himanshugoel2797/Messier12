#include "stdafx.h"
#include <string>
#include <exception>
#include "GraphicsDevice.h"
#include "Messier12.h"

ComPtr<ID3D12Device> GraphicsDevice::m_device;
ComPtr<ID3D12CommandQueue> GraphicsDevice::m_commandQueue;
ComPtr<IDXGISwapChain3> GraphicsDevice::m_swapChain;
ComPtr<ID3D12CommandAllocator> GraphicsDevice::m_CommandAllocator;
ComPtr<ID3D12Resource> GraphicsDevice::m_rtvs[GraphicsDevice::FrameCount];
ComPtr<ID3D12DescriptorHeap> GraphicsDevice::m_rtvHeap;
UINT GraphicsDevice::m_frameIndex, GraphicsDevice::m_rtvDescSize;

inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		// Set a breakpoint on this line to catch Win32 API errors.
		MessageBoxA(NULL, (std::string("Failed to initialize engine, Error Code: ") + std::to_string(hr)).c_str(), "Fatal Error", MSGF_MESSAGEBOX);
#if defined(_DEBUG)
		throw std::exception();
#endif
		
		std::exit(-1);
	}
}

void GraphicsDevice::InitializePipeline()
{
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> dbgController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dbgController))))
	{
		dbgController->EnableDebugLayer();
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

	ComPtr<IDXGIAdapter1> hardwareAdapter;

	hardwareAdapter = nullptr;
	for (UINT adapterIndex = 0; ; ++adapterIndex)
	{
		IDXGIAdapter1* pAdapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &pAdapter))
		{
			// No more adapters to enumerate.
			break;
		}

		// Check to see if the adapter supports Direct3D 12, but don't create the
		// actual device yet.
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
		{
			hardwareAdapter = pAdapter;
		}
		else {
			pAdapter->Release();
		}
	}

	ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_1, IID_PPV_ARGS(&m_device)));

	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.BufferDesc.Width = 960;
	swapChainDesc.BufferDesc.Height = 540;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.OutputWindow = GetCurrentHWND();
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain> swapChain;
	ThrowIfFailed(factory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, &swapChain));
	ThrowIfFailed(swapChain.As(&m_swapChain));

	ThrowIfFailed(factory->MakeWindowAssociation(GetCurrentHWND(), DXGI_MWA_NO_ALT_ENTER));

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));

	//Create a descriptor heap for the RTVs
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FrameCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));
	m_rtvDescSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//Create the RTVs
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (UINT n = 0; n < FrameCount; n++)
	{
		ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_rtvs[n])));
		m_device->CreateRenderTargetView(m_rtvs[n].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, m_rtvDescSize);
	}
}


void GraphicsDevice::Render()
{
	m_swapChain->Present(1, 0);
}

void GraphicsDevice::FSM()
{
	static int state = 0;
	switch (state)
	{
	case 0:
		GraphicsDevice::InitializePipeline();
		state++;
		break;
	default:
		GraphicsDevice::Render();
		break;
	}
}