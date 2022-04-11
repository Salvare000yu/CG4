#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include "WinApp.h"

#pragma once

//directX基盤
class DirectXCommon
{

public://メンバ関数
	void Initialize(WinApp* winApp);

	//描画前
	void PreDraw();
	//描画後処理
	void PostDraw();

	//デバイス取得
	ID3D12Device* GetDevice() { return device.Get(); }

	//コマンドリスト取得
	ID3D12GraphicsCommandList* GetCmdList() { return cmdList.Get(); }

private:
	//デバイス
	//Microsoft::WRL::ComPtr<ID3D12Device>dev;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory6>dxgiFactory;

	WinApp* winApp=nullptr;

	//direct3D系メンバ変数
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapchain;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeaps;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;
	//------
	//バックバッファ
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>backBuffers;
private:
	bool InitializeCommand();

	bool InitializeDevice();

	bool InitializeSwapchain();

	bool InitializeRenderTargetView();

	bool DirectXCommon::InitializeDepthBuffer();

	bool DirectXCommon::InitializeFence();
};

