#pragma once

namespace SDK {
	struct MobBodyRotationComponent : IEntityComponent {
		static constexpr uint32_t type_hash = 0xD7F64BBA;

		float yBodyRot;
		float yBodyRotOld;
	};
}
