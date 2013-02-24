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

#include <math.h>
#include "measureconfigdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"

#define MEASURE_SHOW_WINDOW_WIDTH 100
#define MEASURE_SHOW_WINDOW_HEIGHT 100


#define MIN_MEASRURE_TYPE 0
#define MAX_MEASURE_TYPE END_BAR


NedMeasureConfigDialog::NedMeasureConfigDialog(GtkWindow *parent, int measure_type, bool alter1, bool alter2, bool allow_alter, bool hide_following, int num, int denom, unsigned int sym) :
m_measure_type(measure_type & REP_TYPE_MASK), m_num(num), m_denom(denom), m_symbol(sym), m_hide_following(hide_following), m_allow_alter(allow_alter), m_delete_timesig(false)  {
	GtkWidget *dialog;
	GtkWidget *hbox;
	GtkWidget *measure_type_frame;
	GtkWidget *measure_type_hbox;
	GtkWidget *measure_type_vbox;
	GtkWidget *measure_type_buttons_vbox;
	GtkWidget *time_signature_frame;
	GtkWidget *numerator_label;
	GtkWidget *denominator_label;
	GtkWidget *time_signature_vbox;
	GtkWidget *time_signature_upper_hbox;
	GtkWidget *time_signature_lower_hbox;
	GtkWidget *alter_box;
	GdkColor bgcolor;

	dialog = gtk_dialog_new_with_buttons(_("Measure type"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);
	m_original_type = (measure_type & REP_TYPE_MASK);


	hbox = gtk_hbox_new(FALSE, 5);
	measure_type_hbox = gtk_hbox_new(FALSE, 5);
	measure_type_frame = gtk_frame_new(_("measure type"));
	m_measure_type_show_window = gtk_drawing_area_new ();
	bgcolor.pixel = 1;
	bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
	gtk_widget_modify_bg(GTK_WIDGET(m_measure_type_show_window), GTK_STATE_NORMAL, &bgcolor);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_measure_type_show_window), MEASURE_SHOW_WINDOW_WIDTH, MEASURE_SHOW_WINDOW_HEIGHT);
	gtk_container_add (GTK_CONTAINER(measure_type_hbox), m_measure_type_show_window);

	measure_type_buttons_vbox = gtk_vbox_new(FALSE, 2);
	m_meas_bu_up = gtk_button_new_from_stock("my-arrow-up-icon");
	g_signal_connect (m_meas_bu_up, "pressed", G_CALLBACK(previous_measure_type), (void *) this);
	m_meas_bu_down = gtk_button_new_from_stock ("my-arrow-down-icon");
	g_signal_connect (m_meas_bu_down, "pressed", G_CALLBACK(next_measure_type), (void *) this);
	gtk_box_pack_start(GTK_BOX(measure_type_buttons_vbox), m_meas_bu_up, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(measure_type_buttons_vbox), m_meas_bu_down, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(measure_type_hbox), measure_type_buttons_vbox, FALSE, FALSE, 0);

	if (m_measure_type == MIN_MEASRURE_TYPE) {
		gtk_widget_set_sensitive(m_meas_bu_up, FALSE);
	}
	if (m_measure_type == MAX_MEASURE_TYPE) {
		gtk_widget_set_sensitive(m_meas_bu_down, FALSE);
	}

	g_signal_connect (m_measure_type_show_window, "expose-event", G_CALLBACK (draw_measure_types), (void *) this);

	alter_box = gtk_vbox_new(FALSE, 5);
	m_alter1_check_box = gtk_check_button_new_with_label(_("alternative 1"));
	m_alter2_check_box = gtk_check_button_new_with_label(_("alternative 2"));
	g_signal_connect(m_alter1_check_box, "toggled", G_CALLBACK(alter1_checked), (void *) this);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_alter1_check_box), alter1 && m_allow_alter);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_alter2_check_box), alter2 && m_allow_alter);
	gtk_widget_set_sensitive(m_alter1_check_box, m_allow_alter && measure_type == REPEAT_CLOSE);
	gtk_widget_set_sensitive(m_alter2_check_box, (measure_type == REPEAT_CLOSE) && alter1 && m_allow_alter);
	gtk_box_pack_start(GTK_BOX(alter_box), m_alter1_check_box, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(alter_box), m_alter2_check_box, TRUE, TRUE, 0);

	m_hide_box = gtk_check_button_new_with_label(_("hide following"));
	gtk_widget_set_sensitive(m_hide_box, (measure_type & REP_TYPE_MASK) == END_BAR || (measure_type & REP_TYPE_MASK) == REPEAT_CLOSE);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_hide_box), m_hide_following);
	gtk_box_pack_start(GTK_BOX(alter_box), m_hide_box, TRUE, TRUE, 0);

	measure_type_vbox = gtk_vbox_new(FALSE, 2);
	gtk_container_add (GTK_CONTAINER(measure_type_vbox), measure_type_hbox);
	gtk_container_add (GTK_CONTAINER(measure_type_vbox), alter_box);
	gtk_container_add (GTK_CONTAINER(measure_type_frame), measure_type_vbox);
	gtk_container_add (GTK_CONTAINER(hbox), measure_type_frame);

	time_signature_frame = gtk_frame_new(_("time signature"));
	time_signature_vbox = gtk_vbox_new(FALSE, 5);
	time_signature_upper_hbox = gtk_hbox_new(FALSE, 5);
	time_signature_lower_hbox = gtk_hbox_new(FALSE, 5);
	numerator_label = gtk_label_new(_("Numerator:"));
	denominator_label = gtk_label_new(_("Denominator:"));
	m_numerator = gtk_spin_button_new_with_range (1.0, 20.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), m_num);
	m_denominator = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "8");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "16");
	if (m_denom < 4) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 0);
	}
	else if (m_denom < 8) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 1);
	}
	else if (m_denom < 16) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 2);
	}
	else {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 3);
	}
	m_com_time_bu = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(m_com_time_bu),  gtk_image_new_from_stock("comm-icon", GTK_ICON_SIZE_SMALL_TOOLBAR));
	m_cut_time_bu = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(m_cut_time_bu),  gtk_image_new_from_stock("cut-icon", GTK_ICON_SIZE_SMALL_TOOLBAR));
	g_signal_connect(m_com_time_bu, "toggled", G_CALLBACK (OnCommButtonToggled), (void *) this);
	g_signal_connect(m_cut_time_bu, "toggled", G_CALLBACK (OnCutButtonToggled), (void *) this);
	switch (sym) {
		 case TIME_SYMBOL_COMMON_TIME: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_com_time_bu), TRUE);
		 			gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), 4);
		 			gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 1);
		 			break;
		 case TIME_SYMBOL_CUT_TIME: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_cut_time_bu), TRUE);
		 			gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), 2);
					gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 0);
					break;
		default:                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_cut_time_bu), FALSE);
					gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_com_time_bu), FALSE);
					break;
	}
	gtk_widget_set_sensitive(m_denominator, FALSE);
	gtk_widget_set_sensitive(m_numerator, FALSE);
	gtk_widget_set_sensitive(m_com_time_bu, FALSE);
	gtk_widget_set_sensitive(m_cut_time_bu, FALSE);
	m_timsig_check_box = gtk_check_button_new_with_label(_("enable timesig"));
	g_signal_connect(m_timsig_check_box, "toggled", G_CALLBACK(OnTimesigToggeled), (void *) this);
	m_delete_timsig_check_box = gtk_check_button_new_with_label(_("delete timesig"));
	g_signal_connect(m_delete_timsig_check_box, "toggled", G_CALLBACK(delete_time_sig_checked), (void *) this);
	gtk_widget_set_sensitive(m_delete_timsig_check_box, FALSE);
	gtk_box_pack_start (GTK_BOX (time_signature_upper_hbox), numerator_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_upper_hbox), m_com_time_bu, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_upper_hbox), m_numerator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_lower_hbox), denominator_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_lower_hbox), m_cut_time_bu, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_lower_hbox), m_denominator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), time_signature_upper_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), time_signature_lower_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), m_timsig_check_box, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), m_delete_timsig_check_box, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(time_signature_frame), time_signature_vbox);
	gtk_container_add (GTK_CONTAINER(hbox), time_signature_frame);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), hbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedMeasureConfigDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedMeasureConfigDialog *measure_type_dialog = (NedMeasureConfigDialog *) data;
	measure_type_dialog->m_alter1 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_alter1_check_box));
	measure_type_dialog->m_alter2 = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_alter2_check_box));
	measure_type_dialog->m_hide_following = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_hide_box));
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			measure_type_dialog->m_state = TRUE;
			measure_type_dialog->m_timesig_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_timsig_check_box));
		break;
		default:
			measure_type_dialog->m_state = FALSE;
			measure_type_dialog->m_timesig_state = FALSE;
		break;
	}
	measure_type_dialog->m_num = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(measure_type_dialog->m_numerator));
	switch (gtk_combo_box_get_active(GTK_COMBO_BOX(measure_type_dialog->m_denominator))) {
		case 0: measure_type_dialog->m_denom = 2; break;
		case 1: measure_type_dialog->m_denom = 4; break;
		case 2: measure_type_dialog->m_denom = 8; break;
		default: measure_type_dialog->m_denom = 16; break;
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_com_time_bu))) {
		measure_type_dialog->m_symbol = TIME_SYMBOL_COMMON_TIME;
		measure_type_dialog->m_num = measure_type_dialog->m_denom = 4;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_cut_time_bu))) {
		measure_type_dialog->m_symbol = TIME_SYMBOL_CUT_TIME;
		measure_type_dialog->m_num = measure_type_dialog->m_denom = 2;
	}
	else {
		measure_type_dialog->m_symbol = TIME_SYMBOL_NONE;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedMeasureConfigDialog::OnTimesigToggeled(GtkToggleButton *togglebutton, gpointer data) {
	NedMeasureConfigDialog *measure_type_dialog = (NedMeasureConfigDialog *) data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_timsig_check_box))) {
		 gtk_widget_set_sensitive(measure_type_dialog->m_com_time_bu, TRUE);
		 gtk_widget_set_sensitive(measure_type_dialog->m_cut_time_bu, TRUE);
		 if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_com_time_bu))) {
		 	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_cut_time_bu), FALSE);
		 	gtk_widget_set_sensitive(measure_type_dialog->m_delete_timsig_check_box, TRUE);
		 }
		 else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_cut_time_bu))) {
		 	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_com_time_bu), FALSE);
		 	gtk_widget_set_sensitive(measure_type_dialog->m_delete_timsig_check_box, TRUE);
		 }
		 else {
		 	gtk_widget_set_sensitive(measure_type_dialog->m_numerator, TRUE);
		 	gtk_widget_set_sensitive(measure_type_dialog->m_denominator, TRUE);
		 	gtk_widget_set_sensitive(measure_type_dialog->m_delete_timsig_check_box, TRUE);
		 }
	}
	else {
		 gtk_widget_set_sensitive(measure_type_dialog->m_numerator, FALSE);
		 gtk_widget_set_sensitive(measure_type_dialog->m_denominator, FALSE);
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_delete_timsig_check_box), FALSE);
		 gtk_widget_set_sensitive(measure_type_dialog->m_delete_timsig_check_box, FALSE);
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_cut_time_bu), FALSE);
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(measure_type_dialog->m_com_time_bu), FALSE);
		 gtk_widget_set_sensitive(measure_type_dialog->m_cut_time_bu, FALSE);
		 gtk_widget_set_sensitive(measure_type_dialog->m_com_time_bu, FALSE);
		 measure_type_dialog->m_delete_timesig = false;
	}
}

void NedMeasureConfigDialog::delete_time_sig_checked (GtkToggleButton *togglebutton, gpointer data) {
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;
	the_dialog->m_delete_timesig = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_delete_timsig_check_box));
}

void NedMeasureConfigDialog::getValues(bool *state, int *measure_type, bool *alter1, bool *alter2, bool *hide_following, bool *timsig_state, bool *delete_timesig, int *num, int *denom, unsigned int *sym ) {
	*state = m_state;
	*measure_type = m_measure_type;
	*alter1 = m_alter1 && m_measure_type == 2;
	*alter2 = m_alter2 && m_measure_type == 2;
	*hide_following = m_hide_following;
	*num = m_num;
	*denom = m_denom;
	*timsig_state = m_timesig_state;
	*delete_timesig = m_delete_timesig;
	*sym = m_symbol;
}

gboolean NedMeasureConfigDialog::draw_measure_types(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
#define ZOOM_FACTOR 40.0
#define LINE_XPOS  50.0
#define MINY 30.0
#define MAXY (MINY + ZOOM_FACTOR * 4 *  LINE_DIST)
#define MINI_STAFF_START (LINE_XPOS - 30.0)
#define MINI_STAFF_END (MINI_STAFF_START + 60.0)
	int i;
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;
	cairo_t *cr;

	cr = gdk_cairo_create (the_dialog->m_measure_type_show_window->window);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  0, 0,  the_dialog->m_measure_type_show_window->allocation.width, the_dialog->m_measure_type_show_window->allocation.height);
	cairo_fill(cr);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	switch (the_dialog->m_measure_type) {
		case REPEAT_OPEN: cairo_new_path(cr);
				  cairo_set_line_width(cr, ZOOM_FACTOR * REPEAT_MEASURE_LINE_THICK);
				  cairo_move_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MINY);
				  cairo_line_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MAXY);
				  cairo_stroke(cr);
				  cairo_new_path(cr);
				  cairo_arc(cr, LINE_XPOS + (REP_THICK_LINE_DIST + REP_DOTS_X_DIST) * ZOOM_FACTOR, MINY + REP_DOTS_Y1DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				  cairo_arc(cr, LINE_XPOS + (REP_THICK_LINE_DIST + REP_DOTS_X_DIST) * ZOOM_FACTOR, MINY + REP_DOTS_Y2DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				  cairo_fill(cr);
				  break;
		case REPEAT_CLOSE: cairo_new_path(cr);
				  cairo_set_line_width(cr, ZOOM_FACTOR * REPEAT_MEASURE_LINE_THICK);
				  cairo_move_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MINY);
				  cairo_line_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MAXY);
				  cairo_stroke(cr);
				  cairo_new_path(cr);
				  cairo_arc(cr, LINE_XPOS - REP_DOTS_X_DIST * 0.8 * ZOOM_FACTOR, MINY + REP_DOTS_Y1DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				  cairo_arc(cr, LINE_XPOS - REP_DOTS_X_DIST * 0.8 * ZOOM_FACTOR, MINY + REP_DOTS_Y2DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				  cairo_fill(cr);
				  break;
		case REPEAT_OPEN_CLOSE:
				cairo_new_path(cr);
				cairo_set_line_width(cr, ZOOM_FACTOR * REPEAT_MEASURE_LINE_THICK);
				cairo_move_to(cr, LINE_XPOS, MINY);
				cairo_line_to(cr, LINE_XPOS, MAXY);
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_set_line_width(cr, ZOOM_FACTOR * MEASURE_LINE_THICK);
				cairo_move_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MINY);
				cairo_line_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MAXY);
				cairo_move_to(cr, LINE_XPOS - REP_THICK_LINE_DIST * ZOOM_FACTOR, MINY);
				cairo_line_to(cr, LINE_XPOS - REP_THICK_LINE_DIST * ZOOM_FACTOR, MAXY);
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_arc(cr, LINE_XPOS - (REP_THICK_LINE_DIST + REP_DOTS_X_DIST) * ZOOM_FACTOR, MINY + REP_DOTS_Y1DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				cairo_arc(cr, LINE_XPOS - (REP_THICK_LINE_DIST + REP_DOTS_X_DIST) * ZOOM_FACTOR, MINY + REP_DOTS_Y2DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
				cairo_new_path(cr);
				cairo_arc(cr, LINE_XPOS + (REP_THICK_LINE_DIST + REP_DOTS_X_DIST) * ZOOM_FACTOR, MINY + REP_DOTS_Y1DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				cairo_arc(cr, LINE_XPOS + (REP_THICK_LINE_DIST + REP_DOTS_X_DIST) * ZOOM_FACTOR, MINY + REP_DOTS_Y2DIST * ZOOM_FACTOR,
				  		REP_DOTS_RADIUS * ZOOM_FACTOR, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
				break;
		case END_BAR:   cairo_new_path(cr);
				cairo_set_line_width(cr, ZOOM_FACTOR * REPEAT_MEASURE_LINE_THICK);
				cairo_move_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MINY);
				cairo_line_to(cr, LINE_XPOS + REP_THICK_LINE_DIST * ZOOM_FACTOR, MAXY);
				cairo_stroke(cr);
				break;
		case DOUBLE_BAR:cairo_new_path(cr);
				cairo_set_line_width(cr, ZOOM_FACTOR * MEASURE_LINE_THICK);
				cairo_move_to(cr, LINE_XPOS - REP_DOUBLE_LINE_DIST * ZOOM_FACTOR, MINY);
				cairo_line_to(cr, LINE_XPOS - REP_DOUBLE_LINE_DIST * ZOOM_FACTOR, MAXY);
				cairo_stroke(cr);
				break;
	}
	if (the_dialog->m_measure_type != REPEAT_OPEN_CLOSE) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, ZOOM_FACTOR * MEASURE_LINE_THICK);
		cairo_move_to(cr, LINE_XPOS, MINY);
		cairo_line_to(cr, LINE_XPOS, MAXY);
		cairo_stroke(cr);
	}
	cairo_set_line_width(cr, ZOOM_FACTOR * LINE_THICK);
	for (i = 0; i < 5; i++) {
		cairo_move_to(cr, MINI_STAFF_START, MINY + i * ZOOM_FACTOR * LINE_DIST);
		cairo_line_to(cr, MINI_STAFF_END, MINY + i * ZOOM_FACTOR * LINE_DIST);
	}
	cairo_stroke(cr);
	cairo_destroy (cr);
	return FALSE;
}

void NedMeasureConfigDialog::previous_measure_type (GtkButton *button, gpointer data) {
	GdkRectangle rect;
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;
	if (the_dialog->m_measure_type <= MIN_MEASRURE_TYPE) return;
	the_dialog->m_measure_type--;
	gtk_widget_set_sensitive(the_dialog->m_meas_bu_up, TRUE);
	gtk_widget_set_sensitive(the_dialog->m_meas_bu_down, TRUE);
	if (the_dialog->m_measure_type == MIN_MEASRURE_TYPE) {
		gtk_widget_set_sensitive(the_dialog->m_meas_bu_up, FALSE);
	}
	if (the_dialog->m_allow_alter && the_dialog->m_measure_type == 2) {
		gtk_widget_set_sensitive(the_dialog->m_alter1_check_box, TRUE);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_alter1_check_box))) {
			gtk_widget_set_sensitive(the_dialog->m_alter2_check_box, TRUE);
		}
	}
	else {
		gtk_widget_set_sensitive(the_dialog->m_alter1_check_box, FALSE);
		gtk_widget_set_sensitive(the_dialog->m_alter2_check_box, FALSE);
	}
	if ((the_dialog->m_measure_type & REP_TYPE_MASK) == END_BAR || (the_dialog->m_measure_type & REP_TYPE_MASK) == REPEAT_CLOSE) {
		gtk_widget_set_sensitive(the_dialog->m_hide_box, TRUE);
		if ((the_dialog->m_measure_type & REP_TYPE_MASK) != the_dialog->m_original_type) {
			if ((the_dialog->m_measure_type & REP_TYPE_MASK) == END_BAR) {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), TRUE);
			}
			else {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), FALSE);
			}
		}
		else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), the_dialog->m_hide_following);
		}
	}
	else {
		gtk_widget_set_sensitive(the_dialog->m_hide_box, TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), FALSE);
		gtk_widget_set_sensitive(the_dialog->m_hide_box, FALSE);
	}
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_measure_type_show_window->allocation.width;
	rect.height = the_dialog->m_measure_type_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_measure_type_show_window->window, &rect, FALSE);
}

void NedMeasureConfigDialog::next_measure_type (GtkButton *button, gpointer data) {
	GdkRectangle rect;
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;
	if (the_dialog->m_measure_type >= MAX_MEASURE_TYPE) return;
	the_dialog->m_measure_type++;
	gtk_widget_set_sensitive(the_dialog->m_meas_bu_up, TRUE);
	gtk_widget_set_sensitive(the_dialog->m_meas_bu_down, TRUE);
	if (the_dialog->m_measure_type >= MAX_MEASURE_TYPE) {
		gtk_widget_set_sensitive(the_dialog->m_meas_bu_down, FALSE);
	}
	if (the_dialog->m_allow_alter && the_dialog->m_measure_type == 2) {
		gtk_widget_set_sensitive(the_dialog->m_alter1_check_box, TRUE);
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_alter1_check_box))) {
			gtk_widget_set_sensitive(the_dialog->m_alter2_check_box, TRUE);
		}
	}
	else {
		gtk_widget_set_sensitive(the_dialog->m_alter1_check_box, FALSE);
		gtk_widget_set_sensitive(the_dialog->m_alter2_check_box, FALSE);
	}
	if ((the_dialog->m_measure_type & REP_TYPE_MASK) == END_BAR || (the_dialog->m_measure_type & REP_TYPE_MASK) == REPEAT_CLOSE) {
		gtk_widget_set_sensitive(the_dialog->m_hide_box, TRUE);
		if ((the_dialog->m_measure_type & REP_TYPE_MASK) != the_dialog->m_original_type) {
			if ((the_dialog->m_measure_type & REP_TYPE_MASK) == END_BAR) {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), TRUE);
			}
			else {
				gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), FALSE);
			}
		}
		else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), the_dialog->m_hide_following);
		}
	}
	else {
		gtk_widget_set_sensitive(the_dialog->m_hide_box, TRUE);
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_hide_box), FALSE);
		gtk_widget_set_sensitive(the_dialog->m_hide_box, FALSE);
	}
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_measure_type_show_window->allocation.width;
	rect.height = the_dialog->m_measure_type_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_measure_type_show_window->window, &rect, FALSE);
}

void NedMeasureConfigDialog::alter1_checked (GtkToggleButton *togglebutton, gpointer data) {
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_alter1_check_box))) {
		gtk_widget_set_sensitive(the_dialog->m_alter2_check_box, TRUE);
	}
	else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_alter2_check_box), FALSE);
		gtk_widget_set_sensitive(the_dialog->m_alter2_check_box, FALSE);
	}
}

void NedMeasureConfigDialog::OnCommButtonToggled(GtkToggleButton *togglebutton, gpointer data) {
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_com_time_bu))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_cut_time_bu), FALSE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(the_dialog->m_numerator), 4);
		gtk_combo_box_set_active(GTK_COMBO_BOX(the_dialog->m_denominator), 1);
		gtk_widget_set_sensitive(the_dialog->m_numerator, FALSE);
		gtk_widget_set_sensitive(the_dialog->m_denominator, FALSE);
	}
	else if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_cut_time_bu))) {
		gtk_widget_set_sensitive(the_dialog->m_numerator, TRUE);
		gtk_widget_set_sensitive(the_dialog->m_denominator, TRUE);
	}
}

void NedMeasureConfigDialog::OnCutButtonToggled(GtkToggleButton *togglebutton, gpointer data) {
	NedMeasureConfigDialog *the_dialog = (NedMeasureConfigDialog *) data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_cut_time_bu))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_com_time_bu), FALSE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(the_dialog->m_numerator), 2);
		gtk_combo_box_set_active(GTK_COMBO_BOX(the_dialog->m_denominator), 0);
		gtk_widget_set_sensitive(the_dialog->m_numerator, FALSE);
		gtk_widget_set_sensitive(the_dialog->m_denominator, FALSE);
	}
	else if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_com_time_bu))) {
		gtk_widget_set_sensitive(the_dialog->m_numerator, TRUE);
		gtk_widget_set_sensitive(the_dialog->m_denominator, TRUE);
	}
}


