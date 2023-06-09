#pragma once

#include "container.h"
#include "component.h"

/**
 * A state is the same as a Container, but
 * has different focus behaviour.
 *
 * A state starts invisible and inactive
 *
 * A state becomes active/visible when focus is gained,
 * and inactive/invisible when focus is lost.
 */
class State : public Container {

public:
	State() {
		setVisible(false);
		setAwake(false);
	}

	void onFocus() override;
	bool onHandleMessage(ComponentPtr src, int msg) override;
	std::string const className() const override { return "State"; }
};
