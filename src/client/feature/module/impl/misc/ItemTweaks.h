#pragma once
#include "../../Module.h"

class ItemTweaks : public Module {
public:
	ItemTweaks();

	void onItemRender(Event& ev);

	virtual ~ItemTweaks() = default; // TODO: warn when no virtual destructor in virtual class
};