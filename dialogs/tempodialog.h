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

#ifndef TEMPO_DIALOG_H

#define TEMPO_DIALOG_H
#include "config.h"
#include "resource.h"
#include <gtk/gtk.h>


class NedTempoDialog {
	public:
		NedTempoDialog(GtkWindow *parent, bool relative, unsigned int kind = NOTE_4, bool dot = false, unsigned int tempo = 120);
		void getValues(bool *state, unsigned int *kind, bool *dot, unsigned int *tempo);
	private:
		static void CloseCallBack(GtkDialog *dialog, gint result, gpointer data);
		GtkWidget *m_tempo_spin_box;
		GtkWidget *m_whole_kind;
		GtkWidget *m_kind_2;
		GtkWidget *m_kind_2_dot;
		GtkWidget *m_kind_4;
		GtkWidget *m_kind_4_dot;
		GtkWidget *m_kind_8;
		GtkWidget *m_kind_8_dot;
		GtkWidget *m_kind_16;
		GtkWidget *m_kind_16_dot;
		bool m_state;
		unsigned int m_kind;
		bool m_dot;
		unsigned int m_tempo;
};

#endif /*  TEMPO_DIALOG_H */
