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

#include "printconfigdialog.h"
#include "localization.h"
#include "resource.h"

NedPrintConfigDialog::NedPrintConfigDialog(GtkWindow *parent, const char *print_cmd) :
m_print_cmd(print_cmd) {
	GtkWidget *dialog;
	GtkWidget *print_cmd_frame;

	dialog = gtk_dialog_new_with_buttons(_("Print command"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseDialog), (void *) this);

	print_cmd_frame = gtk_frame_new(_("print command"));
	m_print_entry = gtk_entry_new_with_max_length(50);
	gtk_entry_set_text(GTK_ENTRY(m_print_entry), m_print_cmd);
	gtk_container_add (GTK_CONTAINER(print_cmd_frame), m_print_entry);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), print_cmd_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedPrintConfigDialog::CloseDialog(GtkDialog *dialog, gint result, gpointer data) {
	NedPrintConfigDialog *print_cmd_dialog = (NedPrintConfigDialog *) data;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			print_cmd_dialog->m_state = TRUE;
		break;
		default:
			print_cmd_dialog->m_state = FALSE;
		break;
	}
	print_cmd_dialog->m_print_cmd = strdup(gtk_entry_get_text(GTK_ENTRY(print_cmd_dialog->m_print_entry)));
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedPrintConfigDialog::getValues(bool *state, const char **print_cmd) {
	*state = m_state;
	*print_cmd = m_print_cmd;
}
