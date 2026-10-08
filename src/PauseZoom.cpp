#include "PauseZoom.hpp"
#include "BetterPause.hpp"
#include "BetterInfoUtils.hpp"
#include <Geode/modify/CCScheduler.hpp>
#ifdef GEODE_IS_DESKTOP
#include <Geode/modify/CCMouseDispatcher.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#endif
#include <algorithm>
#include <cmath>
#include <fmt/format.h>

#ifdef GEODE_IS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using namespace geode::prelude;

PauseZoomManager* PauseZoomManager::s_instance = nullptr;

PauseZoomManager* PauseZoomManager::get() {
    if (!s_instance) {
        s_instance = new PauseZoomManager();
    }
    return s_instance;
}

void PauseZoomManager::onPause(PauseLayer* pauseLayer) {
    m_isPaused = true;
    m_isDragging = false;
    m_pressedKeys.clear();
    m_lastMousePos = getMousePos();

    if (!Mod::get()->getSettingValue<bool>("enable-pause-zoom")) return;

#ifdef GEODE_IS_DESKTOP
    if (pauseLayer) {
        if (auto oldBadge = pauseLayer->getChildByID("pause-zoom-badge")) {
            oldBadge->removeFromParent();
        }
        m_badge = PauseZoomBadge::create();
        if (m_badge) {
            m_badge->updateZoom(m_zoom);
            pauseLayer->addChild(m_badge, 999);
        }
    }
#endif
}

void PauseZoomManager::onResume() {
    m_isPaused = false;
    m_isDragging = false;
    m_pressedKeys.clear();
    m_badge = nullptr;

    if (auto pl = PlayLayer::get()) {
        pl->setScale(1.0f);
        pl->setPosition(ccp(0.f, 0.f));
    }
    m_zoom = 1.0f;
    m_pan = ccp(0.f, 0.f);
}

void PauseZoomManager::resetZoom() {
    m_zoom = 1.0f;
    m_pan = ccp(0.f, 0.f);
    if (auto pl = PlayLayer::get()) {
        pl->setScale(1.0f);
        pl->setPosition(ccp(0.f, 0.f));
    }
    updateBadge();
}

void PauseZoomManager::zoom(float delta, CCPoint pivot) {
    auto playLayer = PlayLayer::get();
    if (!playLayer) return;

    float sensitivity = static_cast<float>(Mod::get()->getSettingValue<double>("pause-zoom-sensitivity"));
    float zoomStep = 1.0f + (0.15f * sensitivity);
    float oldScale = playLayer->getScale();
    float newScale = oldScale;

    if (delta > 0) {
        newScale = oldScale * zoomStep;
    } else if (delta < 0) {
        newScale = oldScale / zoomStep;
    }

    newScale = std::clamp(newScale, 0.25f, 15.0f);

    if (std::abs(newScale - oldScale) < 0.0001f) return;

    CCPoint oldPos = playLayer->getPosition();
    CCPoint newPos = pivot - (pivot - oldPos) * (newScale / oldScale);

    m_zoom = newScale;
    playLayer->setScale(newScale);
    playLayer->setPosition(newPos);
    m_pan = newPos;

    updateBadge();
}

void PauseZoomManager::pan(CCPoint delta) {
    auto playLayer = PlayLayer::get();
    if (!playLayer) return;

    CCPoint pos = playLayer->getPosition();
    playLayer->setPosition(pos + delta);
    m_pan = playLayer->getPosition();
}

bool PauseZoomManager::onScroll(float y, float x) {
    if (!m_isPaused) return false;
    if (!Mod::get()->getSettingValue<bool>("enable-pause-zoom")) return false;

    // Check if mouse is hovering over BetterPause's button list
    if (auto pauseLayer = CCScene::get()->getChildByID("PauseLayer")) {
        if (auto betterPause = static_cast<BetterPause*>(pauseLayer->getChildByID("better-pause-node"))) {
            if (betterPause->buttonsList && BetterInfo::isHoveringNode(betterPause->buttonsList)) {
                return false; // let buttonsList scroll
            }
        }
    }

    CCPoint mousePos = getMousePos();
    zoom(y, mousePos);
    return true;
}

void PauseZoomManager::onKey(cocos2d::enumKeyCodes key, bool down) {
    if (down) {
        m_pressedKeys.insert(static_cast<int>(key));
        if (key == cocos2d::enumKeyCodes::KEY_R || key == 'r' || key == 'R') {
            resetZoom();
        }
    } else {
        m_pressedKeys.erase(static_cast<int>(key));
    }
}

void PauseZoomManager::update(float dt) {
    if (!m_isPaused) return;

    auto playLayer = PlayLayer::get();
    if (!playLayer) return;

    if (!CCScene::get()->getChildByID("PauseLayer")) {
        onResume();
        return;
    }

    if (!Mod::get()->getSettingValue<bool>("enable-pause-zoom")) return;

#ifdef GEODE_IS_DESKTOP
    // Pan speed
    float panSpeedSetting = static_cast<float>(Mod::get()->getSettingValue<double>("pause-zoom-pan-speed"));
    float speed = 350.0f * panSpeedSetting;

    // Shift speed multiplier
    bool shiftPressed = false;
    auto kb = CCKeyboardDispatcher::get();
    if (kb && kb->getShiftKeyPressed()) {
        shiftPressed = true;
    }
    if (m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_Shift)) ||
        m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_LeftShift)) ||
        m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_RightShift))) {
        shiftPressed = true;
    }
    if (shiftPressed) {
        speed *= 2.5f;
    }

    CCPoint moveDelta = {0.f, 0.f};

    // W / Up: move PlayLayer down so camera views upwards
    if (m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_W)) ||
        m_pressedKeys.count('W') || m_pressedKeys.count('w') ||
        m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_Up))) {
        moveDelta.y -= speed * dt;
    }
    // S / Down: move PlayLayer up so camera views downwards
    if (m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_S)) ||
        m_pressedKeys.count('S') || m_pressedKeys.count('s') ||
        m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_Down))) {
        moveDelta.y += speed * dt;
    }
    // A / Left: move PlayLayer right so camera views leftwards
    if (m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_A)) ||
        m_pressedKeys.count('A') || m_pressedKeys.count('a') ||
        m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_Left))) {
        moveDelta.x += speed * dt;
    }
    // D / Right: move PlayLayer left so camera views rightwards
    if (m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_D)) ||
        m_pressedKeys.count('D') || m_pressedKeys.count('d') ||
        m_pressedKeys.count(static_cast<int>(cocos2d::enumKeyCodes::KEY_Right))) {
        moveDelta.x -= speed * dt;
    }

    if (moveDelta.x != 0.f || moveDelta.y != 0.f) {
        pan(moveDelta);
    }

    // Mouse drag handling
    CCPoint currentMouse = getMousePos();
#ifdef GEODE_IS_WINDOWS
    bool isMousePanning = ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0) ||
                          ((GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
    if (isMousePanning) {
        if (m_isDragging) {
            CCPoint mouseDelta = currentMouse - m_lastMousePos;
            pan(mouseDelta);
        }
        m_isDragging = true;
    } else {
        m_isDragging = false;
    }
#endif
    m_lastMousePos = currentMouse;
#endif // GEODE_IS_DESKTOP
}

void PauseZoomManager::updateBadge() {
    if (m_badge) {
        m_badge->updateZoom(m_zoom);
    }
}

PauseZoomBadge* PauseZoomBadge::create() {
    auto ret = new PauseZoomBadge();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool PauseZoomBadge::init() {
    if (!CCNode::init()) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = CCScale9Sprite::create("square02_small.png");
    bg->setContentSize({ 90.f, 26.f });
    bg->setColor({ 0, 0, 0 });
    bg->setOpacity(160);

    m_label = CCLabelBMFont::create("Zoom: x1.00", "bigFont.fnt");
    m_label->setScale(0.35f);
    m_label->setPosition({ 45.f, 13.f });
    bg->addChild(m_label);

    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });

    m_button = CCMenuItemSpriteExtra::create(
        bg,
        this,
        menu_selector(PauseZoomBadge::onReset)
    );
    m_button->setPosition({ 0.f, 0.f });
    menu->addChild(m_button);
    this->addChild(menu);

    this->setPosition({ winSize.width - 55.f, winSize.height - 18.f });
    this->setID("pause-zoom-badge");

    return true;
}

void PauseZoomBadge::updateZoom(float zoom) {
    if (!m_label) return;
    m_label->setString(fmt::format("Zoom: x{:.2f}", zoom).c_str());
    if (std::abs(zoom - 1.0f) > 0.01f) {
        m_label->setColor({ 255, 230, 100 });
    } else {
        m_label->setColor({ 255, 255, 255 });
    }
}

void PauseZoomBadge::onReset(CCObject* sender) {
    PauseZoomManager::get()->resetZoom();
}

// Global Hooks for input dispatching
#ifdef GEODE_IS_DESKTOP
class $modify(PauseZoomMouse, cocos2d::CCMouseDispatcher) {
    bool dispatchScrollMSG(float y, float x) {
        if (PauseZoomManager::get()->isPaused()) {
            if (PauseZoomManager::get()->onScroll(y, x)) {
                return true;
            }
        }
        return CCMouseDispatcher::dispatchScrollMSG(y, x);
    }
};

class $modify(PauseZoomKeyboard, cocos2d::CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp) {
        if (PauseZoomManager::get()->isPaused()) {
            PauseZoomManager::get()->onKey(key, isKeyDown);
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);
    }
};
#endif // GEODE_IS_DESKTOP

class $modify(PauseZoomScheduler, cocos2d::CCScheduler) {
    void update(float dt) {
        PauseZoomManager::get()->update(dt);
        CCScheduler::update(dt);
    }
};
