#pragma once
#include <util/LMath.h>
#include <api/eventing/Event.h>

class OutlineSelectionEvent : public Cancellable {
private:
	BlockPos blockPos;
public:

	static const uint32_t hash = TOHASH(OutlineSelectionEvent);

	OutlineSelectionEvent(const BlockPos& blockPos)
		: blockPos(blockPos)
	{
	}

	BlockPos getBlockPos() {
		return blockPos;
	}

	AABB getBoundingBox() {
		return { {(float)blockPos.x, (float)blockPos.y, (float)blockPos.z},
			{(float)blockPos.x + 1.f, (float)blockPos.y + 1.f, (float)blockPos.z + 1.f} };
	}
};