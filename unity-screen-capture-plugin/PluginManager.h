#pragma once



#include <stddef.h>

struct IUnityInterfaces;


class PluginManager
{
public:
	virtual ~PluginManager() { }

	// Process general event like initialization, shutdown, device loss/reset etc.
	virtual void ProcessDeviceEvent(UnityGfxDeviceEventType type, IUnityInterfaces* interfaces) = 0;
};


// Create a graphics API implementation instance for the given API type.
PluginManager* CreatePluginManager(UnityGfxRenderer apiType);
