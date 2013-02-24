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

#ifndef PANGO_CAIRO_TEXT_H

#define PANGO_CAIRO_TEXT_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>
#include <pango/pangocairo.h>

class NedPangoCairoText {
	public:
		NedPangoCairoText(GdkDrawable *d, const char *text, const char* font_family, PangoStyle style, PangoWeight weight, double size, double zoom, double scale, bool hide_trailing_hyphen);
		~NedPangoCairoText();
		NedPangoCairoText *clone();
		void setZoom(double zoom, double scale);
		void adjust(GdkDrawable *d, double zoom, double scale);
		void setText(char *text, double zoom, bool hide_trailing_hyphen);
		bool textDiffers(char *other_text);
		bool moveCursor(int dist);
		void insertAtCursorPosition(const char *text, double zoom);
		bool removeAtCursorPosition(double zoom);
		char *getText();
		double getWidth();
		double getHeight();
		double getInternalWidth();
		double getInternalHeight();
		bool hasHiddenHyphen() {return m_hidden_hyphen;}
		void setCursorToLastPos();
		void setCursorToFirstPos();
		double getFontSize();
		void draw(cairo_t *cr, double xpos, double ypos, double zoom, double scale);
		void drawCursor(cairo_t *cr, double xpos, double ypos, double zoom_factor);
		static double getInternalFactor();
		static void handle_spec_utf8_chars(char *cptr, char *cptr2);
		static void convert_bslash_text_to_utf8_text(const char *in_text, char *out_text); 
		static void makeLilyString(char *n_string, char *buffer);
	private:
		void createLayout(cairo_t *cr, double zoom, double scale);
		void computeCursorPosition();
		void computeDimensions();
		char *inspect_text(const char *text);
		static struct utf8_element {
			const char *short_chars;
			unsigned char utf8code[4];
		} utf8code_array[];
		PangoLayout *m_display_layout;
		PangoLayout *m_measure_layout;
		GdkDrawable *m_drawable;
		bool m_hidden_hyphen;
		char *m_text;
		char *m_measure_text;
		const char *m_font_family;
		PangoStyle m_style;
		PangoWeight m_weight;
		double m_size;
		int m_width;
		int m_height;
		int m_internal_width;
		int m_internal_height;
		bool m_hide_hyphen;
		unsigned int m_cursor_idx;
		PangoRectangle m_cursorpos;
		class specLetter {
			public:
				specLetter(const char *let, int ix, double xo, double yo);
				const char *letter;
				int idx;
				double xoffs, yoffs;
				PangoRectangle pos;
		};
		GList *m_spec_letters;
			
};

#endif /* PANGO_CAIRO_TEXT_H */
