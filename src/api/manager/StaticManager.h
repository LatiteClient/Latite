#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

template <typename Base, typename ... Items>
class StaticManager {
protected:
	std::tuple<Items...> items = { Items()... };
public:
	StaticManager() : items() {
	}
	StaticManager(StaticManager<Base, Items...>&) = delete;
	StaticManager(StaticManager<Base, Items...>&&) = delete;

	void forEach(std::function<void(Base&)> const& func) {
		forEachImpl(func, items);
	}

	template <typename T>
	T& get() {
		return std::get<T>(items);
	}

	virtual ~StaticManager() = default;
private:
	void forEachImpl(std::function<void(Base&)> const&, std::tuple<>&) {
	}

	template <typename ... Items>
	void forEachImpl(std::function<void(Base&)> const& fn, std::tuple<Items ...>& list) {
		fn(list._Myfirst._Val);
		forEachImpl(fn, list._Get_rest());
	}
};
