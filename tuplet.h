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

#ifndef TUPLET_H

#define TUPLET_H
#include "config.h"
#include "resource.h"

#include <gtk/gtk.h>
#include <cairo.h>

class NedPage;
class NedStaff;
class NedSystem;
class NedMainWindow;


class NedTuplet {
	public:
		NedTuplet(int tuplet_val, GList *chords_list, double *topy, double *boty);
		~NedTuplet();
		void computeTuplet(/* int stemdir */  double *topy, double *boty);
		void draw(cairo_t *cr);
		unsigned long long getEndTime();
		NedMainWindow *getMainWindow();
		NedPage *getPage();
		NedSystem *getSystem();
		NedStaff *getStaff();
		void reconfigureLast();
		unsigned long long getMidiStartTime();
		unsigned int getDuration();
		NedChordOrRest *getFirstElement();
		NedChordOrRest *getLastElement();
		GList *getElementList() {return m_chords;}
		void saveTuplet(FILE *fp);
	private:
		GList *m_chords;
		double m_start_x, m_start_y;
		double m_end_x, m_end_y;
		bool m_up;
		int m_tuplet_val;
		bool m_bracket_needed;
};

#endif /* TUPLET_H */
