#pragma once
class UpdatePlayerCameraEvent : public Cancellable {
public:
	static const uint32_t hash = TOHASH(UpdatePlayerCameraEvent);
};