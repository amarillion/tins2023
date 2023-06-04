#pragma once

#include "container.h"

class Messages : public Container {
public:
	enum Behavior { RIGHT_TO_LEFT, POP_UP };
	void showMessage(std::string text, Behavior behavior);

	virtual std::string const className() const override { return "Messages"; }
};
