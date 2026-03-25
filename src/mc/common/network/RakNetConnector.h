#pragma once
#include "mc/Util.h"
#include "mc/Version.h"
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
			if (internalVers > V1_18_12) {
				if (internalVers > V1_19_51) {
					return memory::callVirtual<RakPeer*>(this, 0x19);
				}
				return memory::callVirtual<RakPeer*>(this, 0x17);
			}
			return hat::member_at<RakPeer*>(this, 0x230);
		}
	};
}
