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

#ifndef FREE_CHORD_NAME

#define FREE_CHORD_NAME
#include "freereplaceable.h"
#include <cairo/cairo.h>

class NedPangoCairoText;

class NedChordName : public NedFreeReplaceable {
	public:
		NedChordName(GtkWidget *drawing_area, char *roottext, char *uptext, char *downtext, double fontsize);
		NedChordName(GtkWidget *drawing_area, char *roottext, char *uptext, char *downtext, double fontsize,
					NedChordOrRest *element);
		NedChordName(GtkWidget *drawing_area, char *roottext, char *uptext, char *downtext, double fontsize,
					NedChordOrRest *element, double x, double y, bool relative);
		~NedChordName();
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL);
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_CHORDNAME;}
		virtual void startContextDialog(GtkWidget *ref);
		virtual bool trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy);
		char *getRootText();
		char *getUpText();
		char *getDownText();
		double  getSize() {return m_fontsize;}
		void setTexts(char *roottext, char *uptext, char *downtext, double zoom);
		virtual void setZoom(double zoom, double scale);
		const char *m_font_family;
	private: 
		NedPangoCairoText *m_root_text, *m_up_text, *m_down_text;
		double m_fontsize;
		GtkWidget *m_drawing_area;

};

#endif /* FREE_CHORD_NAME */
