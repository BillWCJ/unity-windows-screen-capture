#include "pch.h"


#include "GC_Manager.h"

#include <iostream>

#include "D3D11.h"
#include "GraphicsHelper.h"
#include "MonitorList.h"
#include <winrt/Windows.Graphics.Capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.capture.h>


#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")


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


extern "C" UNITY_INTERFACE_EXPORT int grabber_get_next_frame(struct DX11ScreenGrabber* grabber, ID3D11Resource * unused);

struct DX11ScreenGrabber {
	ID3D11Device* device;
	winrt::com_ptr <ID3D11Device> d3dDevice{ nullptr };
	ID3D11DeviceContext* context;
	MonitorList* monitor_list;

	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool frame_pool{ nullptr };
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession session{ nullptr };
	winrt::Windows::Graphics::SizeInt32 last_size;
	
	ID3D11Texture2D* dest_tex;
	ID3D11ShaderResourceView* dest_view;
};

int grabber_create_dest_texture(struct DX11ScreenGrabber* grabber)
{
	D3D11_TEXTURE2D_DESC desc;
	HRESULT res;

	std::cout << "creating dest tex for " << grabber->last_size.Width << ":" << grabber->last_size.Height << std::endl;
	
	desc.Width = grabber->last_size.Width;
	desc.Height = grabber->last_size.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	//desc.Usage = D3D11_USAGE_STAGING;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	//desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.CPUAccessFlags = 0;
	//desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
	desc.MiscFlags = 0;


	std::cout << "Setting sub resource data" << std::endl;
	D3D11_SUBRESOURCE_DATA initial;
	initial.SysMemPitch = desc.Width * 4;
	initial.SysMemSlicePitch = desc.Width * desc.Height * 4;
	char* buf = (char*)malloc(initial.SysMemSlicePitch);
	memset(buf, 0x3f, initial.SysMemSlicePitch / 2);
	initial.pSysMem = buf;

	res = grabber->device->CreateTexture2D(&desc, &initial, &grabber->dest_tex);
	if (res != S_OK) {
		return -1;
	}
	// while (grabber_get_next_frame(grabber, NULL))
	// {
	// 	std::cout << "Waiting for valid frame" << std::endl;
	// }
	grabber->context->Flush();
	res = grabber->device->CreateShaderResourceView(grabber->dest_tex, NULL, &grabber->dest_view);
	if (res != S_OK) {

		std::cout << "Failed to create shader resource view" << std::endl;
		return -2;
	}

	return 0;
}

extern "C" UNITY_INTERFACE_EXPORT void grabber_destroy(struct DX11ScreenGrabber* grabber)
{
	if (!grabber)
		return;

	if (grabber->device)
		grabber->device->Release();
	if (grabber->context)
		grabber->context->Release();
	grabber->d3dDevice.detach();

	if (grabber->dest_tex)
		grabber->dest_tex->Release();
	if (grabber->dest_view)
		grabber->dest_view->Release();
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

extern "C" UNITY_INTERFACE_EXPORT struct DX11ScreenGrabber* grabber_create(ID3D11Resource * tex)
{
	struct DX11ScreenGrabber* grabber;
	HRESULT res;
	int r;
	int* ret = &r;

	std::cout << "Allocating grabber" << std::endl;
	grabber = (DX11ScreenGrabber*)malloc(sizeof(*grabber));
	memset(grabber, 0, sizeof(*grabber));

	std::cout << "Getting device" << std::endl;
	tex->GetDevice(&grabber->device);
	grabber->device->GetImmediateContext(&grabber->context);

	std::cout << "Allocating monitor list" << std::endl;
	grabber->monitor_list = new MonitorList(false);
	
	tex->GetDevice(grabber->d3dDevice.put());

	const std::vector<MonitorInfo> monitor_infos = grabber->monitor_list->GetCurrentMonitors();
	const HMONITOR hmon = monitor_infos[0].MonitorHandle;

	std::cout << "Creating Item for " << monitor_infos[0].DisplayName.c_str() << std::endl;
	const winrt::GraphicsCaptureItem item = CreateCaptureItemForMonitor(hmon);

	std::cout << "Creating frame pool" << std::endl;
	grabber->frame_pool = winrt::Direct3D11CaptureFramePool::Create(GetDirectD3DDevice(grabber->d3dDevice), winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, item.Size());
	grabber->session = grabber->frame_pool.CreateCaptureSession(item);
	grabber->last_size = item.Size();
	
	/*
	res = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, NULL, 0, D3D11_SDK_VERSION, &grabber->device, NULL, &grabber->context);
	if (res != S_OK) {
		*ret = -5;
		goto err;
	}
	*/
	
	std::cout << "Creating dest test" << std::endl;
	if (grabber_create_dest_texture(grabber)) {
		*ret = -8;
		goto err;
	}

	return grabber;
err:
	std::cout << "Error!!!" << std::endl;
	grabber_destroy(grabber);
	return NULL;
}

extern "C" UNITY_INTERFACE_EXPORT int grabber_get_next_frame(struct DX11ScreenGrabber* grabber, ID3D11Resource * dest)
{
	// DXGI_OUTDUPL_FRAME_INFO info;
	// IDXGIResource* resource;
	// ID3D11Texture2D* source = NULL;
	// HRESULT res;
	int ret = 0;
	//
	// res = grabber->duplication->AcquireNextFrame(0, &info, &resource);
	//
	// if (res != S_OK)
	// 	return -1;
	//
	// if (!info.AccumulatedFrames) {
	// 	ret = 1;
	// 	goto out;
	// }
	//
	// res = resource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&source);
	// if (res != S_OK) {
	// 	ret = -2;
	// 	goto out;
	// }
	//
	// D3D11_TEXTURE2D_DESC s_desc;
	// source->GetDesc(&s_desc);
	//
	// D3D11_TEXTURE2D_DESC d_desc;
	// grabber->dest_tex->GetDesc(&d_desc);
	//
	// grabber->context->CopyResource(grabber->dest_tex, source);
	
	//grabber->context->Flush();
	//grabber->context->CopyResource(dest, source);
	
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame frame = grabber->frame_pool.TryGetNextFrame();

	if (!frame) {
		return -1;
	}

	try {
		winrt::com_ptr<ID3D11Texture2D> surfaceTexture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(frame.Surface());
		D3D11_TEXTURE2D_DESC lastFrameDesc = GetTextureDesription1(grabber->dest_tex);
		D3D11_TEXTURE2D_DESC newFrameDesc = GetTextureDesription(surfaceTexture);
		const winrt::Windows::Graphics::SizeInt32 contentSize = frame.ContentSize();

		if ((contentSize.Width != grabber->last_size.Width) || (contentSize.Height != grabber->last_size.Height)) {
			grabber->last_size = contentSize;
			std::cout << "update to last size Frame Pool" << ret << std::endl;
		}
		
		if ((newFrameDesc.Width != lastFrameDesc.Width) || (newFrameDesc.Height != lastFrameDesc.Height)) {
			std::cout << "update to last size Frame Pool2" << ret << std::endl;
			grabber->last_size = contentSize;
			
			int ret = grabber_create_dest_texture(grabber);
			std::cout << "Return vault of create des tex" << ret << std::endl;

			std::cout << "Recreating Frame Pool" << ret << std::endl;
			grabber->frame_pool.Recreate(GetDirectD3DDevice(grabber->d3dDevice), winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, 2, grabber->last_size);
		}
		grabber->context->CopyResource(grabber->dest_tex, surfaceTexture.get());

	}
	catch (winrt::hresult_error const& ex)
	{
		winrt::hresult hr = ex.code(); // HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND).
		winrt::hstring message = ex.message(); // The system cannot find the file specified.
		std::cout << "Got error while getting frame:" << ex.message().c_str() << std::endl;
	}

out:
	// if (source)
	// 	source->Release();
	// resource->Release();
	// grabber->duplication->ReleaseFrame();

	return ret;
}

extern "C" UNITY_INTERFACE_EXPORT int grabber_get_width(struct DX11ScreenGrabber* grabber)
{
	return grabber->last_size.Width;
}

extern "C" UNITY_INTERFACE_EXPORT int grabber_get_height(struct DX11ScreenGrabber* grabber)
{
	return grabber->last_size.Height;
}

extern "C" UNITY_INTERFACE_EXPORT ID3D11ShaderResourceView * grabber_get_dest_tex(struct DX11ScreenGrabber* grabber)
{
	return grabber->dest_view;
}