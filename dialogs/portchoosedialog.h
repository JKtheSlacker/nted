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

#ifndef PORT_CHOOSE_DIALOG_H

#define PORT_CHOOSE_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>

class NedPortChooseDialog {
	public:
		NedPortChooseDialog(GtkWindow *parent, GList *ports, unsigned int idx,  bool midi_echo, bool show_echo_box);
		unsigned int getSelection() {return m_idx;}
		bool getMidiEcho() {return m_midi_echo;}
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		unsigned int m_idx;
		GtkWidget *m_port_list_view;
		GtkWidget *m_midi_echo_check_box;
		bool m_midi_echo;
		bool m_with_midi_echo_box;

		
};

#endif /* PORT_CHOOSE_DIALOG_H */
