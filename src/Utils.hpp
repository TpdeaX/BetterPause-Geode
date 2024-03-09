#pragma once
#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <time.h>
#include <string>

using namespace geode::prelude;

#ifdef GEODE_IS_WINDOWS
namespace matdash {
	struct Console {
		std::ofstream out, in;
		Console() {
			AllocConsole();
			out = decltype(out)("CONOUT$", std::ios::out);
			in = decltype(in)("CONIN$", std::ios::in);
			std::cout.rdbuf(out.rdbuf());
			std::cin.rdbuf(in.rdbuf());

			FILE* dummy;
			freopen_s(&dummy, "CONOUT$", "w", stdout);
		}
		~Console() {
			out.close();
			in.close();
		}
	};

	inline void create_console() {
		static Console console;
	}
}
#endif

namespace Utils
{
	const char* getNameObj(cocos2d::CCNode* obj);
	bool returnZeroBool(int i);
	FMODAudioEngine* shareFMOD();
	GameManager* shareManager();

	PlayLayer* getplayLayerA();
	GLubyte convertOpacitySimplf(float opaTM);

	cocos2d::CCDirector* shareDirectorA();

	float getPercentagePerXpos();
	float getPercentageReversePerXpos();
	float getPercentageNow();
	float getPercentageReverseNow();
	float getPercentageNowFix();
	float getPercentageReverseNowFix();
	float getPercentagePerTime();

	void updatePercentageObjects();

	cocos2d::CCSize WinSize();

	template <class R, class T>
	R& from(T base, intptr_t offset) {
		return *reinterpret_cast<R*>(reinterpret_cast<uintptr_t>(base) + offset);
	}

	template<typename T>
	std::string vectorToString(const std::vector<T>& vec) {
		std::ostringstream oss;
		for (size_t i = 0; i < vec.size(); ++i) {
			if (i != 0) {
				oss << " ";
			}
			oss << vec[i];
		}
		return oss.str();
	}

	template<typename T>
	std::vector<T> stringToVector(const std::string& str) {
		std::vector<T> vec;
		std::istringstream iss(str);
		T val;
		while (iss >> val) {
			vec.push_back(val);
		}
		return vec;
	}

	std::string decodeBase64Gzip(const std::string& input);
	bool isSpecificAspectRatio(float targetRatio);
	bool isRoundAspectRatio(float targetRatio);

	std::string getFormattedCreatorName(const std::string& creatorName, GJLevelType levelType);
	std::string buildStarRatingString(int rating, int requested, bool platformer);
	std::string getNameLevelType(GJLevelType type);
	double getTotalSecondsPlayLayer();
	int getTotalAttemptsPlayLayer();
	bool hasParentWithID(cocos2d::CCNode* node, const std::string& parentID);
	cocos2d::CCNode* getParentWithID(cocos2d::CCNode* node, const std::string& parentID);

}


//thx cvolton :3
namespace TimeUtils {
	std::string timeToString(time_t input);
	std::string isoTimeToString(const std::string& input);
	std::string workingTime(int value);
	std::string platformerTime(int value);
	std::string timestampToHumanReadable(time_t timestamp);
}


/*

i took this class from FigmentBoy (https://github.com/FigmentBoy/DialogHandler)

*/

class AttemptAtReversingDialogObject : public cocos2d::CCObject {
protected:
	char pad_0x00[0x60]; // 0x00

	std::string m_content; // 0x32
	std::string m_title; // 0x56
	int type; // 0x80
	_ccColor3B color; // 9x84
	char pad_0x87[0x01]; // 0x87
	float m_unknown; // 0x88
	char pad_0x92[0x04]; // 0x92
	bool m_unknown_2; // 0x96

public:
	bool init(std::string title, std::string text, int type, float unknown, bool also_unknown, _ccColor3B textColor) {
		return reinterpret_cast<bool(__thiscall*)(AttemptAtReversingDialogObject*, std::string, std::string, int, float, bool, _ccColor3B)>
			(geode::base::get() + 0x9a6c0)(this, title, text, type, unknown, also_unknown, textColor);
	}

	static AttemptAtReversingDialogObject* create(std::string title, std::string text, int type, float text_scale, bool is_unskippable, _ccColor3B textColor) {
#ifdef GEODE_IS_WINDOWS

		AttemptAtReversingDialogObject* obj = new AttemptAtReversingDialogObject();

		obj->init(title, text, type, text_scale, is_unskippable, textColor);
		obj->autorelease();
		return obj;
#else
		return reinterpret_cast<AttemptAtReversingDialogObject*>(DialogObject::create(title, text, type, text_scale, is_unskippable, textColor));
#endif
		
	}
};