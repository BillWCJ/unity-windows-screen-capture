#pragma once
#include "direct3d11.interop.h"
#include "pch.h"

inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice GetDirectD3DDevice(winrt::com_ptr <ID3D11Device> d3dDevice) {
    winrt::com_ptr <IDXGIDevice> dxgiDevice = d3dDevice.as<IDXGIDevice>();
    winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice directD3DDevice = CreateDirect3DDeviceFromDxgiDevice(dxgiDevice.get());
    return directD3DDevice;
}

inline D3D11_TEXTURE2D_DESC GetTextureDesription1(ID3D11Texture2D* tex) {
    D3D11_TEXTURE2D_DESC desc = {};
    tex->GetDesc(&desc);
    return desc;
};

inline D3D11_TEXTURE2D_DESC GetTextureDesription(winrt::com_ptr<ID3D11Texture2D> tex) {
    D3D11_TEXTURE2D_DESC desc = {};
    tex->GetDesc(&desc);
    return desc;
};
