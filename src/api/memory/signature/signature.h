#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <stdexcept>

namespace memory {
	class signature_store {
	public:
		const char* mod = "";

		uintptr_t ref(int offset);
		uintptr_t deref(int offset);

		uintptr_t result = 0;
		uintptr_t scan_result = 0;

		std::string_view name;
		std::string_view signature;
	protected:
		std::function<uintptr_t(signature_store& store, uintptr_t res)> on_resolve;
	public:
		signature_store(const char* mod, decltype(on_resolve) onResolve, std::string_view sig,
			std::string_view
#ifdef API_NAMES
			name
#endif
		) :
#ifdef API_NAMES
			name(name),
#endif
			signature(sig),
			on_resolve(onResolve),
			mod(mod){
			if (!mod) {
				//throw std::runtime_error("mod is unspecified");
			}
		};
		bool resolve();
	};
}