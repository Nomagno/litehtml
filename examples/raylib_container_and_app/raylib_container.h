#ifndef LH_RAYLIB_CONTAINER_H
#define LH_RAYLIB_CONTAINER_H

#include <unordered_map>
#include <memory>
#include <functional>
#include <raylib.h>
#include <litehtml/litehtml.h>

// call back interface to draw text, images and other elements
class raylib_container : public litehtml::document_container
{
private:
    std::string base_url = "";
    int width;
    int height;
    litehtml::position clipping;
    std::unordered_map<std::string, Texture> image_cache;
public:
    raylib_container(int width, int height);
    ~raylib_container() = default;


    uintptr_t	create_font(const char* faceName, int size, int weight, litehtml::font_style italic, unsigned int decoration, litehtml::font_metrics* fm) override;
    void				delete_font(uintptr_t hFont) override;
    int					text_width(const char* text, uintptr_t hFont) override;
    void				draw_text(uintptr_t hdc, const char* text, uintptr_t hFont, litehtml::web_color color, const litehtml::position& pos) override;
    int					pt_to_px(int pt) const override;
    int					get_default_font_size() const override;
    const char*			get_default_font_name() const override;
    void				draw_list_marker(uintptr_t hdc, const litehtml::list_marker& marker) override;
    void				load_image(const char* src, const char* baseurl, bool redraw_on_ready) override;
    void				get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override;
    // Note: regular <img> images are also drawn with draw_background
    // bg is guaranteed to have at least one item.
    // backgrounds in bg are in CSS order - the last one is the farthest from the user.
    // only the last background has valid background-color.
    void				draw_background(uintptr_t hdc, const std::vector<litehtml::background_paint>& bg) override;
    void				draw_borders(uintptr_t hdc, const litehtml::borders& borders, const litehtml::position& draw_pos, bool root) override;

    void				set_caption(const char* caption) override;
    void				set_base_url(const char* base_url) override;
    void				link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el) override;
    void				on_anchor_click(const char* url, const litehtml::element::ptr& el) override;
    void				set_cursor(const char* cursor) override;
    void				transform_text(litehtml::string& text, litehtml::text_transform tt) override;
    void				import_file(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl) override;
    void				set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius) override;
    void				del_clip() override;
    void				get_client_rect(litehtml::position& client) const override;
    litehtml::element::ptr	create_element( const char* tag_name,
    										const std::map<std::string, std::string>& attributes,
    										const std::shared_ptr<litehtml::document>& doc) override;

    void				get_media_features(litehtml::media_features& media) const override;
    void				get_language(litehtml::string& language, litehtml::string& culture) const override;
    litehtml::string	resolve_color(const litehtml::string& /*color*/) const { return litehtml::string(); }
};

#endif  // LH_RAYLIB_CONTAINER_H
