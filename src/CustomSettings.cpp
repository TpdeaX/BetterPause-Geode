#include "CustomSettings.hpp"

SettingNode* SettingQuickSettingsValue::createNode(float width) {
    return SettingQuickSettingsNode::create(this, width);
}

void SettingQuickSettingsNode::onQuickSettings(CCObject* sender) {
    SelectQuickSettings::create()->show();
}