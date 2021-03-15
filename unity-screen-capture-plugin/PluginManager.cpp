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


#	if SUPPORT_OPENGL_UNIFIED
	if (apiType == kUnityGfxRendererOpenGLCore || apiType == kUnityGfxRendererOpenGLES20 || apiType == kUnityGfxRendererOpenGLES30)
	{
		extern PluginManager* CreatePluginManager_OpenGLCoreES(UnityGfxRenderer apiType);
		return CreatePluginManager_OpenGLCoreES(apiType);
	}
#	endif // if SUPPORT_OPENGL_UNIFIED

#	if SUPPORT_METAL
	if (apiType == kUnityGfxRendererMetal)
	{
		extern PluginManager* CreatePluginManager_Metal();
		return CreatePluginManager_Metal();
	}
#	endif // if SUPPORT_METAL

#	if SUPPORT_VULKAN
	if (apiType == kUnityGfxRendererVulkan)
	{
		extern PluginManager* CreatePluginManager_Vulkan();
		return CreatePluginManager_Vulkan();
	}
#	endif // if SUPPORT_VULKAN

	// Unknown or unsupported graphics API
	return NULL;
}