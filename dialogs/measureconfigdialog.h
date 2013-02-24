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

#ifndef MEASURE_CONFIGDIALOG_H

#define MEASURE_CONFIGDIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>



class NedMeasureConfigDialog {
	public:
		NedMeasureConfigDialog(GtkWindow *parent, int measure_type, bool alter1, bool alter2, bool allow_alter, bool hide_following, int num, int denom, unsigned int sym);
		void getValues(bool *state, int *measure_type, bool *alter1, bool *alter2, bool *hide_following, bool *timsig_state, bool *delete_timesig, int *num, int *denom, unsigned int *sym);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static void OnTimesigToggeled (GtkToggleButton *togglebutton, gpointer data);
		static void OnCommButtonToggled(GtkToggleButton *togglebutton, gpointer data);
		static void OnCutButtonToggled(GtkToggleButton *togglebutton, gpointer data);
		static gboolean draw_measure_types(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		static void previous_measure_type(GtkButton *button, gpointer user_data);
		static void next_measure_type(GtkButton *button, gpointer user_data);
		static void alter1_checked (GtkToggleButton *togglebutton, gpointer data);
		static void delete_time_sig_checked (GtkToggleButton *togglebutton, gpointer data);
		GtkWidget *m_measure_type_show_window;
		GtkWidget *m_meas_bu_up;
		GtkWidget *m_meas_bu_down;
		GtkWidget *m_alter1_check_box;
		GtkWidget *m_alter2_check_box;
		GtkWidget *m_hide_box;
		GtkWidget *m_numerator;
		GtkWidget *m_denominator;
		GtkWidget *m_com_time_bu;
		GtkWidget *m_cut_time_bu;
		GtkWidget *m_timsig_check_box;
		GtkWidget *m_delete_timsig_check_box;
		bool m_state;
		int m_measure_type;
		int m_original_type;
		int m_num, m_denom;
		unsigned int m_symbol;
		bool m_alter1;
		bool m_alter2;
		bool m_hide_following;
		bool m_allow_alter;
		bool m_timesig_state;
		bool m_delete_timesig;

		
};

#endif /* MEASURE_CONFIGDIALOG_H */
