#include "CustomSettings.hpp"

SettingNode* SettingQuickSettingsValue::createNode(float width) {
    return SettingQuickSettingsNode::create(this, width);
}

void SettingQuickSettingsNode::onQuickSettings(CCObject* sender) {
    auto zOrderP = CCScene::get()->getHighestChildZ();
    auto popup = SelectQuickSettings::create(false);
    popup->show();
    popup->setZOrder(zOrderP);
}