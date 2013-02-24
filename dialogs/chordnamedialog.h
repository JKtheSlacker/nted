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

#ifndef CHORD_NAME_DIALOG_H

#define CHORD_NAME_DIALOG_H
#include "config.h"
#include "resource.h"
#include <gtk/gtk.h>
#include <cairo/cairo.h>

class NedChordNameDialog {
	public:
		NedChordNameDialog(NedMainWindow *main_window, GtkWindow *parent, char *roottext, char *uptext, char *downtext,
			 double fontsize, double ydist, bool ydist_relenvant);
		~NedChordNameDialog();
		void getValues(bool *state, char **roottext, char **uptext, char **downtext, double *fontsize, double *ydist);
		static bool isAlreadyActive() {return m_chord_name_dialog_active;}
	private:
		static bool m_chord_name_dialog_active;
		static double m_chord_name_y_dist;
		static void ResponseCallBack(GtkDialog *dialog, gint result, gpointer data);
		static bool OnKeyRelease(GtkWidget *widget, GdkEventKey *event, gpointer data);
		static void OnValueChanged(GtkSpinButton *spinbutton, gpointer data);
		bool updateMainSpecial();
		NedMainWindow *m_main_window;
		GtkWidget *m_dialog;
		GtkWidget *m_font_size_spin_box;
		GtkWidget *m_ydist_spin_box;
		GtkWidget *m_root_text_widget;
		GtkWidget *m_up_text_widget;
		GtkWidget *m_down_text_widget;
		static double m_font_size;
		char *m_root_text;
		char *m_up_text;
		char *m_down_text;
		int m_state;
};

#endif /* CHORD_NAME_DIALOG_H */
