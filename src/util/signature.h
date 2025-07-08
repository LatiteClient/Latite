#pragma once
#include <cstdint>
#include <string>
#include <functional>
#include <stdexcept>
#include <mnemosyne/scan/signature.hpp>

namespace memory {
	class signature_store {
	public:
		const char* mod = "";

		uintptr_t ref(int offset);
		uintptr_t deref(int offset);

		uintptr_t result = 0;
		uintptr_t scan_result = 0;

		std::string_view name;
		std::optional<mnem::signature> signature;
	protected:
		std::function<uintptr_t(signature_store& store, uintptr_t res)> on_resolve;
	public:
		signature_store(const char* mod, decltype(on_resolve) onResolve, std::optional<mnem::signature> sig,
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

		template <typename T>
		[[nodiscard]] T* as_ptr() {
			return reinterpret_cast<T*>(result);
		}
	};
}