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
        uint_ptr        font;
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

litehtml::uint_ptr raylib_container::create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) {
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
    
    return (litehtml::uint_ptr)f;
}

void raylib_container::delete_font(litehtml::uint_ptr hFont) {
    delete (FontInfo *)hFont;
}

int raylib_container::text_width(const char* text, litehtml::uint_ptr hFont) {
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

void raylib_container::draw_text(litehtml::uint_ptr hdc, const char* text, litehtml::uint_ptr hFont, litehtml::web_color color, const litehtml::position& pos) {
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

void raylib_container::draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) {
    if (marker.pos.x < clipping.x)
        return;
    if (marker.pos.y < clipping.y)
        return;
    if (marker.pos.x >  clipping.x + clipping.width)
        return;
    if (marker.pos.y >  clipping.y + clipping.height)
        return;

    draw_text(hdc, "o", (litehtml::uint_ptr)DEFAULT_FONT, marker.color, marker.pos);
}

void raylib_container::load_image(const char* src, const char* baseurl, bool redraw_on_ready) {
    // This is just for caching the image if you want, not mandatory to implement
    return;
}

void raylib_container::get_image_size(const char* src, const char* baseurl, litehtml::size& sz) {
    // TODO: implement
    sz.width = 10;
    sz.height = 10;
}

// Note: regular <img> images are also drawn with draw_background
// bg is guaranteed to have at least one item.
// backgrounds in bg are in CSS order - the last one is the farthest from the user.
// only the last background has valid background-color.
void raylib_container::draw_background(litehtml::uint_ptr hdc, const std::vector<litehtml::background_paint>& bg) {
    // TODO: implement
    // TODO: apply clipping
}

void raylib_container::draw_borders(litehtml::uint_ptr hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) {
    // TODO: implement
    // TODO: apply clipping
}

void raylib_container::set_caption(const char* caption) {
    SetWindowTitle(caption);
}

void raylib_container::set_base_url(const char* base_url) {
    base_url = base_url;
}

void raylib_container::link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) {
    // TODO: what to do here?
    // For now ignoring it seems wise, since link attributes are mostly for website-specific stuff
    // and liteHTML already automatically processes CSS stylesheet links.
    return;
}

void raylib_container::on_anchor_click(const char* url, const litehtml::element::ptr& el) {
    std::string tmpurl = base_url;
    if (tmpurl != "")
        tmpurl += "/";
    tmpurl += url;
    
    printf("Clicked <%s>: %s\n", el->get_tagName(), tmpurl.c_str());
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

void raylib_container::import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) {
    baseurl = this->base_url;


    std::string tmpurl = base_url;
    if (tmpurl != "")
        tmpurl += "/";
    tmpurl += url;

    char *buffer = 0;
    long length;

    FILE *f = fopen(tmpurl.c_str(), "rb");

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
										const litehtml::string_map& attributes,
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
