#pragma once
#include "mc/Util.h"
#include "util/LMath.h"

namespace SDK {
	class ClientInstance {
	public:
		CLASS_FIELD(class MinecraftGame*, minecraftGame, 0x1A0);
		CLASS_FIELD(class Minecraft*, minecraft, 0x1A8);
		CLASS_FIELD(class LevelRenderer*, levelRenderer, 0x1B8);
		CLASS_FIELD(class PacketSender*, packetSender, 0x1C8);
		CLASS_FIELD(class ClientInputHandler*, inputHandler, 0x1D8);
		CLASS_FIELD(struct Vec2, cursorPos, 0x580);

		static ClientInstance* get();
		class BlockSource* getRegion();
		class LocalPlayer* getLocalPlayer();
		class GuiData* getGuiData();
		class Options* getOptions();
		//class ClientHMDState* getClientHMDState(); Removed in 1.21.111

		void grabCursor();
		void releaseCursor();
	private:
		static ClientInstance* instance;
	};
}