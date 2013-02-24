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

#ifndef FREE_TEXT_H

#define FREE_TEXT_H
#include "freereplaceable.h"
#include <cairo/cairo.h>

class NedPangoCairoText;

#define ANCHOR_LEFT 0
#define ANCHOR_MID 1
#define ANCHOR_RIGHT 2

class NedFreeText : public NedFreeReplaceable {
	public:
		NedFreeText(char *text, GtkWidget *drawing_area, int anchor, unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
				unsigned short segno_sign, int midi_pgm, unsigned int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight);
		NedFreeText(char *text, GtkWidget *drawing_area, int anchor, unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume, 
					unsigned short segno_sign, int midi_pgm, unsigned int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight,
					NedChordOrRest *element);
		NedFreeText(char *text, GtkWidget *drawing_area, int anchor, unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
					unsigned short segno_sign, int midi_pgm, unsigned int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight,
					NedChordOrRest *element,
						double x, double y, bool relative);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL);
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_TEXT;}
		virtual void startContextDialog(GtkWidget *ref);
		virtual bool trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy);
		unsigned int getTempo() {return m_tempo;}
		void setTempo(unsigned int tempo) {m_tempo = tempo;}
		double getTempoInverse();
		unsigned int m_midi_volume;
		bool m_enable_volume_change, m_enable_tempo_change;
		unsigned short m_segno_sign;
		int m_midi_pgm;
		unsigned int m_channel;
		bool m_enable_channel;
		char *getText();
		void getLilyText(char *s);
		void setText(char *text, double zoom);
		virtual void setZoom(double zoom, double scale);
		const char *m_font_family;
		double m_font_size;
		PangoStyle m_font_slant;
		PangoWeight m_font_weight;
		int m_anchor;
	private: 
		unsigned int m_tempo;
		NedPangoCairoText *m_text;
		GtkWidget *m_drawing_area;

};

#endif /* FREE_TEXT_H */
