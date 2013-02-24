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

#ifndef FREE_CHORD_H

#define FREE_CHORD_H
#include "freereplaceable.h"
#include "resource.h"


struct chord_str;
class NedMainWindow;

class NedFreeChord : public NedFreeReplaceable {
	public:
		NedFreeChord(char *code, int chordname, unsigned int status, NedMainWindow *main_window, NedChordOrRest *element, double x, double y, bool relative);
		NedFreeChord(struct chord_str *chordptr, int chordname, unsigned int status, NedMainWindow *main_window);
		NedFreeChord(struct chord_str *chordptr, int chordname, unsigned int status, NedMainWindow *main_window, NedChordOrRest *element);
		NedFreeChord(struct chord_str *chordptr, int chordname, unsigned int status, NedMainWindow *main_window, NedChordOrRest *element, double x, double y, bool relative);
		void setChord(struct chord_str *chordptr);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL);
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_CHORD;}
		virtual void startContextDialog(GtkWidget *ref);
		struct chord_str * getChordPtr() {return m_chordptr;}
		void setChordPtr(struct chord_str * chordptr) {m_chordptr = chordptr;}
		bool trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy);
		virtual double getTop() {return m_y - m_bbox.height;}
		int getChordName() {return m_chordname;}
		const char *getCode();
		unsigned int getStatus() {return m_status;}
	private: 
		void computeBbox();
		struct chord_str *m_chordptr;
		NedMainWindow *m_main_window;
		int m_chordname;
		NedBbox m_bbox;
		unsigned int m_status;
		char m_chord_name[128];
};

#endif /* FREE_CHORD_H */
