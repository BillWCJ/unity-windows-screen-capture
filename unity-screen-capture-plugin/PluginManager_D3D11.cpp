#include "pch.h"

#include <iostream>

#include "PluginManager.h"
#include "PlatformBase.h"
#include <winrt/Windows.Graphics.Capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.capture.h>
#include "MonitorList.h"
#include "direct3d11.interop.h"
// Direct3D 11 implementation of RenderAPI.

#if SUPPORT_D3D11

#include <assert.h>
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"

namespace winrt
{
	using namespace Windows::Foundation;
	using namespace Windows::System;
	using namespace Windows::Graphics;
	using namespace Windows::Graphics::Capture;
	using namespace Windows::Graphics::DirectX;
	using namespace Windows::Graphics::DirectX::Direct3D11;
	using namespace Windows::Foundation::Numerics;
	using namespace Windows::UI;
	using namespace Windows::UI::Composition;
}

void inline CheckCaptureEnabled() {

	bool hasCaptureMetadata = winrt::Windows::Foundation::Metadata::ApiInformation::IsTypePresent(L"Windows.Graphics.Capture.GraphicsCaptureSession");

	bool isCaptureEnabled = winrt::Windows::Graphics::Capture::GraphicsCaptureSession::IsSupported();
	std::cout << "isCaptureEnabled" << isCaptureEnabled << std::endl;
	

	bool isCursorEnabled = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.Graphics.Capture.GraphicsCaptureSession", L"IsCursorCaptureEnabled");
	std::cout << "isCursorEnabled" << isCursorEnabled << std::endl;
	
	auto hasApiContractV8 = winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 8);

	auto hasApiContractV9 = winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 9);
	std::cout << "hasApiContractV9" << hasApiContractV9 << std::endl;

	if (hasApiContractV9) {
		return;
	}
	else {
		throw winrt::hresult_error();
	}
}

inline auto CreateCaptureItemForWindow(HWND hwnd)
{
	auto interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = { nullptr };
	winrt::check_hresult(interop_factory->CreateForWindow(hwnd, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), winrt::put_abi(item)));
	return item;
}

inline auto CreateCaptureItemForMonitor(HMONITOR hmon)
{
	auto interop_factory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem, IGraphicsCaptureItemInterop>();
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem item = { nullptr };
	winrt::check_hresult(interop_factory->CreateForMonitor(hmon, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(), winrt::put_abi(item)));
	return item;
}

inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice GetDirectD3DDevice(winrt::com_ptr <ID3D11Device> d3dDevice) {
	winrt::com_ptr <IDXGIDevice> dxgiDevice = d3dDevice.as<IDXGIDevice>();
	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice directD3DDevice = CreateDirect3DDeviceFromDxgiDevice(dxgiDevice.get());
	return directD3DDevice;
}

inline D3D11_TEXTURE2D_DESC GetTextureDesription(winrt::com_ptr<ID3D11Texture2D> tex) {
	D3D11_TEXTURE2D_DESC desc = {};
	tex->GetDesc(&desc);
	return desc;
};

class PluginManager_D3D11 : public PluginManager
{
public:
	PluginManager_D3D11(FuncPtr debug, IUnityInterfaces* unityInterfaces);
	virtual ~PluginManager_D3D11() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

private:
	void CreateResources();
	void ReleaseResources();

	ID3D11Device* m_device_ = nullptr;
	ID3D11DeviceContext* m_context_ = nullptr;
	winrt::DirectXPixelFormat m_pixel_format_ = winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized;
	winrt::Direct3D11CaptureFramePool m_framePool = nullptr;
	winrt::GraphicsCaptureSession m_session = nullptr;
};

PluginManager* CreatePluginManager_D3D11(FuncPtr debug, IUnityInterfaces* unityInterfaces)
{
	return new PluginManager_D3D11(debug, unityInterfaces);
}


PluginManager_D3D11::PluginManager_D3D11(FuncPtr debug, IUnityInterfaces* unityInterfaces)
{
	std::cout << "BILLPlugin starting" << std::endl;
	debug("BILLPlugin starting");
	CheckCaptureEnabled();
	
}

void PluginManager_D3D11::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
		m_device_ = d3d->GetDevice();
		m_device_->GetImmediateContext(&m_context_);
		CreateResources();
		break;
	}
	case kUnityGfxDeviceEventShutdown:
		ReleaseResources();
		break;
	}
}


void PluginManager_D3D11::CreateResources()
{
	MonitorList monitor_list = MonitorList(false);
	auto mon = monitor_list.GetCurrentMonitors()[0];
	auto hmon = mon.MonitorHandle;
	std::cout << "Capturing with handle " << hmon << std::endl;

	auto item = CreateCaptureItemForMonitor(hmon);

	winrt::com_ptr <ID3D11Device> device;
	device.attach(m_device_);
	m_framePool = winrt::Direct3D11CaptureFramePool::Create(GetDirectD3DDevice(device), m_pixel_format_, 2, item.Size());
	m_session = m_framePool.CreateCaptureSession(item);
	std::cout << "Completed setup" << std::endl;
}

void PluginManager_D3D11::ReleaseResources()
{
}


#endif // #if SUPPORT_D3D11