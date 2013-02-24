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

#ifndef SLUR_POINT_H

#define SLUR_POINT_H
#include "freereplaceable.h"
#include <cairo.h>

class NedSlur;
class NedStaff;

class NedSlurPoint : public NedFreeReplaceable {
	public:
		NedSlurPoint(NedSlur *slur, int p_nr);
		NedSlurPoint(NedSlur *slur, int p_nr, NedChordOrRest *element);
		NedSlurPoint(NedSlur *slur, int p_nr, NedChordOrRest *element, double x, double y, bool relative);
		void getXYPosAndStaff(double *xpos, double *ypos, NedStaff **staff);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL );
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		NedStaff *getStaff();
		NedSlurPoint **getSlurPoints();
		virtual void shift(double x, double y, double zoom_factor, double m_current_scale, double leftx, double topy);
		void setSlur(NedSlur *slur) {m_slur = slur;}
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_SLUR_POINT;}
		virtual void startContextDialog(GtkWidget *ref);
		double computeYposRelative(NedStaff *staff);
		void virtual recompute();
		NedSlur *getSlur() {return m_slur;}
		int m_point_number;
	private:
		NedSlur *m_slur;
	friend class NedSlur;
};

#endif /* SLUR_POINT_H */
