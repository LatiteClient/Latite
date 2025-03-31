#pragma once
#include "Option.h"

namespace SDK {
	

	class Options : std::enable_shared_from_this<Options> {
	public:
		void setPlayerViewPerspective(int perspective) {
			memory::callVirtual<void>(this, 136, perspective);
		}

		int getPlayerViewPerspective() {
			return memory::callVirtual<int>(this, 137);
		}

		bool getVsync() {
			for (auto& option : options) {
				if (option->impl->id == OptionID::Vsync) {
					return static_cast<SDK::BoolOption*>(option.get())->value;
				}
			}
		}

	private:
		void* unknown;
		std::array<std::unique_ptr<class Option>, 788> options;

		virtual ~Options() = 0;
	};
}