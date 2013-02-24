/****************************************************************************************/
/*											*/
/* This program is free software; you can redistribute it and/or modify it under the	*/
/* terms of the GNU General Public License as published by the Free Software		*/
/* Foundation; either version 2 of the License, or (at your option) any later version.	*/
/*											*/
/* This program is distributed in the hope that it will be useful, but WITHOUT ANY	*/
/* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A	*/
/* PARTICULAR PURPOSE. See the GNU General Public License for more details.		*/
/*											*/
/* You should have received a copy of the GNU General Public License along with this	*/
/* program; (See "COPYING"). If not, If not, see <http://www.gnu.org/licenses/>.        */
/*											*/
/*--------------------------------------------------------------------------------------*/
/*											*/
/*  Copyright   Joerg Anders, TU Chemnitz, Fakultaet fuer Informatik, GERMANY           */
/*		ja@informatik.tu-chemnitz.de						*/
/*											*/
/*											*/
/****************************************************************************************/

#include "pangocairotext.h"
#include "resource.h"

#define TEXT_CURSOR_WIDTH 0.04
#define TEXT_CURSOR_HEIGHT 0.3

#define MAX_TEXT_LENGTH 10

#define PANGO_INTERNAL_FONT_FACTOR 1024.0

#define INTERNAL_Y_OFFSET -0.2
#define SPEC_LETTERS_SCALE 600.0

#define CODA_X_OFFSET 0.05
#define CODA_Y_OFFSET 0.31

#define ZERO_X_OFFSET 0.12
#define ZERO_Y_OFFSET 0.29

#define SEGNO_X_OFFSET 0.20
#define SEGNO_Y_OFFSET 0.30

#define SIGN_X_OFFSET 0.04
#define SIGN_Y_OFFSET 0.2

#define FLAT_X_OFFSET 0.0
#define FLAT_Y_OFFSET 0.3

#define NEUTRAL_X_OFFSET 0.01
#define NEUTRAL_Y_OFFSET 0.3

#define HIDDEN_HYPHEN_X_OFFSET 0.01
#define HODDEN_HYPHEN_Y_OFFSET 0.3

struct NedPangoCairoText::utf8_element NedPangoCairoText::utf8code_array[] = {
{".", {    0,    0,  0xc2,   0xa0}},     //		NO-BREAK SPACE                     
{NULL,  {    0,    0,  0xc2,   0xa1}},	//		INVERTED EXCLAMATION MARK	¡
{NULL,  {    0,    0,  0xc2,   0xa2}},	//		CENT SIGN	¢
{NULL,  {    0,    0,  0xc2,   0xa3}},	//		POUND SIGN	£
{NULL,  {    0,    0,  0xc2,   0xa4}},	//		CURRENCY SIGN	¤
{NULL,  {    0,    0,  0xc2,   0xa5}},	//		YEN SIGN	¥
{NULL,  {    0,    0,  0xc2,   0xa6}},	//		BROKEN BAR	¦
{NULL,  {    0,    0,  0xc2,   0xa7}},	//		SECTION SIGN	§
{NULL,  {    0,    0,  0xc2,   0xa8}},	//		DIAERESIS	¨
{"c", {    0,    0,  0xc2,   0xa9}},	//		COPYRIGHT SIGN	©
{NULL,  {    0,    0,  0xc2,   0xaa}},	//		FEMININE ORDINAL INDICATOR	ª
{NULL,  {    0,    0,  0xc2,   0xab}},	//		LEFT-POINTING DOUBLE ANGLE QUOTATION MARK	«
{NULL,  {    0,    0,  0xc2,   0xac}},	//		NOT SIGN	¬
{NULL,  {    0,    0,  0xc2,   0xad}},	//		SOFT HYPHEN	­
{"r",  {    0,    0,  0xc2,   0xae}},	//		REGISTERED SIGN	®
{NULL,  {    0,    0,  0xc2,   0xaf}},	//		MACRON	¯
{NULL,  {    0,    0,  0xc2,   0xb0}},	//		DEGREE SIGN	°
{NULL,  {    0,    0,  0xc2,   0xb1}},	//		PLUS-MINUS SIGN	±
{NULL,  {    0,    0,  0xc2,   0xb2}},	//		SUPERSCRIPT TWO	²
{NULL,  {    0,    0,  0xc2,   0xb3}},	//		SUPERSCRIPT THREE	³
{NULL,  {    0,    0,  0xc2,   0xb4}},	//		ACUTE ACCENT	´
{NULL,  {    0,    0,  0xc2,   0xb5}},	//		MICRO SIGN	µ
{NULL,  {    0,    0,  0xc2,   0xb6}},	//		PILCROW SIGN	¶
{NULL,  {    0,    0,  0xc2,   0xb7}},	//		MIDDLE DOT	·
{NULL,  {    0,    0,  0xc2,   0xb8}},	//		CEDILLA	¸
{NULL,  {    0,    0,  0xc2,   0xb9}},	//		SUPERSCRIPT ONE	¹
{NULL,  {    0,    0,  0xc2,   0xba}},	//		MASCULINE ORDINAL INDICATOR	º
{NULL,  {    0,    0,  0xc2,   0xbb}},	//		RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK	»
{NULL,  {    0,    0,  0xc2,   0xbc}},	//		VULGAR FRACTION ONE QUARTER	¼
{NULL,  {    0,    0,  0xc2,   0xbd}},	//		VULGAR FRACTION ONE HALF	½
{NULL,  {    0,    0,  0xc2,   0xbe}},	//		VULGAR FRACTION THREE QUARTERS	¾
{NULL,  {    0,    0,  0xc2,   0xbf}},	//		INVERTED QUESTION MARK	¿
{NULL,  {    0,    0,  0xc3,   0x80}},	//		LATIN CAPITAL LETTER A WITH GRAVE	À
{NULL,  {    0,    0,  0xc3,   0x81}},	//		LATIN CAPITAL LETTER A WITH ACUTE	Á
{NULL,  {    0,    0,  0xc3,   0x82}},	//		LATIN CAPITAL LETTER A WITH CIRCUMFLEX	Â
{NULL,  {    0,    0,  0xc3,   0x83}},	//		LATIN CAPITAL LETTER A WITH TILDE	Ã
{NULL,  {    0,    0,  0xc3,   0x84}},	//		LATIN CAPITAL LETTER A WITH DIAERESIS	Ä
{NULL,  {    0,    0,  0xc3,   0x85}},	//		LATIN CAPITAL LETTER A WITH RING ABOVE	Å
{NULL,  {    0,    0,  0xc3,   0x86}},	//		LATIN CAPITAL LETTER AE	Æ
{NULL,  {    0,    0,  0xc3,   0x87}},	//		LATIN CAPITAL LETTER C WITH CEDILLA	Ç
{NULL,  {    0,    0,  0xc3,   0x88}},	//		LATIN CAPITAL LETTER E WITH GRAVE	È
{NULL,  {    0,    0,  0xc3,   0x89}},	//		LATIN CAPITAL LETTER E WITH ACUTE	É
{NULL,  {    0,    0,  0xc3,   0x8a}},	//		LATIN CAPITAL LETTER E WITH CIRCUMFLEX	Ê
{NULL,  {    0,    0,  0xc3,   0x8b}},	//		LATIN CAPITAL LETTER E WITH DIAERESIS	Ë
{NULL,  {    0,    0,  0xc3,   0x8c}},	//		LATIN CAPITAL LETTER I WITH GRAVE	Ì
{NULL,  {    0,    0,  0xc3,   0x8d}},	//		LATIN CAPITAL LETTER I WITH ACUTE	Í
{NULL,  {    0,    0,  0xc3,   0x8e}},	//		LATIN CAPITAL LETTER I WITH CIRCUMFLEX	Î
{NULL,  {    0,    0,  0xc3,   0x8f}},	//		LATIN CAPITAL LETTER I WITH DIAERESIS	Ï
{NULL,  {    0,    0,  0xc3,   0x90}},	//		LATIN CAPITAL LETTER ETH	Ð
{NULL,  {    0,    0,  0xc3,   0x91}},	//		LATIN CAPITAL LETTER N WITH TILDE	Ñ
{NULL,  {    0,    0,  0xc3,   0x92}},	//		LATIN CAPITAL LETTER O WITH GRAVE	Ò
{NULL,  {    0,    0,  0xc3,   0x93}},	//		LATIN CAPITAL LETTER O WITH ACUTE	Ó
{NULL,  {    0,    0,  0xc3,   0x94}},	//		LATIN CAPITAL LETTER O WITH CIRCUMFLEX	Ô
{NULL,  {    0,    0,  0xc3,   0x95}},	//		LATIN CAPITAL LETTER O WITH TILDE	Õ
{NULL,  {    0,    0,  0xc3,   0x96}},	//		LATIN CAPITAL LETTER O WITH DIAERESIS	Ö
{NULL,  {    0,    0,  0xc3,   0x97}},	//		MULTIPLICATION SIGN	×
{NULL,  {    0,    0,  0xc3,   0x98}},	//		LATIN CAPITAL LETTER O WITH STROKE	Ø
{NULL,  {    0,    0,  0xc3,   0x99}},	//		LATIN CAPITAL LETTER U WITH GRAVE	Ù
{NULL,  {    0,    0,  0xc3,   0x9a}},	//		LATIN CAPITAL LETTER U WITH ACUTE	Ú
{NULL,  {    0,    0,  0xc3,   0x9b}},	//		LATIN CAPITAL LETTER U WITH CIRCUMFLEX	Û
{NULL,  {    0,    0,  0xc3,   0x9c}},	//		LATIN CAPITAL LETTER U WITH DIAERESIS	Ü
{NULL,  {    0,    0,  0xc3,   0x9d}},	//		LATIN CAPITAL LETTER Y WITH ACUTE	Ý
{NULL,  {    0,    0,  0xc3,   0x9e}},	//		LATIN CAPITAL LETTER THORN	Þ
{NULL,  {    0,    0,  0xc3,   0x9f}},	//		LATIN SMALL LETTER SHARP S	ß
{NULL,  {    0,    0,  0xc3,   0xa0}},	//		LATIN SMALL LETTER A WITH GRAVE	à
{NULL,  {    0,    0,  0xc3,   0xa1}},	//		LATIN SMALL LETTER A WITH ACUTE	á
{NULL,  {    0,    0,  0xc3,   0xa2}},	//		LATIN SMALL LETTER A WITH CIRCUMFLEX	â
{NULL,  {    0,    0,  0xc3,   0xa3}},	//		LATIN SMALL LETTER A WITH TILDE	ã
{NULL,  {    0,    0,  0xc3,   0xa4}},	//		LATIN SMALL LETTER A WITH DIAERESIS	ä
{NULL,  {    0,    0,  0xc3,   0xa5}},	//		LATIN SMALL LETTER A WITH RING ABOVE	å
{NULL,  {    0,    0,  0xc3,   0xa6}},	//		LATIN SMALL LETTER AE	æ
{NULL,  {    0,    0,  0xc3,   0xa7}},	//		LATIN SMALL LETTER C WITH CEDILLA	ç
{NULL,  {    0,    0,  0xc3,   0xa8}},	//		LATIN SMALL LETTER E WITH GRAVE	è
{NULL,  {    0,    0,  0xc3,   0xa9}},	//		LATIN SMALL LETTER E WITH ACUTE	é
{NULL,  {    0,    0,  0xc3,   0xaa}},	//		LATIN SMALL LETTER E WITH CIRCUMFLEX	ê
{NULL,  {    0,    0,  0xc3,   0xab}},	//		LATIN SMALL LETTER E WITH DIAERESIS	ë
{NULL,  {    0,    0,  0xc3,   0xac}},	//		LATIN SMALL LETTER I WITH GRAVE	ì
{NULL,  {    0,    0,  0xc3,   0xad}},	//		LATIN SMALL LETTER I WITH ACUTE	í
{NULL,  {    0,    0,  0xc3,   0xae}},	//		LATIN SMALL LETTER I WITH CIRCUMFLEX	î
{NULL,  {    0,    0,  0xc3,   0xaf}},	//		LATIN SMALL LETTER I WITH DIAERESIS	ï
{NULL,  {    0,    0,  0xc3,   0xb0}},	//		LATIN SMALL LETTER ETH	ð
{NULL,  {    0,    0,  0xc3,   0xb1}},	//		LATIN SMALL LETTER N WITH TILDE	ñ
{NULL,  {    0,    0,  0xc3,   0xb2}},	//		LATIN SMALL LETTER O WITH GRAVE	ò
{NULL,  {    0,    0,  0xc3,   0xb3}},	//		LATIN SMALL LETTER O WITH ACUTE	ó
{NULL,  {    0,    0,  0xc3,   0xb4}},	//		LATIN SMALL LETTER O WITH CIRCUMFLEX	ô
{NULL,  {    0,    0,  0xc3,   0xb5}},	//		LATIN SMALL LETTER O WITH TILDE	õ
{NULL,  {    0,    0,  0xc3,   0xb6}},	//		LATIN SMALL LETTER O WITH DIAERESIS	ö
{NULL,  {    0,    0,  0xc3,   0xb7}},	//		DIVISION SIGN	÷
{NULL,  {    0,    0,  0xc3,   0xb8}},	//		LATIN SMALL LETTER O WITH STROKE	ø
{NULL,  {    0,    0,  0xc3,   0xb9}},	//		LATIN SMALL LETTER U WITH GRAVE	ù
{NULL,  {    0,    0,  0xc3,   0xba}},	//		LATIN SMALL LETTER U WITH ACUTE	ú
{NULL,  {    0,    0,  0xc3,   0xbb}},	//		LATIN SMALL LETTER U WITH CIRCUMFLEX	û
{NULL,  {    0,    0,  0xc3,   0xbc}},	//		LATIN SMALL LETTER U WITH DIAERESIS	ü
{NULL,  {    0,    0,  0xc3,   0xbd}},	//		LATIN SMALL LETTER Y WITH ACUTE	ý
{NULL,  {    0,    0,  0xc3,   0xbe}},	//		LATIN SMALL LETTER THORN	þ
{NULL,  {    0,    0,  0xc3,   0xbf}},	//		LATIN SMALL LETTER Y WITH DIAERESIS	ÿ
{"p", {    0,    0xe2, 0x84, 0x97}},     //              SOUND RECORDING COPYRIGHT  ℗      
{"b", {    0,    0xe2, 0x99, 0xad}},     //              MUSIC FLAT SIGN            ♭      
{"n", {    0,    0xe2, 0x99, 0xae}},     //              MUSIC NATURAL SIGN         ♮      
{"#", {    0,    0xe2, 0x99, 0xaf}},     //              MUSIC SHARP SIGN           ♯      
{"--", {   0,    0xe2, 0x80, 0x94}},     //              EM DASH                    —       
{"...", {  0,    0xe2, 0x80, 0xa6}},     //              HORIZONTAL ELLIPSIS        …       
{NULL,  {    0,    0,    0,      0}}
};


NedPangoCairoText::NedPangoCairoText(GdkDrawable *d, const char *text, const char* font_family, PangoStyle style, PangoWeight weight, double size, double zoom, double scale, bool hide_trailing_hyphen) :
m_display_layout(NULL), m_measure_layout(NULL), m_drawable(d), m_hidden_hyphen(false), m_text(NULL), m_measure_text(NULL), m_font_family(font_family), m_style(style), m_weight(weight), m_size(size * PANGO_INTERNAL_FONT_FACTOR), m_width(0), m_height(0), m_hide_hyphen(hide_trailing_hyphen),
	m_cursor_idx(0), m_spec_letters(NULL) {
	if (text == NULL || strlen(text) < 1) {
		NedResource::Abort("NedPangoCairoText::NedPangoCairoText");
	}
	m_text = inspect_text(text);
	m_measure_text = strdup(text);
	if (hide_trailing_hyphen) {
		if (m_text[strlen(m_text) - 1] == '-') {
			m_hidden_hyphen = true;
			m_text[strlen(m_text) - 1] = '\0';
		}
	}
	m_drawable = d;
	m_font_family = font_family; 
	m_style = style;
	m_weight = weight;
	cairo_t *cr = gdk_cairo_create (m_drawable);
	createLayout(cr, zoom, scale);
	cairo_destroy (cr);
	m_cursor_idx = g_utf8_strlen(m_text, -1);
	computeCursorPosition();
}
	
NedPangoCairoText::~NedPangoCairoText() {
	GList *lptr;

	if (m_display_layout != NULL) {
		g_object_unref (m_display_layout);
		m_display_layout = NULL;
	}
	if (m_measure_layout != NULL) {
		g_object_unref (m_measure_layout);
		m_measure_layout = NULL;
	}
	if (m_text != NULL) {
		free(m_text);
		m_text = NULL;
	}
	for (lptr = g_list_first(m_spec_letters); lptr; lptr = g_list_next(lptr)) {
		delete (specLetter *) lptr->data;
	}
	g_list_free(m_spec_letters);
}

char *NedPangoCairoText::getText() {
	return m_measure_text;
}

double NedPangoCairoText::getInternalFactor() {
	return PANGO_INTERNAL_FONT_FACTOR;
}

void NedPangoCairoText::adjust(GdkDrawable *d, double zoom, double scale) {
	m_drawable = d;
	cairo_t *cr = gdk_cairo_create (m_drawable);
	createLayout(cr, zoom, scale);
	cairo_destroy (cr);
	computeDimensions();
	computeCursorPosition();
}

NedPangoCairoText *NedPangoCairoText::clone() {
	return new NedPangoCairoText(m_drawable, m_measure_text, m_font_family, m_style, m_weight, m_size / PANGO_INTERNAL_FONT_FACTOR, NedResource::getStartZoomScale(), 1.0, m_hide_hyphen);
}

	

void NedPangoCairoText::setText(char *text, double zoom, bool hide_trailing_hyphen) {
	GList *lptr;
	m_hide_hyphen = hide_trailing_hyphen;
	if (m_text != NULL) {
		free(m_text);
		m_text = NULL;
		free(m_measure_text);
		m_measure_text = NULL;
	}
	for (lptr = g_list_first(m_spec_letters); lptr; lptr = g_list_next(lptr)) {
		delete (specLetter *) lptr->data;
	}
	g_list_free(m_spec_letters);
	m_spec_letters = NULL;
	m_hidden_hyphen = false;
	if (text != NULL && strlen(text) > 0) {
		m_text = inspect_text(text);
		m_measure_text = strdup(text);
		if (hide_trailing_hyphen) {
			if (m_text[strlen(m_text) - 1] == '-') {
				m_hidden_hyphen = true;
				m_text[strlen(m_text) - 1] = '\0';
			}
		}
	}
	if (m_text == NULL) {
		pango_layout_set_text (m_display_layout, " ", -1);
	}
	else if (strlen(m_text) < 1) {
		pango_layout_set_text (m_display_layout, " ", -1);
	}
	else {
		pango_layout_set_text (m_display_layout, m_text, -1);
		pango_layout_set_text (m_measure_layout, m_measure_text, -1);
	}
	computeDimensions();
	computeCursorPosition();
}


void NedPangoCairoText::setZoom(double zoom, double scale) {
	cairo_t *cr = gdk_cairo_create (m_drawable);
	PangoFontDescription *desc;
	desc = pango_font_description_new();
	pango_font_description_set_family(desc, m_font_family);
	pango_font_description_set_style(desc, m_style);
	pango_font_description_set_weight(desc, m_weight);
	pango_font_description_set_absolute_size (desc, (int) (m_size * zoom * scale));
	pango_layout_set_font_description (m_display_layout, desc);
	pango_layout_set_font_description (m_measure_layout, desc);
	pango_cairo_update_layout (cr, m_display_layout);
	pango_cairo_update_layout (cr, m_measure_layout);
	pango_layout_get_size (m_measure_layout, &m_width, &m_height);
	pango_layout_get_size (m_display_layout, &m_internal_width, &m_internal_height);
	computeCursorPosition();
	pango_font_description_free (desc);
	cairo_destroy (cr);
}

	
void NedPangoCairoText::createLayout(cairo_t *cr, double zoom, double scale) {
	PangoFontDescription *desc;

	if (m_display_layout != NULL) {
		g_object_unref (m_display_layout);
		m_display_layout = NULL;
	}
	if (m_measure_layout != NULL) {
		g_object_unref (m_measure_layout);
		m_measure_layout = NULL;
	}
	m_display_layout = pango_cairo_create_layout (cr);
	m_measure_layout = pango_cairo_create_layout (cr);
	if (m_text == NULL) {
		pango_layout_set_text (m_display_layout, " ", -1);
		pango_layout_set_text (m_measure_layout, " ", -1);
	}
	else if (strlen(m_text) < 1) {
		pango_layout_set_text (m_display_layout, " ", -1);
		pango_layout_set_text (m_measure_layout, " ", -1);
	}
	else {
		pango_layout_set_text (m_display_layout, m_text, -1);
		pango_layout_set_text (m_measure_layout, m_measure_text, -1);
	}
	desc = pango_font_description_new();
	pango_font_description_set_family(desc, m_font_family);
	pango_font_description_set_style(desc, m_style);
	pango_font_description_set_weight(desc, m_weight);
	pango_font_description_set_absolute_size (desc, (int) (m_size * zoom * scale));
	pango_layout_set_font_description (m_display_layout, desc);
	pango_layout_set_font_description (m_measure_layout, desc);
	pango_cairo_update_layout (cr, m_display_layout);
	pango_cairo_update_layout (cr, m_measure_layout);
	pango_layout_get_size (m_measure_layout, &m_width, &m_height);
	pango_layout_get_size (m_display_layout, &m_internal_width, &m_internal_height);
	pango_font_description_free (desc);
}

bool NedPangoCairoText::textDiffers(char *other_text) {
	if (other_text == NULL || strlen(other_text) < 1) {
		return !(m_text == NULL || strlen(m_text) < 1);
	}
	if (m_measure_text == NULL || strlen(m_measure_text) < 1) return true;
	return strcmp(m_measure_text, other_text);
}

double NedPangoCairoText::getFontSize() {
	return m_size;
}

void NedPangoCairoText::draw(cairo_t *cr, double xpos, double ypos, double zoom, double scale) {
	GList *lptr;
	specLetter *let;
	cairo_matrix_t m;
	double scale_factor =  m_size / SPEC_LETTERS_SCALE * scale;
	double x_shift, y_shift;
	cairo_new_path(cr);
	cairo_move_to(cr, xpos, ypos + INTERNAL_Y_OFFSET * zoom);
	pango_cairo_show_layout (cr, m_display_layout);
	if (m_spec_letters != NULL) {
		cairo_get_matrix(cr, &m);
		cairo_scale(cr, scale_factor, scale_factor);
		for (lptr = g_list_first(m_spec_letters); lptr; lptr = g_list_next(lptr)) {
			let = (specLetter *) lptr->data;
			y_shift = (double) let->pos.y / PANGO_INTERNAL_FONT_FACTOR + let->yoffs;
			x_shift = xpos + (double) let->pos.x / PANGO_INTERNAL_FONT_FACTOR + let->xoffs * zoom;
			y_shift = ypos + (double) let->pos.y / PANGO_INTERNAL_FONT_FACTOR + (INTERNAL_Y_OFFSET + let->yoffs) * zoom;
			cairo_move_to(cr, x_shift / scale_factor, y_shift / scale_factor);
			cairo_show_text(cr, let->letter);
		}
		cairo_set_matrix(cr, &m);
	}
	cairo_stroke(cr);
}

void NedPangoCairoText::drawCursor(cairo_t *cr, double xpos, double ypos, double zoom_factor) {
	cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	cairo_new_path(cr);
	cairo_rectangle (cr, xpos + (double) m_cursorpos.x / PANGO_INTERNAL_FONT_FACTOR,
		 ypos + m_cursorpos.y / PANGO_INTERNAL_FONT_FACTOR,
	TEXT_CURSOR_WIDTH * zoom_factor, TEXT_CURSOR_HEIGHT * zoom_factor);
	cairo_fill (cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
}

void NedPangoCairoText::computeDimensions() {
	cairo_t *cr = gdk_cairo_create (m_drawable);
	pango_cairo_update_layout (cr, m_display_layout);
	pango_layout_get_size (m_measure_layout, &m_width, &m_height);
	pango_layout_get_size (m_display_layout, &m_internal_width, &m_internal_height);
	cairo_destroy (cr);
}


void NedPangoCairoText::insertAtCursorPosition(const char *text, double zoom) {
	char str1[1024], str2[1024];
	gchar *endp;
	//text = "ע";
	if (text == NULL || strlen(text) < 1) return;
	bool RTL = pango_find_base_dir(m_text, -1) == PANGO_DIRECTION_RTL;
	if (m_cursor_idx == 0) {
		strcpy(str1, text);
		strcat(str1, m_text);
		setText(str1, zoom, m_hide_hyphen);
		m_cursor_idx = 0;
		if (RTL) moveCursor(-1);
		computeCursorPosition();
		return;
	}
	if (m_cursor_idx == g_utf8_strlen(m_measure_text, -1)) {
		strcpy(str1, m_text);
		strcat(str1, text);
		setText(str1, zoom, m_hide_hyphen);
	 	m_cursor_idx = strlen(m_measure_text);
		m_cursorpos.x = RTL ? 0 : m_width;
		computeDimensions();
		return;
	}
	g_utf8_strncpy(str1, m_text, m_cursor_idx);
	endp = g_utf8_offset_to_pointer(str1, m_cursor_idx);
	*endp = '\0';
	endp = g_utf8_offset_to_pointer(m_text, m_cursor_idx);
	strcpy(str2, endp);
	strcat(str1, text);
	strcat(str1, str2);
	setText(str1, zoom, m_hide_hyphen);
	computeDimensions();
	moveCursor(RTL ? -1 : 1);
}

bool NedPangoCairoText::removeAtCursorPosition(double zoom) {
	char str1[1024], str2[1024];
	unsigned int old_cursor_idx = m_cursor_idx;
	if (m_cursor_idx == 0) return false;
	if (g_utf8_strlen(m_measure_text, -1) <= 1) return false;
	bool RTL = pango_find_base_dir(m_text, -1) == PANGO_DIRECTION_RTL;

	if (RTL) {
		moveCursor(1);
		if (old_cursor_idx < strlen(m_measure_text)) {
			strcpy(str2, m_text + old_cursor_idx);
		}
		else {
			str2[0] = '\0';
		}
		strncpy(str1, m_text, m_cursor_idx);
		str1[m_cursor_idx] = '\0';
		strcat(str1, str2);
	}
	else {
		moveCursor(-1);
		if (old_cursor_idx < strlen(m_measure_text)) {
			strcpy(str2, m_text + old_cursor_idx);
		}
		else {
			str2[0] = '\0';
		}
		strncpy(str1, m_text, m_cursor_idx);
		str1[m_cursor_idx] = '\0';
		strcat(str1, str2);
	}
	setText(str1, zoom, m_hide_hyphen);
	computeDimensions();
	return true;
}

void NedPangoCairoText::setCursorToLastPos() {
	if (m_text == NULL) return;
	bool RTL = (pango_find_base_dir(m_text, -1) == PANGO_DIRECTION_RTL);

	m_cursor_idx = g_utf8_strlen(m_measure_text, -1);
	if (RTL) {
		m_cursor_idx = 0;
		computeCursorPosition();
	}
	else {
		m_cursorpos.x = m_width;
		m_cursor_idx = g_utf8_strlen(m_text, -1);
	}
}

void NedPangoCairoText::setCursorToFirstPos() {
	if (m_text == NULL) return;
	bool RTL = (pango_find_base_dir(m_text, -1) == PANGO_DIRECTION_RTL);
	if (RTL) {
		m_cursor_idx = g_utf8_strlen(m_text, -1);
		computeCursorPosition();
	}
	else {
		m_cursor_idx = 0;
		computeCursorPosition();
	}
}

bool NedPangoCairoText::moveCursor(int dist) {
	int trailing;
	gchar *cptr;
	int newcursor_idx;
	unsigned int i;
	unsigned int ll = strlen(m_measure_text);
	unsigned int llutf8;
	bool RTL = (pango_find_base_dir(m_text, -1) == PANGO_DIRECTION_RTL);

	if (m_cursor_idx == ll) {
		if ((dist > 0 && !RTL) || (dist < 0 && RTL)) {
			return false;
		}
		if ((dist == -1 && !RTL) || (dist == 1 && RTL)) {
			cptr = m_text;
			llutf8 = g_utf8_strlen(m_measure_text, -1);
			for (i = 0; i < llutf8 - 1; i++) {
				cptr = g_utf8_next_char(cptr);
			}
			m_cursor_idx = cptr - m_text;
			computeCursorPosition();
			return true;
		}
		return false;
	}
	pango_layout_move_cursor_visually(m_measure_layout, TRUE, m_cursor_idx, 0, dist, &newcursor_idx, &trailing);
	if (newcursor_idx < 0) {
		return false;
	}
	if (trailing > 0) {
		m_cursor_idx = ll;
		if (RTL) {
			m_cursorpos.x = 0;
		}
		else {
			m_cursorpos.x = m_width;
		}
		return true;
	}
		
	m_cursor_idx = newcursor_idx;
	computeCursorPosition();
	return true;
}

double NedPangoCairoText::getWidth() {
	return (double) m_width / PANGO_INTERNAL_FONT_FACTOR;
}


double NedPangoCairoText::getHeight() {
	return (double) m_height / PANGO_INTERNAL_FONT_FACTOR;
}

double NedPangoCairoText::getInternalWidth() {
	return (double) m_internal_width / PANGO_INTERNAL_FONT_FACTOR;
}

double NedPangoCairoText::getInternalHeight() {
	return (double) m_internal_height / PANGO_INTERNAL_FONT_FACTOR;
}

void NedPangoCairoText::computeCursorPosition() {
	GList *lptr;
	int i;
	PangoRectangle pos2;
	pango_layout_get_cursor_pos(m_measure_layout, m_cursor_idx, &m_cursorpos, &pos2);
	for (i = 0, lptr = g_list_first(m_spec_letters); lptr; lptr = g_list_next(lptr), i++) {
		pango_layout_get_cursor_pos(m_display_layout, ((specLetter *) lptr->data)->idx,
			 &(((specLetter *) lptr->data)->pos), &pos2);
		((specLetter *) lptr->data)->pos.x = ((specLetter *) lptr->data)->pos.x;
	}
}


char *NedPangoCairoText::inspect_text(const char *text) {
	char Str[1024];
	char *cptr, *cptr2, *cptr3, *cptr4;
	specLetter *let;
	bool small_letter;

	strcpy(Str, text);
	cptr = Str;
	while (*cptr != '\0') {
		let = NULL;
		if (*cptr == '\\') {
			cptr2 = g_utf8_next_char(cptr);
			if (*cptr2 != '\0') {
				small_letter = false;
				cptr3 = g_utf8_next_char(cptr2);
				switch (*cptr2) {
					case 'O': let = new specLetter("\160",
							 cptr - Str,
							 CODA_X_OFFSET, CODA_Y_OFFSET);
						  break;
					case 'o': let = new specLetter("\170",
							 cptr - Str,
							 ZERO_X_OFFSET, ZERO_Y_OFFSET);
						  break;
					case 'S': let = new specLetter(
						  	"\161",
						  	cptr - Str,
						  	SEGNO_X_OFFSET, SEGNO_Y_OFFSET);
						  break;
/*
					case '#': let = new specLetter(
						  	"\100",
						  	cptr - Str,
						  	SIGN_X_OFFSET, SIGN_Y_OFFSET);
							small_letter = true;
						  break;
					case 'b': let = new specLetter(
						  	"\121",
						  	cptr - Str,
						  	FLAT_X_OFFSET,
						  	FLAT_Y_OFFSET);
							small_letter = true;
						  break;
					case 'n': let = new specLetter(
						  	"\122",
						  	cptr - Str,
						  	NEUTRAL_X_OFFSET,
						  	NEUTRAL_Y_OFFSET);
							small_letter = true;
						  break;
*/
					default: handle_spec_utf8_chars(cptr, cptr2);
							 break;
				}
				if (let != NULL) {
					*cptr = ' ';
					*cptr2 = ' ';
					cptr = cptr3;
					m_spec_letters = g_list_append(m_spec_letters, let);
					if (small_letter) {
						cptr--;
						for (cptr4 = cptr; *cptr4 != '\0'; cptr4++) {
							*cptr4 = *(cptr4 + 1);
						}
					}

				}
				else {
					cptr = g_utf8_next_char(cptr);
				}
			}
			else {
				cptr = g_utf8_next_char(cptr);
			}
		}
		else {
			cptr = g_utf8_next_char(cptr);
		}
	}
	return strdup(Str);
}

	

void NedPangoCairoText::handle_spec_utf8_chars(char *cptr, char *cptr2) {
	char *cptr3;
	int i;
	int len_dest, len_src;
	struct utf8_element *utf8_ptr;

	for (utf8_ptr = utf8code_array; utf8_ptr->utf8code[3]; utf8_ptr++) { 
		if (!utf8_ptr->short_chars) continue;
		if (!strncmp(cptr2, utf8_ptr->short_chars, strlen(utf8_ptr->short_chars))) {
			len_dest = 4;
			for (i = 0; i < 4; i++) {
				if (!utf8_ptr->utf8code[i]) len_dest--;
			}
			if (len_dest < 1) {
				NedResource::Abort("NedPangoCairoText::handle_spec_utf8_chars");
			}
			len_src = strlen(utf8_ptr->short_chars) + 1;
			if (len_dest > len_src) {
				for (cptr3 = cptr + strlen(cptr) + (len_dest - len_src); cptr3 >= cptr + (len_dest - len_src); cptr3--) {
					*cptr3 = *(cptr3 - (len_dest - len_src));
				}
			}
			else if (len_dest < len_src) {
				for (cptr3 = cptr + len_dest; *(cptr3 + (len_src - len_dest)); cptr3++) {
					*cptr3 = *(cptr3 + (len_src - len_dest));
				}
				*(cptr3) = '\0';
			}
			for (i = 4 - len_dest; i < 4; i++, cptr++) {
				*cptr = utf8_ptr->utf8code[i];
			}
			return;
		}
	}
}

void NedPangoCairoText::convert_bslash_text_to_utf8_text(const char *in_text, char *out_text) {
	char Str[1024];
	char *cptr, *cptr2;

	strcpy(Str, in_text);
	cptr = Str;
	while (*cptr != '\0') {
		if (*cptr == '\\') {
			cptr2 = g_utf8_next_char(cptr);
			if (*cptr2 != '\0') {
				handle_spec_utf8_chars(cptr, cptr2);
			}
			else {
				cptr = g_utf8_next_char(cptr);
			}
		}
		else {
			cptr = g_utf8_next_char(cptr);
		}
	}
	strcpy(out_text, Str);
}

void NedPangoCairoText::makeLilyString(char *in_string, char *buffer) {
	char *cori, *ori, buf1[1024];

	buffer[0] = '"';
	//strcpy(buf1, in_string);
	convert_bslash_text_to_utf8_text(in_string, buf1);
	ori = buf1;
	cori = buffer + 1;
	while (*ori != '\0') {
		if (*ori == '"' || *ori == '\\') {
			*cori++ = '\\';
		}
		*cori++ = *ori++;
	}
	*cori++ = '"';
	*cori = '\0';
}
				
			
NedPangoCairoText::specLetter::specLetter(const char *let, int ix, double xo, double yo) :
		letter(let), idx(ix), xoffs(xo), yoffs(yo) {}

