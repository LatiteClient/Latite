#pragma once

// Abstract class
class Listener {
public:
	Listener() = default;
	virtual ~Listener() = default;

	virtual bool isListening() { return shouldListen; }
protected:
	bool shouldListen = false;
};