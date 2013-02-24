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

#include <stdio.h>
#include <dirent.h>
#include "lyricseditor.h"
#include "mainwindow.h"
#include "localization.h"
#include "resource.h"

#define EDITOR_WIDTH 600
#define EDITOR_HIGHT 300

#define MAX_LINE_LENGTH 80

NedLyricsEditor::NedLyricsEditor(GtkWindow *parent, NedMainWindow *main_window, int staff_nr):
 m_main_window(main_window), m_last_folder(NULL), m_current_verse(0), m_staff_nr(staff_nr) {
	GtkWidget *buttons_frame;
	GtkWidget *buttons_vbox;
	GtkWidget *main_hbox;
	GtkWidget *load_button;
	GtkWidget *save_button;

	GtkWidget *scrolled_window1;
	GtkWidget *scrolled_window2;
	GtkWidget *scrolled_window3;
	GtkWidget *scrolled_window4;
	GtkWidget *scrolled_window5;

	GtkObject* hscroll1, *vscroll1;
	GtkObject* hscroll2, *vscroll2;
	GtkObject* hscroll3, *vscroll3;
	GtkObject* hscroll4, *vscroll4;
	GtkObject* hscroll5, *vscroll5;

	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *label3;
	GtkWidget *label4;
	GtkWidget *label5;

	GtkWidget *view1;
	GtkWidget *view2;
	GtkWidget *view3;
	GtkWidget *view4;
	GtkWidget *view5;


	m_dialog = gtk_dialog_new_with_buttons(_("Lyrics"), parent, (GtkDialogFlags) 0,
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_APPLY, GTK_RESPONSE_APPLY, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(m_dialog, "response", G_CALLBACK (ResponseCallBack), (void *) this);


	view1 = gtk_text_view_new();
	view2 = gtk_text_view_new();
	view3 = gtk_text_view_new();
	view4 = gtk_text_view_new();
	view5 = gtk_text_view_new();

	m_buffer[0] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view1));
	m_buffer[1] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view2));
	m_buffer[2] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view3));
	m_buffer[3] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view4));
	m_buffer[4] = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view5));

	label1 = gtk_label_new(_("lyrics 1"));
	label2 = gtk_label_new(_("lyrics 2"));
	label3 = gtk_label_new(_("lyrics 3"));
	label4 = gtk_label_new(_("lyrics 4"));
	label5 = gtk_label_new(_("lyrics 5"));

	hscroll1 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	vscroll1 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	scrolled_window1 = gtk_scrolled_window_new(GTK_ADJUSTMENT(hscroll1), GTK_ADJUSTMENT(vscroll1));
	gtk_container_add (GTK_CONTAINER(scrolled_window1), view1);

	hscroll2 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	vscroll2 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	scrolled_window2 = gtk_scrolled_window_new(GTK_ADJUSTMENT(hscroll2), GTK_ADJUSTMENT(vscroll2));
	gtk_container_add (GTK_CONTAINER(scrolled_window2), view2);

	hscroll3 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	vscroll3 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	scrolled_window3 = gtk_scrolled_window_new(GTK_ADJUSTMENT(hscroll3), GTK_ADJUSTMENT(vscroll3));
	gtk_container_add (GTK_CONTAINER(scrolled_window3), view3);

	hscroll4 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	vscroll4 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	scrolled_window4 = gtk_scrolled_window_new(GTK_ADJUSTMENT(hscroll4), GTK_ADJUSTMENT(vscroll4));
	gtk_container_add (GTK_CONTAINER(scrolled_window4), view4);

	hscroll5 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	vscroll5 = gtk_adjustment_new(0.0, 0.0, 100.0, 1.0, 10.0, 100.0);
	scrolled_window5 = gtk_scrolled_window_new(GTK_ADJUSTMENT(hscroll5), GTK_ADJUSTMENT(vscroll5));
	gtk_container_add (GTK_CONTAINER(scrolled_window5), view5);


	m_table = gtk_notebook_new();
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), scrolled_window1, label1);
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), scrolled_window2, label2);
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), scrolled_window3, label3);
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), scrolled_window4, label4);
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), scrolled_window5, label5);

	gtk_widget_set_size_request(view1, EDITOR_WIDTH, EDITOR_HIGHT);
	gtk_widget_set_size_request(view2, EDITOR_WIDTH, EDITOR_HIGHT);
	gtk_widget_set_size_request(view3, EDITOR_WIDTH, EDITOR_HIGHT);
	gtk_widget_set_size_request(view4, EDITOR_WIDTH, EDITOR_HIGHT);
	gtk_widget_set_size_request(view5, EDITOR_WIDTH, EDITOR_HIGHT);
	g_signal_connect (view1, "map-event", G_CALLBACK(OnMap1), (void *) this);
	g_signal_connect (view2, "map-event", G_CALLBACK(OnMap2), (void *) this);
	g_signal_connect (view3, "map-event", G_CALLBACK(OnMap3), (void *) this);
	g_signal_connect (view4, "map-event", G_CALLBACK(OnMap4), (void *) this);
	g_signal_connect (view5, "map-event", G_CALLBACK(OnMap5), (void *) this);

	load_button = gtk_button_new_with_label(_("load from file"));
	save_button = gtk_button_new_with_label(_("save to file"));
	g_signal_connect (save_button, "pressed", G_CALLBACK(OnSave), (void *) this);
	g_signal_connect (load_button, "pressed", G_CALLBACK(OnLoad), (void *) this);
	buttons_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(buttons_vbox), load_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(buttons_vbox), save_button, FALSE, FALSE, 0);

	buttons_frame = gtk_frame_new(_("file"));
	gtk_container_add (GTK_CONTAINER(buttons_frame), buttons_vbox);

	main_hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(main_hbox), buttons_frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(main_hbox), m_table, TRUE, TRUE, 0);
	for (int i = 0; i < MAX_LYRICS_LINES; number_uf_dots[i++] = 0);

	m_main_window->collectLyrics(this, staff_nr);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(m_dialog)->vbox), main_hbox);
	gtk_widget_show_all (m_dialog);
	gtk_dialog_run(GTK_DIALOG(m_dialog));

}

void NedLyricsEditor::ResponseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
		case GTK_RESPONSE_APPLY:
			for (int i = 0; i < MAX_LYRICS_LINES; i++) {
				gtk_text_buffer_get_start_iter(lyrics_editor->m_buffer[i], &(lyrics_editor->m_itr1[i]));
				gtk_text_buffer_get_start_iter(lyrics_editor->m_buffer[i], &(lyrics_editor->m_itr2[i]));
				lyrics_editor->m_newchar[i] = TRUE;
			}
			lyrics_editor->m_main_window->NedMainWindow::updateLyrics(lyrics_editor, lyrics_editor->m_staff_nr);
		break;
	}
	if (result != GTK_RESPONSE_APPLY) {
		delete lyrics_editor;
	}
}

NedLyricsEditor::~NedLyricsEditor() {
	gtk_widget_destroy (GTK_WIDGET(m_dialog));
}

void NedLyricsEditor::appendText(int num, const char *text) {
	GtkTextIter iter;
	char buffer[1024];
	const char *dotbuf = ". ", *dotnewbuf = ".\n";
	int i;


	gtk_text_buffer_get_end_iter (m_buffer[num], &iter);

	for (i = 0; i < number_uf_dots[num]; i++) {
		if (gtk_text_iter_get_line_offset(&iter) > MAX_LINE_LENGTH) {
			gtk_text_buffer_insert(m_buffer[num], &iter, dotnewbuf, strlen(dotnewbuf));
		}
		else {
			gtk_text_buffer_insert(m_buffer[num], &iter, dotbuf, strlen(dotbuf));
		}
	}

	number_uf_dots[num] = 0;

	if (gtk_text_iter_get_line_offset(&iter) > MAX_LINE_LENGTH) {
		strcpy(buffer, "\n");
		strcat(buffer, text);
	}
	else {
		strcpy(buffer, text);
	}
	strcat(buffer, " ");
	gtk_text_buffer_insert(m_buffer[num], &iter, buffer, strlen(buffer));
}

char *NedLyricsEditor::getNextWord(int line) {
	gunichar uc;
	int offs;
	bool nl;

	offs = gtk_text_iter_get_offset(&(m_itr2[line]));
	gtk_text_iter_set_offset(&(m_itr1[line]), offs);
	uc = gtk_text_iter_get_char(&(m_itr1[line]));
	while (m_newchar[line] && uc <= ' ') {
		m_newchar[line] = gtk_text_iter_forward_char(&(m_itr1[line]));
		if (m_newchar[line]) {
			uc = gtk_text_iter_get_char(&(m_itr1[line]));
		}
	}
	if (m_newchar[line]) {
		offs =  gtk_text_iter_get_offset(&(m_itr1[line]));
		gtk_text_iter_set_offset(&(m_itr2[line]), offs);
		while (m_newchar[line] && uc > ' ' /* && uc != '-' */) {
			m_newchar[line] = gtk_text_iter_forward_char(&(m_itr2[line]));
			if (m_newchar[line]) {
				uc = gtk_text_iter_get_char(&(m_itr2[line]));
			}
			else {
				uc = gtk_text_iter_get_char(&(m_itr1[line]));
				gtk_text_buffer_get_end_iter(m_buffer[line], &(m_itr2[line]));
				nl = true;
				while (nl && gtk_text_iter_compare(&(m_itr1[line]), &(m_itr2[line])) < 0 && uc < ' ') {
					nl = gtk_text_iter_forward_char(&(m_itr1[line]));
				}
				if (nl && gtk_text_iter_compare(&(m_itr1[line]), &(m_itr2[line])) < 0) {
					uc = gtk_text_iter_get_char(&(m_itr1[line]));
					while (nl && gtk_text_iter_compare(&(m_itr1[line]), &(m_itr2[line])) < 0 && uc < ' ') {
						nl = gtk_text_iter_backward_char(&(m_itr2[line]));
					}
					if (nl && gtk_text_iter_compare(&(m_itr1[line]), &(m_itr2[line])) <= 0 && uc >= ' ') {
						return gtk_text_iter_get_text(&(m_itr1[line]), &(m_itr2[line]));
					}
					else {
						return NULL;
					}
				}
				else {
					return NULL;
				}
			}
		}
	}
	if (m_newchar[line]) {
		return gtk_text_iter_get_text(&(m_itr1[line]), &(m_itr2[line]));
	}
	return NULL;
}

void NedLyricsEditor::OnSave (GtkButton *button, gpointer data) {
	FILE *fp;
	char *filename, *cptr;
	bool ok = false;
	bool extend = false;
	char the_filename[4096];
	char pathfoldername[1024];
	DIR *the_directory;
	GtkWidget *error_dialog;
	GtkWidget *access_dialog;
	GtkWidget *save_dialog;
	int offs;
	GtkTextIter itr1, itr2;

	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;
	if (lyrics_editor->m_current_verse < 0 || lyrics_editor->m_current_verse >= MAX_LYRICS_LINES) {
		NedResource::Abort("no current verse");
	}

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.txt (Text-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.txt");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	save_dialog = gtk_file_chooser_dialog_new (_("Save lyrics"),
				      //GTK_WINDOW(drum_config_dialog),
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(save_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(save_dialog), file_filter2);

	if (lyrics_editor->m_last_folder != NULL) {
		if ((the_directory = opendir(lyrics_editor->m_last_folder)) != NULL) { // avoid gtk error message
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(save_dialog), lyrics_editor->m_last_folder);
			closedir(the_directory);
		}
		else {
			lyrics_editor->m_last_folder = NULL;
		}
	}
	if (gtk_dialog_run (GTK_DIALOG (save_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (save_dialog));
		strcpy(the_filename, filename);
		g_free (filename);
		ok = true;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (save_dialog))) == file_filter1) {
		extend = true;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".txt")) {
				extend = false;
			}
		}
		if (extend) {
			strcat(the_filename, ".txt");
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
		if (the_filename[0] == '/' && (cptr = strrchr(the_filename, '/')) != NULL) {
			strncpy(pathfoldername, the_filename, cptr - the_filename);
			pathfoldername[cptr - the_filename] = '\0';
			lyrics_editor->m_last_folder = strdup(pathfoldername);
		}
		gtk_text_buffer_get_start_iter(lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr1);
		gtk_text_buffer_get_start_iter(lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr2);
		gtk_text_iter_forward_line(&itr2);
		while (gtk_text_iter_compare(&itr2, &itr1) > 0) {
			fprintf(fp, "%s", gtk_text_iter_get_text(&itr1, &itr2));
			offs = gtk_text_iter_get_offset(&itr2);
			gtk_text_iter_set_offset(&itr1, offs);
			gtk_text_iter_forward_line(&itr2);
		}
		fclose(fp);
	}
}

void NedLyricsEditor::OnLoad (GtkButton *button, gpointer data) {
	FILE *fp;
	GtkWidget *open_dialog;
	GtkWidget *error_dialog;
	char *filename = NULL;
	char pathfoldername[4096], *cptr;
	DIR *the_directory;
	char buffer[128];
	bool ok = false;
	int len;
	GtkTextIter itr1, itr2;

	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;
	if (lyrics_editor->m_current_verse < 0 || lyrics_editor->m_current_verse >= MAX_LYRICS_LINES) {
		NedResource::Abort("no current verse");
	}

	open_dialog = gtk_file_chooser_dialog_new (_("Open lyrics file"),
				      NULL, //GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.txt (Text-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.txt");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), file_filter2);

	if (lyrics_editor->m_last_folder != NULL) {
		if ((the_directory = opendir(lyrics_editor->m_last_folder)) != NULL) { // avoid gtk error message
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(open_dialog), lyrics_editor->m_last_folder);
			closedir(the_directory);
		}
		else {
			lyrics_editor->m_last_folder = NULL;
		}
	}


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
		if (filename[0] == '/' && (cptr = strrchr(filename, '/')) != NULL) {
			strncpy(pathfoldername, filename, cptr - filename);
			pathfoldername[cptr - filename] = '\0';
			lyrics_editor->m_last_folder = strdup(pathfoldername);
		}
		gtk_text_buffer_get_start_iter(lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr1);
		gtk_text_buffer_get_end_iter (lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr2);
		gtk_text_buffer_delete(lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr1, &itr2);
		while (1) {
			if (!NedResource::readWordWithAllChars(fp, buffer)) break;
			len = strlen(buffer);
			gtk_text_buffer_get_end_iter (lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr2);
			if (gtk_text_iter_get_line_offset(&itr2) > MAX_LINE_LENGTH) {
				buffer[len] = '\n';
			}
			else {
				buffer[len] = ' ';
			}
			buffer[len + 1] = '\0';
			gtk_text_buffer_insert(lyrics_editor->m_buffer[lyrics_editor->m_current_verse], &itr2, buffer, strlen(buffer));
		}
	}
}


gboolean NedLyricsEditor::OnMap1(GtkWidget *widget, GdkEvent *event, gpointer data) {
	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;

	lyrics_editor->m_current_verse = 0;
	return FALSE;
}

gboolean NedLyricsEditor::OnMap2(GtkWidget *widget, GdkEvent *event, gpointer data) {
	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;

	lyrics_editor->m_current_verse = 1;
	return FALSE;
}

gboolean NedLyricsEditor::OnMap3(GtkWidget *widget, GdkEvent *event, gpointer data) {
	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;

	lyrics_editor->m_current_verse = 2;
	return FALSE;
}
gboolean NedLyricsEditor::OnMap4(GtkWidget *widget, GdkEvent *event, gpointer data) {
	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;

	lyrics_editor->m_current_verse = 3;
	return FALSE;
}

gboolean NedLyricsEditor::OnMap5(GtkWidget *widget, GdkEvent *event, gpointer data) {
	NedLyricsEditor *lyrics_editor = (NedLyricsEditor *) data;

	lyrics_editor->m_current_verse = 4;
	return FALSE;
}
