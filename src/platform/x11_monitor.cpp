//
// Created by user on 22.09.2026.
//

#include "agent/platform_monitor.h"

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/scrnsaver.h>
#include <unistd.h>

#include <clocale>
#include <fstream>
#include <string>

namespace {
    int ignoreXError(Display*, XErrorEvent*) { return 0; }

    struct XFreeDeleter {
        void operator()(void* p) const { if (p) XFree(p); }
    };

    template <typename T>
    using XPtr = std::unique_ptr<T, XFreeDeleter>;

    std::string baseName(const std::string& path) {
        const std::size_t pos = path.find_last_of('/');
        return pos == std::string::npos ? path : path.substr(pos + 1);
    }
}

class X11Monitor : public IPlatformMonitor {
public:
    X11Monitor() {
        XSetErrorHandler(&ignoreXError);

        std::setlocale(LC_CTYPE, "");

        display_ = XOpenDisplay(nullptr);
        if (!display_) return;

        net_active_window_ = XInternAtom(display_, "_NET_ACTIVE_WINDOW", False);
        net_wm_name_       = XInternAtom(display_, "_NET_WM_NAME", False);
        net_wm_pid_        = XInternAtom(display_, "_NET_WM_PID", False);
        utf8_string_       = XInternAtom(display_, "UTF8_STRING", False);

        int event_base = 0;
        int error_base = 0;
        if (XScreenSaverQueryExtension(display_, &event_base, &error_base)) {
            screensaver_info_ = XScreenSaverAllocInfo();
        }
    }

    ~X11Monitor() override {
        if (screensaver_info_) XFree(screensaver_info_);
        if (display_) XCloseDisplay(display_);
    }

    FocusInfo foregroundWindow() override {
        if (!display_) return {"Unknown", "Unknown"};

        const Window window = activeWindow();
        if (window == None) return {"Unknown", "Unknown"};

        return {processName(window), windowTitle(window)};
    }

    uint64_t idleMillis() override {
        if (!display_ || !screensaver_info_) return 0;

        if (!XScreenSaverQueryInfo(display_, DefaultRootWindow(display_), screensaver_info_)) {
            return 0;
        }

        return static_cast<uint64_t>(screensaver_info_->idle);
    }


    std::string hostname() override {
        char buffer[256] = {};
        if (gethostname(buffer, sizeof(buffer) - 1) != 0) return "Unknown";
        return buffer;
    }

private:
    XPtr<unsigned char> readProperty(Window window, Atom property, Atom type,
                                     unsigned long& nitems) {
        nitems = 0;
        if (property == None) return nullptr;

        Atom actual_type = None;
        int actual_format = 0;
        unsigned long bytes_after = 0;
        unsigned char* raw = nullptr;

        const int status = XGetWindowProperty(display_, window, property, 0, 1024, False,
                                              type, &actual_type, &actual_format,
                                              &nitems, &bytes_after, &raw);

        XPtr<unsigned char> data(raw);

        if (status != Success || actual_type == None || !data) {
            nitems = 0;
            return nullptr;
        }

        return data;
    }

    Window toplevel(Window window) {
        const Window root = DefaultRootWindow(display_);

        while (window != None && window != root) {
            Window window_root = None;
            Window parent = None;
            Window* children = nullptr;
            unsigned int count = 0;

            if (!XQueryTree(display_, window, &window_root, &parent, &children, &count)) {
                return None;
            }
            XPtr<Window> guard(children);

            if (parent == window_root) return window;
            window = parent;
        }

        return None;
    }

    Window activeWindow() {
        unsigned long nitems = 0;
        const XPtr<unsigned char> data =
            readProperty(DefaultRootWindow(display_), net_active_window_, XA_WINDOW, nitems);

        if (data && nitems >= 1) {
            const Window window = *reinterpret_cast<const Window*>(data.get());
            if (window != None) return window;
        }

        Window focus = None;
        int revert_to = 0;
        XGetInputFocus(display_, &focus, &revert_to);

        if (focus == None || focus == PointerRoot) return None;
        return toplevel(focus);
    }

    std::string windowTitle(Window window) {
        unsigned long nitems = 0;
        const XPtr<unsigned char> data = readProperty(window, net_wm_name_, utf8_string_, nitems);

        if (data && nitems > 0) {
            return std::string(reinterpret_cast<const char*>(data.get()), nitems);
        }

        XTextProperty text{};
        if (!XGetWMName(display_, window, &text) || !text.value || text.nitems == 0) {
            return "";
        }
        const XPtr<unsigned char> text_guard(text.value);

        char** list = nullptr;
        int count = 0;
        if (Xutf8TextPropertyToTextList(display_, &text, &list, &count) >= Success
            && list != nullptr) {
            std::string title = count > 0 ? list[0] : std::string();
            XFreeStringList(list);
            return title;
        }

        return std::string(reinterpret_cast<const char*>(text.value), text.nitems);
    }

    std::string processName(Window window) {
        unsigned long nitems = 0;
        const XPtr<unsigned char> data = readProperty(window, net_wm_pid_, XA_CARDINAL, nitems);
        if (!data || nitems == 0) return "Unknown";

        const unsigned long pid = *reinterpret_cast<const unsigned long*>(data.get());
        const std::string proc = "/proc/" + std::to_string(pid);

        char path[4096];
        const ssize_t length = readlink((proc + "/exe").c_str(), path, sizeof(path) - 1);
        if (length > 0) {
            path[length] = '\0';
            return baseName(path);
        }

        std::ifstream comm(proc + "/comm");
        std::string name;
        if (std::getline(comm, name) && !name.empty()) return name;

        return "Unknown";
    }

    Display* display_ = nullptr;
    XScreenSaverInfo* screensaver_info_ = nullptr;

    Atom net_active_window_ = None;
    Atom net_wm_name_ = None;
    Atom net_wm_pid_ = None;
    Atom utf8_string_ = None;
};

std::unique_ptr<IPlatformMonitor> createPlatformMonitor() {
    return std::make_unique<X11Monitor>();
}
