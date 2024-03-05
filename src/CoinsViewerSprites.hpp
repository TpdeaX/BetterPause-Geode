#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

class CoinsViewerSprites : public cocos2d::CCSprite {
public:
	static std::vector<GameObject*> coinsInVector;

	std::vector<cocos2d::CCSprite*> m_coinsSprites = {};
	bool m_pIsLocal = false;

	static CoinsViewerSprites* create(bool isLocal = false);
	bool init(bool isLocal);
	void setCoinSprite(cocos2d::CCSprite* coin, const std::string& spriteName, int i);

private:
	CoinsViewerSprites() {}
};