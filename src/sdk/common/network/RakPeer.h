#pragma once
#include "api/memory/memory.h"

namespace SDK {
	class RakPeer {
	public:
		bool Ping(const char* addy, unsigned short port, bool b1, unsigned int u1) {
			return memory::callVirtual<bool>(this, 0x26, addy, port, b1, u1);
		}

		int GetAveragePing(std::string const& addressOrGuid) {
			return memory::callVirtual<int>(this, 0x29, addressOrGuid); // getlastping
		}
	};
}