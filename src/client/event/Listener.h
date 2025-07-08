#pragma once

// Abstract class
class Listener {
public:
	Listener() = default;
	virtual ~Listener() = default;

	virtual bool shouldListen() { return true; }
};