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

#ifndef KEYSIG_DIALOG_H

#define KEYSIG_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>


class NedKeysigDialog {
	public:
		NedKeysigDialog(GtkWindow *parent, int key_signature_number);
		void getValues(bool *state, int *key_signature_number, bool *adjust_notes);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		GtkWidget *m_sig_radio_buttons[14];
		GtkWidget *m_adjust_check_box;
		bool m_state;
		int m_key_signature_number;
		bool m_do_adjust;

		
};

#endif /* KEYSIG_DIALOG_H */
