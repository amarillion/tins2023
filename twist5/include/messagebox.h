#pragma once

#include "window.h"

class MessageBox : public Window
{
public:

	static int showMessage(const char * title, const char *message,
			const char *btn1, const char *btn2 = nullptr, const char *btn3 = nullptr);
};
