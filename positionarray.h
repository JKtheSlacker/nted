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

#ifndef POSITIONARRAY_H

#define POSITIONARRAY_H

#include "dynarray.h"

class NedChordOrRest;

struct voice_properties {
	char m_stem_decision;
	short m_line_sum;
	short m_note_count;
	short m_rests_count;
};


class NedPosition {
	public:
		NedPosition(NedChordOrRest *element, int offs, unsigned int staff_count);
		void placeRests(struct voice_properties *vprops);
		bool m_valid;
		bool m_has_notes;
		bool m_has_unhidden_elements;
		bool m_has_only_staff_members;
		bool m_has_only_graces;
		bool m_element_has_arpeggio;
		bool isUsed(int offs);
		double m_max_microshift;
		double m_step_width;
		unsigned int m_min_duration;
		unsigned long long m_midi_time;
		bool hasNote();
		void placeAt(double xpos, int *v_array, struct voice_properties *v_props, int mnum, bool out);
		void computeAccidentalPositions(int mnum, bool out);
		void shiftXpos(double xshift, int mnum, int pos);
		void setNewStaffCount(unsigned int newstaffcount);
		void computeMicroShifts(int mnum, int pos, bool out);
		bool hasOtherStaffElements(int type);
		double m_min_width_after, m_min_width_before, m_acc_dist, m_needed_width, m_min_needed_width ;
		void insertElement(NedChordOrRest *element, int offs);
		bool insertStaffElement(NedChordOrRest *element, int staff_offs);
		void empty();
#ifdef UUU
		bool print();
#endif
	private:
		NedDynArray<NedChordOrRest*> m_voice_members;
		NedDynArray<unsigned char> m_count_of_elements_per_staff;
		NedDynArray<NedChordOrRest*> m_staff_members;
		unsigned int m_staff_count;
};


class NedPositionArray {
	public:
		NedPositionArray();
		~NedPositionArray();
		void insertElement(NedChordOrRest *element, int offs, int mnum);
		void insertStaffElement(NedChordOrRest *element, int staff_offs, int mnum);
		void insertGraceElement(NedChordOrRest *element, int offs, int mnum);
		void empty();
		double computeAndSetPositions(double start_position, int *num_elements, double *extra_space_for_accidentals, bool *measure_has_only_staff_members /* means keysig */ ,double timesig_space, int mnum, double spread_fac, bool out);
		void setNewStaffCount(unsigned int newstaffcount);
		double shiftXpos(double extra_space_divided, double shift, int mnum, bool out);
		unsigned int getStoredStaffCount() {return m_staff_count;}
		int getStemDirectionOfTheMeasure(int m_staff_voice_offs);
#ifdef UUU
		void print();
#endif
	private:
		unsigned int m_staff_count;
		unsigned int m_min_duration;
		double m_min_durations_dist;
		NedDynArray<NedPosition*> m_positions;
		NedDynArray<voice_properties> m_voice_properties;
		static int compare_positions(NedPosition **pos1, NedPosition **pos2);

};

#endif /* POSITIONARRAY_H */
