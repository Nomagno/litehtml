#ifndef LH_WEB_COLOR_H
#define LH_WEB_COLOR_H

namespace litehtml
{
	struct def_color
	{
		const char*	name;
		const char*	rgb;
	};

	extern def_color g_def_colors[];

	class document_container;

	struct web_color
	{
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alpha;

		static const web_color transparent;
		static const web_color black;
		static const web_color white;

		web_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		{
			red		= r;
			green	= g;
			blue	= b;
			alpha	= a;
		}

		web_color()
		{
			red		= 0;
			green	= 0;
			blue	= 0;
			alpha	= 0xFF;
		}

		bool operator==(web_color color) const { return red == color.red && green == color.green && blue == color.blue && alpha == color.alpha; }
		bool operator!=(web_color color) const { return !(*this == color); }

		string to_string() const;
		static web_color	from_string(const string& str, document_container* callback);
		static string		resolve_name(const string& name, document_container* callback);
		static bool			is_color(const string& str, document_container* callback);
	};
}

#endif  // LH_WEB_COLOR_H
