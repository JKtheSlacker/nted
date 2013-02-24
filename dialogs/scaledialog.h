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

#ifndef SCALE_DIALOG_H

#define SCALE_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>



class NedScaleDialog {
	public:
		NedScaleDialog(GtkWindow *parent, int scale);
		void getValues(bool *state, int *scale);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		GtkWidget *m_scale_slider;
		bool m_state;
		int m_scale;
};

#endif /* SCALE_DIALOG_H */
