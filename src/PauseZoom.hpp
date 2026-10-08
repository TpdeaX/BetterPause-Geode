#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <unordered_set>

using namespace geode::prelude;

class PauseZoomBadge;

class PauseZoomManager {
private:
    static PauseZoomManager* s_instance;
    bool m_isPaused = false;
    float m_zoom = 1.0f;
    CCPoint m_pan = {0.f, 0.f};
    CCPoint m_lastMousePos = {0.f, 0.f};
    bool m_isDragging = false;
    std::unordered_set<int> m_pressedKeys;
    PauseZoomBadge* m_badge = nullptr;

    PauseZoomManager() = default;

public:
    static PauseZoomManager* get();

    bool isPaused() const { return m_isPaused; }
    float getZoom() const { return m_zoom; }

    void onPause(PauseLayer* pauseLayer);
    void onResume();
    void resetZoom();
    bool onScroll(float y, float x);
    void onKey(cocos2d::enumKeyCodes key, bool down);
    void update(float dt);
    void zoom(float delta, CCPoint pivot);
    void pan(CCPoint delta);
    void updateBadge();
};

class PauseZoomBadge : public CCNode {
private:
    CCLabelBMFont* m_label = nullptr;
    CCMenuItemSpriteExtra* m_button = nullptr;

public:
    static PauseZoomBadge* create();
    bool init() override;
    void updateZoom(float zoom);
    void onReset(CCObject* sender);
};
