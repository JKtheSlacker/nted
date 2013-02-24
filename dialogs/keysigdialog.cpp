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

#include "keysigdialog.h"
#include "localization.h"
#include "staffcontextdialog.h"
#include "resource.h"


NedKeysigDialog::NedKeysigDialog(GtkWindow *parent, int key_signature_number) :
m_state(false), m_key_signature_number(key_signature_number) {
	int i;
	GtkWidget *dialog;
	GtkWidget *key_frame;
	GtkWidget *key_frame_left_vbox;
	GtkWidget *key_frame_right_vbox;
	GtkWidget *radio_hbox;
	GtkWidget *adjust_frame;
	dialog = gtk_dialog_new_with_buttons(_("New keysignature"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	key_frame = gtk_frame_new(_("key"));
	radio_hbox = gtk_hbox_new(FALSE, 2);
	key_frame_left_vbox = gtk_vbox_new(FALSE, 2);
	key_frame_right_vbox = gtk_vbox_new(FALSE, 2);
	m_sig_radio_buttons[0] =  gtk_radio_button_new_with_label (NULL, gettext(NedStaffContextDialog::m_keyTab[0]));
	gtk_box_pack_start (GTK_BOX (key_frame_left_vbox),  m_sig_radio_buttons[0], FALSE, FALSE, 0);
	for (i = -5; i < 7; i++) {
		m_sig_radio_buttons[i+6] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_sig_radio_buttons[0]), gettext(NedStaffContextDialog::m_keyTab[i+6]));
		if (i < 0) {
			gtk_box_pack_start (GTK_BOX (key_frame_left_vbox), m_sig_radio_buttons[i+6], FALSE, FALSE, 0);
		}
		else {
			gtk_box_pack_start (GTK_BOX (key_frame_right_vbox), m_sig_radio_buttons[i+6], FALSE, FALSE, 0);
		}
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_sig_radio_buttons[m_key_signature_number+6]), TRUE);

	gtk_box_pack_start (GTK_BOX (radio_hbox), key_frame_left_vbox,  FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (radio_hbox), key_frame_right_vbox,  FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER(key_frame), radio_hbox);

	adjust_frame = gtk_frame_new(_("adjust"));

	m_adjust_check_box = gtk_check_button_new_with_label(_("adjust notes"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_adjust_check_box), TRUE);
	gtk_container_add (GTK_CONTAINER(adjust_frame), m_adjust_check_box);


	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), key_frame);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), adjust_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedKeysigDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	int i;
	int newkeysig;

	NedKeysigDialog *keysig_dialog = (NedKeysigDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			keysig_dialog->m_state = TRUE;
		break;
		default:
			keysig_dialog->m_state = FALSE;
		break;
	}
	newkeysig = 0;
	keysig_dialog->m_key_signature_number = 0;
	for (i = 0; i < 13; i++) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (keysig_dialog->m_sig_radio_buttons[i]))) {
			newkeysig = i - 6;
		}
	}
	keysig_dialog->m_key_signature_number = newkeysig;
	keysig_dialog->m_do_adjust = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(keysig_dialog->m_adjust_check_box));
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedKeysigDialog::getValues(bool *state, int *key_signature_number, bool *adjust_notes) {
	*state = m_state;
	*key_signature_number = m_key_signature_number;
	*adjust_notes = m_do_adjust;
}

