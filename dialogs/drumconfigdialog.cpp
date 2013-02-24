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
#include <gdk/gdkkeysyms.h>
#include "drumconfigdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"
#include "druminfo.h"
#include "drumconfigdialog.h"

#define DRUM_ELEMENTS_WIDGET_WIDTH 440
#define DRUM_ELEMENTS_WIDGET_HEIGHT 400

#define DRUM_DEMO_XPOS 90.0
#define DRUM_DEMO_O_HH_RAD 4.0
#define DRUM_DEMO_C_HH_LEN 6.0

#define DRUM_SHOW_WIDTH 30
#define DRUM_SHOW_HIGHT 60
#define DRUM_SHOW_BORDER 10

#define DRUM_DEMO_LINE_YPOS 70.0

#define MAX_DRUM_DEMO_LINE_OVER 7

#define DRUM_DEMO_LINE_BEGIN (DRUM_DEMO_XPOS - 40.0)
#define DRUM_DEMO_LINE_END (DRUM_DEMO_XPOS + 40.0)

#define DRUM_DEMO_NOTE_LINE_BEGIN (DRUM_DEMO_XPOS - 10.0)
#define DRUM_DEMO_NOTE_LINE_END (DRUM_DEMO_XPOS + 10.0)


NedDrumConfigDialog::NedDrumConfigDialog(GtkWindow *parent) : m_type(NORMAL_NOTE), m_line(0), m_inside_preview(false) {
	int i;
	GtkWidget *dialog;
	GtkTreeIter iter;

	GtkTreeViewColumn *drum_elements_num_column;
	GtkTreeViewColumn *drum_element_name_column;
	GtkTreeViewColumn *drum_element_short_name_column;
	GtkTreeViewColumn *drum_element_position_column;
	GtkTreeViewColumn *drum_element_symbol_column;
	GtkTreeViewColumn *drum_element_pitch_column;
	GtkCellRenderer *drum_elements_renderer;
	GtkCellRenderer *drum_symbols_renderer;
	GtkWidget *bu_up;
	GtkWidget *bu_down;
	GtkWidget *drum_elements_scroll;
	GtkWidget *elements_hbox;
	GtkWidget *right_vbox;
	GtkWidget *right_hbox;
	GtkWidget *right_arrow_vbox;
	GtkWidget *drum_buttons_grid;
	GtkWidget *drums_button_frame;
	GtkWidget *drums_preview_frame;
	GtkWidget *buttons_frame;
	GtkWidget *buttons_vbox;
	GtkWidget *load_button;
	GtkWidget *save_button;
	GtkWidget *play_button;
	GtkWidget *change_button;
	GtkWidget *default_button;
	GdkColor bgcolor;

	m_black.pixel = 1; m_black.red = m_black.green = m_black.blue = 0;
	m_red.pixel = 2; m_red.red = 65535; m_red.green = m_red.blue = 0;
	m_white.pixel = 3; m_white.red = m_white.green = m_white.blue = 0xffff;


	dialog = gtk_dialog_new_with_buttons(_("Config Drums"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_CLOSE, GTK_RESPONSE_ACCEPT, NULL);

	GTK_WIDGET_SET_FLAGS(dialog, GTK_CAN_FOCUS);
	gtk_widget_add_events(dialog, GDK_KEY_PRESS_MASK);
	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);
	g_signal_connect(dialog, "key-press-event", G_CALLBACK (OnKeyPress), (void *) this);

	m_drum_elements_list_store = gtk_list_store_new (6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT, GDK_TYPE_PIXBUF, G_TYPE_INT);

	struct d_info_str *dinfo_ptr;
	m_line = current_dinfo[0].line;
	m_type = current_dinfo[0].note_head;
	for (i = 0, dinfo_ptr = current_dinfo; dinfo_ptr->name; dinfo_ptr++, i++) {
		gtk_list_store_append (m_drum_elements_list_store, &iter);
		gtk_list_store_set(m_drum_elements_list_store, &iter, 0, i, 1, dinfo_ptr->name, 2, dinfo_ptr->lily_short_name, 3,
			dinfo_ptr->line, 4, selectDrumIcon(dinfo_ptr->note_head), 5, dinfo_ptr->pitch,  -1);
	}
	m_drum_element_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(m_drum_elements_list_store));
	drum_elements_renderer = gtk_cell_renderer_text_new ();
	drum_elements_num_column = gtk_tree_view_column_new_with_attributes (_("no"), drum_elements_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_drum_element_list), drum_elements_num_column);
	drum_element_name_column = gtk_tree_view_column_new_with_attributes (_("drum element name"), drum_elements_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_drum_element_list), drum_element_name_column);
	drum_element_short_name_column = gtk_tree_view_column_new_with_attributes (_("short name"), drum_elements_renderer, "text", 2, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_drum_element_list), drum_element_short_name_column);
	drum_element_position_column = gtk_tree_view_column_new_with_attributes (_("line"), drum_elements_renderer, "text", 3, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_drum_element_list), drum_element_position_column);
	drum_symbols_renderer =  gtk_cell_renderer_pixbuf_new();
	drum_element_symbol_column = gtk_tree_view_column_new_with_attributes (_("symbol"), drum_symbols_renderer, "pixbuf", 4, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_drum_element_list), drum_element_symbol_column);
	drum_element_pitch_column = gtk_tree_view_column_new_with_attributes (_("midi pitch"), drum_elements_renderer, "text", 5, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_drum_element_list), drum_element_pitch_column);
	m_drum_elements_adjustment = gtk_adjustment_new (0.0, 0.0, i, 1.0,  10.0, 10.0);
	drum_elements_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(m_drum_elements_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_drum_element_list), GTK_ADJUSTMENT(m_drum_elements_adjustment));
	gtk_widget_set_size_request(m_drum_element_list, DRUM_ELEMENTS_WIDGET_WIDTH, DRUM_ELEMENTS_WIDGET_HEIGHT);
	g_signal_connect(dialog, "scroll-event", G_CALLBACK (OnScroll), (void *) this);
	g_signal_connect(m_drum_element_list, "cursor-changed", G_CALLBACK (OnCursorChanged), (void *) this);

	drum_buttons_grid = gtk_table_new(3, 5, TRUE);

	m_drum_buttons[0] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[0], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[0]), gtk_image_new_from_pixbuf(NedResource::m_drum1_icon));
	m_drum_buttons[1] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[1], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[1]), gtk_image_new_from_pixbuf(NedResource::m_drum2_icon));
	m_drum_buttons[2] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[2], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[2]), gtk_image_new_from_pixbuf(NedResource::m_drum3_icon));
	m_drum_buttons[3] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[3], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[3]), gtk_image_new_from_pixbuf(NedResource::m_drum4_icon));

	m_drum_buttons[4] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[4], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[4]), gtk_image_new_from_pixbuf(NedResource::m_drum5_icon));
	m_drum_buttons[5] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[5], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[5]), gtk_image_new_from_pixbuf(NedResource::m_drum6_icon));
	m_drum_buttons[6] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[6], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[6]), gtk_image_new_from_pixbuf(NedResource::m_drum7_icon));
	m_drum_buttons[7] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[7], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[7]), gtk_image_new_from_pixbuf(NedResource::m_drum8_icon));

	m_drum_buttons[8] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[8], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[8]), gtk_image_new_from_pixbuf(NedResource::m_drum9_icon));
	m_drum_buttons[9] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[9], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[9]), gtk_image_new_from_pixbuf(NedResource::m_drum10_icon));
	m_drum_buttons[10] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[10], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[10]), gtk_image_new_from_pixbuf(NedResource::m_drum11_icon));
	m_drum_buttons[11] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[11], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[11]), gtk_image_new_from_pixbuf(NedResource::m_drum12_icon));

	m_drum_buttons[12] = gtk_toggle_button_new();
	g_signal_connect(m_drum_buttons[12], "toggled", G_CALLBACK(OnToggle), (void *) this);
	gtk_button_set_image(GTK_BUTTON(m_drum_buttons[12]), gtk_image_new_from_pixbuf(NedResource::m_normal_note_icon));

	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[0], 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[1], 1, 2, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[2], 2, 3, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[3], 3, 4, 0, 1, GTK_SHRINK, GTK_SHRINK, 0, 0);

	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[4], 0, 1, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[5], 1, 2, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[6], 2, 3, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[7], 3, 4, 1, 2, GTK_SHRINK, GTK_SHRINK, 0, 0);

	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[8], 0, 1, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[9], 1, 2, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[10], 2, 3, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[11], 3, 4, 2, 3, GTK_SHRINK, GTK_SHRINK, 0, 0);

	gtk_table_attach(GTK_TABLE(drum_buttons_grid), m_drum_buttons[12], 0, 1, 3, 4, GTK_SHRINK, GTK_SHRINK, 0, 0);

	change_button = gtk_button_new_with_label(_("change element"));
	g_signal_connect (change_button, "pressed", G_CALLBACK(OnChange), (void *) this);
	gtk_table_attach(GTK_TABLE(drum_buttons_grid), change_button, 1, 4, 3, 4, GTK_SHRINK, GTK_SHRINK, 0, 0);


	drums_button_frame = gtk_frame_new(_("note head"));
	gtk_container_add (GTK_CONTAINER(drums_button_frame), drum_buttons_grid);

	m_preview = gtk_drawing_area_new ();
	gtk_widget_add_events(m_preview, GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
	bgcolor.pixel = 1;
	bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
	gtk_widget_modify_bg(GTK_WIDGET(m_preview), GTK_STATE_NORMAL, &bgcolor);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_preview), DRUM_SHOW_WIDTH, DRUM_SHOW_HIGHT);
	g_signal_connect (m_preview, "expose-event", G_CALLBACK (OnExpose), (void *) this);
	g_signal_connect (m_preview, "enter-notify-event", G_CALLBACK (OnEnter), (void *) this);
	g_signal_connect (m_preview, "leave-notify-event", G_CALLBACK (OnLeave), (void *) this);

	bu_up = gtk_button_new_from_stock("my-arrow-up-icon");
	g_signal_connect (bu_up, "pressed", G_CALLBACK(increase_line), (void *) this);
	bu_down = gtk_button_new_from_stock ("my-arrow-down-icon");
	g_signal_connect (bu_down, "pressed", G_CALLBACK(decrease_line), (void *) this);
	
	right_arrow_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(right_arrow_vbox), bu_up, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX(right_arrow_vbox), bu_down, FALSE, FALSE, 0);

	right_hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(right_hbox), m_preview, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(right_hbox), right_arrow_vbox, FALSE, FALSE, 0);

	drums_preview_frame = gtk_frame_new(_("preview"));
	gtk_container_add (GTK_CONTAINER(drums_preview_frame), right_hbox);

	right_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(right_vbox), drums_button_frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(right_vbox), drums_preview_frame, TRUE, TRUE, 0);

	load_button = gtk_button_new_with_label(_("load from file"));
	save_button = gtk_button_new_with_label(_("save to file"));
	default_button = gtk_button_new_with_label(_("load defaults"));
	play_button = gtk_button_new_with_label(_("play element"));

	g_signal_connect (save_button, "pressed", G_CALLBACK(OnSave), (void *) this);
	g_signal_connect (load_button, "pressed", G_CALLBACK(OnLoad), (void *) this);
	g_signal_connect (default_button, "pressed", G_CALLBACK(OnDefault), (void *) this);
	g_signal_connect (play_button, "pressed", G_CALLBACK(OnPlay), (void *) this);

	buttons_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(buttons_vbox), load_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(buttons_vbox), save_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(buttons_vbox), default_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(buttons_vbox), play_button, FALSE, FALSE, 0);


	buttons_frame = gtk_frame_new(_("file"));

	gtk_container_add (GTK_CONTAINER(buttons_frame), buttons_vbox);


	elements_hbox = gtk_hbox_new(FALSE, 5);

	gtk_box_pack_start (GTK_BOX(elements_hbox), buttons_frame, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(elements_hbox), m_drum_element_list, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(elements_hbox), drum_elements_scroll, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(elements_hbox), right_vbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), elements_hbox);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW(m_drum_element_list), gtk_tree_path_new_from_indices (0, -1), NULL, FALSE);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

GdkPixbuf *NedDrumConfigDialog::selectDrumIcon(int note_head) {
	switch(note_head) {
		case CROSS_NOTE1: return NedResource::m_drum1_icon_small;
		case CROSS_NOTE2: return NedResource::m_drum2_icon_small;
		case RECT_NOTE1:  return NedResource::m_drum3_icon_small;
		case RECT_NOTE2:  return NedResource::m_drum4_icon_small;
		case TRIAG_NOTE1:  return NedResource::m_drum5_icon_small;
		case TRIAG_NOTE2:  return NedResource::m_drum6_icon_small;
		case CROSS_NOTE3:  return NedResource::m_drum7_icon_small;
		case CROSS_NOTE4:  return NedResource::m_drum8_icon_small;
		case TRIAG_NOTE3:  return NedResource::m_drum9_icon_small;
		case QUAD_NOTE2:  return NedResource::m_drum10_icon_small;
		case OPEN_HIGH_HAT:  return NedResource::m_drum11_icon_small;
		case CLOSED_HIGH_HAT:  return NedResource::m_drum12_icon_small;
	}
	return NedResource::m_normal_note_icon_small;
}


void NedDrumConfigDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	GtkTreeSelection* selection;
	GList *selected_rows;
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(drum_config_dialog->m_drum_element_list));
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if ( g_list_length(selected_rows) > 0) {
		GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
		char *tp = gtk_tree_path_to_string(path);
		if (sscanf(tp, "%d", &(drum_config_dialog->m_element)) != 1) {
			NedResource::Abort("NedDrumConfigDialog::OnClose: error reading tree path(1)");
		}
	}
	else {
		drum_config_dialog->m_element = 0;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


gboolean NedDrumConfigDialog::OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
#define SCROLL_INCR 20.0
	double adjval;
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	adjval = gtk_adjustment_get_value(GTK_ADJUSTMENT(drum_config_dialog->m_drum_elements_adjustment));

	if (event->direction == GDK_SCROLL_UP) {
		adjval -= SCROLL_INCR;
	}
	else if (event->direction == GDK_SCROLL_DOWN) {
		adjval += SCROLL_INCR;
	}
	gtk_adjustment_set_value(GTK_ADJUSTMENT(drum_config_dialog->m_drum_elements_adjustment), adjval);
	return FALSE;
}

void NedDrumConfigDialog::OnToggle(GtkToggleButton *togglebutton, gpointer data) {
	int i;
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	if (gtk_toggle_button_get_active(togglebutton)) {
		for (i = 0; i < MAXBUTTONS; i++) {
			if (GTK_TOGGLE_BUTTON(drum_config_dialog->m_drum_buttons[i]) == togglebutton) {
				switch(i) {
					case 0: drum_config_dialog->m_type = CROSS_NOTE1; break;
					case 1: drum_config_dialog->m_type = CROSS_NOTE2; break;
					case 2: drum_config_dialog->m_type = RECT_NOTE1; break;
					case 3: drum_config_dialog->m_type = RECT_NOTE2; break;
					case 4: drum_config_dialog->m_type = TRIAG_NOTE1; break;
					case 5: drum_config_dialog->m_type = TRIAG_NOTE2; break;
					case 6: drum_config_dialog->m_type = CROSS_NOTE3; break;
					case 7: drum_config_dialog->m_type = CROSS_NOTE4; break;
					case 8: drum_config_dialog->m_type = TRIAG_NOTE3; break;
					case 9: drum_config_dialog->m_type = QUAD_NOTE2; break;
					case 10: drum_config_dialog->m_type = OPEN_HIGH_HAT; break;
					case 11: drum_config_dialog->m_type = CLOSED_HIGH_HAT; break;
					case 12: drum_config_dialog->m_type = NORMAL_NOTE; break;
				}
				if (GDK_IS_WINDOW(drum_config_dialog->m_preview->window)) {
					gdk_window_invalidate_rect (drum_config_dialog->m_preview->window, NULL, FALSE);
				}
				continue;
			}
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(drum_config_dialog->m_drum_buttons[i]), FALSE);
		}
	}
}

void NedDrumConfigDialog::increase_line (GtkButton *button, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	if (drum_config_dialog->m_line >= 8 + MAX_DRUM_DEMO_LINE_OVER) return;
	drum_config_dialog->m_line++;
	gdk_window_invalidate_rect (drum_config_dialog->m_preview->window, NULL, FALSE);
}
void NedDrumConfigDialog::decrease_line (GtkButton *button, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	if (drum_config_dialog->m_line <= -MAX_DRUM_DEMO_LINE_OVER) return;
	drum_config_dialog->m_line--;
	gdk_window_invalidate_rect (drum_config_dialog->m_preview->window, NULL, FALSE);
}

gboolean NedDrumConfigDialog::OnEnter(GtkWidget *widget, GdkEventCrossing *event, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	drum_config_dialog->m_inside_preview = true;

	return FALSE;
}

gboolean NedDrumConfigDialog::OnLeave(GtkWidget *widget, GdkEventCrossing *event, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	drum_config_dialog->m_inside_preview = false;

	return FALSE;
}

gboolean NedDrumConfigDialog::OnKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;

	if (!drum_config_dialog->m_inside_preview) return FALSE;
	switch (event->keyval) {
		case GDK_Up: increase_line(NULL, data); return TRUE;
		case GDK_Down: decrease_line(NULL, data); return TRUE;
	}
	return FALSE;
}

void NedDrumConfigDialog::OnCursorChanged(GtkTreeView *tree_view, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	GtkTreeSelection* selection;
	GList *selected_rows;
	int *idxs;
	int i, bunr = 0;

	selection =  gtk_tree_view_get_selection (tree_view);
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if ( g_list_length(selected_rows) > 0) {
		GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
		idxs = gtk_tree_path_get_indices (path);
		drum_config_dialog->m_line = current_dinfo[idxs[0]].line;
		drum_config_dialog->m_type = current_dinfo[idxs[0]].note_head;
		switch (drum_config_dialog->m_type) {
			case CROSS_NOTE1: bunr = 0; break;
			case CROSS_NOTE2: bunr = 1; break;
			case RECT_NOTE1: bunr = 2; break;
			case RECT_NOTE2: bunr = 3; break;
			case TRIAG_NOTE1: bunr = 4; break;
			case TRIAG_NOTE2: bunr = 5; break;
			case CROSS_NOTE3: bunr = 6; break;
			case CROSS_NOTE4: bunr = 7; break;
			case TRIAG_NOTE3: bunr = 8; break;
			case QUAD_NOTE2: bunr = 9; break;
			case OPEN_HIGH_HAT: bunr = 10; break;
			case CLOSED_HIGH_HAT: bunr = 11; break;
			case NORMAL_NOTE: bunr = 12; break;
		}
		for (i = 0; i < MAXBUTTONS; i++) {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(drum_config_dialog->m_drum_buttons[i]), bunr == i);
		}
		if (GDK_IS_WINDOW(drum_config_dialog->m_preview->window)) {
			gdk_window_invalidate_rect (drum_config_dialog->m_preview->window, NULL, FALSE);
		}
	}
}

void NedDrumConfigDialog::OnSave (GtkButton *button, gpointer data) {
	FILE *fp;
	char *filename, *cptr;
	bool ok = false;
	bool extend = false;
	char the_filename[4096];
	GtkWidget *error_dialog;
	GtkWidget *access_dialog;
	GtkWidget *save_dialog;
	struct d_info_str *dinfo_ptr;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.ntddrm (Nted-Drum-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.ntddrm");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	save_dialog = gtk_file_chooser_dialog_new (_("Save drum configuration"),
				      //GTK_WINDOW(drum_config_dialog),
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(save_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(save_dialog), file_filter2);
	if (gtk_dialog_run (GTK_DIALOG (save_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (save_dialog));
		strcpy(the_filename, filename);
		g_free (filename);
		ok = true;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (save_dialog))) == file_filter1) {
		extend = true;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntddrm")) {
				extend = false;
			}
		}
		if (extend) {
			strcat(the_filename, ".ntddrm");
		}
	}
	gtk_widget_destroy (save_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (NULL, //GTK_WINDOW(drum_config_dialog),
			//(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
			(GtkDialogFlags) 0,
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = false;
		}
		gtk_widget_destroy (access_dialog);	
	}

	if (ok) {
		if ((fp = fopen(the_filename, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (NULL, //GTK_WINDOW(drum_config_dialog),
				//(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				(GtkDialogFlags) 0,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = false;
		}
	}
	if (ok) {
		fprintf(fp, "NTEDDRM version 1\n");
		for (dinfo_ptr = current_dinfo; dinfo_ptr->name; dinfo_ptr++) {
			fprintf(fp, "%s\t%s\t%d\t", dinfo_ptr->name, dinfo_ptr->lily_short_name, dinfo_ptr->line);
			switch(dinfo_ptr->note_head) {
				case CROSS_NOTE1: fprintf(fp, "CROSS_NOTE1"); break;
				case CROSS_NOTE2: fprintf(fp, "CROSS_NOTE2"); break;
				case RECT_NOTE1:  fprintf(fp, "RECT_NOTE1"); break;
				case RECT_NOTE2:  fprintf(fp, "RECT_NOTE2"); break;
				case TRIAG_NOTE1:  fprintf(fp, "TRIAG_NOTE1"); break;
				case TRIAG_NOTE2:  fprintf(fp, "TRIAG_NOTE2"); break;
				case CROSS_NOTE3:  fprintf(fp, "CROSS_NOTE3"); break;
				case CROSS_NOTE4:  fprintf(fp, "CROSS_NOTE4"); break;
				case TRIAG_NOTE3:  fprintf(fp, "TRIAG_NOTE3"); break;
				case QUAD_NOTE2:  fprintf(fp, "QUAD_NOTE2"); break;
				case OPEN_HIGH_HAT:  fprintf(fp, "OPEN_HIGH_HAT"); break;
				case CLOSED_HIGH_HAT:  fprintf(fp, "CLOSED_HIGH_HAT"); break;
				default: fprintf(fp, "NORMAL_NOTE"); break;
			}
			fprintf(fp, "\t%d\n", dinfo_ptr->pitch);
		}
		fclose(fp);
	}
}

void NedDrumConfigDialog::OnLoad (GtkButton *button, gpointer data) {
	FILE *fp;
	GtkWidget *open_dialog;
	GtkWidget *error_dialog;
	int file_version;
	char *filename = NULL;
	char buffer[128];
	bool ok = false;
	int i;
	struct d_info_str tab[getNumberOfDrumTabElements()], *dptr1, *dptr2;
	NedResource::m_error_message = NULL;
	NedResource::m_input_line = 1;

	open_dialog = gtk_file_chooser_dialog_new (_("Open drum configuration"),
				      NULL, //GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.ntddrm (Nted-Drum-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.ntddrm");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), file_filter2);


	if (gtk_dialog_run (GTK_DIALOG (open_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (open_dialog));
		ok = true;
	}
	gtk_widget_destroy (open_dialog);
	if (ok) {
		if ((fp = fopen(filename, "r")) == NULL) {
			error_dialog = gtk_message_dialog_new (NULL, //GTK_WINDOW(main_window->m_main_window),
				//(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				(GtkDialogFlags) 0,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for reading"), filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
		}
	}
	if (ok) {
		dptr1 = default_dinfo;
		dptr2 = tab;

		do {
			memcpy(dptr2, dptr1, sizeof(struct d_info_str));
			if (dptr1->name) {
				dptr1++; dptr2++;
			}
		}
		while(dptr1->name);
		
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "NTEDDRM")) {
			NedResource::m_error_message = "NTEDDRM expected";
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "version")) {
				NedResource::m_error_message = "version expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &file_version) || file_version < 1 || file_version > 1) {
				NedResource::m_error_message = "version number expected";
			}
		}
		//NedResource::readTillEnd(fp, buffer);
		if (NedResource::m_error_message == NULL) {
			for (i = 0; i < getNumberOfDrumTabElements() - 1; i++) {
				if (!NedResource::readWord(fp, buffer)) {
					NedResource::m_error_message = "drum element name expected";
					break;
				}
				if (strcmp(tab[i].name, buffer)) {
					NedResource::m_error_message = "drum name clash";
					break;
				}
				if (!NedResource::readWord(fp, buffer)) {
					NedResource::m_error_message = "drum short element name expected";
					break;
				}
				if (!NedResource::readInt(fp, &(tab[i].line)) || tab[i].line > 8 + MAX_DRUM_DEMO_LINE_OVER || tab[i].line < -MAX_DRUM_DEMO_LINE_OVER) {
					printf("tab[%d].line = %d, 8 + MAX_DRUM_DEMO_LINE_OVER = %d, -MAX_DRUM_DEMO_LINE_OVER = %d\n"
						, i, tab[i].line, 8 + MAX_DRUM_DEMO_LINE_OVER, -MAX_DRUM_DEMO_LINE_OVER); fflush(stdout);
					NedResource::m_error_message = "line expected";
					break;
				}
				if (!NedResource::readWordWithNum(fp, buffer)) {
					NedResource::m_error_message = "drum symbol expected";
					break;
				}
				if(!strcmp("CROSS_NOTE1", buffer)) {
					tab[i].note_head = CROSS_NOTE1;
				}
				else if(!strcmp("CROSS_NOTE2", buffer)) {
					tab[i].note_head = CROSS_NOTE2;
				}
				else if(!strcmp("RECT_NOTE1", buffer)) {
					tab[i].note_head = RECT_NOTE1;
				}
				else if(!strcmp("RECT_NOTE2", buffer)) {
					tab[i].note_head = RECT_NOTE2;
				}
				else if(!strcmp("TRIAG_NOTE1", buffer)) {
					tab[i].note_head = TRIAG_NOTE1;
				}
				else if(!strcmp("TRIAG_NOTE2", buffer)) {
					tab[i].note_head = TRIAG_NOTE2;
				}
				else if(!strcmp("CROSS_NOTE3", buffer)) {
					tab[i].note_head = CROSS_NOTE3;
				}
				else if(!strcmp("CROSS_NOTE4", buffer)) {
					tab[i].note_head = CROSS_NOTE4;
				}
				else if(!strcmp("TRIAG_NOTE3", buffer)) {
					tab[i].note_head = TRIAG_NOTE3;
				}
				else if(!strcmp("QUAD_NOTE2", buffer)) {
					tab[i].note_head = QUAD_NOTE2;
				}
				else if(!strcmp("OPEN_HIGH_HAT", buffer)) {
					tab[i].note_head = OPEN_HIGH_HAT;
				}
				else if(!strcmp("CLOSED_HIGH_HAT", buffer)) {
					tab[i].note_head = CLOSED_HIGH_HAT;
				}
				else if(!strcmp("NORMAL_NOTE", buffer)) {
					tab[i].note_head = NORMAL_NOTE;
				}
				else {
					NedResource::m_error_message = "drum symbol expected";
					break;
				}
				NedResource::readTillEnd(fp, buffer);
			}
		}
		if (NedResource::m_error_message != NULL) {
			sprintf(buffer, "line: %d: %s", NedResource::m_input_line, NedResource::m_error_message);
			NedResource::Info(buffer);
			return;
		}
		for (dptr1 = current_dinfo, dptr2 = tab; dptr1->name; dptr1++, dptr2++) {
			dptr1->note_head = dptr2->note_head;
			dptr1->line = dptr2->line;
		}
		for (i = 0, dptr1 = current_dinfo; dptr1->name; dptr1++, i++) {
			NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
			GtkTreeIter iter;

			if (!gtk_tree_model_get_iter (GTK_TREE_MODEL(drum_config_dialog->m_drum_elements_list_store), &iter, gtk_tree_path_new_from_indices (i, -1))) {
				printf("error getting iterator\n"); fflush(stdout);
			}
			gtk_list_store_set(drum_config_dialog->m_drum_elements_list_store, &iter, 0, i, 1, dptr1->name, 2, dptr1->lily_short_name, 3,
				dptr1->line, 4, selectDrumIcon(dptr1->note_head), 5, dptr1->pitch,  -1);
		}
	}
}

void NedDrumConfigDialog::OnPlay (GtkButton *button, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	GtkTreeSelection* selection;
	GList *selected_rows;
	int *idxs;

	selection =  gtk_tree_view_get_selection (GTK_TREE_VIEW(drum_config_dialog->m_drum_element_list));
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if ( g_list_length(selected_rows) > 0) {
		GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
		idxs = gtk_tree_path_get_indices (path);
		NedResource::playImmediately(9, 0, 1, &(current_dinfo[idxs[0]].pitch), 64);
	}
}


void NedDrumConfigDialog::OnDefault (GtkButton *button, gpointer data) {
	int i;
	GtkTreeIter iter;
	struct d_info_str *dptr;
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	make_default_drums_current();
	for (i = 0, dptr = current_dinfo; dptr->name; dptr++, i++) {
		if (!gtk_tree_model_get_iter (GTK_TREE_MODEL(drum_config_dialog->m_drum_elements_list_store), &iter, gtk_tree_path_new_from_indices (i, -1))) {
			printf("error getting iterator\n"); fflush(stdout);
		}
		gtk_list_store_set(drum_config_dialog->m_drum_elements_list_store, &iter, 0, i, 1, dptr->name, 2, dptr->lily_short_name, 3,
			dptr->line, 4, selectDrumIcon(dptr->note_head), 5, dptr->pitch,  -1);
	}
	gdk_window_invalidate_rect (drum_config_dialog->m_preview->window, NULL, FALSE);
}

void NedDrumConfigDialog::OnChange (GtkButton *button, gpointer data) {
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	GtkTreeSelection* selection;
	GList *selected_rows;
	GtkTreeIter iter;
	GtkTreePath *path;
	int *idxs;
	struct d_info_str *dinfo_ptr;

	selection =  gtk_tree_view_get_selection (GTK_TREE_VIEW(drum_config_dialog->m_drum_element_list));
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if ( g_list_length(selected_rows) > 0) {
		path = (GtkTreePath *) g_list_first(selected_rows)->data;
		idxs = gtk_tree_path_get_indices (path);
		dinfo_ptr = &(current_dinfo[idxs[0]]);
		dinfo_ptr->line = drum_config_dialog->m_line;
		dinfo_ptr->note_head = drum_config_dialog->m_type;
		if (gtk_tree_model_get_iter (GTK_TREE_MODEL(drum_config_dialog->m_drum_elements_list_store), &iter, path)) {
			gtk_list_store_set(drum_config_dialog->m_drum_elements_list_store, &iter, 0, idxs[0] , 1, dinfo_ptr->name, 2, dinfo_ptr->lily_short_name, 3,
				dinfo_ptr->line, 4, selectDrumIcon(dinfo_ptr->note_head), 5, dinfo_ptr->pitch,  -1);
		}
	}
}


gboolean NedDrumConfigDialog::OnExpose(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
#define ZOOM_LEVEL 9
	NedDrumConfigDialog *drum_config_dialog = (NedDrumConfigDialog *) data;
	cairo_scaled_font_t *scaled_font;
	cairo_glyph_t glyph;
	int i;
	
	cairo_t *cr;

	cr = gdk_cairo_create (drum_config_dialog->m_preview->window);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  0, 0,  drum_config_dialog->m_preview->allocation.width, drum_config_dialog->m_preview->allocation.height);
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
		cairo_move_to(cr, DRUM_DEMO_LINE_BEGIN, DRUM_DEMO_LINE_YPOS + DEMO_LINE_DIST * i);
		cairo_line_to(cr, DRUM_DEMO_LINE_END, DRUM_DEMO_LINE_YPOS + DEMO_LINE_DIST * i);
	}

	for (i = 10; i <= drum_config_dialog->m_line; i += 2) {
		cairo_move_to(cr, DRUM_DEMO_NOTE_LINE_BEGIN, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * i);
		cairo_line_to(cr, DRUM_DEMO_NOTE_LINE_END, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * i);
	}

	for (i = -2; i >= drum_config_dialog->m_line; i -= 2) {
		cairo_move_to(cr, DRUM_DEMO_NOTE_LINE_BEGIN, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * i);
		cairo_line_to(cr, DRUM_DEMO_NOTE_LINE_END, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * i);
	}

	cairo_stroke(cr);
	cairo_new_path(cr);
	glyph.index = BASE + 4;
	switch (drum_config_dialog->m_type) {
		case CROSS_NOTE1:
			glyph.index = BASE + 21; break;
		case CROSS_NOTE2:
			glyph.index = BASE + 22; break;
		case RECT_NOTE1:
			glyph.index = BASE + 23; break;
		case RECT_NOTE2:
			glyph.index = BASE + 24; break;
		case TRIAG_NOTE1:
			glyph.index = BASE + 25; break;
		case TRIAG_NOTE2:
			glyph.index = BASE + 26; break;
		case OPEN_HIGH_HAT:
		case CLOSED_HIGH_HAT:
		case CROSS_NOTE3:
			glyph.index = BASE + 49; break;
		case CROSS_NOTE4:
			glyph.index = BASE + 50; break;
		case TRIAG_NOTE3:
			glyph.index = BASE + 51; break;
		case QUAD_NOTE2:
			glyph.index = BASE + 52; break;
	}
	glyph.x = DRUM_DEMO_XPOS;
	glyph.y = DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * drum_config_dialog->m_line;
	cairo_show_glyphs(cr, &glyph, 1);
	cairo_stroke(cr);
	if (drum_config_dialog->m_type == OPEN_HIGH_HAT || drum_config_dialog->m_type == CLOSED_HIGH_HAT) {
		int cline = drum_config_dialog->m_line > 7 ? drum_config_dialog->m_line + 3 : 8 + 3;
		cairo_new_path(cr);
		cairo_arc(cr, DRUM_DEMO_XPOS, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * cline, DRUM_DEMO_O_HH_RAD, 0, 2.0 * M_PI); 
		if (drum_config_dialog->m_type == CLOSED_HIGH_HAT) {
			cairo_move_to(cr, DRUM_DEMO_XPOS - DRUM_DEMO_C_HH_LEN, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * cline  + DRUM_DEMO_C_HH_LEN);
			cairo_line_to(cr, DRUM_DEMO_XPOS + DRUM_DEMO_C_HH_LEN, DRUM_DEMO_LINE_YPOS + 4 * DEMO_LINE_DIST - DEMO_LINE_DIST / 2.0 * cline  - DRUM_DEMO_C_HH_LEN);
		}
		cairo_stroke(cr);
	}
	cairo_destroy (cr);
	return FALSE;
}

