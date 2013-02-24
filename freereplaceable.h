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

#ifndef FREE_REPLACEABLE_H

#define  FREE_REPLACEABLE_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>

#define FREE_PLACEABLE_VOLSIGN       (1 << 0)
#define FREE_PLACEABLE_TEMPO         (1 << 1) 
#define FREE_PLACEABLE_SLUR_POINT    (1 << 2)
#define FREE_PLACEABLE_LINE_POINT    (1 << 3)
#define FREE_PLACEABLE_TEXT          (1 << 4)
#define FREE_PLACEABLE_SIGN          (1 << 5)
#define FREE_PLACEABLE_CHORD         (1 << 6)
#define FREE_PLACEABLE_SPACER        (1 << 7)
#define FREE_PLACEABLE_CHORDNAME     (1 << 8)

class NedChordOrRest;
class NedMainWindow;

class NedFreeReplaceable {
	public:
		NedFreeReplaceable();
		NedFreeReplaceable(NedChordOrRest *element);
		NedFreeReplaceable(NedChordOrRest *element, double x, double y, bool relative);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL ) = 0;
		virtual void adjust_pointers(struct addr_ref_str *slurlist) = 0;
		void setXY(double x, double y, bool relative);
		void setElement(NedChordOrRest *element);
		void freeFromElement();
		virtual ~NedFreeReplaceable() {}
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) = 0;
		virtual bool trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy);
		virtual void shift(double x, double y, double zoom_factor, double current_scale, double leftx, double topy);
		virtual void shiftOnlyX(double x, double y, double zoom_factor, double current_scale, double leftx, double topy);
		virtual void shiftOnlyY(int staff_nr, double y, double zoom_factor, double current_scale, double leftx, double topy);
		virtual void toRel();
		virtual double getTop() {return m_y;}
		void drawConnection(cairo_t *cr, double xpos, double ypos, double leftx, double topy, double zoom_factor);
		void drawConnectionInFreeText(cairo_t *cr, double xpos, double ypos, double scale, double leftx, double topy, double zoom_factor);
		void getLilyText(char *s);
		double getX() {return m_x;}
		double getY() {return m_y;}
		double getAbsoluteX();
		double getAbsoluteY();
		virtual void setZoom(double zoom, double scale) {}
		virtual bool isActive();
		virtual void startContextDialog(GtkWidget *ref) = 0;
		virtual int getType() = 0;
		void virtual recompute() {};
		double getYposRelative() {return m_yrel;}
		NedChordOrRest *getElement() {return m_element;}
		virtual bool less_than(NedFreeReplaceable *other);
	protected:
		virtual void computeBbox() {}
		double m_x, m_y;
		bool m_abs;
		NedChordOrRest *m_element;

		void setThis();
		void getXYPos(double *xpos, double *ypos);
		void toAbs();
		bool m_is_abs;
	private:
		NedChordOrRest *m_element_at_move_start;
		double m_x_at_start, m_y_at_start;
		double m_last_ref_x, m_last_ref_y, m_last_elements_y;
		double m_yrel;

	friend class NedFixMovedFreePlaceableCommand;
	friend class NedRemoveFreePlaceableCommand;
	friend class NedOctavation;
	friend class NedAcceleration;


};

#endif /* FREE_REPLACEABLE_H */
