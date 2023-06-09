#pragma once

#include "componentbuilder.h"
#include "widget.h"

class Button : public TextWidget
{
private:
    int action;
    ActionFunc actionFunc;

	void doAction();
protected:
	void doDraw (const GraphicsContext &gc) override;

    // accelerator
    int keycode = -1;
    int keymod = -1;

    Bitmap icon;
    void parseAndSetText(std::string value);
public:
    int getAction() { return action; }
	void handleMessage(ComponentPtr src, int msg) override;
	Button () : Button (MSG_ACTIVATE, "") {}
	Button (int action, std::string text, ALLEGRO_BITMAP* aIcon = nullptr) : action(action), actionFunc()
	{
		parseAndSetText(text);
		if (aIcon != nullptr) icon = Bitmap(aIcon); else icon = Bitmap();
		setBorder(Skin::BUTTON);
		SetFlag(D_DOUBLEBUFFER);
	}

	virtual ~Button() {}
    bool wantsFocus () override { return true; }
    void handleEvent(ALLEGRO_EVENT &event) override;
    double getPreferredWidth() override;

    void setIcon(int i) { icon = GetSkin()->GetBitmap(i); SetFlag(D_DIRTY); }

    void setActionCode(int value) { action = value; }
    void setActionFunc(ActionFunc value) { actionFunc = value; }
    void setAccelerator(int aKeycode, int aKeymod = 0) { keycode = aKeycode; keymod = aKeymod; }

    static ComponentBuilder<Button> build(int action, std::string text, int accelerator_key = -1, ALLEGRO_BITMAP* icon = nullptr);
    static ComponentBuilder<Button> build(ActionFunc actionFunc, std::string text, int accelerator_key = -1, ALLEGRO_BITMAP* icon = nullptr);

    // MASking leftovers...
	void MakeExit(); // make this button exit the dialog

	void UpdateSize() override
	{
		TextWidget::UpdateSize();
		if (bufferSizeMismatch())
		{
			resetBuffer();
		}
	}

	std::string const className() const override { return "Button"; }

	bool MsgXChar(int aKeycode, int aKeymod) override {
		if (keycode >= 0 && keymod >= 0 && aKeycode == keycode && aKeymod == keymod) { doAction(); return true; }
		return false;
	}
};
