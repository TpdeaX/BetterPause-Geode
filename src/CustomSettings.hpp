#pragma once
#include <Geode/loader/SettingNode.hpp>
#include "SelectQuickSettings.h"
using namespace geode::prelude;

/*
    --- Quick settings layer ---
*/

class SettingQuickSettingsValue;

class SettingQuickSettingsValue : public SettingValue {
protected:
    std::string m_placeholder;
public:
    SettingQuickSettingsValue(std::string const& key, std::string const& modID, std::string const& placeholder)
        : SettingValue(key, modID), m_placeholder(placeholder) {}

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    SettingNode* createNode(float width) override;
};


class SettingQuickSettingsNode : public SettingNode {
protected:
    bool init(SettingQuickSettingsValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        this->setContentSize({ width, 35.f });
        auto menu = CCMenu::create();
        auto label = CCLabelBMFont::create("Quick Settings Select", "bigFont.fnt");
        label->setScale(0.3f);
        label->setPositionX(-153.f);
        label->setAnchorPoint({ 0.f, 0.5 });
        auto opSpr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        opSpr->setScale(0.6f);
        //auto infoBtn
        auto qssBtn = CCMenuItemSpriteExtra::create(
            opSpr,
            this,
            (cocos2d::SEL_MenuHandler)&SettingQuickSettingsNode::onQuickSettings
        );
        qssBtn->setPositionX(140);
        menu->setPosition(width / 2, 18.f);
        menu->addChild(label);
        menu->addChild(qssBtn);
        this->addChild(menu);
        return true;
    }

public:
    // because for some reason SettingNode doesnt expose description, i have to do all of this, smh.
    void onQuickSettings(CCObject*);

    void commit() override {
        // Let the UI know you have committed the value
        this->dispatchCommitted();
    }

    // Geode calls this to query if the setting value has been changed, 
    // and those changes haven't been committed
    bool hasUncommittedChanges() override {
        return false;
    }

    // Geode calls this to query if the setting has a value that is 
    // different from its default value
    bool hasNonDefaultValue() override {
        return true;
    }

    // Geode calls this to reset the setting's value back to default
    void resetToDefault() override {

    }
    static SettingQuickSettingsNode* create(SettingQuickSettingsValue* value, float width) {
        auto ret = new SettingQuickSettingsNode;
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
