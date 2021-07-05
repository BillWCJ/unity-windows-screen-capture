//#pragma once
//
//struct MonitorInfo
//{
//    MonitorInfo(HMONITOR monitorHandle)
//    {
//        MonitorHandle = monitorHandle;
//        MONITORINFOEX monitorInfo = { sizeof(monitorInfo) };
//        winrt::check_bool(GetMonitorInfo(MonitorHandle, &monitorInfo));
//        std::wstring displayName(monitorInfo.szDevice);
//        DisplayName = displayName;
//    }
//    MonitorInfo(HMONITOR monitorHandle, std::wstring const& displayName)
//    {
//        MonitorHandle = monitorHandle;
//        DisplayName = displayName;
//    }
//
//    HMONITOR MonitorHandle;
//    std::wstring DisplayName;
//
//    bool operator==(const MonitorInfo& monitor) { return MonitorHandle == monitor.MonitorHandle; }
//    bool operator!=(const MonitorInfo& monitor) { return !(*this == monitor); }
//};
//
//class MonitorList
//{
//public:
//    MonitorList(bool includeAllMonitors);
//
//    void Update();
//    const std::vector<MonitorInfo> GetCurrentMonitors() { return m_monitors; }
//
//private:
//    std::vector<MonitorInfo> m_monitors;
//    bool m_includeAllMonitors = false;
//};