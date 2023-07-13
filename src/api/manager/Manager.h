template <typename T>
class Manager {
public:
	Manager() = default;
	~Manager() = default;

protected:
	std::vector<std::shared_ptr<T>> items = {};
};