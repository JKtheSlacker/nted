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

#ifndef FREE_SIGN_H

#define FREE_SIGN_H
#include "freereplaceable.h"

#define SIGN_SEGNO 1
#define SIGN_CODA 2


class NedFreeSign : public NedFreeReplaceable {
	public:
		NedFreeSign(int kind);
		NedFreeSign(int kind, NedChordOrRest *element);
		NedFreeSign(int kind, NedChordOrRest *element, double x, double y, bool relative);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL);
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_SIGN;}
		virtual void startContextDialog(GtkWidget *ref);
		int getKind() {return m_kind;}
	private: 
		int m_kind;
};

#endif /* FREE_SIGN_H */
