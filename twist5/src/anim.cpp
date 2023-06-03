#include <cassert>
#include "anim.h"
#include "color.h"
#include <cstdlib>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <memory>
#include "collection_util.h"

using namespace xdom;
using namespace std;

#include "util.h"

const char * defaultdirections[] = { "n", "e", "s", "w" };

class DefaultDirectionModel : public IDirectionModel
{
private:
	map<string, int> dirMap;
public:
	virtual int idToIndex(const string &id) override
	{
		if (dirMap.find(id) == dirMap.end())
		{
			dirMap.insert (pair<string, int>(id, dirMap.size()));
		}

		return dirMap[id];
	}

	virtual ~DefaultDirectionModel() {}
};

std::shared_ptr<IDirectionModel> Anim::model = make_shared<DefaultDirectionModel>();

struct CascadingProps
{
	/**
	 * properties hotx and hoty start at some default,
	 * and can be overriden at any level in the XML hierarchy.
	 *
	 * This way a cascading stylesheet effect is achieved, and
	 * redundant attributes are avoided.
	 */
	int hotx;
	int hoty;

	/* distinct from hotx, hoty + dx, dy, dz - which define the bounding box,
	 * originx,y + regionw,h define an area on the spritesheet */
	int originx;
	int originy;
	int regionw;
	int regionh;
	string rle;
	CascadingProps () : hotx (0), hoty (0), originx(0), originy(0), regionw(0), regionh(0), rle() {}

	/* check the node for overridable properties */
	void checkProperties (const DomNode &n);
};

/**
 * Anim properties start at some default,
 * and can be overriden at any level in the XML hierarchy.
 *
 */
class AnimBuilder
{
private:
	Anim *temp;
	IBitmapProvider *res;

	enum { TR_ROT };
	/* NB: props must be copied for each branch of the XML tree, so props must be pass-by-value */
	std::vector < Sequence > loadState (const xdom::DomNode &i, CascadingProps props);
	Sequence loadSequence (const xdom::DomNode &i, CascadingProps props); // props must be pass-by-value
	Frame loadFrame (const xdom::DomNode &i, CascadingProps props); // props must be pass-by-value
	CompositePart loadPart (const xdom::DomNode &i, CascadingProps props); // props must be pass-by-value
	ALLEGRO_BITMAP *getLitRle(ALLEGRO_BITMAP *input, int r, int g, int b, int a);
public:
	explicit AnimBuilder(IBitmapProvider *_res) : temp(nullptr), res(_res) {}
	void loadFromXml (const xdom::DomNode &n);
	Anim *get() { return temp; }
};

// private helpers
void checkIntProperty (const DomNode &n, string const &attr, int &ref) {
	if (hasKey(n.attributes, attr)) {
		ref = stoi(n.attributes.at(attr));
	}
}

void checkStringProperty (const DomNode &n, string const &attr, string &ref) {
	if (hasKey(n.attributes, attr)) {
		ref = n.attributes.at(attr);
	}
}

void CascadingProps::checkProperties (const DomNode &n)
{
	checkIntProperty(n, "regionw", regionw);
	checkIntProperty(n, "regionh", regionh);
	checkIntProperty(n, "hotx", hotx);
	checkIntProperty(n, "hoty", hoty);
	checkIntProperty(n, "originx", originx);
	checkIntProperty(n, "originy", originy);

	checkStringProperty(n, "rle", rle);
}

Anim::Anim() : frames(), sizex (0), sizey (0), sizez (0)
{
}

void Anim::loadFromXml (const DomNode &n, IBitmapProvider *res, map<string, Anim*> &result) {
	AnimBuilder builder (res);
	builder.loadFromXml(n);
	string name = n.attributes.at("id");
	Anim *temp = builder.get();
	result[name] = temp;
}

int safeInt(const std::map<std::string, std::string> &aMap, const std::string &aKey, int defaultValue = 0) {
	if(aMap.find(aKey) == aMap.end()) {
		return defaultValue;
	}
	else {
		return stoi(aMap.at(aKey));
	}
}

void AnimBuilder::loadFromXml (const DomNode &n) {
	assert (n.name == "anim");

	string name = n.attributes.at("id");
	vector<DomNode>::const_iterator h;
	
	temp = new Anim();
	
	CascadingProps props;
	props.checkProperties(n);
	temp->sizex = safeInt(n.attributes, "dx");
	temp->sizey = safeInt(n.attributes, "dy");
	temp->sizez = safeInt(n.attributes, "dz");

	h = n.children.begin();
	if (h->name == "state") {
		for (; h != n.children.end(); ++h) {
			temp->frames.push_back (loadState (*h, props));
		}
	}
	else {
		temp->frames.push_back (loadState (n, props));
	}
}

Sequence AnimBuilder::loadSequence(const xdom::DomNode &i, CascadingProps props) {
	Sequence result;
	result.totalLength = 0;
	result.loop = true;

	props.checkProperties(i);

	for (const auto &j: i.children) {
		Frame f = loadFrame (j, props);
		result.add (f);
	}
	assert (result.size() > 0);
	return result;
}

vector < Sequence > AnimBuilder::loadState (const DomNode &i, CascadingProps props) {
	vector < Sequence > result;
	
	vector<DomNode>::const_iterator h;
	
	int totalDirs = 0;

	props.checkProperties(i);

	h = i.children.begin();
	if (h->name == "frame" || h->name == "composite")
	{
		result.push_back (loadSequence (i, props));
		totalDirs = 1;
	}
	else if (h->name == "dir")
	{	
		for (; h != i.children.end(); ++h)
		{
			string id = h->attributes.at("id");
			int dir = Anim::model->idToIndex(id);
			if (dir < 0)
			{
				allegro_message ("Unknown direction '%s'", id.c_str());
				assert (false); // unknown tag
			}
			else
			{
				totalDirs++;
			}
			if (result.size() < (unsigned)(dir+1)) result.resize(dir+1);
			result[dir] = loadSequence (*h, props);
		}
	}
	else
	{
		allegro_message ("Unknown tag %s", h->name.c_str());
		assert (false);
	}
	
	return result;
}

/* 
relx, rely are relative to the animation 
NOTE: this is relatively slow on video bitmaps, but only if the queried pixel actually falls within the bitmaps.
*/
bool Frame::hasPixelAt(int relx, int rely) const {
	for(const auto &i: parts) {
		int xx = relx - i.hotx;
		int yy = rely - i.hoty;
		int ww = al_get_bitmap_width(i.bitmap);
		int hh = al_get_bitmap_height(i.bitmap);
		if (xx < 0 || yy < 0 || xx >= ww || yy >= hh) continue;
		
		ALLEGRO_COLOR color = al_get_pixel(i.bitmap, xx, yy);
		unsigned char r, g, b, a;
		al_unmap_rgba(color, &r, &g, &b, &a);
		if (a > 0) {
			return true;
		}
	}
	return false;
}

void Frame::getCompositeBounds (int &sprx, int &spry, int &width, int &height) const
{
	vector<CompositePart>::const_iterator i;
	i = parts.begin();
	int x1 = i->hotx;
	int y1 = i->hoty;
	int x2 = i->hotx + al_get_bitmap_width(i->bitmap);
	int y2 = i->hoty + al_get_bitmap_height(i->bitmap);
	++i;
	// now grow those bounds if any of the next parts fall outside it
	for (; i != parts.end(); ++i)
	{
		if (i->hotx < x1) x1 = i->hotx;
		if (i->hoty < y1) y1 = i->hoty;
		if (i->hotx + al_get_bitmap_width(i->bitmap) > x2) x2 = i->hotx + al_get_bitmap_width(i->bitmap);
		if (i->hoty + al_get_bitmap_height(i->bitmap) > y2) y2 = i->hoty + al_get_bitmap_height(i->bitmap);
	}
	sprx = x1;
	spry = y1;
	width = x2 - x1;
	height = y2 - y1;
}

Frame AnimBuilder::loadFrame (const DomNode &i, CascadingProps props)
{
	Frame result;
	props.checkProperties(i);

	int time = 0;
	if (hasKey(i.attributes, "time")) {
		time = stoi(i.attributes.at("time"));
	}
	result.length = time;
	if (i.name == "frame") {
		// just a single part.
		CompositePart part = loadPart (i, props);
		result.parts.push_back (part);
	}
	else if (i.name == "composite") {
		for (const auto &j: i.children) {
			// multiple parts
			CompositePart part = loadPart (j, props);
			result.parts.push_back (part);
		}
	}
	else {
		assert (false); // unknown tag
	}
	
	return result;
}

ALLEGRO_BITMAP *AnimBuilder::getLitRle(ALLEGRO_BITMAP *s, int r, int g, int b, int a)
{
	assert (s);
	int w = al_get_bitmap_width(s);
	int h = al_get_bitmap_height(s);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
	ALLEGRO_BITMAP *temp = al_create_bitmap (w, h);
	assert (temp);

	al_set_target_bitmap(temp);

	al_draw_bitmap (s, 0, 0, 0); // copy sprite
	al_draw_filled_rectangle (0, 0, w, h, al_map_rgba(r, g, b, a)); // overlay lit color in complete area
	al_set_blender (ALLEGRO_ADD, ALLEGRO_ZERO, ALLEGRO_ALPHA);
	al_draw_bitmap (s, 0, 0, 0); // copy transparent areas
	al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	char buf[256];
	snprintf (buf, sizeof(buf), "rle_lit_%p", temp);
	res->putBitmap (buf, temp);
	return temp;
}

CompositePart AnimBuilder::loadPart(const DomNode &i, CascadingProps props)
{
	CompositePart result;
	
	props.checkProperties(i);
	result.hotx = props.hotx;
	result.hoty = props.hoty;
	result.originx = props.originx;
	result.originy = props.originy;
	result.regionw = props.regionw;
	result.regionh = props.regionh;
	result.angle = 0;
	string rle = props.rle;

	// handle "lit" effect
	ALLEGRO_BITMAP *temprle = res->getBitmap (rle);
	if (!temprle)
	{
		allegro_message ("error loading animation frame %s", rle.c_str());
	}

	bool isRegion = (result.originx != 0 || result.originy != 0 || 
		result.regionw != 0 || result.regionh != 0);
	
	if (result.regionw == 0) result.regionw = al_get_bitmap_width(temprle);
	if (result.regionh == 0) result.regionh = al_get_bitmap_height(temprle);

	if (isRegion) {
		ALLEGRO_BITMAP *temp2 = al_create_sub_bitmap(temprle, result.originx, result.originy, result.regionw, result.regionh);
		char buf[256];
		snprintf (buf, sizeof(buf), "subbitmap_%p", temp);
		res->putBitmap (buf, temp2);
		temprle = temp2;
	}

	// apply transformations and effects.
	for (const auto &m: i.children) {
		if (m.name == "lit") {
			int r = stoi(m.attributes.at("r"));
			int g = stoi(m.attributes.at("g"));
			int b = stoi(m.attributes.at("b"));
			int a = 255; // default alpha is 255
			if(hasKey(m.attributes, "alpha")) {
				a = stoi(m.attributes.at("alpha"));
			}
			temprle = getLitRle (temprle, r, g, b, a);
		}
		else if (m.name == "flip") {
			result.transformFlag |= ALLEGRO_FLIP_HORIZONTAL;
		}
		else if (m.name == "vflip") {
			result.transformFlag |= ALLEGRO_FLIP_VERTICAL;
		}
		else if (m.name == "rot") {
			int a = stoi(m.attributes.at("angle"));
			if (a >= 0 && a <= 360) {
				result.angle = a;
			}
			else allegro_message ("Invalid rotation %i", a);
		}
	}

	if (!temprle) {
		allegro_message ("error transforming animation frame %s", rle.c_str());
	}
	result.bitmap = temprle;

	return result;
}

void Frame::draw(int x, int y) const
{
	for (auto &i : parts)
	{
		if (i.angle != 0) {
			int w = al_get_bitmap_width(i.bitmap);
			int h = al_get_bitmap_height(i.bitmap);
			al_draw_rotated_bitmap(i.bitmap, w/2, h/2, x + w/2 + i.hotx, y + h/2 + i.hoty,
				(float)i.angle * M_PI / 180.0, i.transformFlag);
		}
		else {
			al_draw_bitmap(i.bitmap, x + i.hotx, y + i.hoty, i.transformFlag);
		}
	}
}

void Frame::draw_tinted(int x, int y, ALLEGRO_COLOR tint) const
{
	for (auto &i : parts) {
		al_draw_tinted_bitmap (i.bitmap, tint, x + i.hotx, y + i.hoty, i.transformFlag);
	}
}

const Frame *Sequence::getFrame (int time) const
{
	if (loop && totalLength > 0)
	{
		time %= totalLength;
	}
	
	unsigned int i = 0;
	while ((i < frames.size()) && (time > frames[i].cumulative)) { i++; }
	if (i == frames.size()) i--;
	
	return &frames[i];
}

const Frame *Anim::getFrame (unsigned int state, unsigned int dir, int time) const
{
	assert (frames.size() > 0);
	assert (state < frames.size());
	dir %= frames[state].size();
	const Sequence &s = frames[state][dir];
	return s.getFrame (time);
}

void Anim::drawFrame (unsigned int state, unsigned int dir, int time, int x, int y) const
{
	const Frame *current = getFrame (state, dir, time);
	current->draw (x, y);
}

void Sequence::add (Frame &f)
{
	totalLength += f.length;
	f.cumulative = totalLength;
	frames.push_back (f);
}

void Anim::add (Frame &f, unsigned int state, unsigned int dir)
{
	if (state >= frames.size()) frames.resize(state + 1);
	if (dir >= frames[state].size()) frames[state].resize(dir + 1);
	frames[state][dir].add(f);
}
