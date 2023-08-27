#pragma once
#include "util/LMath.h"
#include "../actor/Actor.h"
#include <string>

namespace SDK {
	class Level {
	public:
		void playSoundEvent(std::string const& text, Vec3 const& pos, float vol = 1.f, float pitch = 1.f);
		std::vector<SDK::Actor*> getRuntimeActorList();
		class HitResult* getHitResult();
	};
}