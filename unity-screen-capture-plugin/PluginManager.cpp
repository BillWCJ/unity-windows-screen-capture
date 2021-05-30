#include "pch.h"
#include "PluginManager.h"
#include "PlatformBase.h"


PluginManager* CreatePluginManager(UnityGfxRenderer apiType, FuncPtr debug, IUnityInterfaces* unityInterfaces)
{
#	if SUPPORT_D3D11
	if (apiType == kUnityGfxRendererD3D11)
	{
		extern PluginManager* CreatePluginManager_D3D11(FuncPtr debug, IUnityInterfaces * unityInterfaces);
		return CreatePluginManager_D3D11(debug, unityInterfaces);
	}
#	endif // if SUPPORT_D3D11

#	if SUPPORT_D3D12
	if (apiType == kUnityGfxRendererD3D12)
	{
		extern PluginManager* CreatePluginManager_D3D12();
		return CreatePluginManager_D3D12();
	}
#	endif // if SUPPORT_D3D12

	return NULL;
}