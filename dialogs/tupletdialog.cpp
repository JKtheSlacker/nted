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

#include "tupletdialog.h"
#include "localization.h"
#include "resource.h"
#include "config.h"

NedTupletDialog::NedTupletDialog(GtkWindow *parent) {
	GtkWidget *dialog;
	GtkWidget *infolabel;
	GtkWidget *lab1;
	GtkWidget *vbox;
	GtkWidget *hbox;

	dialog = gtk_dialog_new_with_buttons(_("Tuplet"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);

	m_tuplet_spin_box = gtk_spin_button_new_with_range (2.0, 13.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_tuplet_spin_box), 3.0);
	infolabel = gtk_label_new(_("Instead of using this dialog\nyou can simply press <Ctrl> + num"));

	lab1 = gtk_label_new(_("Tuplet value:"));
	hbox = gtk_hbox_new(FALSE, 8);
	gtk_box_pack_start(GTK_BOX(hbox), lab1, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), m_tuplet_spin_box, FALSE, FALSE, 0);

	m_method_check_button = gtk_check_button_new_with_label(_("method 2"));

	vbox = gtk_vbox_new(FALSE, 8);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), m_method_check_button, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), infolabel, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), vbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedTupletDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedTupletDialog *tuplet_dialog = (NedTupletDialog *) data;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			tuplet_dialog->m_state = TRUE;
		break;
		default:
			tuplet_dialog->m_state = FALSE;
		break;
	}
	tuplet_dialog->m_tuplet_val = (int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(tuplet_dialog->m_tuplet_spin_box)) + 0.4);
	tuplet_dialog->m_method = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tuplet_dialog->m_method_check_button)) ? 1 : 0;
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedTupletDialog::getValues(bool *state, int *tupletval, int *method) {
	*state = m_state;
	*tupletval = m_tuplet_val;
	*method = m_method;

}
