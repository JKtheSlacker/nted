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
#include "linesdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"

#define BOX_WIDTH 100
#define BOX_HEIGHT 40
#define RECT_OFFS 4

NedLinesDialog::NedLinesDialog(GtkWindow *parent) : m_current_row(-1), m_current_column(-1) {
	GtkWidget *dialog;
	GtkWidget *line_frame;
	GtkWidget *line_image_widget;

	m_black.pixel = 1; m_black.red = m_black.green = m_black.blue = 0;
	m_red.pixel = 2; m_red.red = 65535; m_red.green = m_red.blue = 0;

	dialog = gtk_dialog_new_with_buttons(_("Lines"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);


	line_frame = gtk_frame_new(_("lines"));
	line_image_widget = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(line_image_widget), NedResource::m_lines_buf_width, NedResource::m_lines_buf_height);

	g_signal_connect (line_image_widget, "expose-event", G_CALLBACK (draw_menu), (void *) this);
	gtk_container_add (GTK_CONTAINER(line_frame), line_image_widget);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), line_frame);
	gtk_widget_show_all (dialog);
	m_drawing_area_gc = gdk_gc_new(line_image_widget->window);
	gtk_widget_add_events(line_image_widget, GDK_BUTTON_PRESS_MASK);
	g_signal_connect (line_image_widget, "button-press-event", G_CALLBACK (change_selection), (void *) this);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedLinesDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedLinesDialog *lines_dialog = (NedLinesDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			lines_dialog->m_state = TRUE;
		break;
		default:
			lines_dialog->m_state = FALSE;
		break;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


void NedLinesDialog::getValues(bool *state, int *linetype) {
	*state = m_state && m_current_row >= 0 && m_current_column >= 0;
	*linetype = 0;

	if (state) {
		switch (m_current_row) {
			case 0: switch(m_current_column) {
				case 0: *linetype = LINE_CRESCENDO; break;
				case 1: *linetype = LINE_DECRESCENDO; break;
			}
			break;
			case 1: switch(m_current_column) {
				case 0: *linetype = LINE_OCTAVATION1; break;
				case 1: *linetype = LINE_OCTAVATION_1; break;
			}
			break;
			case 2: switch(m_current_column) {
				case 0: *linetype = LINE_OCTAVATION2; break;
				case 1: *linetype = LINE_OCTAVATION_2; break;
			}
			break;
		}
	}
}

gboolean NedLinesDialog::draw_menu(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	NedLinesDialog *lines_dialog = (NedLinesDialog *) data;

	gdk_gc_set_foreground(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_black));
	gdk_gc_set_rgb_fg_color(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_black));
	gdk_draw_pixbuf(widget->window, lines_dialog->m_drawing_area_gc, NedResource::m_lines,
		0, 0, 0, 0, NedResource::m_lines_buf_width, NedResource::m_lines_buf_height, GDK_RGB_DITHER_NONE, 0, 0);

	if (lines_dialog->m_current_row >= 0 && lines_dialog->m_current_column >= 0) {
		gdk_gc_set_foreground(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_red));
		gdk_gc_set_rgb_fg_color(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_red));
		gdk_draw_rectangle(widget->window, lines_dialog->m_drawing_area_gc, false,
			lines_dialog->m_current_column * BOX_WIDTH + RECT_OFFS, lines_dialog->m_current_row * BOX_HEIGHT + RECT_OFFS,
				BOX_WIDTH - 2 * RECT_OFFS, BOX_HEIGHT - 2 * RECT_OFFS);
	}
	return FALSE;
}

gboolean NedLinesDialog::change_selection (GtkWidget *widget, GdkEventButton *event, gpointer data) {
	GdkRectangle rect;
        int dummy;

	NedLinesDialog *lines_dialog = (NedLinesDialog *) data;
	lines_dialog->m_current_column = (int) event->x / BOX_WIDTH;
	lines_dialog->m_current_row = (int) event->y / BOX_HEIGHT;

        gdk_window_get_geometry(widget->window, &dummy, &dummy, &rect.width, &rect.height, &dummy);
        rect.x = rect.y = 0;
        gdk_window_invalidate_rect (widget->window, &rect, FALSE);
	return FALSE;
}
