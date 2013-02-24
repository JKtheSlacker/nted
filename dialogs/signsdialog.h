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

#ifndef SIGNS_DIALOG_H

#define SIGNS_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>


class NedSignsDialog {
	public:
		NedSignsDialog(GtkWindow *parent);
		void getValues(bool *state, int *signtype);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static gboolean draw_menu(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		static gboolean change_selection (GtkWidget *widget, GdkEventButton *event, gpointer data);
		bool m_state;
		int m_current_row, m_current_column;
		GdkGC *m_drawing_area_gc;
		GdkColor m_black, m_red;
};

#endif /* SIGNS_DIALOG_H */
