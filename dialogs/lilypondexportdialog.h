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

#ifndef LILYPOND_EXPORT_DIALOG_H

#define LILYPOND_EXPORT_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>

class NedMainWindow;


class NedLilyPondExportDialog {
	public:
		NedLilyPondExportDialog(GtkWindow *parent, NedMainWindow *main_window, char *filename, bool *selected_staves, bool *keep_beams);
		void getValues(bool *state, bool *with_break);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static void OnStaffSelection  (GtkButton *button, gpointer data);
		static void OnBeamSelection  (GtkButton *button, gpointer data);
		NedMainWindow *m_main_window;
		GtkWidget *m_dialog;
		GtkWidget *m_with_break_checkbox;
		GtkWidget *m_file_chooser;
		bool m_state;
		int m_with_breaks;
		char *m_filename;
		bool *m_selected_staves;
		bool *m_keep_beams;
};

#endif /*  LILYPOND_EXPORT_DIALOG_H */
