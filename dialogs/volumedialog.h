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

#ifndef VOLUME_DIALOG_H

#define VOLUME_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>


class NedVolumeDialog {
	public:
		NedVolumeDialog(GtkWindow *parent, int volume, bool relative);
		void getValues(bool *state, int *volume);
	private:
		static void CloseCallBack(GtkDialog *dialog, gint result, gpointer data);
		GtkWidget *m_volume_spin_box;
		bool m_state;
		int m_volume;
};

#endif /*  VOLUME_DIALOG_H */
