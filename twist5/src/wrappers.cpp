#include "wrappers.h"

Bitmap::Bitmap()
:data(nullptr),
   type(VIDEO),
   allocated(false),
   thick(-1,-1)
{
}


Bitmap::Bitmap(int w, int h, Type t)
   :data(nullptr),
   thick(-1,-1)
{
   Create(w, h, t);
}


Bitmap::Bitmap(const Point &s, Type t)
   :data(nullptr),
   thick(-1,-1)
{
   Create(s, t);
}


Bitmap::Bitmap(const char *file, Type t)
   :data(nullptr),
   type(t),
   thick(-1,-1)
{
   Load(file, t);
}


Bitmap::Bitmap(ALLEGRO_BITMAP *bmp) :thick(-1,-1) {
   if (!bmp) {
      type = Bitmap::UNDEFINED;
      data = nullptr;
   }
   else {
      if (al_get_bitmap_flags(bmp) & ALLEGRO_MEMORY_BITMAP) {
         type = Bitmap::MEMORY;
      }
      else {
         type = Bitmap::VIDEO;
      }
   }
   data = bmp;
   allocated = false;
}


Bitmap::Bitmap(const Bitmap &bmp) {
   data = bmp.data;
   type = bmp.type;
   thick = bmp.thick;
   allocated = false;
}


Bitmap::~Bitmap() {
   Destroy();
}


Bitmap::operator ALLEGRO_BITMAP*() const {
   return data;
}


Bitmap& Bitmap::operator=(ALLEGRO_BITMAP *bmp) {
   Destroy();
   if (!bmp) {
      type = Bitmap::UNDEFINED;
      data = nullptr;
   }
   else {
      if (al_get_bitmap_flags(bmp) & ALLEGRO_MEMORY_BITMAP) {
         type = Bitmap::MEMORY;
      }
      else {
         type = Bitmap::MEMORY;
      }
   }
   data = bmp;
   allocated = false;
   thick.x(-1);
   thick.y(-1);
   return *this;
}


Bitmap& Bitmap::operator=(const Bitmap &bmp) {
   Destroy();
   data = bmp.data;
   type = bmp.type;
   thick = bmp.thick;
   allocated = false;
   return *this;
}


Bitmap& Bitmap::Set(ALLEGRO_BITMAP *bmp, bool copy, Type t) {
   Destroy();
   if (!bmp) {
      data = nullptr;
      type = UNDEFINED;
      allocated = false;
   }
   else {
      if (copy && bmp) {
         if (t != UNDEFINED) {
            type = t;
         }
         else {
            type = VIDEO;
            if (al_get_bitmap_flags(bmp) & ALLEGRO_MEMORY_BITMAP)
               type = MEMORY;
         }
         Create(al_get_bitmap_width(bmp), al_get_bitmap_height(bmp), type);
         _MASKING_BITMAP_STORE_AND_SET_ALL(data)
         al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
         al_draw_bitmap(bmp, 0, 0, 0);
         _MASKING_BITMAP_RESTORE

         allocated = true;
      }
      else {
         data = bmp;
         if (al_get_bitmap_flags(bmp) & ALLEGRO_MEMORY_BITMAP) {
            type = Bitmap::MEMORY;
         }
         else {
            type = VIDEO;
         }
         allocated = false;
      }
   }
   return *this;
}


Bitmap& Bitmap::Set(const Bitmap &bmp, bool copy, Type t) {
   Destroy();
   if (copy && bmp) {
      if (t != UNDEFINED) {
         type = t;
      }
      else {
         type = bmp.type;
      }
      Create(bmp.w(), bmp.h(), type);
      _MASKING_BITMAP_STORE_AND_SET_ALL(data)
      al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
      al_draw_bitmap(bmp, 0, 0, 0);
      _MASKING_BITMAP_RESTORE
      allocated = true;
   }
   else {
      data = bmp.data;
      type = bmp.type;
      allocated = false;
   }
   return *this;
}

Error Bitmap::Load(const std::string &file, Type t)
{
	return Load(file.c_str(), t);
}

Error Bitmap::Load(const char *file, Type t) {
   ALLEGRO_PATH *alp = al_create_path(file);
   if (!al_filename_exists(al_path_cstr(alp, ALLEGRO_NATIVE_PATH_SEP))) {
      al_destroy_path(alp);
      return Error(Error::NO_FILE);
   }
   al_destroy_path(alp);

   ALLEGRO_STATE state;
   al_store_state(&state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
   if (t == MEMORY)
      al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP|ALLEGRO_CONVERT_BITMAP);
   else
      al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP|ALLEGRO_CONVERT_BITMAP);

   ALLEGRO_BITMAP *tmp = al_load_bitmap(file);

   al_restore_state(&state);

   if (!tmp) {
      return Error(Error::LOAD_FILE);
   }

   Destroy();
   data = tmp;
   type = t;
   allocated = true;
   return Error(Error::NONE);
}


void Bitmap::Save(const char *file) {
   al_save_bitmap(file, data);
}


Bitmap& Bitmap::Create(int w, int h, Type t) {
   type = t;
   ALLEGRO_STATE state;
   al_store_state(&state, ALLEGRO_STATE_NEW_BITMAP_PARAMETERS);
   switch (t) {
      case MEMORY: al_set_new_bitmap_flags(ALLEGRO_MEMORY_BITMAP|ALLEGRO_CONVERT_BITMAP); break;
      case VIDEO:  al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP|ALLEGRO_CONVERT_BITMAP); break;
      default: ALLEGRO_ASSERT(0);
   }
   al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA);
   data = al_create_bitmap(w, h);
   al_restore_state(&state);
   allocated = true;

   return *this;
}


Bitmap& Bitmap::CreateSubBitmap(const Bitmap &parent, int x, int y, int w, int h) {
   int xx = x < 0 ? 0 : x;
   int yy = y < 0 ? 0 : y;
   int ww = (x + w > parent.w()) ? parent.w() - x : w;
   int hh = (y + h > parent.h()) ? parent.h() - y : h;

   data = al_create_sub_bitmap(parent.data, xx, yy, ww, hh);
   type = parent.type;
   allocated = true;
   return *this;
}


void Bitmap::Destroy() {
   if (data && allocated) {
      al_destroy_bitmap(data);
      data = nullptr;
      allocated = false;
   }
}


////////////////////////////////////////////////////////////////////////////////
// Draws a dotted rectangle.
void Bitmap::DrawDottedRect(int x1, int y1, int x2, int y2, const ALLEGRO_COLOR &c) {
   int x, y;
   for (x=x1; x<=x2; x+=2) {
      Putpixel(x, y1, c);
      Putpixel(x, y2, c);
   }
   for(y=y1; y<=y2; y+=2) {
      Putpixel(x1, y, c);
      Putpixel(x2, y, c);
   }
}


////////////////////////////////////////////////////////////////////////////////
// Draws a 3D frame from (x1,y1) to (x2,y2) with colors c2 and c3 and fills it with
// c1. If c1==transparent, it is considered transparent.
void Bitmap::Draw3DFrame(int x1, int y1, int x2, int y2,
                              const ALLEGRO_COLOR &c1,
                              const ALLEGRO_COLOR &c2,
                              const ALLEGRO_COLOR &c3)
{
   //TODO: does equality test work here?
//   if (c1 != TRANSPARENT) Rectfill(x1, y1, x2, y2, c1);

   const ALLEGRO_COLOR c4 = al_map_rgb(c2.r + (c3.r - c2.r)/3, c2.g + (c3.g - c2.g)/3, c2.b + (c3.b - c2.b)/3);
   const ALLEGRO_COLOR c5 = al_map_rgb(c3.r - (c3.r - c2.r)/3, c3.g - (c3.g - c2.g)/3, c3.b - (c3.b - c2.b)/3);

   Vline(x1, y1, y2, c4);
   Hline(x1, y1, x2, c4);
   Vline(x1+1, y1+1, y2-1, c2);
   Hline(x1+1, y1+1, x2-1, c2);
   Vline(x2-1, y1+1, y2, c5);
   Hline(x1+1, y2-1, x2, c5);
   Hline(x1, y2, x2, c3);
   Vline(x2, y1, y2, c3);
}


////////////////////////////////////////////////////////////////////////////////
// Draws a 3D circle on (x,y) wth the radius r
void Bitmap::Draw3DCircle(int x, int y, int r,
                               const ALLEGRO_COLOR &c1,
                               const ALLEGRO_COLOR &c2,
                               const ALLEGRO_COLOR &c3)
 {
   //TODO: does equality test work here?
//   if (c1 != BLACK) Circlefill(x, y, r, c1);

   unsigned char r2, r3, g2, g3, b2, b3;
   al_unmap_rgb(c1, &r2, &g2, &b2);
   al_unmap_rgb(c1, &r3, &g3, &b3);
   ALLEGRO_COLOR c4 = al_map_rgb(r2 + (r3 - g2)/3, g2 + (g3 - g2)/3, b2 + (b3 - b2)/3);
   ALLEGRO_COLOR c5 = al_map_rgb(r3 - (r3 - r2)/3, g3 - (g3 - g2)/3, b3 - (b3 - b2)/3);

   SetClip(0, 0, x + (r>>1), y + (r>>1));
   Circle(x, y, r, c4);
   Circle(x, y, r-1, c2);

   SetClip(x - (r>>1), y - (r>>1), w()-1, h()-1);
   Circle(x, y, r-1, c5);
   Circle(x, y, r, c3);

   SetClip(0, 0, w()-1, h()-1);
}


////////////////////////////////////////////////////////////////////////////////
// Draws an X mark with the centre on (x,y), size s and color col
void Bitmap::DrawXMark(int x, int y, int s, const ALLEGRO_COLOR &c) {
   int x1 = x - (s>>1);
   int x2 = x + (s>>1);
   int y1 = y - (s>>1);
   int y2 = y + (s>>1);

   Line(x1, y1, x2, y2, c);
   Line(x1, y1+1, x2-1, y2, c);
   Line(x1+1, y1, x2, y2-1, c);

   Line(x1, y2, x2, y1, c);
   Line(x1, y2-1, x2-1, y1, c);
   Line(x1+1, y2, x2, y1+1, c);
}


////////////////////////////////////////////////////////////////////////////////
// Draws a check mark
void Bitmap::DrawCheckMark(int x, int y, const ALLEGRO_COLOR &c) {
   Line(x, y+2, x+2, y+4, c);
   Line(x, y+3, x+2, y+5, c);
   Line(x, y+4, x+2, y+6, c);
   Line(x+3, y+3, x+6, y, c);
   Line(x+3, y+4, x+6, y+1, c);
   Line(x+3, y+5, x+6, y+2, c);
}


////////////////////////////////////////////////////////////////////////////////
// Draws an arrow in the given direction:
//    0 - left
//    1 - right
//    2 - up
//    3 - down
void Bitmap::DrawArrow(int x, int y, const ALLEGRO_COLOR &c, int dir) {
   switch (dir) {
   case 0:
      Putpixel(x, y+3, c);
      Vline(x+1, y+2, y+4, c);
      Vline(x+2, y+1, y+5, c);
      Vline(x+3, y, y+6, c);
      break;
   case 1:
      Vline(x, y, y+6, c);
      Vline(x+1, y+1, y+5, c);
      Vline(x+2, y+2, y+4, c);
      Putpixel(x+3, y+3, c);
      break;
   case 2:
      Putpixel(x+3, y, c);
      Hline(x+2, y+1, x+4, c);
      Hline(x+1, y+2, x+5, c);
      Hline(x, y+3, x+6, c);
      break;
   case 3:
      Hline(x, y, x+6, c);
      Hline(x+1, y+1, x+5, c);
      Hline(x+2, y+2, x+4, c);
      Putpixel(x+3, y+3, c);
      break;
   };
}


////////////////////////////////////////////////////////////////////////////////
// TiledBlit(...) tiles the source bitmap onto the destination bitmap. It
// does so by cutting the source in 9 smaller pieces as shown bellow.
//       1       2       3
//   +-------+-------+-------+  The corner subbitmaps: (1,1), (1,3), (3,1) and (3,3) get
//   |       |       |       |  mapped directly on the destination bitmap and usually
// 1 | (1,1) | (1,2) | (1,3) |  occupy most of the source bitmap.
//   |       |       |       |
//   +-------+-------+-------+  The parts in column 2 are 'thick_x' pixels wide and the parts
// 2 | (2,1) | (2,2) | (2,3) |  in row 2 are 'thick_y' pixels high. Those parts get tiled
//   +-------+-------+-------+  onto the destination bitmap horizontally or vertically.
//   |       |       |       |
// 3 | (3,1) | (3,2) | (3,3) |  The centre tile is tiled across the rest of the destination bitmap.
//   |       |       |       |
//   +-------+-------+-------+
// The source bitmap can comprise several subbitmaps, which all heva to be of equal dimensions.
// Only one of those subbitmaps is then used for tiling.
// Parameters passed:
//  &dest - the destination bitmap
//  dest_x|y - the starting point in the destination bitmap (def = 0)
//  blit_w|h - the size of the rectangle on destination rectangle (def = dest->w|h)
//  offset_x|y - the horizontal|vertical offset of the subbitmap that is used (def = 0)
//  count_x|y - the number of subbitmaps in the source bitmap (def = 0)
//  thick_x|y - the thickness of the second column|row in the subbitmap (def = 0)
// Important:
//  Because the source bitmap is divided into (count_x * count_y) subbitmaps its
//  width has to be divisable with count_x and the height has to be divisable with
//  count_y (e.g. the standard button bitmap has 1 column of 4 subbitmaps which
//  means that its width is not important while the height has to be a multiple
//  of 4, like 60, 72, 80, etc.). Furthermore, if the thickness of the middle
//  row and column is not specified then 1/3 of the source subbitmap is used
//  (i.e. the subbitmap is divided into 9 equal parts). This means that the source
//  bitmap's height and width have to be divisable with 3*count_x and 3*count_y
//  respectively (e.g. the button bitmap has to have the width that is a multiple
//  of 3 and the height has to be a multiple of 12, like 72, 96, etc.).
//  If this is not true the results may be unexpected.

void blitRow(Bitmap& src, Bitmap& dest, int sx, int sy, int dx, int dy, int sw, int sh, int tw, int destw)
{
	al_draw_bitmap_region(src, sx, sy, sw, sh, dx, dy, 0);

   int x2 = destw - sw;
   int mbw = tw;
   for (int cx = sw; cx < x2; cx += tw)
   {
      // check width of piece we're blitting
      if(cx+tw > x2)
         mbw -= (cx+tw) - x2;

      al_draw_bitmap_region(src, sx + sw, sy, mbw, sh, dx + cx, dy, 0);
   }

   al_draw_bitmap_region(src, sx + sw + tw, sy, sw, sh, dx + destw - sw, dy, 0);
}


// we could easily merge this function with the above one but its complex enough already
void blitCol(Bitmap& src, Bitmap& dest, int sx, int sy, int dx, int dy, int sw, int sh, int th, int desth)
{
   src.Blit(dest, sx, sy, dx, dy, sw, sh);

   int y2 = desth - sh;
   int mbh = th;
   for (int cy = sh; cy < y2; cy += th)
   {
      // check height of piece we're blitting
      if(cy+th > y2)
         mbh -= (cy+th) - y2;

      src.Blit(dest, sx, sy + sh, dx, dy + cy, sw, mbh);
   }

   src.Blit(dest, sx, sy + sh + th, dx, dy + desth - sh, sw, sh);
}


void Bitmap::TiledBlit(
   Bitmap &dst,
   int sectionx, int sectiony,
   int destx, int desty,
   int destw, int desth,
   int countx, int county)
{
	_MASKING_BITMAP_STORE_AND_SET_ALL(dst)
	al_hold_bitmap_drawing(true);
   int srcw = w()/countx;
   int srch = h()/county;
   int srcx = sectionx*srcw;
   int srcy = sectiony*srch;
   int tw = (thick.x() != -1) ? thick.x() : srcw/3;
   int th = (thick.y() != -1) ? thick.y() : srch/3;
   int sw = (srcw - tw)/2;
   int sh = (srch - th)/2;

   // first row
   blitRow(*this, dst, srcx, srcy, destx, desty, sw, sh, tw, destw);

   // centre rows
   int y2 = desth - sh;
   int mbh = th;
   for (int y = sh; y < y2; y += th)
   {
      // check width of piece we're blitting
      if(y+th > y2)
         mbh -= (y+th) - y2;

      blitRow(*this, dst, srcx, srcy + sh, destx, desty + y, sw, mbh, tw, destw);
   }

   blitRow(*this, dst, srcx, srcy + sh + th, destx, desty + desth - sh, sw, sh, tw, destw);
	al_hold_bitmap_drawing(false);
	_MASKING_BITMAP_RESTORE
}


void Bitmap::HorizontalTile(Bitmap &dst, int section, int destx, int desty, int destw, int count) {
   int srcw = w();
   int srch = h()/count;
   int srcx = 0;
   int srcy = section*srch;
   int tw = (thick.x() != -1) ? thick.x() : srcw/3;
   int sw = (srcw - tw)/2;
   blitRow(*this, dst, srcx, srcy, destx, desty, sw, srch, tw, destw);
}


void Bitmap::VerticalTile(Bitmap &dst, int section, int destx, int desty, int desth, int count) {
   int srcw = w()/count;
   int srch = h();
   int srcx = section*srcw;
   int srcy = 0;
   int th = (thick.y() != -1) ? thick.y() : srch/3;
   int sh = (srch - th)/2;
   blitCol(*this, dst, srcx, srcy, destx, desty, srcw, sh, th, desth);
}

Bitmap Bitmap::null;

int MASSettings::screenHeight;
int MASSettings::screenWidth;
bool MASSettings::fullscreen;
int MASSettings::gfxMode;
int MASSettings::colorDepth;
int MASSettings::soundVolume;
bool MASSettings::useVideoMemory;
void MASSettings::Save() { /* TODO */ }
