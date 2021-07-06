#pragma once

#include "pch.h"

#include <iostream>
#include <windows.graphics.capture.interop.h>

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

inline bool CheckCaptureEnabled() {
	const bool has_capture_metadata = winrt::Windows::Foundation::Metadata::ApiInformation::IsTypePresent(L"Windows.Graphics.Capture.GraphicsCaptureSession");
	std::cout << "hasCaptureMetadata: " << has_capture_metadata << std::endl;

	const bool is_capture_enabled = winrt::Windows::Graphics::Capture::GraphicsCaptureSession::IsSupported();
	std::cout << "isCaptureEnabled: " << is_capture_enabled << std::endl;

	const bool is_cursor_enabled = winrt::Windows::Foundation::Metadata::ApiInformation::IsPropertyPresent(L"Windows.Graphics.Capture.GraphicsCaptureSession", L"IsCursorCaptureEnabled");
	std::cout << "isCursorEnabled: " << is_cursor_enabled << std::endl;

	const bool  has_api_contract_v8 = winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 8);
	std::cout << "hasApiContractV8: " << has_api_contract_v8 << std::endl;

	const bool  has_api_contract_v9 = winrt::Windows::Foundation::Metadata::ApiInformation::IsApiContractPresent(L"Windows.Foundation.UniversalApiContract", 9);
	std::cout << "hasApiContractV9" << has_api_contract_v9 << std::endl;

	if (has_api_contract_v9) {
		return true;
	}
	return false;
}