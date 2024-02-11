#include <Geode/Geode.hpp>
#include <Geode/Bindings.hpp>
#include "Utils.hpp"

class QuickSettingsButtons : public GJOptionsLayer {
public:

	static QuickSettingsButtons* create();
	bool init();
};