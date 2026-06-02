#pragma once

namespace SDK {
	struct RenderRotationComponent : IEntityComponent {
		static constexpr uint32_t type_hash = 0xD15944E2;

		Vec2 rotation;
	};
}
