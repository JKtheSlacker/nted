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

#include "volumedialog.h"
#include "localization.h"
#include "resource.h"
#include "config.h"

NedVolumeDialog::NedVolumeDialog(GtkWindow *parent, int volume, bool relative) {
	GtkWidget *dialog;
	GtkWidget *lab;
	GtkWidget *hbox;

	dialog = gtk_dialog_new_with_buttons(_("Set Volume"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);

	if (relative) {
		m_volume_spin_box = gtk_spin_button_new_with_range (-127.0, 127.0, 1.0);
	}
	else {
		m_volume_spin_box = gtk_spin_button_new_with_range (0.0, 127.0, 1.0);
	}
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_volume_spin_box), volume);

	lab = gtk_label_new(_("volume:"));
	hbox = gtk_hbox_new(FALSE, 8);
	gtk_box_pack_start(GTK_BOX(hbox), lab, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), m_volume_spin_box, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), hbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedVolumeDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedVolumeDialog *volume_dialog = (NedVolumeDialog *) data;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			volume_dialog->m_state = TRUE;
		break;
		default:
			volume_dialog->m_state = FALSE;
		break;
	}
	volume_dialog->m_volume = (int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(volume_dialog->m_volume_spin_box)) + 0.4);
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedVolumeDialog::getValues(bool *state, int *volume) {
	*state = m_state;
	*volume = m_volume;
}
