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

#include "textdialog.h"
#include "localization.h"
#include "resource.h"
#include "freetext.h"
#include "pangocairotext.h"
#include "config.h"

const char *NedTextDialog::m_static_font_family = TEXT_FONT_NAME_DEFAULT;
double NedTextDialog::m_static_font_size = TEXT_FONT_SIZE_DEFAULT;
PangoStyle NedTextDialog::m_static_font_slant = TEXT_FONT_SLANT_DEFAULT;
PangoWeight NedTextDialog::m_static_font_weight = TEXT_FONT_WEIGHT_DEFAULT;

NedTextDialog::NedTextDialog(GtkWindow *parent, char *text, int anchor,  unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
	unsigned short segno_sign, int midi_program, int channel, bool enable_channel, 
	const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight) :
	m_tempo(tempo), m_volume(volume), m_enable_tempo(enable_tempo), m_enable_volume(enable_volume),
	 m_segno_sign(segno_sign), m_midi_program_number(midi_program), m_channel(channel), m_enable_channel(enable_channel), m_anchor(anchor), 
		m_text(text), m_font_family(font_family), m_font_size(font_size), m_font_slant(font_slant), m_font_weight(font_weight) {
	GtkWidget *dialog;
	GtkWidget *text_frame;
	GtkWidget *font_vbox;
	GtkWidget *volume_frame;
	GtkWidget *volume_hbox;
	GtkWidget *tempo_frame;
	GtkWidget *anchor_frame;
	GtkWidget *tempo_hbox;
	GtkWidget *ds_frame;
	GtkWidget *ds_v_box;
	GtkWidget *anchor_h_box;
	GtkWidget *instr_frame;
	GtkWidget *channel_frame;
	GtkWidget *channel_hbox;

	GtkListStore *instrumend_list_store;
	GtkTreeIter iter;
	GtkCellRenderer *instrument_renderer;
	GtkTreeViewColumn *instrument_column;
	GtkTreeViewColumn *instrument_num_column;
	GtkTreePath* instrument_tree_path;

	GtkWidget *main_v_box;

	char *fontstring;
	PangoFontDescription *f_descr;
	int i;
	char Str[128];

	dialog = gtk_dialog_new_with_buttons(_("Text"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);

	if (text == NULL) {
		m_font_family = m_static_font_family;
		m_font_slant = m_static_font_slant;
		m_font_weight = m_static_font_weight;
		font_size = m_static_font_size;
	}
		

	f_descr = pango_font_description_new();
	pango_font_description_set_family(f_descr, m_font_family);
	pango_font_description_set_style(f_descr, m_font_slant);
	pango_font_description_set_weight(f_descr, m_font_weight);
	pango_font_description_set_size(f_descr, PS_ZOOM * NedPangoCairoText::getInternalFactor() * font_size);
	fontstring = pango_font_description_to_string(f_descr);
	

	text_frame = gtk_frame_new(_("text"));
	m_text_widget = gtk_entry_new_with_max_length(50);
	if (m_text != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_text_widget), m_text);
	}

	m_font_button = gtk_font_button_new_with_font(fontstring);
	g_free(fontstring);
	pango_font_description_free(f_descr);
	gtk_font_button_set_title(GTK_FONT_BUTTON(m_font_button), _("select font"));
	font_vbox =  gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(font_vbox), m_text_widget, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(font_vbox), m_font_button, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(text_frame), font_vbox);
	g_signal_connect(m_font_button, "font-set", G_CALLBACK(FontCallBack), (void *) this);

	anchor_frame = gtk_frame_new(_("anchor"));
	anchor_h_box = gtk_hbox_new(FALSE, 2);

	m_anchor_left = gtk_radio_button_new_with_label(NULL, _("left"));
	m_anchor_mid = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_anchor_left), _("mid"));
	m_anchor_right = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_anchor_left), _("right"));

	gtk_box_pack_start(GTK_BOX(anchor_h_box), m_anchor_left, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(anchor_h_box), m_anchor_mid, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(anchor_h_box), m_anchor_right, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(anchor_frame), anchor_h_box);

	switch (m_anchor) {
		case ANCHOR_LEFT: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_anchor_left), TRUE); break;
		case ANCHOR_RIGHT: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_anchor_right), TRUE); break;
		default: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_anchor_mid), TRUE); break;
	}

	volume_frame = gtk_frame_new(_("volume"));
	volume_hbox = gtk_hbox_new(FALSE, 2);

	m_volume_spin_box = gtk_spin_button_new_with_range (0.0, 127.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_volume_spin_box), volume);
	gtk_widget_set_sensitive(m_volume_spin_box, m_enable_volume);

	m_check_volume = gtk_check_button_new_with_label(_("enable volume change"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_volume), m_enable_volume);
	gtk_box_pack_start(GTK_BOX(volume_hbox), m_check_volume, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(volume_hbox), m_volume_spin_box, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(volume_frame), volume_hbox);
	g_signal_connect(m_check_volume, "toggled", G_CALLBACK(VolumeToggled), (void *) this);


	tempo_frame = gtk_frame_new(_("tempo"));
	tempo_hbox = gtk_hbox_new(FALSE, 2);
	m_tempo_spin_box = gtk_spin_button_new_with_range (10.0, 999.0, 1.0);
	gtk_widget_set_sensitive(m_tempo_spin_box, m_enable_tempo);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_tempo_spin_box), tempo);
	m_check_tempo = gtk_check_button_new_with_label(_("enable tempo change"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_tempo), m_enable_tempo);
	gtk_box_pack_start(GTK_BOX(tempo_hbox), m_check_tempo, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(tempo_hbox), m_tempo_spin_box, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(tempo_frame), tempo_hbox);
	g_signal_connect(m_check_tempo, "toggled", G_CALLBACK(TempoToggled), (void *) this);

	channel_frame = gtk_frame_new(_("channel"));
	channel_hbox = gtk_hbox_new(FALSE, 2);
	m_channel_spin_box = gtk_spin_button_new_with_range (1.0, 16.0, 1.0);
	gtk_widget_set_sensitive(m_channel_spin_box, m_enable_channel);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_channel_spin_box), channel + 1);
	m_check_channel = gtk_check_button_new_with_label(_("enable channel change"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_channel), m_enable_channel);
	gtk_box_pack_start(GTK_BOX(channel_hbox), m_check_channel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(channel_hbox), m_channel_spin_box, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(channel_frame), channel_hbox);
	g_signal_connect(m_check_channel, "toggled", G_CALLBACK(ChannelToggled), (void *) this);

	ds_frame = gtk_frame_new(_("D.S"));
	ds_v_box = gtk_vbox_new(FALSE, 2);

	m_segno_none = gtk_radio_button_new_with_label(NULL, _("No segno meaning"));
	m_ds_al_fine = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_segno_none), _("D.S. al Fine"));
	m_ds_al_coda = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_segno_none), _("D.S. al Coda"));
	m_d_capo_al_fine = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_segno_none), _("D. Capo al Fine"));
	m_d_capo_al_coda = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_segno_none), _("D. Capo al Coda"));
	m_fine = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_segno_none), _("Fine"));
	m_al_coda = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_segno_none), _("al Coda"));

	switch(segno_sign) {
		case SEGNO_D_S_AL_FINE: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_ds_al_fine), TRUE); break;
		case SEGNO_D_S_AL_CODA: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_ds_al_coda), TRUE); break;
		case SEGNO_D_CAPO_AL_FINE: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_d_capo_al_fine), TRUE); break;
		case SEGNO_D_CAPO_AL_CODA: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_d_capo_al_coda), TRUE); break;
		case SEGNO_FINE: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_fine), TRUE); break;
		case SEGNO_AL_CODA: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_al_coda), TRUE); break;
		default: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_segno_none), TRUE); break;
	}

	gtk_box_pack_start(GTK_BOX(ds_v_box), m_segno_none, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ds_v_box), m_ds_al_fine, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ds_v_box), m_ds_al_coda, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ds_v_box), m_d_capo_al_fine, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ds_v_box), m_d_capo_al_coda, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ds_v_box), m_fine, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ds_v_box), m_al_coda, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(ds_frame), ds_v_box);

	instrumend_list_store = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);

	for (i = 0; i < NedResource::getNumInstruments(); i++) {
		gtk_list_store_append (instrumend_list_store, &iter);
		gtk_list_store_set (instrumend_list_store, &iter, 0, i, 1, gettext(NedResource::GM_Instruments[i]), -1);
	}
	m_instrument_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(instrumend_list_store));
	instrument_renderer = gtk_cell_renderer_text_new ();
	instrument_num_column = gtk_tree_view_column_new_with_attributes (_("no"), instrument_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_instrument_list), instrument_num_column);
	instrument_column = gtk_tree_view_column_new_with_attributes (_("instruments"), instrument_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_instrument_list), instrument_column);
	m_instrument_adjustment = gtk_adjustment_new (0.0, 0.0, NedResource::getNumInstruments(), 1.0,  10.0, 10.0);
	m_instrument_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(m_instrument_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_instrument_list), GTK_ADJUSTMENT(m_instrument_adjustment));
	gtk_widget_set_size_request(m_instrument_list, 150, 100);
	sprintf(Str, "%d", m_midi_program_number < 0 ? 0: m_midi_program_number);
	instrument_tree_path = gtk_tree_path_new_from_string(Str);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (m_instrument_list), instrument_tree_path, NULL, FALSE, 0.0, 0.0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW (m_instrument_list), instrument_tree_path, NULL, FALSE);
	g_signal_connect(dialog, "scroll-event", G_CALLBACK (OnScroll), (void *) this);

	if (m_midi_program_number < 0) {
		gtk_widget_set_sensitive(m_instrument_list, FALSE);
		gtk_widget_set_sensitive(m_instrument_scroll, FALSE);
	}

	instr_frame = gtk_frame_new(_("instrument"));
	m_check_instr = gtk_check_button_new_with_label(_("enable instrument change"));
	gtk_container_add (GTK_CONTAINER(instr_frame), m_check_instr);

	main_v_box = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_v_box), anchor_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_v_box), text_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_v_box), volume_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_v_box), tempo_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_v_box), channel_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_v_box), instr_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_v_box), ds_frame, FALSE, FALSE, 0);

	m_main_h_box = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(m_main_h_box), main_v_box, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(m_main_h_box), m_instrument_scroll, TRUE, TRUE, 0);
	gtk_box_pack_end(GTK_BOX(m_main_h_box), m_instrument_list, TRUE, TRUE, 0);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_check_instr), m_midi_program_number >= 0);
	g_signal_connect(m_check_instr, "toggled", G_CALLBACK(InstrToggled), (void *) this);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), m_main_h_box);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));
}

void NedTextDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedTextDialog *text_dialog = (NedTextDialog *) data;
	const gchar *newtext;
	GtkTreeSelection* selection;
	GList *selected_rows;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			text_dialog->m_state = TRUE;
		break;
		default:
			text_dialog->m_state = FALSE;
		break;
	}
	if (text_dialog->m_state) {
		text_dialog->m_text = NULL;
		newtext = gtk_entry_get_text(GTK_ENTRY(text_dialog->m_text_widget));
		if (strlen(newtext) > 0) {
			text_dialog->m_text = strdup(newtext);
		}
		text_dialog->m_enable_tempo = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(text_dialog->m_check_tempo));
		text_dialog->m_enable_volume = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(text_dialog->m_check_volume));
		text_dialog->m_volume = (int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(text_dialog->m_volume_spin_box)) + 0.4);
		text_dialog->m_tempo = (unsigned int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(text_dialog->m_tempo_spin_box)) + 0.4);
		text_dialog->m_enable_channel = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(text_dialog->m_check_channel));
		text_dialog->m_channel = (unsigned int) (gtk_spin_button_get_value(GTK_SPIN_BUTTON(text_dialog->m_channel_spin_box)) + 0.4) - 1;
		text_dialog->retrieveFontParams();
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_ds_al_fine))) text_dialog->m_segno_sign = SEGNO_D_S_AL_FINE;
		else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_ds_al_coda))) text_dialog->m_segno_sign = SEGNO_D_S_AL_CODA;
		else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_d_capo_al_fine))) text_dialog->m_segno_sign = SEGNO_D_CAPO_AL_FINE;
		else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_d_capo_al_coda))) text_dialog->m_segno_sign = SEGNO_D_CAPO_AL_CODA;
		else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_fine))) text_dialog->m_segno_sign = SEGNO_FINE;
		else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_al_coda))) text_dialog->m_segno_sign = SEGNO_AL_CODA;
		else text_dialog->m_segno_sign = 0;

		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_anchor_left))) text_dialog->m_anchor = ANCHOR_LEFT;
		else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (text_dialog->m_anchor_right))) text_dialog->m_anchor = ANCHOR_RIGHT;
		else text_dialog->m_anchor = ANCHOR_MID;

		selection =  gtk_tree_view_get_selection (GTK_TREE_VIEW(text_dialog->m_instrument_list));
		selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);

		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(text_dialog->m_check_instr))) {
			if ( g_list_length(selected_rows) > 0) {
				GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
				char *tp = gtk_tree_path_to_string(path);
				if (sscanf(tp, "%d", &(text_dialog->m_midi_program_number)) != 1) {
					NedResource::Abort("NedStaffContextDialog::OnClose: error reading tree path(1)");
				}
			}
			else {
				text_dialog->m_midi_program_number = -1;
			}
		}
		else {
			text_dialog->m_midi_program_number = -1;
		}
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedTextDialog::getValues(bool *state, char **text, int *anchor, unsigned int *tempo, bool *enable_tempo, unsigned int *volume, bool *enable_volume,
	unsigned short *segno_sign, int *midi_program, unsigned int *channel, bool *enable_channel, const char **font_family, double *font_size, PangoStyle *font_slant, PangoWeight *font_weight) {
	*state = m_state;
	*text = m_text;
	*font_family = m_font_family;
	*font_size = m_font_size;
	*font_slant = m_font_slant;
	*font_weight = m_font_weight;
	*tempo = m_tempo;
	*volume = m_volume;
	*enable_tempo = m_enable_tempo;
	*enable_volume = m_enable_volume;
	*enable_channel = m_enable_channel;
	*channel = m_channel;
	*segno_sign = m_segno_sign;
	*anchor = m_anchor;
	*midi_program = m_midi_program_number;
	if (m_state) {
		m_static_font_family = m_font_family;
		m_static_font_slant = m_font_slant;
		m_static_font_weight = m_font_weight;
		m_static_font_size = m_font_size;
	}
}

void NedTextDialog::FontCallBack(GtkButton *button, gpointer data) {
	NedTextDialog *text_dialog = (NedTextDialog *) data;

	text_dialog->retrieveFontParams();
}

void NedTextDialog::retrieveFontParams() {
	gchar *fontname;
	PangoFontDescription* f_descr;


	g_object_get(m_font_button, "font-name", &fontname, NULL);
	f_descr = pango_font_description_from_string(fontname);
	m_font_family = pango_font_description_get_family(f_descr);
	m_font_size = pango_font_description_get_size(f_descr) / NedPangoCairoText::getInternalFactor() / PS_ZOOM;
	m_font_slant = pango_font_description_get_style(f_descr);
	m_font_weight = pango_font_description_get_weight(f_descr);
	g_free (fontname);
}

void NedTextDialog::TempoToggled(GtkToggleButton *button, gpointer data) {
	NedTextDialog *text_dialog = (NedTextDialog *) data;

	gtk_widget_set_sensitive(text_dialog->m_tempo_spin_box, gtk_toggle_button_get_active(button));
}

void NedTextDialog::InstrToggled(GtkToggleButton *button, gpointer data) {
	NedTextDialog *text_dialog = (NedTextDialog *) data;

	if (gtk_toggle_button_get_active(button)) {
		gtk_widget_set_sensitive(text_dialog->m_instrument_list, TRUE);
		gtk_widget_set_sensitive(text_dialog->m_instrument_scroll, TRUE);
	}
	else {
		gtk_widget_set_sensitive(text_dialog->m_instrument_list, FALSE);
		gtk_widget_set_sensitive(text_dialog->m_instrument_scroll, FALSE);
	}

}

void NedTextDialog::VolumeToggled(GtkToggleButton *button, gpointer data) {
	NedTextDialog *text_dialog = (NedTextDialog *) data;

	gtk_widget_set_sensitive(text_dialog->m_volume_spin_box, gtk_toggle_button_get_active(button));
}

void NedTextDialog::ChannelToggled(GtkToggleButton *button, gpointer data) {
	NedTextDialog *text_dialog = (NedTextDialog *) data;

	gtk_widget_set_sensitive(text_dialog->m_channel_spin_box, gtk_toggle_button_get_active(button));
}

gboolean NedTextDialog::OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
#define SCROLL_INCR 20.0
	double adjval;
	NedTextDialog *text_dialog = (NedTextDialog *) data;
	adjval = gtk_adjustment_get_value(GTK_ADJUSTMENT(text_dialog->m_instrument_adjustment));

	if (event->direction == GDK_SCROLL_UP) {
		adjval -= SCROLL_INCR;
	}
	else if (event->direction == GDK_SCROLL_DOWN) {
		adjval += SCROLL_INCR;
	}
	gtk_adjustment_set_value(GTK_ADJUSTMENT(text_dialog->m_instrument_adjustment), adjval);
	return FALSE;
}
