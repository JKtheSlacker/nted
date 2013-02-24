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

#ifndef PAPER_CONFIG_DIALOG_H

#define PAPER_CONFIG_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>

#define DELETE_STAFF_CMD 12


class NedPaperConfigDialog {
	public:
		NedPaperConfigDialog(GtkWindow *parent, const char *current_paper, bool portrait, bool with_cancel);
		void getValues(bool *state, const char **paper, bool *portrait);
	private:
		static void CloseCallBack(GtkDialog *dialog, gint result, gpointer data);
		const char *m_paper;
		bool m_state;
		bool m_portrait;
		GtkWidget *m_radio_box_A5;
		GtkWidget *m_radio_box_A4;
		GtkWidget *m_radio_box_A3;
		GtkWidget *m_radio_box_B4;
		GtkWidget *m_radio_box_Letter;
		GtkWidget *m_radio_box_Legal;
		GtkWidget *m_radio_box_230x293;
		GtkWidget *m_radio_box_portrait;
		GtkWidget *m_radio_box_landspace;
};

#endif /*  PAPER_CONFIG_DIALOG_H */
