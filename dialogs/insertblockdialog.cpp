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

#include "insertblockdialog.h"
#include "localization.h"
#include "resource.h"
#include "config.h"

NedInsertBlockDialog::NedInsertBlockDialog(GtkWindow *parent) {
	GtkWidget *dialog;
	GtkWidget *lab;
	GtkWidget *hbox;

	dialog = gtk_dialog_new_with_buttons(_("Insert blocks"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);

	m_block_spin_box = gtk_spin_button_new_with_range (1.0, 20.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_block_spin_box), 1.0);

	lab = gtk_label_new(_("block count:"));
	hbox = gtk_hbox_new(FALSE, 8);
	gtk_box_pack_start(GTK_BOX(hbox), lab, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), m_block_spin_box, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), hbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedInsertBlockDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedInsertBlockDialog *insert_dialog = (NedInsertBlockDialog *) data;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			insert_dialog->m_state = TRUE;
		break;
		default:
			insert_dialog->m_state = FALSE;
		break;
	}
	insert_dialog->m_blockcount = (int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(insert_dialog->m_block_spin_box)) + 0.4);
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedInsertBlockDialog::getValues(bool *state, int *blockcount) {
	*state = m_state;
	*blockcount = m_blockcount;
}
