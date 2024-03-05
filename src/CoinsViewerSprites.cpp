#include "CoinsViewerSprites.hpp"

std::vector<GameObject*> CoinsViewerSprites::coinsInVector = {};

CoinsViewerSprites* CoinsViewerSprites::create(bool isLocal) {
	auto ret = new CoinsViewerSprites();

	if (ret && ret->init(isLocal)) {
		ret->autorelease();
		return ret;
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool CoinsViewerSprites::init(bool isLocal) {
	if (!cocos2d::CCSprite::init()) {
		return false;
	}

	m_pIsLocal = isLocal;

	auto playLayer = Utils::getplayLayerA();
	if (!playLayer || coinsInVector.empty()) {
		return true;
	}

	const auto withoutCoinSprite = m_pIsLocal ? "secretCoin_b_01_001.png" : "secretCoin_2_b_01_001.png";
	const auto withCoinSprite = m_pIsLocal ? "secretCoinUI_001.png" : "secretCoinUI2_001.png";
	std::vector<std::string> coinSprites = { };

	for (size_t i = 0; i < coinsInVector.size(); i++) {
		if (playLayer->hasUniqueCoin(typeinfo_cast<EffectGameObject*>(coinsInVector[i]))) {
			coinSprites.push_back(withCoinSprite);
		}
		else {
			coinSprites.push_back(withoutCoinSprite);
		}
	}

	for (size_t i = 0; i < coinSprites.size(); i++) {
		auto coin = cocos2d::CCSprite::createWithSpriteFrameName(coinSprites[i].c_str());
		addChild(coin);
		auto spriteName = playLayer->hasUniqueCoin(typeinfo_cast<EffectGameObject*>(coinsInVector[i])) ? withCoinSprite : withoutCoinSprite;
		m_coinsSprites.push_back(coin);
		this->setCoinSprite(m_coinsSprites[i], coinSprites[i], i);
	}

	return true;
}

void CoinsViewerSprites::setCoinSprite(cocos2d::CCSprite* coin, const std::string& spriteName, int i) {
	if (!coin) {
		return;
	}

	coin->initWithSpriteFrameName(spriteName.c_str());
	coin->setScale(0.6f);
	coin->setPositionY(-32.f * i);


	if (!PlayLayer::get()->m_level->m_stars) {
		coin->setColor({ 255, 175, 75 });
	}
}
