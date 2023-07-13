#pragma once

class Latite {
public:
	static Latite& get() noexcept;
	class ModuleManager& getModuleManager() noexcept;
	void doEject() noexcept;
	void initialize(HINSTANCE hInst);

	Latite() = default;
	~Latite() = default;
private:

	void onUnload();
	HINSTANCE dllInst;
};