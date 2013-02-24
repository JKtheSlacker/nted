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

#ifndef TEMPO_SIGN_H

#define TEMPO_SIGN_H
#include "freereplaceable.h"

class NedTempoSign : public NedFreeReplaceable {
	public:
		NedTempoSign(unsigned int kind, unsigned int tempo);
		NedTempoSign(unsigned int kind, unsigned int tempo, NedChordOrRest *element);
		NedTempoSign(unsigned int kind, unsigned int tempo, NedChordOrRest *element, double x, double y, bool relative);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL);
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_TEMPO;}
		unsigned int getKind() {return m_kind;}
		unsigned int getTempo() {return m_tempo;}
		void setTempo(unsigned int kind, unsigned int tempo);
		virtual void startContextDialog(GtkWidget *ref);
		double getTempoInverse();
		bool getDot() {return m_dot;}
	private: 
		void computeGlypths();
		unsigned int m_kind;
		unsigned int m_tempo;
		cairo_glyph_t m_glyphs[3];
		int m_number_len;
		bool m_dot;

};

#endif /* TEMPO_SIGN_H */
