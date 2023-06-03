#include "animedit.h"

#include <cassert>

#include "text.h"
#include "resources.h"
#include "DrawStrategy.h"
#include "anim.h"

using namespace std;

class AnimComponent2 : public AnimComponent {
public:
	AnimComponent2(Anim *anim) : AnimComponent(anim) { }
	 void draw(const GraphicsContext &gc) override {
		AnimComponent::draw(gc);
		al_draw_rectangle(getx(), gety(), getx() + getw(), gety() + geth(), GREEN, 1.0);
	}
};

class AnimEditorImpl : public AnimEditor
{
public:
	AnimEditorImpl()
	{
		SetFlag(D_DISABLE_CHILD_CLIPPING);
	}

	virtual ~AnimEditorImpl() {
	}

	void init(std::shared_ptr<Resources> res) override {

		add (ClearScreen::build(BLACK).get());
		
		auto anims = res->getAnims();

		//TODO: apply flow layout to group of components...
		int xco = 0;
		int yco = 0;
		int maxh = 0;

		for (auto &anim: anims) {
			auto &key = anim.first;
			Anim *aa = anim.second;

			for(int state = 0; state < aa->getNumStates(); ++state) {
				int margin = 32;
				// apply margin of 32 pixels
				// TODO: figure out optimal margin based on sizes of frames...
				int w = aa->sizex + margin * 2;
				int h = aa->sizey + margin * 2;
				if (h > maxh) maxh = h;

				auto animComponent = make_shared<AnimComponent2>(aa);
				animComponent->setLocation(xco + margin, yco + margin, aa->sizex, aa->sizey);
				animComponent->setState(state);
				animComponent->setDir(0);
				add(animComponent);
				xco += w;
				if (xco > 800) { //TODO hardcoded width...
					xco = 0;
					yco += maxh;
					maxh = 0;
				}
			}
		}
		
	}

	//  void draw(const GraphicsContext &gc) override {
		
	// 	auto res = parent->getResources();

	// 	// draw each animation one by one...
	// 	// draw bounding box...
	// }

	 void done() override {

	}

};

shared_ptr<AnimEditor> AnimEditor::newInstance()
{
	return make_shared<AnimEditorImpl>();
}
