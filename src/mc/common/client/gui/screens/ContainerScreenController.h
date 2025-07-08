#pragma once
#include "ScreenController.h"

namespace SDK {
	class ContainerScreenController : public ScreenController {
	public:
		void _handleTakePlace(const std::string& viewName,
			int slot, bool b);
		void* _getSelectedSlotInfo();
	};
}