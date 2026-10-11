#pragma once
#include <Geode/Geode.hpp>
#include <string>
#include <cstring>
#include <fstream>
#include <chrono>
#include <iomanip>

namespace BetterPauseDebug {
    inline std::string getDesktopLogPath() {
#ifdef GEODE_IS_WINDOWS
        char* userProfile = std::getenv("USERPROFILE");
        if (userProfile && std::strlen(userProfile) > 0) {
            return std::string(userProfile) + "\\Desktop\\MoreOptionsLayer_debug.log";
        }
#endif
        return (geode::dirs::getSaveDir() / "MoreOptionsLayer_debug.log").string();
    }

    inline void log(const std::string& msg) {
        geode::log::info("[BetterPauseDebug] {}", msg);
        try {
            std::string logPath = getDesktopLogPath();
            std::ofstream ofs(logPath, std::ios::app);
            if (ofs.is_open()) {
                auto now = std::chrono::system_clock::now();
                auto in_time_t = std::chrono::system_clock::to_time_t(now);
                struct tm buf;
#ifdef GEODE_IS_WINDOWS
                localtime_s(&buf, &in_time_t);
#else
                localtime_r(&in_time_t, &buf);
#endif
                ofs << "[" << std::put_time(&buf, "%Y-%m-%d %H:%M:%S") << "] " << msg << "\n";
                ofs.flush();
            }
        } catch (...) {}
    }
}
