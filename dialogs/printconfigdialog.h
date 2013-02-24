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

#ifndef PRINT_CONFIG_DIALOG_H

#define PRINT_CONFIG_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>


class NedPrintConfigDialog {
	public:
		NedPrintConfigDialog(GtkWindow *parent, const char *print_cmd);
		void getValues(bool *state, const char **print_cmd);
	private:
		static void CloseDialog(GtkDialog *dialog, gint result, gpointer data);
		bool m_state;
		const char *m_print_cmd;
		GtkWidget *m_print_entry;
};

#endif /*  PRINT_CONFIG_DIALOG_H */
