//
// Created by user on 22.09.2026.
//

#include <windows.h>

#include "agent/platform_monitor.h"

class WindowsMonitor : public IPlatformMonitor {
public:
    FocusInfo foregroundWindow() override {
        HWND hwnd = GetForegroundWindow();
        if (!hwnd) {
            return {"Unknown","Unknown"};
        }

        return {getProcessName(hwnd), getWindowTitle(hwnd)};
    }

    uint64_t idleMillis() override {
        LASTINPUTINFO lii;
        lii.cbSize = sizeof(lii);

        bool success = GetLastInputInfo(&lii);

        if (!success) {
            return 0;
        }

        DWORD idle = GetTickCount() - lii.dwTime;

        return idle;
    }

    std::string hostname() override {
        wchar_t buffer[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
        bool success = GetComputerNameW(buffer, &size);

        if (!success) {
            return "Unknown";
        }

        std::string hostname = WideToMulti(buffer, static_cast<int>(size));

        return hostname;
    }
private:
    std::string getWindowTitle(HWND hwnd) {
        int raw_len = GetWindowTextLengthW(hwnd);
        std::wstring lpWindowTitle;
        lpWindowTitle.resize(raw_len + 1);

        int length = GetWindowTextW(hwnd, lpWindowTitle.data(), raw_len + 1);

        std::string windowTitle = WideToMulti(lpWindowTitle.c_str(), length);

        return windowTitle;
    }

    std::string getProcessName(HWND hwnd) {
        DWORD pid = 0;

        GetWindowThreadProcessId(hwnd, &pid);

        HANDLE handle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
        if (handle == nullptr) {
            return "Unknown";
        }

        DWORD size = MAX_PATH + 1;
        wchar_t buffer[MAX_PATH + 1];
        bool success = QueryFullProcessImageNameW(handle, 0, buffer, &size);

        if (!success) {
            CloseHandle(handle);
            return "Unknown";
        }

        std::string processName = WideToMulti(buffer, static_cast<int>(size));
        size_t pos = processName.rfind('\\');
        if (pos != std::string::npos) {
            processName = processName.substr(pos + 1);
        }

        CloseHandle(handle);

        return processName;
    }

    std::string WideToMulti(LPCWCH src, int srcLen) {
        std::string buffer;
        int size = WideCharToMultiByte(CP_UTF8, 0, src, srcLen, nullptr, 0, nullptr, nullptr);

        buffer.resize(size);
        WideCharToMultiByte(CP_UTF8, 0, src, srcLen, buffer.data(), size, nullptr, nullptr);

        return buffer;
    }
};

std::unique_ptr<IPlatformMonitor> createPlatformMonitor() {
    return std::make_unique<WindowsMonitor>();
}
