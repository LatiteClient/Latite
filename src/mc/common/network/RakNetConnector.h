#pragma once
#include "mc/Util.h"
#include "util/memory.h"
#include "RakPeer.h"

#include <string>

namespace SDK {
	class RakNetConnector {
	public:
		// please check for null
		static RakNetConnector* get();

		RakNetConnector() = delete;

		CLASS_FIELD(std::string, ipAddress, 0x3D0);
		CLASS_FIELD(std::string, dns, 0x3F0);
		CLASS_FIELD(uint16_t, port, 0x434);
		CLASS_FIELD(std::string, featuredServer, 0x4D0);

		class RakPeer* getPeer() {
			return memory::callVirtual<RakPeer*>(this, 0x19);
		}
	};
}
