#pragma once
#include <cstdint>
#include <string>
#include <functional>

namespace memory {
	class signature_store {
	private:
		std::string str;
		std::function<void(uintptr_t res)> onResolve;
		uintptr_t result = 0;
		uintptr_t scanResult = 0;

		uintptr_t ref(int offset);
		uintptr_t deref(int offset);
	public:
		signature_store(std::string str, decltype(onResolve) onResolve) : str(str), onResolve(onResolve) {};
		void resolve();
	};
}