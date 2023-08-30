#include "pch.h"
#include "CommandRequestPacket.h"
#include "sdk/Util.h"

SDK::CommandRequestPacket::CommandRequestPacket(std::string command) {
	memset(this, 0x0, sizeof(CommandRequestPacket));
	*reinterpret_cast<uintptr_t*>(this) = Signatures::Vtable::CommandRequestPacket.result;
	this->unkInt1 = 2;
	this->unkInt2 = 1;
	this->type = 0;
	this->command.setString(command.c_str());
	this->unkBool = false; // needsTranslation i think
	// TODO: this is for 1.20, command request version changes
	util::directAccess<int>(this, 0x90) = 34;
}
