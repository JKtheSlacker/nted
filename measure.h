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

#ifndef MEASURE_H

#define MEASURE_H
#include "config.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <cairo.h>
#include "positionarray.h"

class SpecialMeasure;
class NedSystem;
class NedMainWindow;

#define SPECIAL_POSITION_NONE  0
#define SPECIAL_POSITION_START 1
#define SPECIAL_POSITION_END   2

class NedMeasure {
	public:
		NedMeasure();
		void setSystem(NedSystem *system) {m_system = system;}
		void setMeasureNumber(int num, GList *special_measures, bool force = false);
		unsigned int getMeasureDuration() {return (unsigned int) (midi_end - midi_start);}
		double m_staff_elements_width; // width of leading keysigs and timesigs
		bool m_measure_has_only_staff_members;
		int position;
		int getSpecial();
		void setSpecial(int type);
		bool barRequiresHideFollowing();
		unsigned long long midi_start, midi_end;
		double start, end;
		char *getNumberString() {return m_measure_number_str;}
		int getMeasureNumber() {return m_measure_number;}
		void draw(cairo_t *cr, int special_position, double xpos, double content_x_pos, double sys_y_pos, double top, 
			double bottom, double bottom_ori, double leftx, double topy, double staff_border_dist, double zoom_factor, int zoom_level, bool rep_line_needed, int spec_measure_at_end, bool out);
		double getNeededSpaceBefore();
		double getNeededSpaceAfter();
		double shiftXpos(double extra_space_divided, double shift, bool out);
		double computeAndSetPositions(double start_position, int *num_elements, double *extra_space_for_accidentals, bool *measure_has_only_staff_members /* means keysig */, bool out);
		bool isNearStart(double x);
		bool isNearEnd(double x);
		bool nearRepeatElement(double x, double y);
		unsigned int getStoredStaffCount();
		void setNewStaffCount(int newstaffcount);
		NedPositionArray m_position_array;
		int getStemDirectionOfTheMeasure(int staff_voice_offs) {return m_position_array.getStemDirectionOfTheMeasure(staff_voice_offs);}
		double m_spread_fac;
		bool m_special_spread;
		bool m_hide_following;
	private:
		int m_measure_number;
		char m_measure_number_str[8];
		SpecialMeasure *m_special_descr;
		NedSystem *m_system;
	friend class NedMainWindow;
};

#endif /* MEASURE_H */
