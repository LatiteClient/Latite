#pragma once

class Latite {
public:
	static Latite& get() noexcept;
	void doEject() noexcept;
	void initialize(HINSTANCE hInst);

	Latite() = default;
	~Latite() = default;
private:
	HINSTANCE dllInst;
};