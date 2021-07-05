#include "pch.h"
//#include "MonitorList.h"
//
//std::vector<MonitorInfo> EnumerateAllMonitors(bool includeAllMonitors)
//{
//    std::vector<MonitorInfo> monitors;
//    EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR hmon, HDC, LPRECT, LPARAM lparam)
//        {
//            auto& monitors = *reinterpret_cast<std::vector<MonitorInfo>*>(lparam);
//            monitors.push_back(MonitorInfo(hmon));
//
//            return TRUE;
//        }, reinterpret_cast<LPARAM>(&monitors));
//    if (monitors.size() > 1 && includeAllMonitors)
//    {
//        monitors.push_back(MonitorInfo(nullptr, L"All Displays"));
//    }
//    return monitors;
//}
//
//MonitorList::MonitorList(bool includeAllMonitors)
//{
//    m_includeAllMonitors = includeAllMonitors;
//    m_monitors = EnumerateAllMonitors(m_includeAllMonitors);
//}
//
//void MonitorList::Update()
//{
//    auto monitors = EnumerateAllMonitors(m_includeAllMonitors);
//    std::map<HMONITOR, MonitorInfo> newMonitors;
//    for (auto& monitor : monitors)
//    {
//        newMonitors.insert({ monitor.MonitorHandle, monitor });
//    }
//
//    std::vector<int> monitorIndexesToRemove;
//    auto index = 0;
//    for (auto& monitor : m_monitors)
//    {
//        auto search = newMonitors.find(monitor.MonitorHandle);
//        if (search == newMonitors.end())
//        {
//            monitorIndexesToRemove.push_back(index);
//        }
//        else
//        {
//            newMonitors.erase(search);
//        }
//        index++;
//    }
//}
