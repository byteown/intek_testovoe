cmake_minimum_required(VERSION 3.14)
project(intek_testovoe LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY  https://github.com/nlohmann/json
        GIT_TAG         v3.12.0
        GIT_SHALLOW     TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        httplib
        GIT_REPOSITORY  https://github.com/yhirose/cpp-httplib
        GIT_TAG         v0.57.1
        GIT_SHALLOW     TRUE
)
set(HTTPLIB_USE_OPENSSL_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_ZLIB_IF_AVAILABLE OFF CACHE BOOL "" FORCE)
set(HTTPLIB_USE_BROTLI_IF_AVAILABLE OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(httplib)

add_executable(intek_testovoe
        src/main.cpp
        include/agent/activity_sample.h
        include/agent/platform_monitor.h
        include/agent/sample_queue.h
        include/agent/disk_buffer.h
        include/agent/http_sender.h
        include/agent/config.h
        include/agent/agent.h
        src/agent.cpp
        src/disk_buffer.cpp
        src/http_sender.cpp
)

if(WIN32)
    target_sources(intek_testovoe PRIVATE
            src/platform/windows_monitor.cpp
    )
    target_link_libraries(intek_testovoe PRIVATE
            user32
    )
    target_compile_definitions(intek_testovoe PRIVATE
            _WIN32_WINNT=0x0601
            WIN32_LEAN_AND_MEAN NOMINMAX
    )
elseif(UNIX)
    target_sources(intek_testovoe PRIVATE
            src/platform/x11_monitor.cpp
    )
    find_package(X11 REQUIRED)
    target_link_libraries(intek_testovoe PRIVATE
            ${X11_LIBRARIES}
            ${X11_Xscreensaver_LIB}
    )
else()
    message(FATAL_ERROR "NO SUPPORTING PLATFORM")
endif()

target_include_directories(intek_testovoe PRIVATE
        include
)

find_package(Threads REQUIRED)

target_link_libraries(intek_testovoe PRIVATE
        nlohmann_json::nlohmann_json
        httplib::httplib
        Threads::Threads
)
