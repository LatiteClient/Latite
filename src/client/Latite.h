#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Latite {
public:
	static Latite& get() noexcept;
	static class ModuleManager& getModuleManager() noexcept;
	static class CommandManager& getCommandManager() noexcept;
	static class ClientMessageSink& getClientMessageSink() noexcept;

	void doEject() noexcept;
	void initialize(HINSTANCE hInst);

	Latite() = default;
	~Latite() = default;
private:

	void onUnload();
	HINSTANCE dllInst;
};