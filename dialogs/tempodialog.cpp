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

#include "tempodialog.h"
#include "localization.h"
#include "resource.h"
#include "config.h"

NedTempoDialog::NedTempoDialog(GtkWindow *parent, bool relative, unsigned int kind /* = NOTE_4 */, bool dot /* = false */, unsigned int tempo /* = 120 */) :
	m_kind(kind), m_dot(dot), m_tempo(tempo) {
	GtkWidget *dialog;
	GtkWidget *lab;
	GtkWidget *kind_hbox;
	GtkWidget *tempo_hbox;
	GtkWidget *kind_frame;
	GtkWidget *tempo_frame;

	GtkWidget *vbox_whole;
	GtkWidget *vbox_note_2;
	GtkWidget *vbox_note_2_dot;
	GtkWidget *vbox_note_4;
	GtkWidget *vbox_note_4_dot;
	GtkWidget *vbox_note_8;
	GtkWidget *vbox_note_8_dot;
	GtkWidget *vbox_note_16;
	GtkWidget *vbox_note_16_dot;

	GtkWidget *image_whole;
	GtkWidget *image_note_2;
	GtkWidget *image_note_2_dot;
	GtkWidget *image_note_4;
	GtkWidget *image_note_4_dot;
	GtkWidget *image_note_8;
	GtkWidget *image_note_8_dot;
	GtkWidget *image_note_16;
	GtkWidget *image_note_16_dot;

	dialog = gtk_dialog_new_with_buttons(_("Set Tempo"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);


	m_whole_kind =  gtk_radio_button_new (NULL);
	m_kind_2  = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_2_dot  = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_4  = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_4_dot  = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_8  = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_8_dot  = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_16 = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));
	m_kind_16_dot = gtk_radio_button_new_from_widget (GTK_RADIO_BUTTON (m_whole_kind));

	image_whole = gtk_image_new_from_stock("fullnote_icon", GTK_ICON_SIZE_BUTTON);
	image_note_2 = gtk_image_new_from_stock("halfnote-icon", GTK_ICON_SIZE_BUTTON);
	image_note_2_dot = gtk_image_new_from_stock("halfnote-dot-icon", GTK_ICON_SIZE_BUTTON);
	image_note_4 = gtk_image_new_from_stock("quarternote-icon", GTK_ICON_SIZE_BUTTON);
	image_note_4_dot = gtk_image_new_from_stock("quarternote-dot-icon", GTK_ICON_SIZE_BUTTON);
	image_note_8 = gtk_image_new_from_stock("eighthnote-icon", GTK_ICON_SIZE_BUTTON);
	image_note_8_dot = gtk_image_new_from_stock("eighthnote-dot-icon", GTK_ICON_SIZE_BUTTON);
	image_note_16 = gtk_image_new_from_stock("sixteenthnote-icon", GTK_ICON_SIZE_BUTTON);
	image_note_16_dot = gtk_image_new_from_stock("sixteenthnote-dot-icon", GTK_ICON_SIZE_BUTTON);

	vbox_whole = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_whole), m_whole_kind, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_whole), image_whole, FALSE, FALSE, 4);

	vbox_note_2 = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_2), m_kind_2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_2), image_note_2, FALSE, FALSE, 4);

	vbox_note_2_dot = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_2_dot), m_kind_2_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_2_dot), image_note_2_dot, FALSE, FALSE, 4);

	vbox_note_4 = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_4), m_kind_4, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_4), image_note_4, FALSE, FALSE, 4);

	vbox_note_4_dot = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_4_dot), m_kind_4_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_4_dot), image_note_4_dot, FALSE, FALSE, 4);

	vbox_note_8 = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_8), m_kind_8, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_8), image_note_8, FALSE, FALSE, 4);

	vbox_note_8_dot = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_8_dot), m_kind_8_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_8_dot), image_note_8_dot, FALSE, FALSE, 4);

	vbox_note_16 = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_16), m_kind_16, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_16), image_note_16, FALSE, FALSE, 4);

	vbox_note_16_dot = gtk_vbox_new(FALSE, 2); 
	gtk_box_pack_start(GTK_BOX(vbox_note_16_dot), m_kind_16_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox_note_16_dot), image_note_16_dot, FALSE, FALSE, 4);


	kind_hbox = gtk_hbox_new(FALSE, 8);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_whole, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_2, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_2_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_4, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_4_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_8, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_8_dot, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_16, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(kind_hbox), vbox_note_16_dot, FALSE, FALSE, 0);
	kind_frame = gtk_frame_new(_("kind"));
	gtk_container_add (GTK_CONTAINER(kind_frame), kind_hbox);
	switch (kind) {
		case WHOLE_NOTE: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_whole_kind), TRUE); break;
		case NOTE_2: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_2), !m_dot);
			     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_2_dot), m_dot); break;
		case NOTE_4: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_4), !m_dot);
			     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_4_dot), m_dot); break;
		case NOTE_8: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_8), !m_dot);
			     gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_8_dot), m_dot); break;
		default: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_16), !m_dot); 
			 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_kind_16_dot), TRUE); break;
	}

	if (relative) {
		m_tempo_spin_box = gtk_spin_button_new_with_range (-999, 999.0, 1.0);
	}
	else {
		m_tempo_spin_box = gtk_spin_button_new_with_range (10.0, 999.0, 1.0);
	}
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_tempo_spin_box), tempo);

	lab = gtk_label_new(_("tempo:"));
	tempo_hbox = gtk_hbox_new(FALSE, 8);
	gtk_box_pack_start(GTK_BOX(tempo_hbox), lab, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tempo_hbox), m_tempo_spin_box, FALSE, FALSE, 0);
	tempo_frame = gtk_frame_new(_("tempo"));
	gtk_container_add (GTK_CONTAINER(tempo_frame), tempo_hbox);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), kind_frame);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), tempo_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedTempoDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedTempoDialog *tempo_dialog = (NedTempoDialog *) data;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			tempo_dialog->m_state = TRUE;
		break;
		default:
			tempo_dialog->m_state = FALSE;
		break;
	}
	tempo_dialog->m_tempo = (unsigned int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(tempo_dialog->m_tempo_spin_box)) + 0.4);
	tempo_dialog->m_kind = NOTE_4;
	tempo_dialog->m_dot = false;
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_whole_kind))) {
		tempo_dialog->m_kind = WHOLE_NOTE;
		tempo_dialog->m_dot = false;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_2))) {
		tempo_dialog->m_kind = NOTE_2;
		tempo_dialog->m_dot = false;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_2_dot))) {
		tempo_dialog->m_kind = NOTE_2;
		tempo_dialog->m_dot = true;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_4))) {
		tempo_dialog->m_kind = NOTE_4;
		tempo_dialog->m_dot = false;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_4_dot))) {
		tempo_dialog->m_kind = NOTE_4;
		tempo_dialog->m_dot = true;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_8))) {
		tempo_dialog->m_kind = NOTE_8;
		tempo_dialog->m_dot = false;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_8_dot))) {
		tempo_dialog->m_kind = NOTE_8;
		tempo_dialog->m_dot = true;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_16))) {
		tempo_dialog->m_kind = NOTE_16;
		tempo_dialog->m_dot = false;
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tempo_dialog->m_kind_16_dot))) {
		tempo_dialog->m_kind = NOTE_16;
		tempo_dialog->m_dot = true;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedTempoDialog::getValues(bool *state, unsigned int *kind, bool *dot, unsigned int *tempo) {
	*state = m_state;
	*tempo = m_tempo;
	*kind = m_kind;
	*dot = m_dot;
}
