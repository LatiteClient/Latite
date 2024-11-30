#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

template <typename Base, typename ... Items>
class StaticManager {
protected:
	std::tuple<Items...> items = { Items()... };
	std::vector<std::shared_ptr<Base>> dynamicItems;
public:
	StaticManager() : items() {
	}
	StaticManager(StaticManager<Base, Items...>&) = delete;
	StaticManager(StaticManager<Base, Items...>&&) = delete;

	template <typename T, typename ... Params>
	void addDynamicItem(Params&&... args) {
		dynamicItems.push_back(std::make_shared<T>(...args);
	}

	void forEach(std::function<void(Base&)> const& func) {
		forEachImpl(func, items);
		for (auto& item : dynamicItems) {
			func(*item);
		}
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
