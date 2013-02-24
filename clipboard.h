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

#ifndef CLIPBOARD_H

#define CLIPBOARD_H
#include "config.h"
#include "resource.h"

#include <gtk/gtk.h>

class NedVoice;
class NedStaff;

class NedClipBoardElement {
	public:
		NedClipBoardElement();
		GList *chords_or_rests_to_move;
		NedVoice *from_voice, *to_voice;
		NedStaff *from_staff, *to_staff;
		void convertAllToClones();
};

class NedClipBoard {
	public:
		NedClipBoard();
		GList *m_elements;
		GList *element_counter;
		static int count;
		void setRefTimeAndDuration(unsigned long long time, unsigned long long duration) {
			m_reftime = time; m_duration = duration;
		}
		void setDeleteOnly() {m_delete_only = true;}
		bool getInsertOnly() {return m_insert_only;}
		bool onlyStaffElems();
		void setInsertOnly();
		void execute(bool adjust = FALSE);
		void unexecute(bool adjust = FALSE);
		void execute_reverse(bool adjust = FALSE);
		void unexecute_reverse(bool adjust = FALSE);
		bool onlyWholeRests();
	private:
		bool m_delete_only;
		bool m_insert_only;
		unsigned long long m_reftime, m_duration;
};

#endif /* CLIPBOARD_H */
