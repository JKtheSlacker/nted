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

#ifndef MUTE_CONFIGDIALOG_H

#define MUTE_CONFIGDIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>

struct staff_context_str;


class NedMuteDialog {
	public:
		NedMuteDialog(GtkWindow *parent, bool *mutes, struct staff_context_str *staff_contexts, int numstaffs);
		void getValues(bool *state);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static void OnAll(GtkButton *button, gpointer data);
		static void OnNone(GtkButton *button, gpointer data);
		GtkWidget **m_checkboxes;
		bool *m_mutes;
		bool m_state;
		int m_numstaffs;
};

#endif /* MUTE_CONFIGDIALOG_H */
