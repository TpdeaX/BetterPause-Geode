#pragma once

#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>
#include <optional>
#include <map>
#include <unordered_map>
#include <vector>

using namespace geode::prelude;

class BetterPauseManager : public CCObject {
protected:
    inline static BetterPauseManager* m_instance = nullptr;
    
public:
    int posQuickASafe[6] = { -10, -4, -5, -2, 4, 5 };
    int posQuickA[6] = { -10, -4, -5, -2, 4, 5 };

    static BetterPauseManager* sharedState() {
        if (m_instance == nullptr) {
            m_instance = new BetterPauseManager;
            m_instance->retain();
        }

        return m_instance;
    }

    void loadState();

    void saveState();

};