#pragma once
#include "Option.h"

namespace SDK {
	

	class Options : std::enable_shared_from_this<Options> {
	public:
		void setPlayerViewPerspective(int perspective) {
			memory::callVirtual<void>(this, 123, perspective);
		}

		int getPlayerViewPerspective() {
			return memory::callVirtual<int>(this, 124);
		}

		bool getVsync() {
			auto it = std::find_if(options.begin(), options.end(), [](std::unique_ptr<Option>& option) {
				if (option && option->impl->id == OptionID::Vsync) {
					return true;
				}
				});

			if (it == options.end()) {
				return false;
			}

			return static_cast<BoolOption*>(it->get())->value;
		}

	private:
		void* unknown;
		std::array<std::unique_ptr<class Option>, 776> options;

		virtual ~Options() = 0;
	};
}