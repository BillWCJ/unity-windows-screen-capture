#include "pch.h"
#include "PluginManager.h"
#include "PlatformBase.h"

// Direct3D 11 implementation of RenderAPI.

#if SUPPORT_D3D11

#include <assert.h>
#include <d3d11.h>
#include "IUnityGraphicsD3D11.h"


class PluginManager_D3D11 : public PluginManager
{
public:
	PluginManager_D3D11(FuncPtr debug, IUnityInterfaces* unityInterfaces);
	virtual ~PluginManager_D3D11() { }

	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces);

private:
	void CreateResources();
	void ReleaseResources();

private:
	ID3D11Device* m_Device;
};

PluginManager* CreatePluginManager_D3D11(FuncPtr debug, IUnityInterfaces* unityInterfaces)
{
	return new PluginManager_D3D11(debug, unityInterfaces);
}


PluginManager_D3D11::PluginManager_D3D11(FuncPtr debug, IUnityInterfaces* unityInterfaces)
{
}

void PluginManager_D3D11::ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces)
{
	switch (type)
	{
	case kUnityGfxDeviceEventInitialize:
	{
		IUnityGraphicsD3D11* d3d = interfaces->Get<IUnityGraphicsD3D11>();
		m_Device = d3d->GetDevice();
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
}


void PluginManager_D3D11::ReleaseResources()
{
}



#endif // #if SUPPORT_D3D11