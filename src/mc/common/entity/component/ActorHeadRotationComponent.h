#pragma once

namespace SDK {
	struct ActorHeadRotationComponent : IEntityComponent {
		static constexpr uint32_t type_hash = 0xBABE7211;

		float yHeadRot;
		float yHeadRotOld;
	};
}
