#include "raylib_container.h"
#include <raylib.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdio>

/*
    struct list_marker
    {
        string            image;
        const char*        baseurl;
        list_style_type    marker_type;
        web_color        color;
        position        pos;
        int                index;
        uintptr_t        font;
    };
*/

raylib_container::raylib_container(int width, int height) {
    this->width = width;
    this->height = height;

    clipping.x = 0;
    clipping.y = 0;
    clipping.width = width;
    clipping.height = height;
}

std::string makeurl(const char *base, const char *url) {
    if (base == NULL)
        base = "";
    if (url == NULL)
        url = "";

    std::string tmpurl = base;
    if (tmpurl == "")
        tmpurl += "./";
    else
        tmpurl += "/";
    tmpurl += url;
    return tmpurl;
}

struct FontInfo {
    unsigned size;
    unsigned weight;
    unsigned spacing;
    litehtml::font_style style;
    Font ray_font;
};

FontInfo default_font;

#define DEFAULT_FONT_SIZE 15
#define DEFAULT_FONT_NAME "Bobi"

void init_default_font() {
    if (default_font.size == 0) {
        FontInfo *f = new FontInfo;
        f->size = DEFAULT_FONT_SIZE;
        f->weight = 300;
        f->style = litehtml::font_style_normal;
        f->ray_font = GetFontDefault();
        f->spacing = 3; // TODO: IDK what the spacing should be
    }
}

#define RAYFONT(x) (((void *)x == (void *)0) ? (init_default_font(), &default_font) : ((FontInfo *)x))
#define DEFAULT_FONT RAYFONT(0)

uintptr_t raylib_container::create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) {
    init_default_font();

    FontInfo *f = new FontInfo;
    f->size = size;
    f->weight = size;
    f->style = italic;
    f->ray_font = GetFontDefault();
    f->spacing = 3; // TODO: IDK what the spacing should be

    Vector2 randomsize = MeasureTextEx(f->ray_font, "||SOME TEXT||", size, f->spacing);
    Vector2 xsize = MeasureTextEx(f->ray_font, "x", size, f->spacing);
    
    fm->ascent = randomsize.y*3/4;
    fm->descent = randomsize.y/4;
    fm->height = randomsize.y;
    fm->x_height = xsize.y;
    
    return (uintptr_t)f;
}

void raylib_container::delete_font(uintptr_t hFont) {
    delete (FontInfo *)hFont;
}

int raylib_container::text_width(const char* text, uintptr_t hFont) {
    if (text == NULL)
        return 0;
    if (text[0] == ' ' && text[1] == '\0') {
        Vector2 sizefull = MeasureTextEx(RAYFONT(hFont)->ray_font, "x x", RAYFONT(hFont)->size, RAYFONT(hFont)->spacing);
        Vector2 sizex = MeasureTextEx(RAYFONT(hFont)->ray_font, "x", RAYFONT(hFont)->size, RAYFONT(hFont)->spacing);
        return sizefull.x-sizex.x;
    }
    
    Vector2 size = MeasureTextEx(RAYFONT(hFont)->ray_font, text, RAYFONT(hFont)->size, RAYFONT(hFont)->spacing);
    return size.x;
}

void raylib_container::draw_text(uintptr_t hdc, const char* text, uintptr_t hFont, litehtml::web_color color, const litehtml::position& pos) {
    Color ray_color = {
        .r = color.red,
        .g = color.green,
        .b = color.blue,
        .a = color.alpha,
    };

    // TODO: apply clipping
    DrawTextEx(RAYFONT(hFont)->ray_font, text, (Vector2){(float)pos.x, (float)pos.y} , RAYFONT(hFont)->size, RAYFONT(hFont)->spacing, ray_color);
}

int raylib_container::pt_to_px(int pt) const {
    return pt*2;
}

int raylib_container::get_default_font_size() const {
    return DEFAULT_FONT_SIZE;
}

const char* raylib_container::get_default_font_name() const {
    return DEFAULT_FONT_NAME;
}

void raylib_container::draw_list_marker(uintptr_t hdc, const litehtml::list_marker& marker) {
    if (marker.pos.x < clipping.x)
        return;
    if (marker.pos.y < clipping.y)
        return;
    if (marker.pos.x > clipping.x + clipping.width)
        return;
    if (marker.pos.y > clipping.y + clipping.height)
        return;

    draw_text(hdc, "o", (uintptr_t)DEFAULT_FONT, marker.color, marker.pos);
}

void raylib_container::load_image(const char* src, const char* baseurl, bool redraw_on_ready) {
    // This is just for caching the image if you want, not mandatory to implement
    if (image_cache.find(makeurl(baseurl, src)) == image_cache.end()) {
        image_cache[makeurl(baseurl, src)] = LoadImage(makeurl(baseurl, src).c_str());
    }
}

void raylib_container::get_image_size(const char* src, const char* baseurl, litehtml::size& sz) {
    Image im = image_cache[makeurl(baseurl, src)];
    sz.width = im.width;
    sz.width = im.height;
}

void print_background_paint(const litehtml::background_paint &p) {
    printf(
    "url:%s\n"
    "baseurl:%s\n"
    "is_root:%d\n"
    "x,y:%d,%d\n"
    "w,h:%d,%d\n"
    "attachment:%d\n"
    "repeat:%d\n"
    "color:%d,%d,%d,%d\n"
    "clip:%d,%d; %d,%d\n"
    "origin:%d,%d; %d,%d\n"
    "border:%d,%d; %d,%d\n"
    "\n",
    p.image.c_str(),
    p.baseurl.c_str(),
    p.is_root,
    p.position_x, p.position_y,
    p.image_size.width, p.image_size.height,
    p.attachment,
    p.repeat,
    p.color.red, p.color.green, p.color.blue, p.color.alpha,
    p.clip_box.x, p.clip_box.y, p.clip_box.width, p.clip_box.height,
    p.origin_box.x, p.origin_box.y, p.origin_box.width, p.origin_box.height,
    p.border_box.x, p.border_box.y, p.border_box.width, p.border_box.height
    );
}

// Note: regular <img> images are also drawn with draw_background
// bg is guaranteed to have at least one item.
// backgrounds in bg are in CSS order - the last one is the farthest from the user.
// only the last background has valid background-color.
void raylib_container::draw_background(uintptr_t hdc, const std::vector<litehtml::background_paint>& bg) {
    //for (const litehtml::background_paint &paint : bg) {
    for_each (bg.rbegin(), bg.rend(), [=](const litehtml::background_paint &paint) {
        //print_background_paint(paint);

        std::string imagepath = "";
        if (paint.image != "")
            imagepath = makeurl((paint.baseurl == "") ? base_url.c_str() : paint.baseurl.c_str(), paint.image.c_str());

        Color ray_color = {
            .r = paint.color.red,
            .g = paint.color.green,
            .b = paint.color.blue,
            .a = paint.color.alpha,
        };

        // TODO: apply clipping
        if (imagepath == "") {
            // solid background color
            unsigned x, y, w, h;
            if (paint.is_root) {
                x = 0;
                y = 0;
                w = width;
                h = height;
            } else {
                x = paint.position_x;
                y = paint.position_y;
                w = paint.image_size.width;
                h = paint.image_size.height;
            }

            Image im = GenImageColor(w, h, ray_color);
            Texture2D tex = LoadTextureFromImage(im);

            DrawTexture(tex, x, y, ray_color);

            UnloadTexture(tex);
            UnloadImage(im);
        } else {
            Image im = image_cache[imagepath];
            Texture2D tex = LoadTextureFromImage(im);

            int saved_w = tex.width;
            int saved_h = tex.height;

            if (paint.image_size.width < tex.width)
                tex.width = paint.image_size.width;
            if (paint.image_size.height < tex.height)
                tex.height = paint.image_size.height;

            DrawTexture(tex, paint.position_x, paint.position_y, ray_color);

            tex.width = saved_w;
            tex.height = saved_h;

            UnloadTexture(tex);
        }


    });
    // }
}

void raylib_container::draw_borders(uintptr_t hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) {
	/*enum border_style
	{
		border_style_none,
		border_style_hidden,
		border_style_dotted,
		border_style_dashed,
		border_style_solid,
		border_style_double,
		border_style_groove,
		border_style_ridge,
		border_style_inset,
		border_style_outset
	}*/

	/*struct borders
	{
		border			left;
		border			top;
		border			right;
		border			bottom;
		border_radiuses	radius;
	}*/

    Color ray_color = {
        .r = borders.left.color.red,
        .g = borders.left.color.green,
        .b = borders.left.color.blue,
        .a = borders.left.color.alpha,
    };

    Rectangle rect = (Rectangle){
        .x = (float)draw_pos.x,
        .y = (float)draw_pos.y,
        .width = (float)draw_pos.width,
        .height = (float)draw_pos.height
    };

    DrawRectangleLinesEx(rect, borders.left.width, ray_color); 
    
    // TODO: apply clipping
}

void raylib_container::set_caption(const char* caption) {
    SetWindowTitle(caption);
}

void raylib_container::set_base_url(const char* baseurl) {
    base_url = baseurl;
}

void raylib_container::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {
    // TODO: what to do here?
    // For now ignoring it seems wise, since link attributes are mostly for website-specific stuff
    // and liteHTML already automatically processes CSS stylesheet links.
    return;
}

void raylib_container::on_anchor_click(const char* url, const litehtml::element::ptr& el) {
    printf("Clicked <%s>: %s\n", el->get_tagName(), makeurl(base_url.c_str(), url).c_str());
    return;
}

void raylib_container::set_cursor(const char* cursor) {
    // This is for mouse styling, ignore
    return;
}

std::string str_toupper(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::toupper(c); }
                  );
    return s;
}
std::string str_tolower(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); }
                  );
    return s;
}

std::string str_toupper_first(std::string s)
{
    if (s.size() > 0)
        s[0] = std::toupper(s[0]);
    return s;
}

void raylib_container::transform_text(litehtml::string& text, litehtml::text_transform tt) {
    switch (tt) {
    case litehtml::text_transform_none:
        return;
    case litehtml::text_transform_capitalize:
        text = str_toupper_first(text);
        return;
    case litehtml::text_transform_uppercase:
        text = str_toupper(text);
        return;
    case litehtml::text_transform_lowercase:
        text = str_tolower(text);
        return;
    }
}

void raylib_container::import_file(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) {
    // TODO: There is no documentation indicating that this has to be done
    baseurl = makeurl(baseurl.c_str(), url.c_str());

    char *buffer = 0;
    long length;

    FILE *f = fopen(makeurl(base_url.c_str(), url.c_str()).c_str(), "rb");

    if (f)
    {
      fseek(f, 0, SEEK_END);
      length = ftell(f);
      fseek(f, 0, SEEK_SET);
      buffer = (char *)malloc(length+1);
      if (buffer)
      {
        fread (buffer, 1, length, f);
      }
      fclose (f);
    }

    if (buffer)
    {
        buffer[length] = '\0';
        text = std::string(buffer);
    }
}

void raylib_container::set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius /*corner radius for rounded rectangles*/) {
    clipping.x = pos.x;
    clipping.y = pos.y;
    clipping.width = pos.width;
    clipping.height = pos.height;
}

void raylib_container::del_clip() {
    clipping.x = 0;
    clipping.y = 0;
    clipping.width = this->width;
    clipping.height = this->height;
}

void raylib_container::get_client_rect(litehtml::position& client) const {
    client.x = 0;
    client.y = 0;
    client.width = this->width;
    client.height = this->height;
}
litehtml::element::ptr raylib_container::create_element( const char* tag_name,
										const std::map<std::string, std::string>& attributes,
										const std::shared_ptr<litehtml::document>& doc) {
    // You can process custom tags here if you want
    return NULL;
}

void raylib_container::get_media_features(litehtml::media_features& media) const {
    media.type = litehtml::media_type_screen;
}
void raylib_container::get_language(litehtml::string& language, litehtml::string& culture) const {
    language = "EN";
    culture = "GB";
}
