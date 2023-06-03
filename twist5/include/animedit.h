#pragma once

#include "state.h"

class Resources;

class AnimEditor : public State
{
public:
	virtual void init(std::shared_ptr<Resources> res) = 0;
	virtual void done() = 0;
	static std::shared_ptr<AnimEditor> newInstance();
};
