#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/binding/DialogObject.hpp>
#include "Utils.hpp"


using namespace geode::prelude;

class AttemptAtReversingDialogObject : public cocos2d::CCObject {
public:

	static AttemptAtReversingDialogObject* create(gd::string title, gd::string text, int type, float text_scale, bool is_unskippable, cocos2d::_ccColor3B textColor) {
		AttemptAtReversingDialogObject* obj = new AttemptAtReversingDialogObject();

		DialogObject::initA(obj, title, text, type, text_scale, is_unskippable, textColor);
		
		obj->autorelease();

		return obj;
	}
};