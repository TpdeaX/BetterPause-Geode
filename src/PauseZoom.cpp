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
    m_autoHiddenByZoom = false;
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
            m_badge->updateBadge(m_zoom, m_pan);
            pauseLayer->addChild(m_badge, 9999);
        }
    }
#endif
}

void PauseZoomManager::onResume() {
    m_isPaused = false;
    m_isDragging = false;
    m_autoHiddenByZoom = false;
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

    if (m_autoHiddenByZoom) {
        if (auto scene = CCScene::get()) {
            if (auto pauseLayer = scene->getChildByID("PauseLayer")) {
                if (auto bp = typeinfo_cast<BetterPause*>(pauseLayer->getChildByID("better-pause-node"))) {
                    if (bp->isHidden) {
                        bp->onHide(nullptr);
                    }
                }
            }
        }
        m_autoHiddenByZoom = false;
    }

    updateBadge();
}

void PauseZoomManager::autoHideMenu() {
    if (!m_isPaused) return;
    auto scene = CCScene::get();
    if (!scene) return;
    auto pauseLayer = scene->getChildByID("PauseLayer");
    if (!pauseLayer) return;
    auto bp = typeinfo_cast<BetterPause*>(pauseLayer->getChildByID("better-pause-node"));
    if (!bp) return;

    if (!bp->isHidden) {
        m_autoHiddenByZoom = true;
        bp->onHide(nullptr);
    }
}

void PauseZoomManager::clampPan() {
    auto playLayer = PlayLayer::get();
    if (!playLayer) return;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    if (winSize.width <= 0.0f || winSize.height <= 0.0f) return;

    float marginX = winSize.width * 0.20f;
    float marginY = winSize.height * 0.20f;

    float minX = std::min(winSize.width * (1.0f - m_zoom), 0.0f) - marginX;
    float maxX = std::max(winSize.width * (1.0f - m_zoom), 0.0f) + marginX;
    float minY = std::min(winSize.height * (1.0f - m_zoom), 0.0f) - marginY;
    float maxY = std::max(winSize.height * (1.0f - m_zoom), 0.0f) + marginY;

    m_pan.x = std::clamp(m_pan.x, minX, maxX);
    m_pan.y = std::clamp(m_pan.y, minY, maxY);

    playLayer->setPosition(m_pan);
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
    m_pan = newPos;
    clampPan();

    autoHideMenu();
    updateBadge();
}

void PauseZoomManager::pan(CCPoint delta) {
    auto playLayer = PlayLayer::get();
    if (!playLayer) return;

    m_pan = m_pan + delta;
    clampPan();

    autoHideMenu();
    updateBadge();
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
        m_badge->updateBadge(m_zoom, m_pan);
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
    menu->setTouchPriority(-500);

    m_button = CCMenuItemSpriteExtra::create(
        bg,
        this,
        menu_selector(PauseZoomBadge::onReset)
    );
    m_button->setPosition({ 0.f, 0.f });
    menu->addChild(m_button);
    this->addChild(menu);

    // Create minimap / radar box
    m_minimap = CCDrawNode::create();
    const float MW = 76.0f;
    const float MH = 42.0f;
    m_minimap->setPosition({ -MW / 2.0f, -13.0f - 4.0f - MH });
    this->addChild(m_minimap);

    this->setPosition({ winSize.width - 55.f, winSize.height - 18.f });
    this->setID("pause-zoom-badge");

    return true;
}

void PauseZoomBadge::updateBadge(float zoom, CCPoint pan) {
    if (m_label) {
        m_label->setString(fmt::format("Zoom: x{:.2f}", zoom).c_str());
        if (std::abs(zoom - 1.0f) > 0.01f || pan.getLength() > 1.0f) {
            m_label->setColor({ 255, 230, 100 });
        } else {
            m_label->setColor({ 255, 255, 255 });
        }
    }

    if (!m_minimap) return;

    m_minimap->clear();

    const float MW = 76.0f;
    const float MH = 42.0f;

    // Outer radar frame
    CCPoint outerPts[4] = {
        ccp(0.0f, 0.0f),
        ccp(MW, 0.0f),
        ccp(MW, MH),
        ccp(0.0f, MH)
    };
    m_minimap->drawPolygon(
        outerPts,
        4,
        ccc4f(0.06f, 0.06f, 0.08f, 0.70f),
        1.0f,
        ccc4f(0.55f, 0.55f, 0.60f, 0.85f)
    );

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    if (winSize.width <= 0.0f || winSize.height <= 0.0f || zoom <= 0.0f) return;

    // Viewport calculation within PlayLayer
    float normX = -pan.x / (winSize.width * zoom);
    float normY = -pan.y / (winSize.height * zoom);
    float normW = 1.0f / zoom;
    float normH = 1.0f / zoom;

    float x1 = normX * MW;
    float x2 = (normX + normW) * MW;
    float y1 = normY * MH;
    float y2 = (normY + normH) * MH;

    x1 = std::clamp(x1, 0.0f, MW);
    x2 = std::clamp(x2, 0.0f, MW);
    y1 = std::clamp(y1, 0.0f, MH);
    y2 = std::clamp(y2, 0.0f, MH);

    if (x2 - x1 < 3.0f) {
        if (x1 + 3.0f <= MW) x2 = x1 + 3.0f;
        else x1 = std::max(0.0f, x2 - 3.0f);
    }
    if (y2 - y1 < 3.0f) {
        if (y1 + 3.0f <= MH) y2 = y1 + 3.0f;
        else y1 = std::max(0.0f, y2 - 3.0f);
    }

    CCPoint innerPts[4] = {
        ccp(x1, y1),
        ccp(x2, y1),
        ccp(x2, y2),
        ccp(x1, y2)
    };

    m_minimap->drawPolygon(
        innerPts,
        4,
        ccc4f(1.0f, 0.10f, 0.10f, 0.22f),
        1.5f,
        ccc4f(1.0f, 0.20f, 0.20f, 0.95f)
    );
}

void PauseZoomBadge::updateZoom(float zoom) {
    updateBadge(zoom, PauseZoomManager::get()->getPan());
}

void PauseZoomBadge::onReset(CCObject* sender) {
    PauseZoomManager::get()->resetZoom();
}

void PauseZoomBadge::setVisible(bool visible) {
    CCNode::setVisible(true);
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
