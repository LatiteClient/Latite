#pragma once
class UpdatePlayerCameraEvent : public Event {
public:
	static const uint32_t hash = TOHASH(UpdatePlayerCameraEvent);

	void setViewAngles(Vec2 const& angles) {
		newViewAngles = angles;
	}

	std::optional<Vec2> getNewRot() {
		return newViewAngles;
	}
private:
	std::optional<Vec2> newViewAngles;
};