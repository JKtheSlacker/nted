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

#include "clefconfigdialog.h"
#include "localization.h"
#include "resource.h"

#define CLEF_SHOW_WINDOW_WIDTH 90
#define CLEF_SHOW_WINDOW_HEIGHT 120




NedClefConfigDialog::NedClefConfigDialog(GtkWindow *parent, int clef_number, int octave_shift) :
m_octave_shift(octave_shift), m_state(false), m_current_clef(clef_number) {
	GtkWidget *dialog;
	GtkWidget *clef_hbox;
	GtkWidget *clef_buttons_vbox;
	GtkWidget *clef_frame;
	GtkWidget *octave_shift_label;
	GtkWidget *octave_shift_vbox;
	GdkColor bgcolor;

	m_newclef = m_current_clef;

	dialog = gtk_dialog_new_with_buttons(_("Clef configuration"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);


	clef_frame = gtk_frame_new(_("clef"));
	m_clef_show_window = gtk_drawing_area_new ();
	bgcolor.pixel = 1;
	bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
	gtk_widget_modify_bg(GTK_WIDGET(m_clef_show_window), GTK_STATE_NORMAL, &bgcolor);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_clef_show_window), CLEF_SHOW_WINDOW_WIDTH, CLEF_SHOW_WINDOW_HEIGHT);

	clef_hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(clef_hbox), m_clef_show_window, FALSE, FALSE, 0);
	clef_buttons_vbox = gtk_vbox_new(FALSE, 2);
	m_clef_bu_up = gtk_button_new_from_stock("my-arrow-up-icon");
	g_signal_connect (m_clef_bu_up, "pressed", G_CALLBACK(decrease_clef_number), (void *) this);
	m_clef_bu_down = gtk_button_new_from_stock ("my-arrow-down-icon");
	g_signal_connect (m_clef_bu_down, "pressed", G_CALLBACK(increase_clef_number), (void *) this);
	gtk_box_pack_start(GTK_BOX(clef_buttons_vbox), m_clef_bu_up, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(clef_buttons_vbox), m_clef_bu_down, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(clef_hbox), clef_buttons_vbox, FALSE, FALSE, 0);

	octave_shift_vbox = gtk_vbox_new(FALSE, 2);
	switch (m_octave_shift) {
		case 12: m_octave_shift = 8; break;
		case -12: m_octave_shift = -8; break;
	}
	m_octave_shift_spin_box = gtk_spin_button_new_with_range(-8.0, 8.0, 8.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_octave_shift_spin_box), m_octave_shift);
	octave_shift_label = gtk_label_new(_("Octave shift:"));
	gtk_box_pack_start (GTK_BOX (octave_shift_vbox), octave_shift_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (octave_shift_vbox), m_octave_shift_spin_box, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(clef_hbox), octave_shift_vbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(clef_frame), clef_hbox);
	g_signal_connect (m_octave_shift_spin_box, "value-changed", G_CALLBACK (octave_shift_change), (void *) this);

	m_adjust_check_box = gtk_check_button_new_with_label(_("adjust notes"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_adjust_check_box), TRUE);
	gtk_box_pack_start (GTK_BOX (octave_shift_vbox), m_adjust_check_box, FALSE, FALSE, 0);

	if (m_newclef == MINCLEF_NUMBER) {
		gtk_widget_set_sensitive(m_clef_bu_up, FALSE);
	}
	if (m_newclef == MAXCLEF_NUMBER) {
		gtk_widget_set_sensitive(m_clef_bu_down, FALSE);
	}

	g_signal_connect (m_clef_show_window, "expose-event", G_CALLBACK (draw_clefs), (void *) this);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), clef_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedClefConfigDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {

	NedClefConfigDialog *clef_dialog = (NedClefConfigDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			clef_dialog->m_state = TRUE;
		break;
		default:
			clef_dialog->m_state = FALSE;
		break;
	}
	clef_dialog->m_octave_shift = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(clef_dialog->m_octave_shift_spin_box));
	switch (clef_dialog->m_octave_shift) {
		case 8: clef_dialog->m_octave_shift = 12; break;
		case -8: clef_dialog->m_octave_shift = -12; break;
		default: clef_dialog->m_octave_shift = 0; break;
	}
	clef_dialog->m_do_adjust = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(clef_dialog->m_adjust_check_box));

	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedClefConfigDialog::getValues(bool *state, int *clef_number, int *octave_shift, bool *adjust_notes) {
	*state = m_state;
	m_current_clef = m_newclef;
	*octave_shift = m_octave_shift;
	*clef_number = m_current_clef;
	*adjust_notes = m_do_adjust;
}

gboolean NedClefConfigDialog::draw_clefs(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
#define ZOOM_LEVEL 9
	NedClefConfigDialog *the_dialog = (NedClefConfigDialog *) data;
	cairo_scaled_font_t *scaled_font;
	cairo_glyph_t glyph;
	int i;
	double y_offs = 0.0, oct_y_offs = 0.0, oct_x_offs = 0.0;
	bool octave_shift = false;
	
	cairo_t *cr;

	cr = gdk_cairo_create (the_dialog->m_clef_show_window->window);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  0, 0,  the_dialog->m_clef_show_window->allocation.width, the_dialog->m_clef_show_window->allocation.height);
	cairo_fill(cr);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	scaled_font = NedResource::getScaledFont(ZOOM_LEVEL);
#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(ZOOM_LEVEL));
	cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	cairo_new_path(cr);
	cairo_set_line_width(cr, DEMO_LINE_THICK);
	for (i = 0; i < 5; i++) {
		cairo_move_to(cr, DEMO_LINE_BEGIN, DEMO_LINE_YPOS + DEMO_LINE_DIST * i);
		cairo_line_to(cr, DEMO_LINE_END, DEMO_LINE_YPOS + DEMO_LINE_DIST * i);
	}
	cairo_stroke(cr);
	cairo_new_path(cr);
	switch (the_dialog->m_newclef) {
		case TREBLE_CLEF:
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_TREBLE_DIST_UP); octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_TREBLE_DIST_DOWN); octave_shift = true; break;
				  }
				  glyph.index = BASE + 2; break;
		case BASS_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_BASS_DIST_UP); octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_BASS_DIST_DOWN); octave_shift = true; break;
				  }
				glyph.index = BASE + 3; break;
		case ALTO_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_UP);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_DOWN);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
					         octave_shift = true; break;
				  }
				glyph.index = BASE + 1; break;
		case SOPRAN_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_UP);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_DOWN);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
				  }
				glyph.index = BASE + 1; y_offs = 2 * DEMO_LINE_DIST; break;
		case TENOR_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_UP);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_DOWN);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
				  }
				glyph.index = BASE + 1; y_offs = -DEMO_LINE_DIST; break;
		case NEUTRAL_CLEF1: 
		case NEUTRAL_CLEF2: 
		case NEUTRAL_CLEF3: 
				glyph.index = BASE + 27; break;
	}
	glyph.x = 50.0;
	glyph.y = 70.0 + y_offs;
	cairo_show_glyphs(cr, &glyph, 1);
	switch (the_dialog->m_newclef) {
		case NEUTRAL_CLEF1: glyph.index = NUMBERBASE + 1; 
					glyph.x = 60.0;
					glyph.y = 120.0;
					cairo_show_glyphs(cr, &glyph, 1);
					break;
		case NEUTRAL_CLEF2: glyph.index = NUMBERBASE + 2; 
					glyph.x = 60.0;
					glyph.y = 120.0;
					cairo_show_glyphs(cr, &glyph, 1);
					break;
		case NEUTRAL_CLEF3: glyph.index = NUMBERBASE + 3; 
					glyph.x = 60.0;
					glyph.y = 120.0;
					cairo_show_glyphs(cr, &glyph, 1);
					break;
	}
	if (octave_shift) {
		glyph.x = 50.0 + oct_x_offs;
		glyph.y = 70.0 + y_offs + oct_y_offs;
		glyph.index = BASE + 28;
		cairo_show_glyphs(cr, &glyph, 1);
	}
	cairo_destroy (cr);
	return FALSE;
}

void NedClefConfigDialog::decrease_clef_number (GtkButton *button, gpointer data) {
	GdkRectangle rect;
	NedClefConfigDialog *the_dialog = (NedClefConfigDialog *) data;
	if (the_dialog->m_newclef <= MINCLEF_NUMBER) return;
	the_dialog->m_newclef--;
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_up, TRUE);
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_down, TRUE);
	if (the_dialog->m_newclef == MINCLEF_NUMBER) {
		gtk_widget_set_sensitive(the_dialog->m_clef_bu_up, FALSE);
	}
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_clef_show_window->allocation.width;
	rect.height = the_dialog->m_clef_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_clef_show_window->window, &rect, FALSE);
}

void NedClefConfigDialog::increase_clef_number (GtkButton *button, gpointer data) {
	GdkRectangle rect;
	NedClefConfigDialog *the_dialog = (NedClefConfigDialog *) data;
	if (the_dialog->m_newclef >= MAXCLEF_NUMBER) return;
	the_dialog->m_newclef++;
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_up, TRUE);
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_down, TRUE);
	if (the_dialog->m_newclef >= MAXCLEF_NUMBER) {
		gtk_widget_set_sensitive(the_dialog->m_clef_bu_down, FALSE);
	}
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_clef_show_window->allocation.width;
	rect.height = the_dialog->m_clef_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_clef_show_window->window, &rect, FALSE);
}

void NedClefConfigDialog::octave_shift_change(GtkSpinButton *spinbutton, gpointer data) {
	GdkRectangle rect;
	NedClefConfigDialog *context_dialog = (NedClefConfigDialog *) data;

	context_dialog->m_octave_shift = (int) gtk_spin_button_get_value(spinbutton);
	switch (context_dialog->m_octave_shift) {
		case 8: context_dialog->m_octave_shift = 12; break;
		case -8: context_dialog->m_octave_shift = -12; break;
	}
	rect.x = rect.y = 0;
	rect.width = context_dialog->m_clef_show_window->allocation.width;
	rect.height = context_dialog->m_clef_show_window->allocation.height;
	gdk_window_invalidate_rect (context_dialog->m_clef_show_window->window, &rect, FALSE);
}
