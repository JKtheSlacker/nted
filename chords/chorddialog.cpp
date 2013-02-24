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

#include "chorddialog.h"
#include "localization.h"
#include "resource.h"
#include "chordpainter.h"
#include "chordstruct.h"
#include "mainwindow.h"
#include <cairo.h>

#define BUTTONBOXHEIGHT 40
#define CHORD_SHOW_WINDOW_WIDTH 210
#define CHORD_SHOW_WINDOW_HEIGHT (300 - BUTTONBOXHEIGHT)

#define CHORD_SELECTION_SHOW_WINDOW_WIDTH 110
#define CHORD_SELECTION_SHOW_WINDOW_HEIGHT CHORD_SHOW_WINDOW_HEIGHT

#define ROOT_BOX_WIDTH 60
#define MODIES_BOX_WIDTH 60

bool NedChordDialog::persistent_draw_diagram = true;
bool NedChordDialog::persistent_draw_chord_name = true;


NedChordDialog::NedChordDialog(GtkWindow *parent, NedMainWindow *main_window, struct chord_str *chord_struct, unsigned int status) :
m_current_chords(NULL), m_chordname(0), m_state(false), m_status (status), m_main_window(main_window), m_chords_show_window_y_offset(0.0) {
	GtkWidget *dialog;
	GtkWidget *main_hbox;
	GtkWidget *chord_vbox;
	GtkWidget *chord_frame;
	GtkWidget *ruler_frame;
	GtkWidget *ruler_hbox;
	GtkWidget *button_v_box;
	GtkWidget *button_frame;
	GtkListStore * root_list_store;
	GtkCellRenderer *root_list_renderer;
	GtkTreeViewColumn *root_list_column;
	GtkTreeIter iter;
	GtkObject *root_list_adjustment;
	GtkWidget *root_list_scroll;
	GtkListStore * modies_list_store;
	GtkCellRenderer *modies_list_renderer;
	GtkTreeViewColumn *modies_list_column;
	GtkObject *modies_list_adjustment;
	GtkWidget *modies_list_scroll;
	GtkWidget *chord_selection_frame;
	GtkWidget *chord_selection_hbox;
	GdkColor bgcolor;
	const char **cptr;
	int ll;

	m_current_chord = ned_chords;
	current_root_name = NedChordPainter::roots[0];
	current_mody_name = NedChordPainter::modies2[0];
	strcpy(m_user_selection, "C");
	if (chord_struct == NULL) {
		m_status &= (~(ROOT_MASK | MOFI_MASK));
		if (persistent_draw_diagram) {
			m_status |= GUITAR_CHORD_DRAW_DIAGRAM;
		}
		else {
			m_status &= (~(GUITAR_CHORD_DRAW_DIAGRAM));
		}
		if (persistent_draw_chord_name) {
			m_status |= GUITAR_CHORD_DRAW_TEXT;
		}
		else {
			m_status &= (~(GUITAR_CHORD_DRAW_TEXT));
		}
	}
	else {
		persistent_draw_diagram = (m_status & GUITAR_CHORD_DRAW_DIAGRAM) != 0;
		persistent_draw_chord_name = (m_status & GUITAR_CHORD_DRAW_TEXT) != 0;
	}

	dialog = gtk_dialog_new_with_buttons(_("Chords"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	chord_frame = gtk_frame_new(_("chord"));
	m_chord_show_window = gtk_drawing_area_new ();
	bgcolor.pixel = 1;
	bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
	gtk_widget_modify_bg(GTK_WIDGET(m_chord_show_window), GTK_STATE_NORMAL, &bgcolor);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_chord_show_window),CHORD_SHOW_WINDOW_WIDTH, CHORD_SHOW_WINDOW_HEIGHT);

	chord_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(chord_vbox), m_chord_show_window, FALSE, FALSE, 0);

	button_frame = gtk_frame_new(_("config"));
	button_v_box = gtk_vbox_new(FALSE, 2);	
	m_text_checkbox = gtk_check_button_new_with_label(_("text"));
	m_diagram_checkbox = gtk_check_button_new_with_label(_("diagram"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_text_checkbox), m_status & GUITAR_CHORD_DRAW_TEXT);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_diagram_checkbox), m_status & GUITAR_CHORD_DRAW_DIAGRAM);

	gtk_box_pack_start(GTK_BOX(button_v_box), m_diagram_checkbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(button_v_box), m_text_checkbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(button_frame), button_v_box);
	gtk_box_pack_start(GTK_BOX(chord_vbox), button_frame, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER(chord_frame), chord_vbox);

	g_signal_connect (m_chord_show_window, "expose-event", G_CALLBACK (draw_chord_window), (void *) this);

	root_list_store = gtk_list_store_new (1, G_TYPE_STRING);

	for (ll = 0, cptr = NedChordPainter::roots; *cptr != NULL; cptr++, ll++) {
		gtk_list_store_append (root_list_store, &iter);
		gtk_list_store_set (root_list_store, &iter, 0, *cptr, -1);
	}
	m_root_list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(root_list_store));
	root_list_renderer = gtk_cell_renderer_text_new ();
	root_list_column = gtk_tree_view_column_new_with_attributes (_("root"), root_list_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_root_list_view), root_list_column);
	root_list_adjustment = gtk_adjustment_new (0.0, 0.0, ll, 1.0, 10.0, 10.0);
	root_list_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(root_list_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_root_list_view), GTK_ADJUSTMENT(root_list_adjustment));
	gtk_widget_set_size_request(m_root_list_view, ROOT_BOX_WIDTH, CHORD_SHOW_WINDOW_HEIGHT);

	g_signal_connect (m_root_list_view, "cursor-changed", G_CALLBACK (OnRootChange), (void *) this);

	modies_list_store = gtk_list_store_new (1, G_TYPE_STRING);

	for (ll = 0, cptr = NedChordPainter::modies; *cptr != NULL; cptr++, ll++) {
		gtk_list_store_append (modies_list_store, &iter);
		gtk_list_store_set (modies_list_store, &iter, 0, *cptr, -1);
	}
	m_modies_list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(modies_list_store));
	modies_list_renderer = gtk_cell_renderer_text_new ();
	modies_list_column = gtk_tree_view_column_new_with_attributes (_("mody"), root_list_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_modies_list_view), modies_list_column);
	modies_list_adjustment = gtk_adjustment_new (0.0, 0.0, ll, 1.0, 10.0, 10.0);
	modies_list_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(modies_list_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_modies_list_view), GTK_ADJUSTMENT(modies_list_adjustment));
	gtk_widget_set_size_request(m_modies_list_view, MODIES_BOX_WIDTH, CHORD_SHOW_WINDOW_HEIGHT);

	g_signal_connect (m_modies_list_view, "cursor-changed", G_CALLBACK (OnModiesChange), (void *) this);

	ruler_frame = gtk_frame_new(_("selection"));
	ruler_hbox = gtk_hbox_new(FALSE, 2);

	gtk_box_pack_start(GTK_BOX(ruler_hbox), m_root_list_view, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ruler_hbox), root_list_scroll, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ruler_hbox), m_modies_list_view, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(ruler_hbox), modies_list_scroll, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER(ruler_frame), ruler_hbox);

	chord_selection_frame = gtk_frame_new(_("chordselect"));

	m_chord_selection_show_window = gtk_drawing_area_new ();
	gtk_widget_add_events(m_chord_selection_show_window, GDK_BUTTON_PRESS_MASK);
	g_signal_connect (m_chord_selection_show_window, "expose-event", G_CALLBACK (draw_chord_show_window), (void *) this);
	g_signal_connect (m_chord_selection_show_window, "size-allocate", G_CALLBACK (OnSizeChanged), (void *) this);
	g_signal_connect (m_chord_selection_show_window, "button-press-event", G_CALLBACK (OnNewChordSelect), (void *) this);
	g_signal_connect(dialog, "scroll-event", G_CALLBACK (OnScroll), (void *) this);
	bgcolor.pixel = 1;
	bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
	gtk_widget_modify_bg(GTK_WIDGET(m_chord_selection_show_window), GTK_STATE_NORMAL, &bgcolor);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_chord_selection_show_window),CHORD_SELECTION_SHOW_WINDOW_WIDTH, CHORD_SELECTION_SHOW_WINDOW_HEIGHT);
	m_chord_selection_scale = gtk_vscale_new_with_range(0.0, 100.0, 1.0);
	gtk_scale_set_draw_value(GTK_SCALE(m_chord_selection_scale), FALSE);

	g_signal_connect (m_chord_selection_scale, "value-changed", G_CALLBACK (OnScaleChange), (void *) this);

	chord_selection_hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(chord_selection_hbox), m_chord_selection_show_window, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(chord_selection_hbox), m_chord_selection_scale, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(chord_selection_frame), chord_selection_hbox);

	main_hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_hbox), chord_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_hbox), ruler_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_hbox), chord_selection_frame, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), main_hbox);
	gtk_widget_show_all (dialog);
	searchChordPtr(chord_struct);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

NedChordDialog::~NedChordDialog() {
	if (m_current_chords != NULL) {
		g_list_free(m_current_chords);
	}
}

void NedChordDialog::searchChordPtr(struct chord_str *chord_struct) {
	struct chord_str *cptr;
	GdkRectangle rect;
#define ZOOM_SMALL 0.4
	double zoom = ZOOM_SMALL, max;

	if (chord_struct == NULL) {
		updateChord("C", &m_chordname);
		return;
	}
	if (m_current_chords != NULL) {
		g_list_free(m_current_chords);
		m_current_chords = NULL;
	}
	for (cptr = ned_chords; cptr->chordname1 != NULL; cptr++) {
		if (!strcmp(cptr->chordname1, chord_struct->chordname1))  {
			if (g_list_find(m_current_chords, cptr) != NULL) continue;
			m_current_chords = g_list_append(m_current_chords, cptr);
			if (cptr == chord_struct) {
				m_current_chord = cptr;
				m_chordname = 0;
			}
		}
		if (cptr->chordname2 != NULL && chord_struct->chordname2 != NULL) {
			if (g_list_find(m_current_chords, cptr) != NULL) continue;
			if (!strcmp(cptr->chordname2, chord_struct->chordname2))  {
				m_current_chords = g_list_append(m_current_chords, cptr);
			}
			if (cptr == chord_struct) {
				m_current_chord = cptr;
				m_chordname = 1;
			}
		}
	}
#define TOP_BORDER 20.0
	max = g_list_length(m_current_chords) * (F_COUNT + 1) * F_DIST * zoom - m_chord_selection_show_window->allocation.height + 2 * TOP_BORDER;
	if (max <= 0.0) max = 1.0;
	gtk_range_set_range(GTK_RANGE(m_chord_selection_scale), 0.0, max);
	gtk_range_set_value(GTK_RANGE(m_chord_selection_scale), 0.0);
	rect.x = rect.y = 0;
	rect.width = m_chord_selection_show_window->allocation.width;
	rect.height = m_chord_selection_show_window->allocation.height;
	gdk_window_invalidate_rect (m_chord_selection_show_window->window, &rect, FALSE);
}

void NedChordDialog::updateChord(const char *chordname, int *chord_num) {
	bool first = true;
	struct chord_str *cptr;
	GdkRectangle rect;
#define ZOOM_SMALL 0.4
	double zoom = ZOOM_SMALL, max;
	*chord_num = 0;

	if (m_current_chords != NULL) {
		g_list_free(m_current_chords);
		m_current_chords = NULL;
	}
	for (cptr = ned_chords; cptr->chordname1 != NULL; cptr++) {
		if (!strcmp(cptr->chordname1, chordname))  {
			m_current_chords = g_list_append(m_current_chords, cptr);
			if (first) {
				first = false;
				m_current_chord = cptr;
			}
		}
		if (cptr->chordname2 != NULL) {
			if (!strcmp(cptr->chordname2, chordname)) {
				m_current_chords = g_list_append(m_current_chords, cptr);
				if (first) {
					first = false;
					m_current_chord = cptr;
					*chord_num = 1;
				}
			}
		}
	}
#define TOP_BORDER 20.0
	max = g_list_length(m_current_chords) * (F_COUNT + 1) * F_DIST * zoom - m_chord_selection_show_window->allocation.height + 2 * TOP_BORDER;
	if (max <= 0.0) max = 1.0;
	gtk_range_set_range(GTK_RANGE(m_chord_selection_scale), 0.0, max);
	gtk_range_set_value(GTK_RANGE(m_chord_selection_scale), 0.0);
	rect.x = rect.y = 0;
	rect.width = m_chord_selection_show_window->allocation.width;
	rect.height = m_chord_selection_show_window->allocation.height;
	gdk_window_invalidate_rect (m_chord_selection_show_window->window, &rect, FALSE);
}

gboolean NedChordDialog::draw_chord_show_window(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	GList *lptr;
	cairo_t *cr;
	int i;
#define LEFT_BORDER 20.0
	double zoom = ZOOM_SMALL;
	NedChordDialog *the_dialog = (NedChordDialog *) data;

	cr = gdk_cairo_create (the_dialog->m_chord_selection_show_window->window);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  0, 0,  the_dialog->m_chord_selection_show_window->allocation.width, the_dialog->m_chord_selection_show_window->allocation.height);
	cairo_fill(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

	for (i = 0, lptr = g_list_first(the_dialog->m_current_chords); lptr; lptr = g_list_next(lptr), i++) {
		if (the_dialog->m_current_chord == (struct chord_str *) lptr->data) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 1.0);
		}
		NedChordPainter::draw_chord(cr, (struct chord_str *) lptr->data, -1, GUITAR_CHORD_DRAW_DIAGRAM | GUITAR_CHORD_DRAW_TEXT, false, LEFT_BORDER,
				TOP_BORDER + i * (F_COUNT + 1) * F_DIST * zoom - the_dialog->m_chords_show_window_y_offset, ZOOM_SMALL, 
				the_dialog->m_main_window->getCurrentZoomLevel());
		if (the_dialog->m_current_chord == (struct chord_str *) lptr->data) {
			cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
		}
	}
	cairo_destroy (cr);
	return FALSE;
}

void NedChordDialog::OnSizeChanged(GtkWidget *widget, GtkAllocation *allocation, gpointer data) {
	NedChordDialog *the_dialog = (NedChordDialog *) data;
	double zoom = ZOOM_SMALL, max;

	max = g_list_length(the_dialog->m_current_chords) * (F_COUNT + 1) * F_DIST * zoom - the_dialog->m_chord_selection_show_window->allocation.height + 2 * TOP_BORDER;
	if (max <= 0.0) max = 1.0;
	gtk_range_set_range(GTK_RANGE(the_dialog->m_chord_selection_scale), 0.0, max);
}

bool NedChordDialog::OnNewChordSelect(GtkWidget *widget, GdkEventButton *event, gpointer data) {
	NedChordDialog *the_dialog = (NedChordDialog *) data;
	GdkRectangle rect;
	double zoom = ZOOM_SMALL;
	int num = (int) ((event->y + the_dialog->m_chords_show_window_y_offset - TOP_BORDER) / ((F_COUNT + 1) * F_DIST * zoom));
	if (num < 0 ) num = 0;
	if (num >= (int) g_list_length(the_dialog->m_current_chords)) num = g_list_length(the_dialog->m_current_chords) - 1;
	the_dialog->m_current_chord = (struct chord_str *) g_list_nth(the_dialog->m_current_chords, num)->data;
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_chord_show_window->allocation.width;
	rect.height = the_dialog->m_chord_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_chord_show_window->window, &rect, FALSE);
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_chord_selection_show_window->allocation.width;
	rect.height = the_dialog->m_chord_selection_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_chord_selection_show_window->window, &rect, FALSE);
	return FALSE;
}

void NedChordDialog::OnScaleChange (GtkRange *range, gpointer data)  {
	GdkRectangle rect;
	NedChordDialog *the_dialog = (NedChordDialog *) data;
	the_dialog->m_chords_show_window_y_offset = gtk_range_get_value(range);
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_chord_selection_show_window->allocation.width;
	rect.height = the_dialog->m_chord_selection_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_chord_selection_show_window->window, &rect, FALSE);
}

void NedChordDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedChordDialog *chord_dialog = (NedChordDialog *) data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chord_dialog->m_diagram_checkbox))) {
		chord_dialog->m_status |= GUITAR_CHORD_DRAW_DIAGRAM;
		persistent_draw_diagram = true;
	}
	else {
		persistent_draw_diagram = false;
		chord_dialog->m_status &= (~(GUITAR_CHORD_DRAW_DIAGRAM));
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chord_dialog->m_text_checkbox))) {
		chord_dialog->m_status |= GUITAR_CHORD_DRAW_TEXT;
		persistent_draw_chord_name = true;
	}
	else {
		persistent_draw_chord_name = false;
		chord_dialog->m_status &= (~(GUITAR_CHORD_DRAW_TEXT));
	}
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			chord_dialog->m_state = TRUE;
		break;
		default:
			chord_dialog->m_state = FALSE;
		break;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

bool NedChordDialog::OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
	NedChordDialog *chord_dialog = (NedChordDialog *) data;
	double adjval;

#define SCROLL_INCR 20.0

	adjval = gtk_range_get_value(GTK_RANGE(chord_dialog->m_chord_selection_scale));
	if (event->direction == GDK_SCROLL_UP) {
		adjval -= SCROLL_INCR;
	}
	else if (event->direction == GDK_SCROLL_DOWN) {
		adjval += SCROLL_INCR;
	}
	gtk_range_set_value(GTK_RANGE(chord_dialog->m_chord_selection_scale), adjval);
	return FALSE;
}

void NedChordDialog::getValues(bool *state, struct chord_str **chord_ptr, int *chordname, unsigned int *status) {
	*state = m_state && ((m_status & (GUITAR_CHORD_DRAW_DIAGRAM | GUITAR_CHORD_DRAW_TEXT)) != 0);
	*chord_ptr = m_current_chord;
	*chordname = m_chordname;
	*status = m_status;
}


gboolean NedChordDialog::draw_chord_window(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	NedChordDialog *the_dialog = (NedChordDialog *) data;
	
	cairo_t *cr;

	cr = gdk_cairo_create (the_dialog->m_chord_show_window->window);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  0, 0,  the_dialog->m_chord_show_window->allocation.width, the_dialog->m_chord_show_window->allocation.height);
	cairo_fill(cr);

#define TEXT_SIZE 20.0
#define TEXT_X_START 10.0
#define TEXT_Y_START 20.0

#define TEXT2_Y_START 40.0

	cairo_new_path(cr);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, TEXT_SIZE);
	cairo_move_to(cr, TEXT_X_START,  TEXT_Y_START);
	cairo_show_text(cr, the_dialog->m_current_chord->chordname1);
	if (the_dialog->m_current_chord->chordname2 != NULL) {
		cairo_move_to(cr, TEXT_X_START,  TEXT2_Y_START);
		cairo_show_text(cr, the_dialog->m_current_chord->chordname2);
	}
	cairo_stroke(cr);

#define W_X_START 10.0
#define W_Y_START 80.0

	NedChordPainter::draw_chord(cr, the_dialog->m_current_chord, -1, (GUITAR_CHORD_DRAW_DIAGRAM | GUITAR_CHORD_DRAW_TEXT), false, W_X_START, W_Y_START, 1.0, the_dialog->m_main_window->getCurrentZoomLevel());


	cairo_destroy (cr);
	return FALSE;
}

void NedChordDialog::OnRootChange(GtkTreeView *tree_view, gpointer data)  {
	GtkTreeSelection* selection;
	GList *selected_rows;
	int num;
	GdkRectangle rect;
	NedChordDialog *the_dialog = (NedChordDialog *) data;

	selection =  gtk_tree_view_get_selection (tree_view);
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if (g_list_length(selected_rows) != 1) {
		printf("nichts\n"); fflush(stdout);
		return;
	}
	GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
	char *tp = gtk_tree_path_to_string(path);
	if (sscanf(tp, "%d", &num) != 1) {
		NedResource::Abort("NedChordDialog::OnRootChange: error reading tree path");
	}
	the_dialog->m_status &= (~(ROOT_MASK));
	the_dialog->m_status |= num;
	the_dialog->current_root_name = NedChordPainter::roots[num];
	strcpy(the_dialog->m_user_selection, the_dialog->current_root_name);
	strcat(the_dialog->m_user_selection, the_dialog->current_mody_name);
	the_dialog->m_chords_show_window_y_offset = 0.0;
	the_dialog->updateChord(the_dialog->m_user_selection, &(the_dialog->m_chordname));
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_chord_show_window->allocation.width;
	rect.height = the_dialog->m_chord_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_chord_show_window->window, &rect, FALSE);
}

void NedChordDialog::OnModiesChange(GtkTreeView *tree_view, gpointer data)  {
	GtkTreeSelection* selection;
	GList *selected_rows;
	int num;
	GdkRectangle rect;
	NedChordDialog *the_dialog = (NedChordDialog *) data;

	selection =  gtk_tree_view_get_selection (tree_view);
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if (g_list_length(selected_rows) != 1) {
		printf("NedChordDialog::OnModiesChange: nichts\n"); fflush(stdout);
		return;
	}
	GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
	char *tp = gtk_tree_path_to_string(path);
	if (sscanf(tp, "%d", &num) != 1) {
		NedResource::Abort("NedChordDialog::OnModiesChange: error reading tree path");
	}
	the_dialog->m_status &= (~(MOFI_MASK));
	the_dialog->m_status |= (num << ROOT_BITS);
	the_dialog->current_mody_name = NedChordPainter::modies2[num];
	strcpy(the_dialog->m_user_selection, the_dialog->current_root_name);
	strcat(the_dialog->m_user_selection, the_dialog->current_mody_name);
	the_dialog->m_chords_show_window_y_offset = 0.0;
	the_dialog->updateChord(the_dialog->m_user_selection, &(the_dialog->m_chordname));
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_chord_show_window->allocation.width;
	rect.height = the_dialog->m_chord_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_chord_show_window->window, &rect, FALSE);
}

