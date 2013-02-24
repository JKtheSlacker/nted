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

//#define KEYDEBUG

#include <stdio.h>
#include <time.h>
#include <ft2build.h>
#include <freetype/freetype.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <cairo-ps.h>
#include <libintl.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <dirent.h>
#include <unistd.h>
#include <gdk/gdkx.h>

#ifdef CAIRO_HAS_SVG_SURFACE
#include <cairo-svg.h>
#endif

#ifdef CAIRO_HAS_PDF_SURFACE
#include <cairo-pdf.h>
#endif

#include "localization.h"
#include "mainwindow.h"
#include "freereplaceable.h"
#include "page.h"
#include "system.h"
#include "clipboard.h"
#include "chordorrest.h"
#include "note.h"
#include "staff.h"
#include "voice.h"
#include "measure.h"
#include "commandhistory.h"
#include "commandlist.h"
#include "notemovecommand.h"
#include "notemoverelativecommand.h"
#include "changenotestatus.h"
#include "changenotehead.h"
#include "untieforwardcommand.h"
#include "appendnewpagecommand.h"
#include "removelastpagecommand.h"
#include "removepagecommand.h"
#include "deletestaffcommand.h"
#include "deletesystemcommand.h"
#include "clefconfigdialog.h"
#include "staffcontextdialog.h"
#include "lilypondexportdialog.h"
#include "measureconfigdialog.h"
#include "midirecordconfigdialog.h"
#include "drumconfigdialog.h"
#include "appendsystemcommand.h"
#include "spacementdialog.h"
#include "scoreinfodialog.h"
#include "setstaffcontext.h"
#include "keysigdialog.h"
#include "linesdialog.h"
#include "signsdialog.h"
#include "freesign.h"
#include "tupletdialog.h"
#include "printconfigdialog.h"
#include "deletechordcommand.h"
#include "ereasechordorrestcommand.h"
#include "paperconfigdialog.h"
#include "paperconfigcommand.h"
#include "insertnewpagecommand.h"
#include "portchoosedialog.h"
#include "insertblockdialog.h"
#include "shiftstaffcommand.h"
#include "lyricseditor.h"
#include "changemeasuretypecommand.h"
#include "movespecmeasurecommand.h"
#include "changemeasuretimsigcommand.h"
#include "changechordorreststatus.h"
#include "changeupbeatcommand.h"
#include "changetimsigcommand.h"
#include "removestaffelemcommand.h"
#include "removefreeplaceablecommand.h"
#include "fixmovedfreeplaceablecommand.h"
#include "restmovecommand.h"
#include "fixintermediatecommand.h"
#include "changemeasurespread.h"
#include "staffselectdialog.h"
#include "changespacement.h"
#include "changestaffkeysigcmd.h"
#include "changestaffclefcmd.h"
#include "transposecommand.h"
#include "changetie.h"
#include "slurpoint.h"
#include "volumesign.h"
#include "tempodialog.h"
#include "textdialog.h"
#include "chordnamedialog.h"
#include "importer.h"
#include "musicxmlimport.h"
#include "mutedialog.h"
#include "selectordialog.h"
#include "midiimportdialog.h"
#include "chorddialog.h"
#include "scaledialog.h"
#include "chordstruct.h"
#include "linepoint.h"
#include "line3.h"
#include "freetext.h"
#include "midiimporter.h"
#include "midirecorder.h"
#include "pangocairotext.h"

#define SHIFT_ACCEL 3.0

#ifdef WITH_TIME_TEST
#define MIN_TIME_BETWEEN_MOTION 20
#endif


const char *NedMainWindow::guiDescription =
"<ui>"
"  <menubar name='MainMenu'>"
"    <menu action='FileMenu'>"
"      <menuitem action='New'/>"
"      <menuitem action='Open'/>"
"      <menuitem action='Save'/>"
"      <menuitem action='SaveAs'/>"
"      <separator/>\n"
"      <menuitem action='new_window'/>"
"      <menuitem action='excerpt'/>"
"      <menuitem action='close_window'/>"
"      <separator/>\n"
"      <menu action='OpenRecentMenu'>"
"      </menu>"
"      <separator/>\n"
"      <menuitem action='config_paper'/>"
"      <menuitem action='change_scale'/>"
"      <menuitem action='config_record'/>"
"      <menuitem action='config_drums'/>" 
"      <separator/>\n"
"      <menuitem action='write_ps_action'/>"
#ifdef CAIRO_HAS_PDF_SURFACE
"      <menuitem action='write_pdf'/>"
#endif
"      <menuitem action='write_png'/>"
#ifdef CAIRO_HAS_SVG_SURFACE
"      <menuitem action='write_svg'/>"
#endif
"      <menuitem action='export_midi'/>"
"      <menuitem action='export_lily'/>"
"      <separator/>\n"
"      <menuitem action='import_musicxml'/>"
"      <menuitem action='import_midi'/>"
//"      <menuitem action='import_midi2'/>"
"      <separator/>\n"
"      <menuitem action='print_file'/>"
"      <separator/>\n"
"      <menuitem action='Exit'/>"
"    </menu>"
"    <menu action='ViewMenu'>"
"      <menuitem action='show_measure_numbers'/>"
"      <menuitem action='show_hidden_elements'/>"
"      <menuitem action='zoom_in'/>"
"      <menuitem action='zoom_out'/>"
"    </menu>"
"    <menu action='EditMenu'>"
"      <menuitem action='copy'/>"
"      <menuitem action='paste'/>"
"      <menuitem action='undo'/>"
"      <menuitem action='redo'/>"
"      <menuitem action='delete_block'/>"
"      <menuitem action='empty_block'/>"
"      <menuitem action='reposit_all'/>"
"      <menuitem action='play'/>"
"      <menuitem action='remove_tuplet'/>"
"      <menuitem action='remove_page'/>"
"      <menuitem action='remove_empty_pages'/>"
"      <menuitem action='change_spacement'/>"
"      <menu action='UpbeatMenu'>"
"          <menuitem action='set_upbeat_measure'/>"
"          <menuitem action='set_upbeat_start'/>"
"      </menu>"
"      <menuitem action='score_info'/>"
"      <menu action='SystemLayout'>"
"          <menuitem action='set_brace'/>"
"          <menuitem action='set_bracket'/>"
"          <menuitem action='set_connected'/>"
"          <menuitem action='remove_element'/>"
"      </menu>"
"      <menu action='PrefMenu'>"
"        <menuitem action='config_midi_out'/>"
"        <menuitem action='config_midi_in'/>"
"        <menuitem action='config_print_cmd'/>"
//"      <menuitem action='spec_meas_test'/>"
"      </menu>"
"    </menu>"
"    <menu action='InsertMenu'>"
"      <menuitem action='append_system'/>"
"      <menuitem action='insert_block'/>"
"      <menuitem action='insert_tuplet'/>"
"      <menuitem action='insert_clef'/>"
"      <menuitem action='insert_keysig'/>"
"      <menuitem action='insert_lines'/>"
"      <menuitem action='insert_signs'/>"
"      <menuitem action='insert_slur'/>"
"      <menuitem action='insert_chord'/>"
"      <menuitem action='insert_chord_name'/>"
"      <menuitem action='insert_spacer'/>"
"      <menu action='DynamicsMenu'>"
"         <menuitem action='dyn_ppp'/>\n"
"         <menuitem action='dyn_pp'/>\n"
"         <menuitem action='dyn_p'/>\n"
"         <menuitem action='dyn_mp'/>\n"
"         <menuitem action='dyn_sp'/>\n"
"         <menuitem action='dyn_mf'/>\n"
"         <menuitem action='dyn_sf'/>\n"
"         <menuitem action='dyn_f'/>\n"
"         <menuitem action='dyn_ff'/>\n"
"         <menuitem action='dyn_fff'/>\n"
"      </menu>"
"      <menuitem action='insert_tempo'/>"
"      <menuitem action='insert_text'/>"
"      <menuitem action='insert_accelerato'/>"
"      <menuitem action='insert_ritardando'/>"
"      <menuitem action='append_staff'/>"
"      <menuitem action='append_system'/>"
"      <menuitem action='import_recorded'/>"
/*
"      <menuitem action='insert_page'/>"
 "      <menuitem action='append_page'/>" */
"      <menu action='LyricsMenu'>"
"         <menuitem action='lyrics_mode1'/>\n"
"         <menuitem action='lyrics_mode2'/>\n"
"         <menuitem action='lyrics_mode3'/>\n"
"         <menuitem action='lyrics_mode4'/>\n"
"         <menuitem action='lyrics_mode5'/>\n"
"      </menu>"
"    </menu>"
"    <menu action='ToolsMenu'>"
"      <menuitem action='lyrics_editor'/>"
"      <menuitem action='mute_staves'/>"
"      <menuitem action='transpose'/>"
"      <menuitem action='hide_rests'/>"
"      <menuitem action='unhide_rests'/>"
"      <menuitem action='hide_rests_of_all_voices'/>"
"      <menuitem action='unhide_rests_of_all_voices'/>"
"      <menuitem action='set_all_halfs_to_sharp'/>"
"      <menuitem action='set_all_halfs_to_flat'/>"
"    </menu>"
"    <menu action='HelpMenu'>"
#ifdef YELP_PATH
"      <menuitem action='show_docu'/>"
#endif
"      <menuitem action='about_nted'/>"
"      <menuitem action='show_license'/>"
"    </menu>"
"  </menubar>\n"
"  <toolbar name='ToolBar'>\n"
"    <toolitem action='toggle_insert_erease_mode'/>\n"
"    <toolitem action='set_page_mode'/>\n"
"    <toolitem action='toggle_keyboard_insert_mode'/>\n"
"    <toolitem action='Save'/>\n"
"    <toolitem action='zoom_in'/>\n"
"    <toolitem action='zoom_out'/>\n"
"    <toolitem action='undo'/>\n"
"    <toolitem action='redo'/>\n"
"    <toolitem action='toolbox'/>"
"    <toolitem action='play'/>"
"    <toolitem action='color_notes'/>"
"    <toolitem action='customize_beam'/>\n"
"    <toolitem action='flip_stem'/>\n"
"    <toolitem action='midi_input'/>\n"
"    <toolitem action='record_midi'/>\n"
"  </toolbar>\n"
"  <accelerator name='n' action='toggle_insert_erease_mode'/>\n"
"  <accelerator name='p' action='set_page_mode'/>\n"
"  <accelerator name='k' action='toggle_keyboard_insert_mode'/>\n"
"  <accelerator name='t' action='reposit_all'/>\n"
"  <accelerator name='i' action='record_midi'/>\n"
"  <accelerator name='<crtl>a' action='append_staff'/>\n"
"  <accelerator name='<ctrl>r' action='hide_rests'/>\n"
"  <accelerator name='<ctrl>u' action='unhide_rests'/>\n"
"  <accelerator name='<ctrl>f' action='flip_stem'/>\n"
"  <accelerator name='<ctrl>j' action='customize_beam'/>\n"
"  <accelerator name='<ctrl>h' action='insert_slur'/>\n"
"</ui>";


const GtkActionEntry NedMainWindow::file_entries[] = {
  { "FileMenu", NULL, _("_File") },
  { "OpenRecentMenu", NULL, _("Open_Recent") },
  { "ExportMenu", NULL, _("_Export") },
  { "ViewMenu", NULL, _("_View") },
  { "EditMenu", NULL, _("_Edit") },
  { "ToolsMenu", NULL, _("_Tools") },
  { "SystemLayout", NULL, _("_System layout ...") },
  { "InsertMenu", NULL, _("_Insert") },
  { "UpbeatMenu", NULL, _("_Upbeat ...") },
  { "PrefMenu", NULL, _("_Preferences ...") },
  { "DynamicsMenu", NULL, _("_Dynamics ...") },
  { "LyricsMenu", NULL, _("_Lyrics") },
  { "HelpMenu", NULL, _("_Help") },
  { "config_midi_out", NULL, _("Configure MIDI out..."), NULL, _("configure MIDI output"), G_CALLBACK(config_midi_out) },
  { "config_midi_in", NULL, _("Configure MIDI in..."), NULL, _("configure MIDI input"), G_CALLBACK(config_midi_in) },
  { "config_paper", NULL, _("Configure Paper..."), NULL, _("configure paper"), G_CALLBACK(config_paper) },
  { "config_print_cmd", NULL, _("Configure Print..."), NULL, _("configure print command"), G_CALLBACK(config_print_cmd) },
  { "New", GTK_STOCK_NEW, _("New"), NULL, _("New File"), G_CALLBACK(new_file) },
  { "Open", GTK_STOCK_OPEN, _("Open  ..."), "<control>O", _("Open a file"), G_CALLBACK(restore_score) },
  { "Save", GTK_STOCK_SAVE, _("Save  ..."), NULL, _("Save a file"), G_CALLBACK(save_score) },
  { "SaveAs", GTK_STOCK_SAVE, _("Save As  ..."), NULL, _("Save a file"), G_CALLBACK(save_score_as) },
  { "print_file", GTK_STOCK_PRINT, _("Print"), NULL, _("print file"), G_CALLBACK(print_file) },
  { "import_musicxml", NULL, _("Import MusicXML ..."), NULL, _("import a musicxml file"), G_CALLBACK(import_musicxml) },
  { "import_midi", NULL, _("Import MIDI..."), NULL, _("import a midi file"), G_CALLBACK(import_midi) },
  { "import_midi2", NULL, _("Import MIDI(2)..."), NULL, _("import a midi file(2)"), G_CALLBACK(import_midi2) },
  { "copy", GTK_STOCK_COPY, _("Copy"), "<control>C", _("copy - Ctrl + C"), G_CALLBACK(copy_data) },
  { "paste", GTK_STOCK_PASTE, _("Paste"), "<control>V", _("paste - Ctrl + V"), G_CALLBACK(paste_data) },
  { "delete_block", NULL, _("Delete block"), "<control>X", _("delete block - Ctrl + X"), G_CALLBACK(delete_block) },
  { "append_system", NULL, _("append a system at end"), "<control>B", _("append system - Ctrl + B"), G_CALLBACK(append_system) },
  { "insert_block", NULL, _("Insert block"), NULL, _("insert block"), G_CALLBACK(insert_block) },
  { "insert_clef", NULL, _("Insert clef ..."), NULL, _("insert a clef"), G_CALLBACK(insert_clef) },
  { "empty_block", NULL, _("Empty block"), "<control>E", _("empty block - Ctrl + E"), G_CALLBACK(empty_block) },
  { "reposit_all", NULL, _("reposit all"), "<control>T", _("reposit the whole score - Ctrl + T"), G_CALLBACK(reposit_all) },
  { "export_midi", NULL, _("Export MIDI  ..."), NULL, _("export a midi file"), G_CALLBACK(export_midi) },
  { "export_lily", NULL, _("Export LilyPond  ..."), NULL, _("LilyPond export"), G_CALLBACK(export_lily) },
#ifdef CAIRO_HAS_PDF_SURFACE
  { "write_pdf", NULL,  _("Export PDF ..."), NULL,  _("export PDF"),  G_CALLBACK(write_pdf) }, 
#endif
  { "write_png", NULL,  _("Export P_NG ..."), NULL,  _("Write a png image"),  G_CALLBACK(write_png) }, 
#ifdef CAIRO_HAS_SVG_SURFACE
  { "write_svg", NULL,  _("Export S_VG ..."), NULL,  _("Write an svg image"),  G_CALLBACK(write_svg) }, 
#endif
  { "write_ps_action", NULL,  _("Export P_S ..."), NULL,  _("Write PostScript"), G_CALLBACK(write_ps) },
  { "new_window", NULL,  _("New Window"), NULL,  _("Open a new window"), G_CALLBACK(new_window) },
  { "excerpt", NULL,  _("Excerpt ..."), NULL,  _("Make an excerpt of the score"), G_CALLBACK(create_excerpt) },
  { "close_window", NULL,  _("Close Window"), NULL,  _("Close current window"), G_CALLBACK(close_window) },
/*  { "append_page", NULL, _("Append page"), NULL, _("append an empty page"), G_CALLBACK(append_page) },
  { "insert_page", NULL, _("Insert page"), NULL, _("insert an empty page"), G_CALLBACK(insert_page) }, */
  { "remove_page", NULL, _("Remove last page"), NULL, _("remove the last page"), G_CALLBACK(remove_page) },
  { "remove_empty_pages", NULL, _("Remove empty pages"), NULL, _("remove empty pages at end"), G_CALLBACK(remove_empty_pages) },
  { "change_spacement", NULL, _("Change spacement ..."), NULL, _("change the horizontal spacement"), G_CALLBACK(change_spacement) },
  { "change_scale", NULL, _("Change Scale ..."), NULL, _("change scale"), G_CALLBACK(change_scale) },
  { "config_record", NULL, _("Configure Record ..."), NULL, _("configure record"), G_CALLBACK(config_record) },
  { "config_drums", NULL, _("Configure Drums ..."), NULL, _("configure drums"), G_CALLBACK(config_drums) },
  { "toolbox", "tool-icon", _("Tools ..."), NULL, _("Tools "), G_CALLBACK(popup_tools) },
  { "set_upbeat_measure", NULL, _("give first measure"), NULL, _("set upbeat by giving the start of first full measure"), G_CALLBACK(set_upbeat_measure) },
  { "set_upbeat_start", NULL, _("give start"), NULL, _("set upbeat by giving the start of the score"), G_CALLBACK(set_upbeat_start) },
  { "score_info", NULL, _("Score info ..."), NULL, _("edit score info"), G_CALLBACK(edit_score_info) },
  { "insert_keysig", NULL, _("Insert keysig ... "), NULL, _("insert a key signature"), G_CALLBACK(insert_keysig) },
  { "insert_lines", NULL, _("Insert lines ... "), NULL, _("insert a horizontal line"), G_CALLBACK(insert_lines) },
  { "insert_signs", NULL, _("Insert sign ... "), NULL, _("insert a free placeable sign"), G_CALLBACK(insert_signs) },
  { "insert_slur", NULL, _("Slur"), "<ctrl>h", _("insert a slur - Ctrl+H"), G_CALLBACK(insert_slur) },
  { "insert_chord", NULL, _("chord ..."), NULL , _("insert a chord"), G_CALLBACK(insert_chord) },
  { "insert_chord_name", NULL, _("chordname ..."), NULL , _("insert a chordname"), G_CALLBACK(insert_chord_name) },
  { "insert_spacer", NULL, _("spacer"), NULL , _("insert a staff spacer"), G_CALLBACK(insert_spacer) },
  { "append_staff", NULL, _("Append staff"), "<control>a", _("append a new staff"), G_CALLBACK(append_staff) },
  { "import_recorded", NULL, _("Import recorded"), NULL, _("import recorded staff"), G_CALLBACK(importRecorded) },
  { "lyrics_editor", NULL, _("Lyrics editor ..."), NULL, _("edit lyrics in text editor"), G_CALLBACK(lyrics_editor) },
  { "dyn_ppp", NULL, _("ppp"), NULL, _("insert ppp"), G_CALLBACK(insert_ppp)},
  { "dyn_pp", NULL, _("pp"), NULL, _("insert pp"), G_CALLBACK(insert_pp)},
  { "dyn_p", NULL, _("p"), NULL, _("insert p"), G_CALLBACK(insert_p)},
  { "dyn_mp", NULL, _("mp"), NULL, _("insert mp"), G_CALLBACK(insert_mp)},
  { "dyn_sp", NULL, _("sp"), NULL, _("insert sp"), G_CALLBACK(insert_sp)},
  { "dyn_mf", NULL, _("mf"), NULL, _("insert mf"), G_CALLBACK(insert_mf)},
  { "dyn_sf", NULL, _("sf"), NULL, _("insert sf"), G_CALLBACK(insert_sf)},
  { "dyn_f", NULL, _("f"), NULL, _("insert f"), G_CALLBACK(insert_f)},
  { "dyn_ff", NULL, _("ff"), NULL, _("insert ff"), G_CALLBACK(insert_ff)},
  { "dyn_fff", NULL, _("fff"), NULL, _("insert fff"), G_CALLBACK(insert_fff)},
  { "insert_tempo", NULL, _("tempo ..."), NULL, _("insert tempo"), G_CALLBACK(insert_tempo)},
  { "insert_text", NULL, _("Text ..."), NULL, _("insert text"), G_CALLBACK(insert_text)},
  { "insert_accelerato", NULL, _("accelerando"), NULL, _("insert accelerando"), G_CALLBACK(insert_accelerando)},
  { "insert_ritardando", NULL, _("ritardando"), NULL, _("insert ritardando"), G_CALLBACK(insert_ritardando)},
  { "lyrics_mode1", NULL, _("lyrics1"), NULL, _("insert lyrics at line 1"), G_CALLBACK(set_lyrics_mode1)},
  { "lyrics_mode2", NULL, _("lyrics2"), NULL, _("insert lyrics at line 2"), G_CALLBACK(set_lyrics_mode2)},
  { "lyrics_mode3", NULL, _("lyrics3"), NULL, _("insert lyrics at line 3"), G_CALLBACK(set_lyrics_mode3)},
  { "lyrics_mode4", NULL, _("lyrics4"), NULL, _("insert lyrics at line 4"), G_CALLBACK(set_lyrics_mode4)},
  { "lyrics_mode5", NULL, _("lyrics5"), NULL, _("insert lyrics at line 5"), G_CALLBACK(set_lyrics_mode5)},
  { "insert_tuplet", NULL, _("Insert tuplet ..."), NULL, _("insert a tuplet (Ctrl + num)"), G_CALLBACK(insert_tuplet) },
  { "set_brace", "brace-icon", _("brace"), NULL, _("group staves by means of a brace"), G_CALLBACK(set_brace_system_delimiter) },
  { "set_bracket", "bracket-icon", _("bracket"), NULL, _("group staves by means of a bracket"), G_CALLBACK(set_bracket_system_delimiter) },
  { "set_connected", NULL, _("connect barlines"), NULL, _("set connected bar lines"), G_CALLBACK(set_connected_bar_line) },
  { "remove_element", GTK_STOCK_CANCEL, _("remove group"), NULL, _("remove a group of connected staves"), G_CALLBACK(remove_system_delimiter) },
  { "remove_tuplet", NULL, _("Remove tuplet"), NULL, _("remove a tuplet"), G_CALLBACK(remove_tuplet) },
  { "mute_staves", NULL, _("Muting ..."), NULL, _("mute some staves"), G_CALLBACK(mute_staves) },
  { "transpose", NULL, _("Transpose ..."), NULL, _("transpose"), G_CALLBACK(transpose) },
  { "hide_rests", NULL, _("hide rests"), "<control>R", _("hide (selected) rests"), G_CALLBACK(hide_rests) },
  { "unhide_rests", NULL, _("unhide rests"), "<control>U", _("unhide (selected) rests"), G_CALLBACK(unhide_rests) },
  { "hide_rests_of_all_voices", NULL, _("hide rests (all v.)"), NULL, _("hide selected rests (all v.)"), G_CALLBACK(hide_rests_of_all_voices) },
  { "unhide_rests_of_all_voices", NULL, _("unhide rests (all v.)"), NULL,  _("unhide selected rests (all v.)"), G_CALLBACK(unhide_rests_of_all_voices) },
  { "set_all_halfs_to_sharp", NULL, _("set all to sharp ..."), NULL,  _("set all flats to sharp"), G_CALLBACK(set_all_halfs_to_sharp) },
  { "set_all_halfs_to_flat", NULL, _("set all to flat ..."), NULL,  _("set all sharps to flat"), G_CALLBACK(set_all_halfs_to_flat) },
  { "about_nted", NULL, _("About NtEd"), NULL, NULL, G_CALLBACK(show_about) },
#ifdef YELP_PATH
  { "show_docu", NULL, _("Documentation ..."), NULL, NULL, G_CALLBACK(show_docu) },
#endif
  { "show_license", NULL, _("License"), NULL, NULL, G_CALLBACK(show_license) },
  { "Exit", GTK_STOCK_QUIT, _("E_xit"), "<control>Q", _("Exit the program"), G_CALLBACK(quit_app)}
};

const GtkToggleActionEntry NedMainWindow::accessory_buttons[] = {
   {"play", GTK_STOCK_MEDIA_PLAY, "play", NULL, _("play"), G_CALLBACK(do_play), FALSE},
   {"color_notes", "color-notes", "color notes", NULL, _("voices in different colors"), G_CALLBACK(set_paint_colored), FALSE},
   {"show_measure_numbers", NULL, _("show measure numbers"), NULL, _("show measure numbers"),  G_CALLBACK(initiate_repaint), FALSE},
   {"show_hidden_elements", NULL, _("show hidden elements"), NULL, _("show hidden elements"),  G_CALLBACK(toggle_show_hidden), FALSE},
   {"midi_input", "midikeyboard-icon", _("midi input"), NULL, _("midi input"), G_CALLBACK(toggle_midi_input), FALSE},
   {"record_midi", "record-icon", _("record midi"), "i", _("record midi - i"), G_CALLBACK(toggle_record_midi), FALSE}
};

const GtkActionEntry NedMainWindow::main_tools[] =  {
  {"zoom_in", GTK_STOCK_ZOOM_IN, _("in"), "<control>plus", _("Zoom in Ctrl + +"),  G_CALLBACK(zoom_in)},
  {"zoom_out", GTK_STOCK_ZOOM_OUT, _("out"), "<control>minus", _("Zoom out Ctrl + -"), G_CALLBACK(zoom_out)},
  {"undo", GTK_STOCK_UNDO, _("undo"), "<control>z", _("Undo Ctrl + z"), G_CALLBACK(do_undo)},
  {"redo", GTK_STOCK_REDO, _("redo"), "<control>y", _("Redo Ctrl + y"), G_CALLBACK(do_redo)},
  {"customize_beam", "isolate-icon", _("customize beam"), "<ctrl>j", _("customize beam - Ctrl + j"), G_CALLBACK(customize_beam)},
  {"flip_stem", "flipstem-icon", _("flip stem"), "<control>f", _("flip stem - Ctrl + f"), G_CALLBACK(flip_stem)}
};

const GtkToggleActionEntry NedMainWindow::insert_erease_button[] = {
  {"toggle_insert_erease_mode", "insert-mode-icon", _("insert"), "n", _("Toggle Insert/Erase Mode - n"), G_CALLBACK(toggle_insert_erease_mode), FALSE},
};

const GtkToggleActionEntry NedMainWindow::toggle_mode_buttons[] = {
  {"set_page_mode", "shift-mode-icon", _("page"), "p", _("Page Mode - p"),  G_CALLBACK(toggle_shift_mode), FALSE},
  {"toggle_keyboard_insert_mode", "key-icon", _("key"), "k", _("Keyboard insert mode - k"), G_CALLBACK(prepare_keyboard_insertion),  FALSE},
};


NedMainWindow::NedMainWindow() :
m_selected_note(NULL),
 m_selected_chord_or_rest(NULL),
 m_selected_stem(NULL),
 m_selected_free_replaceable(NULL),
 m_selected_spec_measure(NULL),
 m_selected_tie_forward(NULL),
 m_freetext(NULL),
 m_special_measures(NULL),
 m_last_staff_during_key_insertion(NULL),
 m_current_scale(SCALE_GRANULARITY), 
 m_leftx(0),
 m_topy(0),
 m_mouse_x(0),
 m_mouse_y(0),
 m_current_zoom_level(START_ZOOM_LEVEL),
 m_special_type(-1),
 m_pages(NULL),
 m_recentFileMergeId(-1),
 m_numerator(4),
 m_denominator(4),
 m_timesig_symbol(TIME_SYMBOL_NONE),
 m_staff_count(1),
 m_config_changed(FALSE),
 m_midi_tempo_inverse(START_TEMPO_INVERSE),
 m_last_touched_system(NULL),
 m_first_page_yoffs(0.0),
 m_upbeat_inverse(0),
 m_selected_group(NULL),
 m_avoid_feedback(FALSE),
 m_avoid_feedback_action(FALSE),
 m_lyrics_mode(NO_LYRICS),
 m_midi_recorder(NULL), 
 m_pointer_xpos(-1.0),
 m_paint_colored(FALSE),
 m_portrait(TRUE),
 m_keyboard_ctrl_mode(FALSE),
 m_draw_postscript(false),
 m_netto_indent(0.0),
 m_2ndnetto_indent(0.0),
 m_first_system_indent(0.0),
 m_2nd_system_indent(0.0),
 m_global_spacement_fac(1.0), 
 m_last_cursor_x(-1),
 m_last_cursor_y(-1),
 m_last_obj(NULL),
 m_record_midi_numerator(4),
 m_record_midi_denominator(4),
 m_record_midi_pgm(0),
 m_record_midi_tempo_inverse(170000),
 m_record_midi_metro_volume(64),
 m_record_midi_triplet_recognition(true),
 m_record_midi_f_piano(false),
 m_record_midi_dnt_split(false),
 m_record_midi_keysig(0)
#ifdef YELP_PATH
,m_docu(NULL)
#endif
 {
 	int i;
	NedResource::m_main_windows = g_list_append(NedResource::m_main_windows, this);

	m_command_history = new NedCommandHistory(this);
	m_special_sub_type.m_special_sub_type_int = -1;
	m_special_sub_type.m_chord_info.chord_ptr = NULL;

	if ((m_current_paper = NedResource::getPaperInfo("A4")) == NULL) {
		NedResource::Abort("paper info A4 not available");
	}

	m_score_info = new ScoreInfo();

	m_current_filename[0] = '\0';

#ifdef TTT
	char *str1, *str2, *str3, *str4;
	str1 = strdup("Хěвел");
	str2 = strdup("     Хěвел");
	str3 = strdup("Хěвел       ");
	str4 = strdup("    Хěвел       ");
	NedResource::removeWhiteSpaces(&str1);
	NedResource::DbgMsg(DBG_TESTING, "str1 = *%s*\n", str1);
	NedResource::removeWhiteSpaces(&str2);
	NedResource::DbgMsg(DBG_TESTING, "str2 = *%s*\n", str2);
	NedResource::removeWhiteSpaces(&str3);
	NedResource::DbgMsg(DBG_TESTING,"str3 = *%s*\n", str3);
	NedResource::removeWhiteSpaces(&str4);
	NedResource::DbgMsg(DBG_TESTING, "str4 = *%s*\n", str4);
#endif


	
	m_staff_contexts[0].m_key_signature_number = 0;
	m_staff_contexts[0].m_clef_number = 0;
	m_staff_contexts[0].m_clef_octave_shift = 0;
	m_staff_contexts[0].m_different_voices = false;
	for (i = 0; i < MAX_STAFFS; i++) {
		m_staff_contexts[i].m_staff_name = NULL;
		m_staff_contexts[i].m_staff_short_name = NULL;
		m_staff_contexts[i].m_group_name = NULL;
		m_staff_contexts[i].m_group_short_name = NULL;
	}
	for (i = 0; i < VOICE_COUNT; i++) {
		m_staff_contexts[0].voices[i].m_midi_volume = 64;
		m_staff_contexts[0].voices[i].m_midi_pan = 64;
		m_staff_contexts[0].voices[i].m_midi_program = 0;
		m_staff_contexts[0].m_muted[i] = false;
	}
	m_staff_contexts[0].m_midi_chorus = 0;
	m_staff_contexts[0].m_play_transposed = 0;
	m_staff_contexts[0].m_midi_reverb = 0;
	m_staff_contexts[0].m_midi_channel = 0;
	m_staff_contexts[0].m_flags = 0;
	m_selection_rect.width = m_selection_rect.height = 0;
	gettimeofday(&m_last_motion_call, NULL);
}

void NedMainWindow::createLayout(char *fname, char *yelparg) {
	FILE *fp;
	GtkAccelGroup *accel_group;
	GtkActionGroup *tool_action_group;
	GtkActionGroup *cursor_action_group;
	GtkActionGroup *accessories_action_group;
	GtkActionGroup *insert_erease_action_group;
	GError *error;
	GtkWidget *menubar;
	GtkWidget *toolbar;
	GdkColor bgcolor;
	GtkToolItem* pageitem;
	GtkToolItem* voicesitem;
	GtkWidget *vc_bu_vbox, *vc_bu_hbox1, *vc_bu_hbox2;
	int i;
	char pathfoldername[1024], *cptr;
#ifdef YELP_PATH
	m_docu = yelparg;
#endif

	m_hand = gdk_cursor_new(GDK_HAND2);
	m_pointer = gdk_cursor_new(GDK_TOP_LEFT_ARROW);
	m_pencil = gdk_cursor_new(GDK_PENCIL);


	m_current_zoom = NedResource::getZoomFactor(m_current_zoom_level);
	m_main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

	gtk_window_set_default_size (GTK_WINDOW (m_main_window), NedResource::m_width, NedResource::m_height);
	gtk_window_set_title (GTK_WINDOW (m_main_window), "Nted");
	gtk_window_set_default_icon(NedResource::m_the_nted_icon);

	m_menu_action_group = gtk_action_group_new ("MenuActions");
	gtk_action_group_set_translation_domain(m_menu_action_group, NULL);
	gtk_action_group_add_actions (m_menu_action_group, file_entries, G_N_ELEMENTS (file_entries), (void *) this);

	for (i = 0; i < MAX_RECENT_FILES; i++) {
		char ac_name[128], ac_label[128];
		sprintf(ac_name, "open_recent_action_%d", i);
		sprintf(ac_label, "Open Recent %d", i);
		m_recent_actions[i] = gtk_action_new (ac_name, ac_label, NULL, NULL);
	}
	m_open_recent_action_group = gtk_action_group_new ("OpenRecent");
	gtk_action_group_set_translation_domain(m_open_recent_action_group, NULL);

	tool_action_group = gtk_action_group_new ("ToolActions");
	gtk_action_group_set_translation_domain(tool_action_group, NULL);
	gtk_action_group_add_actions (tool_action_group, main_tools, G_N_ELEMENTS (main_tools), (void *) this);

	cursor_action_group = gtk_action_group_new ("CursorActions");
	gtk_action_group_set_translation_domain(cursor_action_group, NULL);
	gtk_action_group_add_toggle_actions (cursor_action_group, toggle_mode_buttons, G_N_ELEMENTS (toggle_mode_buttons), (void *) this);

	insert_erease_action_group = gtk_action_group_new ("InsertEreaseActions");
	gtk_action_group_set_translation_domain(insert_erease_action_group, NULL);
	gtk_action_group_add_toggle_actions (insert_erease_action_group, insert_erease_button, G_N_ELEMENTS (insert_erease_button), (void *) this);

	accessories_action_group = gtk_action_group_new ("AccessoryAction");
	gtk_action_group_set_translation_domain(accessories_action_group, NULL);
	gtk_action_group_add_toggle_actions (accessories_action_group, accessory_buttons, G_N_ELEMENTS (accessory_buttons), (void *) this);



	m_ui_manager = gtk_ui_manager_new ();
	gtk_ui_manager_insert_action_group (m_ui_manager, m_menu_action_group, 0);
	gtk_ui_manager_insert_action_group (m_ui_manager, tool_action_group, 1);
	gtk_ui_manager_insert_action_group (m_ui_manager, insert_erease_action_group, 3);
	gtk_ui_manager_insert_action_group (m_ui_manager, cursor_action_group, 3);
	gtk_ui_manager_insert_action_group (m_ui_manager, accessories_action_group, 4);
	gtk_ui_manager_insert_action_group (m_ui_manager, m_open_recent_action_group, 5);
	accel_group = gtk_ui_manager_get_accel_group (m_ui_manager);
	gtk_window_add_accel_group (GTK_WINDOW (m_main_window), accel_group);


	error = NULL;
	if (!gtk_ui_manager_add_ui_from_string (m_ui_manager, guiDescription, -1, &error)) {
		g_message ("building menus failed: %s", error->message);
		g_error_free (error);
		NedResource::Abort("error");
	}

	menubar = gtk_ui_manager_get_widget (m_ui_manager, "/MainMenu");
	toolbar = gtk_ui_manager_get_widget (m_ui_manager, "/ToolBar");
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

	m_undo_action = gtk_ui_manager_get_action (m_ui_manager, "/MainMenu/EditMenu/undo");
	if (m_undo_action == NULL) {
		NedResource::Abort("m_undo_action not found");
	}
	gtk_action_set_sensitive(m_undo_action, FALSE);
	m_redo_action = gtk_ui_manager_get_action (m_ui_manager, "/MainMenu/EditMenu/redo");
	if (m_redo_action == NULL) {
		NedResource::Abort("m_redo_action not found");
	}
	gtk_action_set_sensitive(m_redo_action, FALSE);

	m_insert_erease_mode_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/ToolBar/toggle_insert_erease_mode"));
	if (m_insert_erease_mode_action == NULL) {
		NedResource::Abort("insert_mode_erease_action not found");
	}

	m_shift_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/ToolBar/set_page_mode"));
	if (m_shift_action == NULL) {
		NedResource::Abort("m_shift_action not found");
	}
	m_keyboard_insert_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/ToolBar/toggle_keyboard_insert_mode"));
	if (m_keyboard_insert_action == NULL) {
		NedResource::Abort("m_keyboard_insert_action not found");
	}
	m_midi_input_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/ToolBar/midi_input"));
	if (m_midi_input_action == NULL) {
		NedResource::Abort("m_midi_input_action not found");
	}
	m_midi_record_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/ToolBar/record_midi"));
	if (m_midi_record_action == NULL) {
		NedResource::Abort("m_midi_record_action not found");
	}
	m_replay_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/ToolBar/play"));
	if (m_replay_action == NULL) {
		NedResource::Abort("m_replay_action not found");
	}
	m_remove_page_action = gtk_ui_manager_get_action (m_ui_manager, "/MainMenu/EditMenu/remove_page");
	if (m_remove_page_action == NULL) {
		NedResource::Abort("m_remove_page_action not found");
	}
	m_show_hidden_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/MainMenu/ViewMenu/show_hidden_elements"));
	if (m_show_hidden_action == NULL) {
		NedResource::Abort("m_show_hidden_action not found");
	}
	m_show_measure_numbers_action = GTK_TOGGLE_ACTION(gtk_ui_manager_get_action (m_ui_manager, "/MainMenu/ViewMenu/show_measure_numbers"));
	if (m_show_measure_numbers_action == NULL) {
		NedResource::Abort("m_show_measure_numbers_action not found");
	}
	m_customize_beam_button = gtk_ui_manager_get_widget(m_ui_manager, "/ToolBar/customize_beam");
	if (m_customize_beam_button == NULL) {
		NedResource::Abort("m_customize_beam_button not found");
	}

	gtk_action_set_sensitive(m_remove_page_action, FALSE);
    	g_signal_connect (m_main_window, "delete-event",
		      G_CALLBACK (close_window2), this);
    	g_signal_connect (m_main_window, "set-focus",
		      G_CALLBACK (OnSetFocus), this);
	double w = m_portrait ? m_current_paper->width : m_current_paper->height;
	double h = m_portrait ? m_current_paper->height : m_current_paper->width;
	w /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
	h /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
	NedPage *page = new NedPage(this, w, h, 0, 1, TRUE);
	m_pages = g_list_append(m_pages,  page);
	reposit();
	m_page_selector = gtk_spin_button_new_with_range (1.0, g_list_length(m_pages), 1.0);

	pageitem = gtk_tool_item_new();
	gtk_container_set_border_width(GTK_CONTAINER(pageitem), 5);
	gtk_container_add(GTK_CONTAINER(pageitem), m_page_selector);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), pageitem, 8);

	vc_bu_vbox = gtk_vbox_new (FALSE, 0);
	vc_bu_hbox1 = gtk_hbox_new (FALSE, 0);
	vc_bu_hbox2 = gtk_hbox_new (FALSE, 0);
	m_v1bu = gtk_radio_button_new_with_label(NULL, "1");
	m_v2bu = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_v1bu), "2");
	m_v3bu = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_v1bu), "3");
	m_v4bu = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_v1bu), "4");

	m_voice_buttons = gtk_radio_button_get_group(GTK_RADIO_BUTTON(m_v1bu));

	GdkColor bucolor;
	bucolor.pixel = 0;
	bucolor.red = V1RED; bucolor.green = V1GREEN; bucolor.blue = V1BLUE;
	gtk_widget_modify_base(m_v1bu, GTK_STATE_NORMAL, &bucolor);
	bucolor.red = V2RED; bucolor.green = V2GREEN; bucolor.blue = V2BLUE;
	gtk_widget_modify_base(m_v2bu, GTK_STATE_NORMAL, &bucolor);
	bucolor.red = V3RED; bucolor.green = V3GREEN; bucolor.blue = V3BLUE;
	gtk_widget_modify_base(m_v3bu, GTK_STATE_NORMAL, &bucolor);
	bucolor.red = V4RED; bucolor.green = V4GREEN; bucolor.blue = V4BLUE;
	gtk_widget_modify_base(m_v4bu, GTK_STATE_NORMAL, &bucolor);

	g_signal_connect (GTK_BUTTON(m_v1bu), "pressed", G_CALLBACK (select_voice), (void *) this);
	g_signal_connect (GTK_BUTTON(m_v2bu), "pressed", G_CALLBACK (select_voice), (void *) this);
	g_signal_connect (GTK_BUTTON(m_v3bu), "pressed", G_CALLBACK (select_voice), (void *) this);
	g_signal_connect (GTK_BUTTON(m_v4bu), "pressed", G_CALLBACK (select_voice), (void *) this);
	gtk_box_pack_start (GTK_BOX (vc_bu_hbox1), m_v1bu, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vc_bu_hbox1), m_v2bu, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vc_bu_hbox2), m_v3bu, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vc_bu_hbox2), m_v4bu, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vc_bu_vbox), vc_bu_hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vc_bu_vbox), vc_bu_hbox2, FALSE, FALSE, 0);
	voicesitem = gtk_tool_item_new();
	gtk_container_add(GTK_CONTAINER(voicesitem), vc_bu_vbox);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), voicesitem, 10);

    m_drawing_area = gtk_drawing_area_new ();

    bgcolor.pixel = 0;
    bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
    gtk_widget_modify_bg(GTK_WIDGET(m_drawing_area), GTK_STATE_NORMAL, &bgcolor);


	 GTK_WIDGET_SET_FLAGS(m_drawing_area, GTK_CAN_FOCUS);
	 GTK_WIDGET_SET_FLAGS(m_main_window, GTK_CAN_FOCUS);
	gtk_widget_add_events(m_drawing_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK |
			GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
    g_signal_connect (m_drawing_area, "expose-event",
		      G_CALLBACK (handle_expose), (void *) this);
    g_signal_connect (m_drawing_area, "button-press-event",
		      G_CALLBACK (handle_button_press), (void *) this);
    g_signal_connect (m_drawing_area, "button-release-event",
		      G_CALLBACK (handle_button_release), (void *) this);
    g_signal_connect (m_drawing_area, "motion-notify-event",
		      G_CALLBACK (handle_motion), (void *) this);
    g_signal_connect (m_drawing_area, "size-allocate",
		      G_CALLBACK (size_change_handler), (void *) this);
    g_signal_connect (m_drawing_area, "key-press-event",
		      G_CALLBACK (key_press_handler), (void *) this);
    g_signal_connect (m_drawing_area, "key-release-event",
		      G_CALLBACK (key_release_handler), (void *) this);
    g_signal_connect (m_drawing_area, "enter-notify-event",
		      G_CALLBACK (window_enter_handler), (void *) this);
    g_signal_connect (m_drawing_area, "leave-notify-event",
		      G_CALLBACK (window_leave_handler), (void *) this);
    g_signal_connect (m_drawing_area, "scroll-event",
		      G_CALLBACK (scroll_event_handler), (void *) this);
    g_signal_connect (m_main_window, "delete-event",
		      G_CALLBACK (handle_delete), (void *) this);
    g_signal_connect (m_page_selector, "value-changed",
		      G_CALLBACK (handle_page_request), (void *) this);

	m_main_vbox = gtk_vbox_new (FALSE, 0);
	
	gtk_box_pack_start (GTK_BOX (m_main_vbox), menubar, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (m_main_vbox), toolbar, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (m_main_vbox), m_drawing_area, TRUE, TRUE, 0);
    gtk_container_add (GTK_CONTAINER (m_main_window), m_main_vbox);

    gtk_widget_show_all (m_main_window);
   gtk_toggle_action_set_active(m_show_measure_numbers_action, TRUE);
	m_im_context = gtk_im_context_simple_new ();
	g_signal_connect (m_im_context, "commit", G_CALLBACK (OnCommit), this);


    gdk_window_set_cursor (GDK_WINDOW(m_drawing_area->window), m_pointer);
    gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(m_insert_erease_mode_action), TRUE);
    if (fname != NULL) {
	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr, "Cannot open %s\n", fname);
		
	}
	else {
		do_restore(fp, fname);
		fclose(fp);
		if (fname[0] == '/' && (cptr = strrchr(fname, '/')) != NULL) {
			strncpy(pathfoldername, fname, cptr - fname);
			pathfoldername[cptr - fname] = '\0';
			NedResource::m_last_folder = strdup(pathfoldername);
		}
	}
    }
    updateRecentFiles();
    NedResource::popup_tools(this);

#ifdef WITH_BANNER
#define BANNERSIZE 4.0
    m_banner1 = new NedPangoCairoText(m_drawing_area->window, "Vorführung", STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
                                        STAFF_NAME_FONT_WEIGHT, BANNERSIZE, m_current_zoom, getCurrentScale(), false);
    m_banner2 = new NedPangoCairoText(m_drawing_area->window, "dieser", STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
                                        STAFF_NAME_FONT_WEIGHT, BANNERSIZE, m_current_zoom, getCurrentScale(), false);
    m_banner3 = new NedPangoCairoText(m_drawing_area->window, "Software", STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
                                        STAFF_NAME_FONT_WEIGHT, BANNERSIZE, m_current_zoom, getCurrentScale(), false);
    m_banner4 = new NedPangoCairoText(m_drawing_area->window, "11:00 Uhr", STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
                                        STAFF_NAME_FONT_WEIGHT, BANNERSIZE, m_current_zoom, getCurrentScale(), false);
    m_banner5 = new NedPangoCairoText(m_drawing_area->window, "Raum N105", STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
                                        STAFF_NAME_FONT_WEIGHT, BANNERSIZE, m_current_zoom, getCurrentScale(), false);
#endif
}

NedMainWindow::~NedMainWindow() {
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		delete (NedPage *) lptr->data;
	}
	g_list_free(m_pages);
	m_pages = NULL;
	gtk_widget_destroy (m_main_window);
}


bool NedMainWindow::isTimsigChangingMeasure(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return false;
	}

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == meas_num && ((SpecialMeasure *) lptr->data)->type == TIMESIG) {
			return true;
		}
	}
	return false;
}
	


int NedMainWindow::getNumerator(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return m_numerator;
	}

	int num = m_numerator;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number > meas_num) {
			return num;
		}
		if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) continue;
		num = ((SpecialMeasure *) lptr->data)->numerator;
		/*
		if (meas_num == 1) {
			NedResource::DbgMsg(DBG_TESTING, "Stelle 3 num = %d, ((SpecialMeasure *) lptr->data)->measure_number = %d\n",
				num, ((SpecialMeasure *) lptr->data)->measure_number);
		}
		*/
	}

	/*
	if (meas_num == 1) {
		NedResource::DbgMsg(DBG_TESTING, "Stelle 4 num = %d\n", num);
	}
	*/
	return num;

}

int NedMainWindow::getDenominator(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return m_denominator;
	}

	int denom = m_denominator;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number > meas_num) {
			return denom;
		}
		if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) continue;
		denom = ((SpecialMeasure *) lptr->data)->denominator;
	}

	return denom;

}

unsigned int NedMainWindow::getTimesigSymbol(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return TIME_SYMBOL_NONE;
	}

	unsigned int symbol = m_timesig_symbol;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number > meas_num) {
			return symbol;
		}
		if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) continue;
		symbol = ((SpecialMeasure *) lptr->data)->symbol;
	}

	return symbol;

}

unsigned int NedMainWindow::getTimesigSymbolOfThisMeasure(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return TIME_SYMBOL_NONE;
	}

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == meas_num) {
			if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) return TIME_SYMBOL_NONE;
			return ((SpecialMeasure *) lptr->data)->symbol;
		}
	}

	return TIME_SYMBOL_NONE;

}

int NedMainWindow::getNumeratorOfThisMeasure(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return 4;
	}


	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == meas_num) {
			if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) return 4;
			return ((SpecialMeasure *) lptr->data)->numerator;
		}
	}

	return 4;

}

int NedMainWindow::getDenominatorOfThisMeasure(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return 4;
	}

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == meas_num) {
			if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) return 4;
			return ((SpecialMeasure *) lptr->data)->denominator;
		}
	}

	return 4;
}



unsigned int NedMainWindow::getMeasureDuration(int meas_num) {
	GList *lptr;
	if (meas_num < 0) {
		return WHOLE_NOTE;
	}

	unsigned int duration = m_numerator * WHOLE_NOTE / m_denominator;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number > meas_num) {
			return duration;
		}
		if ((((SpecialMeasure *) lptr->data)->type & TIMESIG) == 0) continue;
		duration = ((SpecialMeasure *) lptr->data)->measure_duration;
	}

	return duration;

}

bool NedMainWindow::findTimeOfMeasure(int meas_num, unsigned long long *meas_time) {
	GList *lptr;
	unsigned long long system_offs = 0;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		if (((NedPage *) lptr->data)->findTimeOfMeasure(meas_num, meas_time, &system_offs)) {
			*meas_time += system_offs;
			return true;
		}
	}
	return false;
}

void NedMainWindow::setAllUnpositioned() {
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->setAllUnpositioned();
	}
}

void NedMainWindow::zoomFreeReplaceables(double zoom, double scale) {
	GList *lptr;
	if (m_score_info->title != NULL && strlen(m_score_info->title->getText()) > 0) {
		m_score_info->title->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->subject != NULL && strlen(m_score_info->subject->getText()) > 0) {
		m_score_info->subject->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->composer != NULL && strlen(m_score_info->composer->getText()) > 0) {
		m_score_info->composer->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->arranger != NULL && strlen(m_score_info->arranger->getText()) > 0) {
		m_score_info->arranger->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->copyright != NULL && strlen(m_score_info->copyright->getText()) > 0) {
		m_score_info->copyright->setZoom(m_current_zoom, 1.0);
	}

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->zoomFreeReplaceables(zoom, scale);
	}
}

void NedMainWindow::removeEmptyInteriourPages() {
	GList *lptr;
	bool empty_found;

	do {
		empty_found = false;
		lptr = g_list_first(m_pages);

		while (lptr) {
			if (((NedPage *) lptr->data)->isEmpty()) {
				m_pages = g_list_delete_link(m_pages, lptr);
				lptr = g_list_first(m_pages);
				empty_found = true;
				continue;
			}
			lptr = g_list_next(lptr);
				
		}
	}
	while (empty_found);
}

NedSystem *NedMainWindow::getNextSystem(NedPage *page, NedCommandList *command_list /* ev. == NULL */) {
	GList *lptr;
	int len;
	unsigned int measure_number;
	NedPage *newpage;


	if ((lptr = g_list_find(m_pages, page)) == NULL) {
		NedResource::Abort("NedMainWindow::getNextSystem");
	}
	lptr = g_list_next(lptr);
	if (lptr == NULL) {
		if (command_list == NULL) {
			return NULL;
		}
		len = g_list_length(m_pages);
		measure_number = ((NedPage *) g_list_last(m_pages)->data)->getNumberOfLastMeasure();
		NedAppendNewPageCommand *app_page_command = new NedAppendNewPageCommand(this, len, measure_number + 1);
		app_page_command->execute(); // the commend list is not executed
		resetPointerLastTouchedSystems();
		command_list->addCommand(app_page_command);
		lptr = g_list_last(m_pages);
	}
	newpage = (NedPage *) lptr->data;
	if (newpage->isEmpty()) return NULL;
	return newpage->getFirstSystem();
}

NedPage *NedMainWindow::getNextPage(NedPage *page, NedCommandList *command_list /* ev. == NULL */) {
	GList *lptr;
	int len;
	unsigned int measure_number;

	if ((lptr = g_list_find(m_pages, page)) == NULL) {
		NedResource::Abort("NedMainWindow::getNextPage");
	}
	if ((lptr = g_list_next(lptr)) == NULL) {
		if (command_list == NULL) {
			return NULL;
		}
		measure_number = ((NedPage *) g_list_last(m_pages)->data)->getNumberOfLastMeasure();
		len = g_list_length(m_pages);
		NedAppendNewPageCommand *app_page_command = new NedAppendNewPageCommand(this, len, measure_number + 1);
		app_page_command->execute(); // the commend list is not executed
		command_list->addCommand(app_page_command);
		lptr = g_list_last(m_pages);
	}
	return ((NedPage *) lptr->data);
}

NedPage *NedMainWindow::getPreviousPage(NedPage *page) {
	GList *lptr;

	if ((lptr = g_list_find(m_pages, page)) == NULL) {
		NedResource::Abort("NedMainWindow::getNextPage");
	}
	if ((lptr = g_list_previous(lptr)) == NULL) {
		return NULL;
	}
	return ((NedPage *) lptr->data);
}


void NedMainWindow::setUnRedoButtons(bool execute_possible, bool unexecute_possible) {

	gtk_action_set_sensitive(m_undo_action, unexecute_possible);
	gtk_action_set_sensitive(m_redo_action, execute_possible);
}

void NedMainWindow::updatePageCounter() {
	gtk_spin_button_set_range(GTK_SPIN_BUTTON(m_page_selector), 1, g_list_length(m_pages));
	m_selection_rect.width = m_selection_rect.height = 0;
	gtk_action_set_sensitive(m_remove_page_action, (g_list_length(m_pages) > 1));
}

double NedMainWindow::getLeftX() {
	if (m_draw_postscript) return 0.0;
	return m_leftx;
}

double NedMainWindow::getTopY() {
	if (m_draw_postscript) return 0.0;
	return m_topy;
}



void NedMainWindow::draw (cairo_t *cr, int width, int height)
{
	cairo_scaled_font_t *scaled_font;
	bool dummy = false;
	scaled_font = NedResource::getScaledFont(m_current_zoom_level);
	GdkColor sel_color;
	//cairo_scale(cr, 0.4, 0.4);
#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(m_current_zoom_level));
	cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	GList *lptr;
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->draw(cr, gtk_toggle_action_get_active(m_show_measure_numbers_action), width, height);
	}
	cairo_new_path(cr);
#ifdef WITH_BANNER
#define BANNER_Y_DIST 180
#define BANNER_X 50
	cairo_set_source_rgba(cr, 0.0, 0.0, 1.0, 0.3);
	m_banner1->draw(cr, BANNER_X, 100 + 0 * BANNER_Y_DIST, 200, 1);
	m_banner2->draw(cr, BANNER_X, 100 + 1 * BANNER_Y_DIST, 200, 1);
	m_banner3->draw(cr, BANNER_X, 100 + 2 * BANNER_Y_DIST, 200, 1);
	m_banner4->draw(cr, BANNER_X, 100 + 3 * BANNER_Y_DIST, 200, 1);
	m_banner5->draw(cr, BANNER_X, 100 + 4 * BANNER_Y_DIST, 200, 1);
	cairo_stroke(cr);
	cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.0);
#endif


	if (!NedResource::isPlaying() && m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, 1.0);
		cairo_set_source_rgb (cr, 0.4, 0.0, 1.0);
		cairo_move_to(cr, m_selection_rect.x, m_selection_rect.y);
		cairo_line_to(cr, m_selection_rect.x + m_selection_rect.width, m_selection_rect.y);
		cairo_line_to(cr, m_selection_rect.x + m_selection_rect.width, m_selection_rect.y + m_selection_rect.height);
		cairo_line_to(cr, m_selection_rect.x , m_selection_rect.y + m_selection_rect.height);
		cairo_line_to(cr, m_selection_rect.x , m_selection_rect.y);
		cairo_stroke(cr);

		cairo_scale(cr, getCurrentScale(), getCurrentScale());
    		sel_color.pixel = 0;
    		sel_color.red = 200; sel_color.green = 0; sel_color.blue =  250;
		for (lptr = g_list_first(m_selected_group); lptr; lptr = g_list_next(lptr)) {
			((NedChordOrRest *) lptr->data)->draw(cr, &dummy);
		}
		cairo_identity_matrix(cr);
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}

}

void NedMainWindow::repaintDuringReplay(NedNote *notes[], int num_notes) {
	int i;
	bool dummy;
	cairo_t *cr;
	cr = gdk_cairo_create (m_drawing_area->window);
	cairo_scaled_font_t *scaled_font;
	scaled_font = NedResource::getScaledFont(m_current_zoom_level);
#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(m_current_zoom_level));
	cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	cairo_scale(cr, getCurrentScale(), getCurrentScale());
	for (i = 0; i < num_notes; i++) {
		if (notes[i]->getChord()->getType() == TYPE_GRACE) {
			notes[i]->getChord()->draw(cr, &dummy);
			continue;
		}
		notes[i]->draw(cr);
	}
	cairo_destroy (cr);

}

void NedMainWindow::reposit(NedCommandList *command_list /* = NULL */, NedPage *start_page /* = NULL */, NedSystem *start_system /* = NULL */,
		bool with_progress_bar /* = false */) {
	GList *lptr, *plptr;
	GtkWidget *progressbar = NULL, *progressdialog = NULL;
	bool changes, ch;
	//bool page_deleted = false;
	int pass, lowest_page = 0;
	int page_number;
	double percent;
	NedSystem *system, *next_system;
	m_start_system_for_renumbering = NULL;
	m_start_measure_number_for_renumbering = (1 << 30);
	//NedRemovePageCommand *rem_page_cmd;
	int measure_number;


	if (start_page == NULL) {
		start_page = ((NedPage *) g_list_first(m_pages)->data);
		plptr = g_list_first(m_pages);
	}
	else {
		if ((plptr = g_list_find(m_pages, start_page)) == NULL) {
			// can happen in unexecute if the page ist inserted
			//NedResource::Abort("NedMainWindow::reposit");
			start_page = ((NedPage *) g_list_first(m_pages)->data);
			plptr = g_list_first(m_pages);
		}
	}
	resetPointerLastTouchedSystems();

	if (with_progress_bar) {
		progressdialog = gtk_dialog_new_with_buttons("Placing", NULL, (GtkDialogFlags) 0, NULL);
		progressbar = gtk_progress_bar_new();
		gtk_container_add (GTK_CONTAINER (GTK_DIALOG(progressdialog)->vbox), progressbar);
		gtk_widget_show_all(progressdialog);
	}

#ifdef XXX
	unsigned int old_page_map = 0;
	unsigned int page_map = 0;
	int zz = 0;
#endif
	do {
#ifdef XXX
		page_map = 0;
#endif
		changes = FALSE;
		if (start_system == NULL) {
			system = start_page->getFirstSystem();
		}
		else {
			system = start_system;
			system->m_is_positioned = false;
		}
		measure_number = system->getNumberOfFirstMeasure();
		if (with_progress_bar) {
			lowest_page = 100000;
		}
		while (system != NULL) {
			ch = system->reposit(measure_number, command_list, &next_system);
			/*
			if (ch) {
				printf("pag: %d, sys %d: changes = 1\n", system->getPage()->getPageNumber(),
					system->getSystemNumber()); fflush(stdout);
			}
			*/
			measure_number = system->getNumberOfLastMeasure();
			measure_number++;
#ifdef XXX
			if (ch) {
				page_map |= (1 << system->getPage()->getPageNumber());
			}
#endif
			if (with_progress_bar) {
				if (ch) {
					if (system->getPage()->getPageNumber() < lowest_page) {
						lowest_page = system->getPage()->getPageNumber();
					}
				}
			}
			changes = changes || ch;
			system = next_system;
		}
		if (with_progress_bar) {
			percent = (double) lowest_page / (double) g_list_length(m_pages);
			if (percent > 1.0) percent = 1.0;
			gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressbar), percent);
			g_main_context_iteration(NULL, FALSE);
		}
#ifdef XXX
		if (old_page_map != page_map) {
			for (int i = 0; i < 31; i++) {
				NedResource::DbgMsg(DBG_TESTING, "%c ", (page_map & (1 << i)) == 0 ? '.' : 'x');
			}
			NedResource::DbgMsg(DBG_TESTING, "\n"); 
			old_page_map = page_map;
		}
		zz++;
#endif
	}
	while (changes);
#ifdef XXX
	NedResource::DbgMsg(DBG_TESTING, "%d elemente bewegt\n", NedClipBoard::count); 
	//NedClipBoard::count = 0;
#endif

	for (pass = 0; pass < 2; pass++) {
		do {
			changes = FALSE;
			ch = TRUE;
			for (lptr = plptr; lptr; lptr = g_list_next(lptr)) {
				ch = ((NedPage *) lptr->data)->placeStaffs(pass, command_list);
				changes = changes || ch;
			}
		}
		while (changes);
	}
#ifdef UNNEEDED // removing a page is already done during placeStaffs (above)
	if (command_list != NULL) {
		command_list->setRenumber();
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->isEmpty()) {
				rem_page_cmd = new NedRemovePageCommand(this, (NedPage *) lptr->data);
				rem_page_cmd->execute();
				page_deleted = true;
				lptr = g_list_first(m_pages);
				command_list->addCommand(rem_page_cmd);
			}
		}
		if (page_deleted) {
			command_list->setRenumberPages();
			renumberPages();
		}
			
	}
#endif
	deletePagesWithoutSystem();
	for (page_number = 0, lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->setPageNumber(page_number++);
		((NedPage *) lptr->data)->recomputeFreeReplaceables();
		((NedPage *) lptr->data)->computeTuplets(); // do it here again, otherwise
               // it can happen some tuplets are not recomputed
	}
	renumberMeasures(NULL, 1, true);
	if (with_progress_bar) {
		 gtk_widget_destroy (GTK_WIDGET(progressdialog));
	}
}

void NedMainWindow::renumberMeasures(NedSystem *start_system /* =  NULL */, int measure_number /* = 1 */, bool force /* = false */) {
	NedSystem *system;
	NedPage *start_page;

/*
	GList *lptr;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == 17) {
			NedResource::DbgMsg(DBG_TESTING, "setspecial (17) auf 0x0 (0x%x)\n", &(((SpecialMeasure *) lptr->data)->measure_number));
		}
		((SpecialMeasure *) lptr->data)->measure = NULL;
	}
	*/

	system = start_system;
	if (start_system == NULL) {
		start_page = (NedPage *) g_list_first(m_pages)->data;
		system = start_page->getFirstSystem();
	}
	while (system != NULL) {
		system->renumberMeasures(&measure_number, m_special_measures, force);
		system = system->getPage()->getNextSystem(system);
	}
}

void NedMainWindow::setNewMeasureSpread(double spread) {
	NedSystem *system;
	system = ((NedPage *) g_list_first(m_pages)->data)->getFirstSystem();

	while (system != NULL) {
		system->setNewMeasureSpread(spread);
		system = system->getPage()->getNextSystem(system);
	}
}

bool NedMainWindow::needsARepLine(int measure_number) {
	int current_type = 0;
	GList *lptr;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number > measure_number) {
			return current_type != 0;
		}
		switch ((((SpecialMeasure *) lptr->data)->type & END_TYPE_MASK)) {
			case REP1END:
			case REP2END: current_type = 0; break;
		}
		switch ((((SpecialMeasure *) lptr->data)->type & START_TYPE_MASK)) {
			case REP1START:
			case REP2START: current_type = ((SpecialMeasure *) lptr->data)->type; break;
		}
	}
	return false;
}

gboolean NedMainWindow::handle_expose (GtkWidget *widget,
       GdkEventExpose *event,
       gpointer	       data)
{
	cairo_t *cr;
	NedMainWindow *main_window = (NedMainWindow *) data;
	GList *lptr;
	int c, line, yy, l;
	double xpos, ypos, bottom;
	NedStaff *dummy;

	
	cr = gdk_cairo_create (widget->window);
	
	main_window->draw (cr, widget->allocation.width, widget->allocation.height);
	
	if (main_window->m_lyrics_mode < 0 && (main_window->getMainWindowStatus() & INSERT_EREASE_MODE) != 0 &&
			!NedResource::isPlaying() && main_window->m_pointer_xpos >= 0.0) {
		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->findLine(main_window->m_pointer_xpos, main_window->m_pointer_ypos, &ypos, &line, &bottom, &dummy))  {
				ypos *= (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
				bottom *= (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
				xpos = main_window->m_pointer_xpos;
#ifdef OOO
#ifdef CCC
				Display *dpy = cairo_xlib_surface_get_display (cairo_get_target (cr));
				Window win = cairo_xlib_surface_get_drawable (cairo_get_target (cr));
#else
				Display *dpy = gdk_x11_drawable_get_xdisplay(widget->window);
				Window win =gdk_x11_drawable_get_xid(widget->window);
#endif
				GC gc = DefaultGC(dpy, DefaultScreen(dpy));
				XSetForeground(dpy, gc, 0x0000bb);
				linewidth = (int) main_window->m_current_zoom * 0.5;
				rad1 = (int) main_window->m_current_zoom * 0.15;
				rad2 = (int) main_window->m_current_zoom * 0.1;
				XDrawArc(dpy, win, gc, main_window->m_pointer_xpos - rad1, ypos - rad2, 2 * rad1, 2 * rad2, 0, 64 * 360);
				if (line < -9) {
					c = -(line + 8) / 2; 
					for (l = 0; l < c; l++) {
						yy = (int) (bottom - ((5 + l) * LINE_DIST) * main_window->m_current_zoom);
						XDrawLine(dpy, win, gc, main_window->m_pointer_xpos - linewidth / 2, yy, main_window->m_pointer_xpos + linewidth / 2, yy);
					}
				}
				if (line > 1) {
					c = line / 2; 
					for (l = 0; l < c; l++) {
						yy = (int) (bottom + ((1 + l) * LINE_DIST) * main_window->m_current_zoom);
						XDrawLine(dpy, win, gc, main_window->m_pointer_xpos - linewidth / 2, yy, main_window->m_pointer_xpos + linewidth / 2, yy);
					}
				}
				break;
			}
#else
				double dlinewidth =  main_window->m_current_zoom * 0.5 * (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
				double drad1 = main_window->m_current_zoom * 0.15 * (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
				double drad2 = main_window->m_current_zoom * 0.24 * (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
				cairo_new_path(cr);
				cairo_set_line_width(cr, 0.02 * main_window->m_current_zoom);
				cairo_set_source_rgb (cr, 0.0, 0.0, 0.9);
				//cairo_arc(cr, main_window->m_pointer_xpos - rad1, ypos - rad1, 2 * rad1, 0, 2.0 *M_PI);
				cairo_move_to(cr, xpos - drad1, ypos);
				cairo_curve_to(cr,  xpos - drad1, ypos,
							 xpos, ypos + drad2,
							 xpos +  drad1, ypos);
				cairo_curve_to(cr, xpos +  drad1, ypos,
							xpos, ypos - drad2,
							xpos - drad1, ypos);
/*
				cairo_move_to(cr, main_window->m_pointer_xpos - drad1, ypos);
				cairo_line_to(cr, main_window->m_pointer_xpos, ypos + drad2);
				cairo_line_to(cr, main_window->m_pointer_xpos + drad1, ypos);
				cairo_line_to(cr, main_window->m_pointer_xpos, ypos - drad2);
				cairo_line_to(cr, main_window->m_pointer_xpos - drad1, ypos);
*/
				cairo_stroke(cr);
				if (line < -9) {
					c = -(line + 8) / 2; 
					for (l = 0; l < c; l++) {
						yy = (int) (bottom - ((5 + l) * LINE_DIST) * main_window->m_current_zoom * main_window->getCurrentScale());
						
						cairo_new_path(cr);
						cairo_move_to(cr, xpos - dlinewidth / 2, yy);
						cairo_line_to(cr, xpos + dlinewidth / 2, yy);
						cairo_stroke(cr);
					}
				}
				if (line > 1) {
					c = line / 2; 
					for (l = 0; l < c; l++) {
						yy = (int) (bottom + ((1 + l) * LINE_DIST) * main_window->m_current_zoom * main_window->getCurrentScale());
						cairo_new_path(cr);
						cairo_move_to(cr, xpos - dlinewidth / 2, yy);
						cairo_line_to(cr, xpos + dlinewidth / 2, yy);
						cairo_stroke(cr);
					}
				}
				break;
			}
#endif
		}
	}
	cairo_destroy (cr);
	
	return FALSE;
}

void NedMainWindow::do_undo(GtkWidget  *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedResource::m_avoid_immadiate_play = TRUE;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_selected_note = NULL;
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_spec_measure = NULL;
	main_window->m_selected_tie_forward = NULL;
	main_window->m_command_history->unexecute();
	main_window->repaint();
	NedResource::m_avoid_immadiate_play = FALSE;
}

int NedMainWindow::getLyricsMode() {
	return m_lyrics_mode;
}

unsigned int NedMainWindow::getMainWindowStatus() {
	unsigned int status = 0;
	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(m_insert_erease_mode_action))) {
		status |= INSERT_EREASE_MODE;
	}
	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(m_shift_action))) {
		status |= SHIFT_MODE;
	}
	return status;
}

bool NedMainWindow::getShowHidden() {
	return gtk_toggle_action_get_active(m_show_hidden_action);
}

bool NedMainWindow::hasFocus() {
	return  gtk_widget_is_focus(m_main_window);
}

void NedMainWindow::do_redo(GtkWidget  *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedResource::m_avoid_immadiate_play = TRUE;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_selected_note = NULL;
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_spec_measure = NULL;
	main_window->m_selected_tie_forward = NULL;
	main_window->m_command_history->execute();
	main_window->repaint();
	NedResource::m_avoid_immadiate_play = FALSE;
}


int NedMainWindow::getCurrentVoice() {
	int voice_nr = 3;
	GSList *l;
	for (l = m_voice_buttons; l ; l = l->next, voice_nr--) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(l->data))) {
			return voice_nr; 
		}
	}
	NedResource::Abort("NedMainWindow::getCurrentVoice");
	return 0;
}

void NedMainWindow::select_voice(GtkButton *button, gpointer data) {}
void NedMainWindow::do_play(GtkAction *action, gpointer data) {
	if (!NedResource::alsaSequencerOutOk()) {
		NedResource::Info(_("Cannot replay because could not open ALSA sequencer"));
		return;
	}
	NedMainWindow *main_window = (NedMainWindow *) data;

	main_window->replay(gtk_toggle_action_get_active(GTK_TOGGLE_ACTION (action)));
}

void NedMainWindow::replay(bool on) {
	int i, j;
	GList *lptr;
	SpecialMeasure *spec_ptr;
	int last_rep_open = 1, last_alternative1 = -1;
	m_last_y_adjustment = NULL;

	if (NedResource::isPlaying() && NedResource::getMainwWindowWithLastFocus() != this) {
		 gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(m_replay_action), FALSE);
		 return;
	}

	if (on) {
   		gtk_toggle_action_set_active(m_midi_record_action, FALSE);
   		gtk_toggle_action_set_active(m_midi_input_action, FALSE);
		for (i = 0; i < m_staff_count; i++) {
			m_staff_contexts[i].m_effective_channel = m_staff_contexts[i].m_midi_channel;
			m_staff_contexts[i].m_pitch_offs = 0;
			if (m_staff_contexts[i].m_different_voices) {
				for (j = 0; j < VOICE_COUNT; j++) {
					m_staff_contexts[i].voices[j].m_current_midi_program = m_staff_contexts[i].voices[j].m_midi_program;
					NedResource::setStartVolume(m_staff_contexts[i].m_midi_channel, j,
						 m_staff_contexts[i].voices[j].m_midi_volume);
				}
			}
			else {
				for (j = 0; j < VOICE_COUNT; j++) {
					m_staff_contexts[i].voices[j].m_current_midi_program = m_staff_contexts[i].voices[j].m_midi_program;
					NedResource::setStartVolume(m_staff_contexts[i].m_midi_channel, j,
						 m_staff_contexts[i].voices[0].m_midi_volume);
				}
			}
		}

		for (i = 0; i < m_staff_count; i++) {
			//NedResource::MidiProgramChange(m_staff_contexts[i].m_midi_channel, m_staff_contexts[i].voices[0].m_current_midi_program);
			NedResource::MidiCtrl(MIDI_CTL_MSB_PAN, m_staff_contexts[i].m_midi_channel, m_staff_contexts[i].voices[0].m_midi_pan);
			NedResource::MidiCtrl(MIDI_CTL_LSB_PAN, m_staff_contexts[i].m_midi_channel, 0);
			NedResource::MidiCtrl(MIDI_CTL_E1_REVERB_DEPTH, m_staff_contexts[i].m_midi_channel, m_staff_contexts[i].m_midi_reverb);
			NedResource::MidiCtrl(MIDI_CTL_E3_CHORUS_DEPTH, m_staff_contexts[i].m_midi_channel, m_staff_contexts[i].m_midi_chorus);
		}


		NedResource::prepareMidiEventList(m_midi_tempo_inverse);
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->prepareReplay();
		}
		NedResource::correctFermataTempo();
		for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
			spec_ptr = (SpecialMeasure *) lptr->data;
			switch (spec_ptr->type & REP_TYPE_MASK) {
				case REPEAT_OPEN: last_rep_open = spec_ptr->measure_number; break;
				case REPEAT_CLOSE: NedResource::copyAllBetweenMeasures(last_rep_open, last_alternative1, spec_ptr->measure_number);
							last_alternative1 = -1; break;
				case REPEAT_OPEN_CLOSE: NedResource::copyAllBetweenMeasures(last_rep_open, -1, spec_ptr->measure_number);
							last_rep_open = spec_ptr->measure_number; 
							last_alternative1 = -1;
							break;
			}
			switch (spec_ptr->type & START_TYPE_MASK) {
				case REP1START: last_alternative1 = spec_ptr->measure_number; break;
			}
		}
		NedResource::handleSegnos();
		NedResource::startReplay(m_selected_note, m_midi_tempo_inverse);
		/*
		m_selected_note = NULL;
		m_selected_spec_measure = NULL;
		m_selected_chord_or_rest = NULL;
		m_selected_tie_forward = NULL;
		*/
	}
	else {
		NedResource::stopReplay();
	}
}

void NedMainWindow::resetActiveFlags() {
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->resetActiveFlags();
	}
}

double NedMainWindow::determineTempoInverse(NedChordOrRest *element) {
	double tempoinverse = m_midi_tempo_inverse;
	bool found = false;

	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->determineTempoInverse(element, element->getSystem(), &tempoinverse, &found);
		if (found) break;
	}

	if (!found) {
		NedResource::Abort("NedMainWindow::determineTempo");
	}
	return tempoinverse;
}

void NedMainWindow::deleteStaff(int staff_number) {
	int i;
	GList *lptr;

	if (staff_number < 0 || staff_number >= m_staff_count || m_staff_count < 2) {
		NedResource::Abort("NedMainWindow::deleteStaff");
	}


	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->deleteStaff(staff_number);
	}

	for (i = staff_number; i < m_staff_count - 1; i++) {
		m_staff_contexts[i] = m_staff_contexts[i+1];
	}
	m_staff_count--;
	computeSystemIndent();
}

void NedMainWindow::shiftStaff(int staff_number, int position) {
	int i;
	GList *lptr;
	struct staff_context_str context;

	if (staff_number < 0 || staff_number >= m_staff_count || position < 0 || position >= m_staff_count) {
		NedResource::Abort("NedMainWindow::shiftStaff");
	}

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->shiftStaff(staff_number, position);
	}

	context = m_staff_contexts[staff_number];
	if (position > staff_number) {
		for (i = staff_number; i < position; i++) {
			m_staff_contexts[i] = m_staff_contexts[i+1];
		}
	}
	else {
		for (i = staff_number; i > position; i--) {
			 m_staff_contexts[i] = m_staff_contexts[i-1];
		}
	}
	m_staff_contexts[position] = context;
	reposit();
	repaint();
}

void NedMainWindow::restoreStaff(int staff_number, staff_context_str *staff_context) {
	int i;
	GList *lptr;

	if (staff_number < 0 || staff_number > m_staff_count) {
		NedResource::Abort("NedMainWindow::restoreStaff");
	}


	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->restoreStaff(staff_number);
	}

	for (i = m_staff_count; i > staff_number; i--) {
		m_staff_contexts[i] = m_staff_contexts[i-1];
	}
	m_staff_contexts[staff_number] = *staff_context;
	m_staff_count++;
	computeSystemIndent();
}

void NedMainWindow::do_staff_config(int staff_number, NedSystem *system) {
	bool state, delete_staff, delete_system, do_adjust;
	int clef_number, key_signature_number, numerator, denominator, tempo;
	int chorus, reverb;
	int pan[VOICE_COUNT], vol[VOICE_COUNT], midi_program[VOICE_COUNT];
	char *staff_name, *staff_short_name;
	char *group_name, *group_short_name;
	bool different_voices;
	unsigned int symbol = m_timesig_symbol;
	int channel, position, play_transposed;
	NedChangeTimeSigCommand *change_timesig_command;
	int octave_shift = 0;
	bool allow_delete_systems;
	NedCommandList *command_list = NULL;
	bool text_diff;
	bool newmuted[VOICE_COUNT];
	int i;
	/*
	allow_delete_systems = ((g_list_length(m_pages) > 1) || (((NedPage *) g_list_first(m_pages)->data)->getSystemCount() > 1));

	if (allow_delete_systems && system->getPage()->getPageNumber() == 0 && ((NedPage *) g_list_first(m_pages)->data)->getSystemCount() < 2) {
		allow_delete_systems = false;
	}
	*/

	allow_delete_systems = system->getPage()->getSystemCount() > 1;


	if (NedResource::isPlaying()) return;

	for (i = 0; i < VOICE_COUNT; i++) {
		pan[i] = m_staff_contexts[staff_number].voices[i].m_midi_pan;
		vol[i] = m_staff_contexts[staff_number].voices[i].m_midi_volume; 
		midi_program[i] = m_staff_contexts[staff_number].voices[i].m_midi_program;
	}


	NedStaffContextDialog *dialog = new NedStaffContextDialog (GTK_WINDOW(m_main_window), this, m_staff_contexts[staff_number].m_different_voices, allow_delete_systems, m_staff_count, staff_number,
		m_staff_contexts[staff_number].m_staff_name != NULL ? m_staff_contexts[staff_number].m_staff_name->getText() : NULL, 
		m_staff_contexts[staff_number].m_staff_short_name != NULL ? m_staff_contexts[staff_number].m_staff_short_name->getText() : NULL, 
		m_staff_contexts[staff_number].m_group_name != NULL ? m_staff_contexts[staff_number].m_group_name->getText() : NULL, 
		m_staff_contexts[staff_number].m_group_short_name != NULL ? m_staff_contexts[staff_number].m_group_short_name->getText() : NULL, 
		m_staff_contexts[staff_number].m_clef_number, m_staff_contexts[staff_number].m_clef_octave_shift, m_staff_contexts[staff_number].m_key_signature_number,
		m_numerator, m_denominator, symbol, vol, midi_program,
		m_staff_contexts[staff_number].m_midi_channel, (int) (60000.0 / m_midi_tempo_inverse),
		pan, m_staff_contexts[staff_number].m_midi_chorus, m_staff_contexts[staff_number].m_play_transposed,
		m_staff_contexts[staff_number].m_midi_reverb, m_staff_contexts[staff_number].m_muted);
	dialog->getValues(&state, &different_voices, &delete_staff, &delete_system, &position, &staff_name, &staff_short_name, &group_name, &group_short_name, &clef_number, &octave_shift, &key_signature_number, &do_adjust, &numerator, &denominator,
		 &symbol, vol, midi_program, &channel, &tempo, pan, &chorus, &play_transposed, &reverb, &m_config_changed, newmuted);
	delete dialog;


	if (delete_staff) {
		NedCommandList *command_list = new NedCommandList(this);
		NedResource::m_recorded_staff = NULL;
		NedDeleteStaffCommand *delete_staff_command = new NedDeleteStaffCommand(this, staff_number);
		command_list->addCommand(delete_staff_command);
		delete_staff_command->execute();
		setAllUnpositioned();
		command_list->setFullReposit();
		reposit(command_list);
		m_command_history->addCommandList(command_list);
		return;
	}
	if (delete_system) {
		NedCommandList *command_list = new NedCommandList(this);
		NedDeleteSystemCommand *delete_system_command = new NedDeleteSystemCommand(system);
		command_list->addCommand(delete_system_command);
		delete_system_command->execute();
		setAllUnpositioned();
		command_list->setFullReposit();
		reposit(command_list);
		m_command_history->addCommandList(command_list);
		return;
	}
	if (state) {
		if (staff_number != position) {
			NedShiftStaffCommand *shift_staff_command = new NedShiftStaffCommand(this, staff_number, position);
			command_list = new NedCommandList(this);
			command_list->addCommand(shift_staff_command);
			shift_staff_command->execute();
			command_list->setFullReposit();
			m_command_history->addCommandList(command_list);
			computeSystemIndent();
			setAndUpdateClefTypeAndKeySig();
			setAllUnpositioned();
			reposit(command_list);
			repaint();
			return; // this has priority; all other operations are ignored
		}
		if (m_staff_contexts[staff_number].m_staff_name != NULL) {
			text_diff = m_staff_contexts[staff_number].m_staff_name->textDiffers(staff_name);
		}
		else {
			text_diff = (staff_name != NULL);
		}
		if (text_diff) {
			if (m_staff_contexts[staff_number].m_staff_name != NULL) {
				delete m_staff_contexts[staff_number].m_staff_name;
				m_staff_contexts[staff_number].m_staff_name = NULL;
			}
			if (staff_name != NULL && strlen(staff_name) > 0) {
				m_staff_contexts[staff_number].m_staff_name = new NedPangoCairoText(m_drawing_area->window, staff_name, STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
					STAFF_NAME_FONT_WEIGHT, STAFF_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
			}
		}
		if (staff_name != NULL) {
			free(staff_name);
		}
		if (m_staff_contexts[staff_number].m_staff_short_name != NULL) {
			text_diff = m_staff_contexts[staff_number].m_staff_short_name->textDiffers(staff_short_name);
		}
		else {
			text_diff = (staff_short_name != NULL);
		}
		if (text_diff) {
			if (m_staff_contexts[staff_number].m_staff_short_name != NULL) {
				delete m_staff_contexts[staff_number].m_staff_short_name;
				m_staff_contexts[staff_number].m_staff_short_name = NULL;
			}
			if (staff_short_name != NULL && strlen(staff_short_name) > 0) {
				m_staff_contexts[staff_number].m_staff_short_name = new NedPangoCairoText(m_drawing_area->window, staff_short_name, STAFF_SHORT_NAME_FONT, STAFF_SHORT_NAME_FONT_SLANT,
					STAFF_SHORT_NAME_FONT_WEIGHT, STAFF_SHORT_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
			}
		}
		if (staff_short_name != NULL) {
			free(staff_short_name);
		}
		if (m_staff_contexts[staff_number].m_group_short_name != NULL) {
			text_diff = m_staff_contexts[staff_number].m_group_short_name->textDiffers(group_name);
		}
		else {
			text_diff = (group_name != NULL);
		}
		if (text_diff) {
			if (m_staff_contexts[staff_number].m_group_name != NULL) {
				delete m_staff_contexts[staff_number].m_group_name;
				m_staff_contexts[staff_number].m_group_name = NULL;
			}
			if (group_name != NULL && strlen(group_name) > 0) {
				m_staff_contexts[staff_number].m_group_name = new NedPangoCairoText(m_drawing_area->window, group_name, STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
					STAFF_NAME_FONT_WEIGHT, STAFF_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
			}
		}
		if (group_name != NULL) {
			free(group_name);
		}
		if (m_staff_contexts[staff_number].m_group_short_name != NULL) {
			text_diff = m_staff_contexts[staff_number].m_group_short_name->textDiffers(group_short_name);
		}
		else {
			text_diff = (group_short_name != NULL);
		}
		if (text_diff) {
			if (m_staff_contexts[staff_number].m_group_short_name != NULL) {
				delete m_staff_contexts[staff_number].m_group_short_name;
				m_staff_contexts[staff_number].m_group_short_name = NULL;
			}
			if (group_short_name != NULL && strlen(group_short_name) > 0) {
				m_staff_contexts[staff_number].m_group_short_name = new NedPangoCairoText(m_drawing_area->window, group_short_name, STAFF_SHORT_NAME_FONT, STAFF_SHORT_NAME_FONT_SLANT,
					STAFF_SHORT_NAME_FONT_WEIGHT, STAFF_SHORT_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
			}
		}
		if (group_short_name != NULL) {
			free(group_short_name);
		}
		if (m_staff_contexts[staff_number].m_different_voices != different_voices) {
			m_staff_contexts[staff_number].m_different_voices = different_voices;
		}
		for (i = 0; i < VOICE_COUNT; i++) {
			if (midi_program[i] >= 0) {
				m_staff_contexts[staff_number].voices[i].m_midi_program = midi_program[i];
			}
			m_staff_contexts[staff_number].voices[i].m_midi_volume = vol[i];
			m_staff_contexts[staff_number].voices[i].m_midi_pan = pan[i];
			m_staff_contexts[staff_number].m_muted[i] = newmuted[i];
		}
		m_staff_contexts[staff_number].m_midi_channel = channel;
		m_staff_contexts[staff_number].m_midi_chorus = chorus;
		m_staff_contexts[staff_number].m_play_transposed = play_transposed;
		m_staff_contexts[staff_number].m_midi_reverb = reverb;
		m_midi_tempo_inverse = 60000.0 / (double) tempo;
		if (m_numerator != numerator || m_denominator != denominator || m_timesig_symbol != symbol) {
			if (command_list == NULL) {
				command_list = new NedCommandList(this);
			}
			change_timesig_command = new NedChangeTimeSigCommand(this, numerator, denominator, symbol);
			change_timesig_command->execute();
			command_list->addCommand(change_timesig_command);
			command_list->setFullReposit();
			//setAllUnpositioned();
			//reposit(command_list);
			checkForElementsToSplit(command_list);
		}
		if (m_staff_contexts[staff_number].m_key_signature_number != key_signature_number) {
			if (command_list == NULL) {
				command_list = new NedCommandList(this);
			}
			NedChangeStaffKeysigCmd *change_staff_keysig_cmd = new NedChangeStaffKeysigCmd(this, staff_number, key_signature_number, do_adjust);
			change_staff_keysig_cmd->execute();
			command_list->addCommand(change_staff_keysig_cmd);
			command_list->setFullReposit();
			setAllUnpositioned();

		}
		if (m_staff_contexts[staff_number].m_clef_number != clef_number || m_staff_contexts[staff_number].m_clef_octave_shift != octave_shift) {
			if (command_list == NULL) {
				command_list = new NedCommandList(this);
			}
			NedChangeStaffClefCmd *change_staff_clef_cmd = new NedChangeStaffClefCmd(this, staff_number, clef_number, octave_shift, do_adjust);
			change_staff_clef_cmd->execute();
			command_list->addCommand(change_staff_clef_cmd);
			command_list->setFullReposit();
			setAllUnpositioned();
		}
		if (command_list != NULL) {
			command_list->setFullReposit();
			m_command_history->addCommandList(command_list);
		}

		computeSystemIndent();
		setAndUpdateClefTypeAndKeySig();
		setAllUnpositioned();
		reposit(command_list);
		repaint();
	}
}

void NedMainWindow::computeSystemIndent() {
	int i;
	double bracket_indent = 0.0, nested_braceindent = 0.0;
	double width, width2 = 0.0;
	bool inside_bracket = false;
	bool inside_brace = false;
	m_2ndnetto_indent = 
	m_netto_indent = 
	m_first_system_indent = 
	m_2nd_system_indent = 0.0;
	for (i = 0; i < m_staff_count; i++) {
		if (m_staff_contexts[i].m_flags & BRACKET_START) {
			bracket_indent = BRACKET_INDENT; 
			inside_bracket = true;
		}
		if (m_staff_contexts[i].m_flags & BRACKET_END) {
			if (inside_bracket && m_staff_contexts[i].m_group_name) {
				if (m_staff_contexts[i].m_group_name->getWidth() > width2) {
					width2 = m_staff_contexts[i].m_group_name->getWidth();
				}
			}
			inside_bracket = false;
		}
		if (m_staff_contexts[i].m_flags & BRACE_END) {
			if (inside_brace && m_staff_contexts[i].m_group_name) {
				if (m_staff_contexts[i].m_group_name->getWidth() > width2) {
					width2 = m_staff_contexts[i].m_group_name->getWidth() + BRACE_X_OFFSET;
				}
			}
			inside_brace = false;
		}
		if ((m_staff_contexts[i].m_flags & BRACE_START)) {
			inside_brace = true;
			if (inside_bracket) {
				nested_braceindent = NESTED_BRACE_INDENT;
			}
			bracket_indent = BRACKET_INDENT; 

		}
		if (width2 > m_netto_indent) {
			m_netto_indent = width2;
		}
		if (m_staff_contexts[i].m_staff_name == NULL) continue;
		width = m_staff_contexts[i].m_staff_name->getWidth();
		if (width > m_netto_indent) {
			m_netto_indent = width;
		}
	}
	if (m_netto_indent > 0.0) {
		m_first_system_indent = m_netto_indent / m_current_zoom / getCurrentScale() + SYSTEM_INTENT_SEC_SPACE  + bracket_indent + 2 * nested_braceindent;
	}
	inside_brace = inside_bracket = 0;
	width2 = 0.0;
	for (i = 0; i < m_staff_count; i++) {
		if (m_staff_contexts[i].m_flags & BRACKET_START) {
			inside_bracket = true;
		}
		if (m_staff_contexts[i].m_flags & BRACKET_END) {
			if (inside_bracket && m_staff_contexts[i].m_group_short_name) {
				if (m_staff_contexts[i].m_group_short_name->getWidth() > width2) {
					width2 = m_staff_contexts[i].m_group_short_name->getWidth();
				}
			}
			inside_bracket = false;
		}
		if (m_staff_contexts[i].m_flags & BRACE_END) {
			if (inside_brace && m_staff_contexts[i].m_group_short_name) {
				if (m_staff_contexts[i].m_group_short_name->getWidth() > width2) {
					width2 = m_staff_contexts[i].m_group_short_name->getWidth();
				}
			}
			inside_brace = false;
		}
		if ((m_staff_contexts[i].m_flags & BRACE_START)) {
			inside_brace = true;
		}
		if (width2 > m_2ndnetto_indent) {
			m_2ndnetto_indent = width2;
		}
		if (m_staff_contexts[i].m_staff_short_name == NULL) continue;
		width = m_staff_contexts[i].m_staff_short_name->getWidth();
		if (width > m_2ndnetto_indent) {
			m_2ndnetto_indent = width;
		}
	}
	if (m_2ndnetto_indent > 0.0) {
		m_2nd_system_indent = m_2ndnetto_indent / m_current_zoom / getCurrentScale() + 2 * SYSTEM_INTENT_SEC_SPACE  + bracket_indent + 2 * nested_braceindent;
	}
	
}

void NedMainWindow::setSelected(NedChordOrRest *chord_or_rest, NedNote *note) {
	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(m_keyboard_insert_action))) {
		m_selected_chord_or_rest = NULL;
		m_selected_tie_forward = NULL;
		m_selected_note = NULL;
		return;
	}
	m_selected_chord_or_rest = chord_or_rest;
	m_selected_note = note;
}

void NedMainWindow::setVisibleSystem(NedSystem *system) {
#define OFFS 1.0
	NedPage *page;
	page = system->getPage();
	double ytop = system->getYPos() * m_current_zoom * getCurrentScale() - m_topy;
	double height = system->getHeight() * m_current_zoom * getCurrentScale();
	double page_height = page->getHeight() * m_current_zoom * getCurrentScale();
	double ybot = ytop + height;
	bool repaint_needed = FALSE;
	if (ybot > m_drawing_area->allocation.height) {
		m_topy = (system->getYPos() - OFFS) * m_current_zoom;
		if (m_topy + m_drawing_area->allocation.height > page_height) m_topy = page_height - m_drawing_area->allocation.height;
		if (m_topy < 0.0) m_topy = 0.0;
		repaint_needed = TRUE;
	}
	if (ytop < 0) {
		m_topy = 0.0;
		if (m_topy + m_drawing_area->allocation.height > page_height) m_topy = page_height - m_drawing_area->allocation.height;
		repaint_needed = TRUE;
	}
	else if (ytop < 0) {
		m_topy = ytop + OFFS;
		if (m_topy < 0) {
			m_topy = 0;
		}
		repaint_needed = TRUE;
	}
	if (m_selected_group != NULL) {
		g_list_free(m_selected_group);
		m_selected_group = NULL;
	}
	m_selection_rect.width = m_selection_rect.height = 0;
	if (repaint_needed) {
		repaint();
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), page->getPageNumber() + 1);
	}

}

/*
void NedMainWindow::setVisiblePage(NedPage *page) {
	int page_nr;
	m_leftx = page->getXPos() * m_current_zoom;
	if (m_leftx < 0.0) m_leftx = 0.0;
	if ((page_nr = g_list_index(m_pages, page)) >= 0) {
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), page_nr + 1);
	}
	repaint();
}
*/

void NedMainWindow::drawVisibleRectangle(cairo_t *cr, NedNote *note) {
	double pointx = (note->getPage()->getContentXpos() + note->getChord()->getXPos()) * m_current_zoom - m_leftx;
	double pointy = (note->getSystem()->getYPos() + note->getStaff()->getTopPos()) * m_current_zoom - m_topy;
	double width = (double) m_drawing_area->allocation.width / getCurrentScale();
	double height = (double)  m_drawing_area->allocation.height / getCurrentScale();
	printf("left: %f, top: %f, width: %f, height: %f\n", pointx, pointy, width, height);
}




void NedMainWindow::setVisible(NedNote *note) {
	double pointx = (note->getPage()->getContentXpos() + note->getChord()->getXPos()) * m_current_zoom - m_leftx;
	double pointy = (note->getSystem()->getYPos() + note->getStaff()->getBottomPos()) * m_current_zoom - m_topy;
	//double pointy = note->getSystem()->getYPos() * m_current_zoom - m_topy;
	double width = (double) m_drawing_area->allocation.width / getCurrentScale();
	double height = (double)  m_drawing_area->allocation.height / getCurrentScale();

	bool repaint_needed = false;

	if (pointx > width) {
		if ((note->getPage()->getContentXpos() + note->getChord()->getXPos() - 
		          note->getPage()->getXPos()) * m_current_zoom < width) {
			m_leftx = note->getPage()->getXPos() * m_current_zoom;
		}
		else {
			m_leftx = (note->getPage()->getContentXpos() + note->getChord()->getXPos()) * m_current_zoom;
			if (m_leftx < 0.0) m_leftx = 0.0;
		}
		repaint_needed = true;
	}
	else if (pointx < 0.0) {
		if ((note->getPage()->getContentXpos() + note->getChord()->getXPos() - 
		          note->getPage()->getXPos()) * m_current_zoom < width) {
			m_leftx = note->getPage()->getXPos() * m_current_zoom;
		}
		else {
			m_leftx = (note->getPage()->getContentXpos() + note->getChord()->getXPos()) * m_current_zoom - m_drawing_area->allocation.width;
		}
		repaint_needed = true;
	}

	if (m_last_y_adjustment != note->getSystem()) {
		if (pointy > height) {
			m_topy = (note->getSystem()->getYPos() - OFFS) * m_current_zoom;
			if (m_topy < 0.0) m_topy = 0.0;
			m_last_y_adjustment = note->getSystem();
			repaint_needed = true;
		}
		if (pointy < 0) {
			if (note->getSystem()->getBottomPos() * m_current_zoom < height) {
				m_topy = 0.0;
			}
			else {
				m_topy = (note->getSystem()->getYPos() - OFFS) * m_current_zoom;
			}
			m_last_y_adjustment = note->getSystem();
			repaint_needed = true;
		}
	}
	if (repaint_needed) {
		repaint();
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), note->getPage()->getPageNumber() + 1);
	}
}

void NedMainWindow::setVisible(NedChordOrRest *chord_or_rest) {
	double pointx = (chord_or_rest->getPage()->getContentXpos() + chord_or_rest->getXPos()) * m_current_zoom - m_leftx;
	double pointy = (chord_or_rest->getSystem()->getYPos() + chord_or_rest->getStaff()->getTopPos()) * m_current_zoom - m_topy;
	double width = (double) m_drawing_area->allocation.width / getCurrentScale();
	double height = (double)  m_drawing_area->allocation.height / getCurrentScale();

	bool repaint_needed = false;

	if (pointx > width) {
		if ((chord_or_rest->getPage()->getContentXpos() + chord_or_rest->getXPos() - 
		          chord_or_rest->getPage()->getXPos()) * m_current_zoom < width) {
			m_leftx = chord_or_rest->getPage()->getXPos() * m_current_zoom;
		}
		else {
			m_leftx = (chord_or_rest->getPage()->getContentXpos() + chord_or_rest->getXPos()) * m_current_zoom;
			if (m_leftx < 0.0) m_leftx = 0.0;
		}
		repaint_needed = true;
	}
	else if (pointx < 0.0) {
		if ((chord_or_rest->getPage()->getContentXpos() + chord_or_rest->getXPos() - 
		          chord_or_rest->getPage()->getXPos()) * m_current_zoom < width) {
			m_leftx = chord_or_rest->getPage()->getXPos() * m_current_zoom;
		}
		else {
			m_leftx = (chord_or_rest->getPage()->getContentXpos() + chord_or_rest->getXPos()) * m_current_zoom - m_drawing_area->allocation.width;
		}
		repaint_needed = true;
	}

	if (pointy > height) {
		m_topy = (chord_or_rest->getSystem()->getYPos() - OFFS) * m_current_zoom;
		if (m_topy < 0.0) m_topy = 0.0;
		repaint_needed = true;
	}
	else if (pointy < 0) {
		if (chord_or_rest->getSystem()->getBottomPos() * m_current_zoom < height) {
			m_topy = 0.0;
		}
		else {
			m_topy = (chord_or_rest->getSystem()->getYPos() - OFFS) * m_current_zoom;
		}
		repaint_needed = true;
	}
	if (repaint_needed) {
		repaint();
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), chord_or_rest->getPage()->getPageNumber() + 1);
	}
}

void NedMainWindow::toggle_shift_mode(GtkAction *action, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_avoid_feedback) return;
	if (gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action))) {
		main_window->m_avoid_feedback = TRUE;
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(main_window->m_insert_erease_mode_action), FALSE);
		main_window->m_avoid_feedback = FALSE;
	}
	main_window->setCursor();
}


void NedMainWindow::toggle_insert_erease_mode(GtkAction *action, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_avoid_feedback) return;
	if (gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action))) {
		main_window->m_avoid_feedback = TRUE;
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(main_window->m_shift_action), FALSE);
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(main_window->m_midi_record_action), FALSE);
		NedResource::resetSomeButtons();
		NedResource::resetTieButton();
		main_window->m_avoid_feedback = FALSE;
		main_window->m_selected_note = NULL;
		main_window->m_selected_chord_or_rest = NULL;
		main_window->m_selected_spec_measure = NULL;
		main_window->m_selected_tie_forward = NULL;
	}
	main_window->setCursor();
	main_window->repaint();
}

void NedMainWindow::resetMidiInput() {
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(m_midi_input_action), FALSE);
}



void NedMainWindow::prepare_keyboard_insertion(GtkAction *action, gpointer data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION (action))) {
		if (NedChordNameDialog::isAlreadyActive()) return;
		gtk_toggle_action_set_active(main_window->m_midi_record_action, FALSE);
		main_window->m_last_cursor_x = main_window->m_last_cursor_y = -1;
		main_window->m_last_obj = NULL;
	}
}

void NedMainWindow::setCursor() {
	if (getMainWindowStatus() & SHIFT_MODE) {
		gdk_window_set_cursor (GDK_WINDOW(m_drawing_area->window), m_hand);
		return;
	}
	if (m_lyrics_mode != NO_LYRICS) {
		gdk_window_set_cursor (GDK_WINDOW(m_drawing_area->window), m_pencil);
	}
	else {
		gdk_window_set_cursor (GDK_WINDOW(m_drawing_area->window), m_pointer);
	}
		
}

void NedMainWindow::changeLength() {
	if (NedResource::isPlaying()) return;
	if (m_selected_chord_or_rest != NULL && m_selected_chord_or_rest->getType() == TYPE_CHORD && 
	     NedResource::getCurrentLength() > STROKEN_GRACE && (((getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (m_keyboard_ctrl_mode))) {

		GList *lptr;
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->tryChangeLength(m_selected_chord_or_rest)) break;
		}
	}
}

void NedMainWindow::changeState() {
	unsigned int status;
	NedCommandList *command_list;
	if (m_selected_chord_or_rest != NULL && ((m_selected_chord_or_rest->getType() & (TYPE_CHORD | TYPE_REST)) != 0)  && 
	     NedResource::getCurrentLength() > STROKEN_GRACE && (((getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (m_keyboard_ctrl_mode))) {
	     status = m_selected_chord_or_rest->getStatus();
	     status &= (~(CHORD_STAT_MASK));
	     status |= (NedResource::getStatus() & CHORD_STAT_MASK);
	     command_list = new NedCommandList(this, m_selected_chord_or_rest->getSystem());
	     command_list->addCommand(new NedChangeChordOrRestStatusCommand(m_selected_chord_or_rest, status));
	     command_list->execute();
	     m_command_history->addCommandList(command_list);
	     repaint();
	}
}

void NedMainWindow::changeDots() {
	if (NedResource::isPlaying()) return;
	if (m_selected_chord_or_rest != NULL && m_selected_chord_or_rest->getType() == TYPE_CHORD && 
	     NedResource::getCurrentLength() > STROKEN_GRACE && (((getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (m_keyboard_ctrl_mode))) {

		GList *lptr;
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->tryChangeLength(m_selected_chord_or_rest)) break;
		}
	}
}

void NedMainWindow::changeNoteHead() {
	if (NedResource::isPlaying()) return;
	if (m_selected_chord_or_rest != NULL && m_selected_chord_or_rest->getType() == TYPE_CHORD && 
		NedResource::getCurrentLength() > STROKEN_GRACE && (((getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (m_keyboard_ctrl_mode))) {

		if (m_selected_note == NULL) return;
		if (m_selected_note->getNoteHead() == NedResource::getCurrentNoteHead()) return;
		NedCommandList *command_list = new NedCommandList(this, m_selected_chord_or_rest->getSystem());
		command_list->addCommand(new NedChangeNoteHeadCommand(m_selected_note, NedResource::getCurrentNoteHead()));
		command_list->execute();
		m_command_history->addCommandList(command_list);
		repaint();
	}
}

void NedMainWindow::changeTieState() {
	if (!NedResource::getTiedMode() && m_selected_note != NULL && m_selected_note->getTieForward() != NULL) {
		NedCommandList *command_list = new NedCommandList(this, m_selected_chord_or_rest->getSystem());
		command_list->addCommand(new NedUnTieForwardCommand(m_selected_note, m_selected_note->getTieForward()));
		m_command_history->addCommandList(command_list);
		command_list->execute();
	}
}

void NedMainWindow::changeNoteState() {
	if (m_selected_note != NULL && m_selected_chord_or_rest != NULL && (((getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (m_keyboard_ctrl_mode))) {
		unsigned int status = m_selected_note->getStatus();
		status &= (~(ACCIDENTAL_MASK));
		status |= ACCIDENTAL_MASK & NedResource::getStatus();
		NedCommandList *command_list = new NedCommandList(this, m_selected_note->getSystem());
		command_list->addCommand(new NedChangeNoteStatusCommand(m_selected_note, status));
		m_command_history->addCommandList(command_list);
		command_list->execute();
	}
}

void NedMainWindow::set_paint_colored(GtkAction *action, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_paint_colored = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));
	main_window->repaint();
}

void NedMainWindow::initiate_repaint(GtkAction *action, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->repaint();
}


void NedMainWindow::customize_beam(GtkAction *action, gpointer data) {
	if (NedResource::isPlaying()) return;
	NedChordOrRest *first_element = NULL;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM) {
		if (g_list_length(main_window->m_selected_group) > 0) {
			first_element = main_window->findAndBeamElementsOfCurrentVoice();
			if (first_element != NULL) {
				main_window->reposit(NULL, first_element->getPage(), first_element->getSystem());
				main_window->repaint();
			}
		}
		return;
	}
	if (main_window->m_selected_chord_or_rest != NULL && main_window->m_selected_chord_or_rest->getType() == TYPE_CHORD) {
		NedCommandList *command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
		main_window->m_selected_chord_or_rest->getVoice()->isolate(command_list, main_window->m_selected_chord_or_rest);
		if (command_list->getNumberOfCommands() > 0) {
			main_window->m_command_history->addCommandList(command_list);
		}
		else {
			delete command_list;
		}
		main_window->reposit(NULL, main_window->m_selected_chord_or_rest->getPage(), main_window->m_selected_chord_or_rest->getSystem());
		main_window->repaint();
	}
}
void NedMainWindow::flip_stem(GtkAction *action, gpointer data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (main_window->m_selected_chord_or_rest == NULL) return;
	if (main_window->m_selected_chord_or_rest->getType() != TYPE_CHORD) return;
	if (main_window->m_selected_chord_or_rest->getFirstNote()->getNoteHead() == GUITAR_NOTE_STEM ||
		main_window->m_selected_chord_or_rest->getFirstNote()->getNoteHead() == GUITAR_NOTE_NO_STEM) return;
	NedCommandList *command_list;
	command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());

	main_window->m_selected_chord_or_rest->flipStem(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->reposit(NULL, main_window->m_selected_chord_or_rest->getPage(), main_window->m_selected_chord_or_rest->getSystem());
	main_window->repaint();
}


void NedMainWindow::resetSomeButtons() {
	NedResource::resetSomeButtons();
}

void NedMainWindow::disconnectKeys() {
	if (m_lyrics_mode != NO_LYRICS) return;
	gtk_action_disconnect_accelerator(GTK_ACTION(m_shift_action));
 	gtk_action_disconnect_accelerator(GTK_ACTION(m_keyboard_insert_action));
 	gtk_action_disconnect_accelerator(GTK_ACTION(m_insert_erease_mode_action));
        gtk_action_disconnect_accelerator(GTK_ACTION(m_shift_action));
        gtk_action_disconnect_accelerator(GTK_ACTION(m_replay_action));
        gtk_action_disconnect_accelerator(GTK_ACTION(m_midi_record_action));
}


void NedMainWindow::insert_ppp(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_PPP;
	main_window->prepareInsertion();
}

void NedMainWindow::insert_pp(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_PP;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_p(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_P;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_mp(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_MP;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_sp(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_SP;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_mf(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_MF;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_sf(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_SF;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_f(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_F;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_ff(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_FF;
	main_window->prepareInsertion();
}
void NedMainWindow::insert_fff(GtkAction *widget, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_DYNAMIC;
	main_window->m_special_sub_type.m_special_sub_type_int = VOL_FFF;
	main_window->prepareInsertion();
}

void NedMainWindow::insert_tempo(GtkAction *widget, gpointer data) {
	bool state;
	unsigned int kind, tempo;
	bool dot;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedTempoDialog *dialog = new NedTempoDialog((GTK_WINDOW(main_window->m_main_window)), false);
	dialog->getValues(&state, &kind, &dot, &tempo);
	delete dialog;
	main_window->m_special_type = TYPE_TEMPO;
	main_window->m_special_sub_type.m_special_sub_type_int = dot ? (3 * kind / 2 / FACTOR) : (kind / FACTOR);
	main_window->m_special_sub_type.m_special_sub_type_int |= (tempo << 16);
	main_window->prepareInsertion();

}

void NedMainWindow::insert_text(GtkAction *widget, gpointer data) {
	bool state;
	char *textptr = NULL;
	const char *font_family;
	PangoStyle font_slant;
	PangoWeight font_weight;
	bool enable_volume = false;
	bool enable_tempo = false;
	unsigned short segno_sign = 0;
	int anchor = ANCHOR_MID;
	unsigned int volume = 64;
	unsigned int tempo = 120;
	unsigned int channel;
	bool enable_channel;
	int midi_pgm;
	double font_size;

	NedMainWindow *main_window = (NedMainWindow *) data;
	font_slant = TEXT_FONT_SLANT_DEFAULT;
	font_size = TEXT_FONT_SIZE_DEFAULT;
	font_weight = TEXT_FONT_WEIGHT_DEFAULT;

	NedTextDialog *dialog = new NedTextDialog (GTK_WINDOW(main_window->m_main_window), textptr, anchor, tempo, enable_tempo, volume, enable_volume, segno_sign, -1, 1, false, TEXT_FONT_NAME_DEFAULT, TEXT_FONT_SIZE_DEFAULT, TEXT_FONT_SLANT_DEFAULT, TEXT_FONT_WEIGHT_DEFAULT);
	dialog->getValues(&state, &textptr, &anchor, &tempo, &enable_tempo, &volume, &enable_volume, &segno_sign, &midi_pgm, &channel, &enable_channel, &font_family, &font_size, &font_slant, &font_weight);
	if (state) {
		/*
		NedResource::DbgMsg(DBG_TESTING, "family = %s, size = %f, slant = ", font_family, font_size);
		switch (font_slant) {
			case CAIRO_FONT_SLANT_NORMAL: NedResource::DbgMsg(DBG_TESTING, "CAIRO_FONT_SLANT_NORMAL"); break;
			case CAIRO_FONT_SLANT_ITALIC: NedResource::DbgMsg(DBG_TESTING, "CAIRO_FONT_SLANT_ITALIC"); break;
			case CAIRO_FONT_SLANT_OBLIQUE: NedResource::DbgMsg(DBG_TESTING, "CAIRO_FONT_SLANT_OBLIQUE"); break;
			default: NedResource::DbgMsg(DBG_TESTING, "unknown"); break;
		}

		NedResource::DbgMsg(DBG_TESTING, "; weight = ");
	
		switch (font_weight) {
			case CAIRO_FONT_WEIGHT_NORMAL: NedResource::DbgMsg(DBG_TESTING, "CAIRO_FONT_WEIGHT_NORMAL"); break;
			case CAIRO_FONT_WEIGHT_BOLD:  NedResource::DbgMsg(DBG_TESTING, "CAIRO_FONT_WEIGHT_BOLD"); break;
			default: NedResource::DbgMsg(DBG_TESTING, "unknown"); break;
		}
		NedResource::DbgMsg(DBG_TESTING, "\n"); 
		*/
		if (main_window->m_freetext != NULL) {
			delete main_window->m_freetext;
			main_window->m_freetext = NULL;
		}
		if (textptr != NULL) {
			if (strlen(textptr) > 0) {
				main_window->m_freetext = new NedFreeText(textptr, main_window->m_drawing_area, anchor, tempo, enable_tempo,
								 volume, enable_volume, segno_sign, midi_pgm, 
								channel, enable_channel,
								font_family, font_size, font_slant, font_weight);
				free(textptr);
				main_window->m_special_type = TYPE_FREE_TEXT;
				main_window->prepareInsertion();
			}
		}
	}
	delete dialog;
}

void NedMainWindow::set_lyrics_mode1(GtkAction *widget, gpointer data) {
	if (NedChordNameDialog::isAlreadyActive()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->disconnectKeys();
	main_window->m_lyrics_mode = LYRICS1;
	main_window->setCursor();
	main_window->repaint();
}

void NedMainWindow::set_lyrics_mode2(GtkAction *widget, gpointer data) {
	if (NedChordNameDialog::isAlreadyActive()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->disconnectKeys();
	main_window->m_lyrics_mode = LYRICS2;
	main_window->setCursor();
	main_window->repaint();
}

void NedMainWindow::set_lyrics_mode3(GtkAction *widget, gpointer data) {
	if (NedChordNameDialog::isAlreadyActive()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->disconnectKeys();
	main_window->m_lyrics_mode = LYRICS3;
	main_window->setCursor();
	main_window->repaint();
}

void NedMainWindow::set_lyrics_mode4(GtkAction *widget, gpointer data) {
	if (NedChordNameDialog::isAlreadyActive()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->disconnectKeys();
	main_window->m_lyrics_mode = LYRICS4;
	main_window->setCursor();
	main_window->repaint();
}

void NedMainWindow::set_lyrics_mode5(GtkAction *widget, gpointer data) {
	if (NedChordNameDialog::isAlreadyActive()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->disconnectKeys();
	main_window->m_lyrics_mode = LYRICS5;
	main_window->setCursor();
	main_window->repaint();
}

void NedMainWindow::resetLyricsMode() {
	if (m_lyrics_mode == NO_LYRICS) return;
	gtk_action_connect_accelerator(GTK_ACTION(m_shift_action));
 	gtk_action_connect_accelerator(GTK_ACTION(m_keyboard_insert_action));
 	gtk_action_connect_accelerator(GTK_ACTION(m_insert_erease_mode_action));
        gtk_action_connect_accelerator(GTK_ACTION(m_shift_action));
        gtk_action_connect_accelerator(GTK_ACTION(m_replay_action));
        gtk_action_connect_accelerator(GTK_ACTION(m_midi_record_action));
	m_lyrics_mode = NO_LYRICS;
	setCursor();
}

void NedMainWindow::lyrics_editor(GtkWidget *widget, void *data) {
	int staff_nr;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_lyrics_mode != NO_LYRICS) return;
	if (main_window->m_selected_chord_or_rest != NULL) {
		staff_nr = main_window->m_selected_chord_or_rest->getStaff()->getStaffNumber();
	}
	else if (main_window->m_staff_count < 2) {
		staff_nr = 0;
	}
	else {
		NedResource::Info("Please select a chord or rest");
		return;
	}
		
	new NedLyricsEditor(GTK_WINDOW(main_window->m_main_window), main_window, staff_nr);
}

void NedMainWindow::updateLyrics(NedLyricsEditor *leditor, int staff_nr) {
	GList *lptr;
	NedCommandList *command_list = new NedCommandList(this);
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->setLyrics(command_list, leditor, staff_nr);
	}
	setAllUnpositioned();
	command_list->setFullReposit();
	reposit(command_list);
	m_command_history->addCommandList(command_list);
	repaint();
}

void NedMainWindow::collectLyrics(NedLyricsEditor *leditor, int staff_nr) {
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->collectLyrics(leditor, staff_nr);
	}
}

void NedMainWindow::stopReplay() {
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(m_replay_action), FALSE);
	repaint();
}

void NedMainWindow::repaint() {
	GdkRectangle rect;
	int dummy;
	gdk_window_get_geometry(m_drawing_area->window, &dummy, &dummy, &rect.width, &rect.height, &dummy);
	rect.x = rect.y = 0;
	gdk_window_invalidate_rect (m_drawing_area->window, &rect, FALSE);
}

void NedMainWindow::new_file(GtkWidget *widget, void *data) {
	int i;
	GtkWidget *unsaved_dialog;
	bool stop_new = FALSE, do_save = FALSE;
	GList *lptr;

	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (main_window->m_config_changed || main_window->m_command_history->unsavedOperations()) {
		unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				_("Save Changes ?"));
		gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
			case GTK_RESPONSE_YES: do_save = TRUE; break;
			case GTK_RESPONSE_REJECT: stop_new = TRUE; break;
		}
		gtk_widget_destroy (unsaved_dialog);	
		if (stop_new) {
			return;
		}
		if (do_save) {
			main_window->save_score(widget, data);
		}
	}
	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		delete ((NedPage *) lptr->data);
	}
	g_list_free(main_window->m_pages);
	main_window->m_pages = NULL;
	for (lptr = g_list_first(main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) {
		free(lptr->data);
	}
	g_list_free(main_window->m_special_measures);
	main_window->m_special_measures = NULL;
	main_window->m_current_paper = NedResource::getPaperInfo("A4");
	main_window->m_portrait = TRUE;
	double w = main_window->m_portrait ? main_window->m_current_paper->width : main_window->m_current_paper->height;
	double h = main_window->m_portrait ? main_window->m_current_paper->height : main_window->m_current_paper->width;
	w /= PS_ZOOM * (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
	h /= PS_ZOOM * (double) main_window->m_current_scale / (double) SCALE_GRANULARITY;
	main_window->m_global_spacement_fac = 1.0;
	main_window->m_numerator = 4;
	main_window->m_denominator = 4;
	main_window->m_timesig_symbol = TIME_SYMBOL_NONE;
	main_window->m_upbeat_inverse = 0;
	main_window->m_staff_count = 1;
	main_window->m_current_scale = SCALE_GRANULARITY;
	main_window->m_selected_note = NULL;
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_tie_forward = NULL;
	main_window->m_selected_spec_measure = NULL;
	main_window->m_config_changed = FALSE;
	main_window->resetLyricsMode();
	main_window->m_midi_tempo_inverse = START_TEMPO_INVERSE;
	main_window->m_staff_contexts[0].m_key_signature_number = 0;
	main_window->m_staff_contexts[0].m_clef_number = 0;
	main_window->m_staff_contexts[0].m_clef_octave_shift = 0;
	main_window->m_staff_contexts[0].m_different_voices = false;
	for (i = 0; i < VOICE_COUNT; i++) {
		main_window->m_staff_contexts[0].voices[i].m_midi_volume = 64;
		main_window->m_staff_contexts[0].voices[i].m_midi_program = 0;
		main_window->m_staff_contexts[0].voices[i].m_midi_pan = 64;
		main_window->m_staff_contexts[0].m_muted[i] = false;
	}
	main_window->m_staff_contexts[0].m_midi_channel = 0;
	main_window->m_staff_contexts[0].m_flags = 0;
	for (i = 0; i < MAX_STAFFS; i++) {
		if (main_window->m_staff_contexts[i].m_staff_name != NULL) {
			delete main_window->m_staff_contexts[i].m_staff_name;
			main_window->m_staff_contexts[i].m_staff_name = NULL;
		}
		if (main_window->m_staff_contexts[i].m_staff_short_name != NULL) {
			delete main_window->m_staff_contexts[i].m_staff_short_name;
			main_window->m_staff_contexts[i].m_staff_short_name = NULL;
		}
		if (main_window->m_staff_contexts[i].m_group_name != NULL) {
			delete main_window->m_staff_contexts[i].m_group_name;
			main_window->m_staff_contexts[i].m_group_name = NULL;
		}
		if (main_window->m_staff_contexts[i].m_group_short_name != NULL) {
			delete main_window->m_staff_contexts[i].m_group_short_name;
			main_window->m_staff_contexts[i].m_group_short_name = NULL;
		}
	}
	main_window->m_selection_rect.width = 0;
	main_window->m_first_page_yoffs = 0.0;
	if (main_window->m_selected_group != NULL) {
		g_list_free(main_window->m_selected_group);
		main_window->m_selected_group = NULL;
	}
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(main_window->m_v1bu), TRUE);
	gtk_toggle_action_set_active(main_window->m_show_hidden_action, FALSE);
	delete main_window->m_score_info;
	main_window->m_score_info = new ScoreInfo();
	main_window->computeSystemIndent();
	main_window->computeScoreTextExtends();
	main_window->m_command_history->reset();
	main_window->m_current_filename[0] = '\0';
	main_window->updatePageCounter();
	main_window->m_pages = g_list_append(main_window->m_pages, new NedPage(main_window, w, h, 0, 1, TRUE));
	main_window->resetButtons();
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window->m_page_selector), 1);
	main_window->m_topy = main_window->m_leftx = 0;
	gtk_window_set_title (GTK_WINDOW (main_window->m_main_window), "Nted");
	main_window->renumberMeasures();
	main_window->reposit();
	main_window->repaint();

}

void NedMainWindow::save_score(GtkWidget  *widget, void *data) {
	FILE *fp;
	GtkWidget *error_dialog;
	char fnamenew[1024];

	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_current_filename[0] == '\0') {
		save_score_as(widget, data);
		return;
	}
	sprintf(fnamenew, "%s.new", main_window->m_current_filename);
	if ((fp = fopen(fnamenew, "w")) == NULL) {
		error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			_("Cannot open %s for writing"), fnamenew);
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (error_dialog);	
		return;
	}
	main_window->storeScore(fp);
	if (rename(fnamenew, main_window->m_current_filename) < 0) {
		error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			_("Cannot rename %s to %s"), fnamenew, main_window->m_current_filename);
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (error_dialog);	
	}

}
	

void NedMainWindow::save_score_as(GtkWidget  *widget, void *data) {
	FILE *fp;
	char fnamenew[1024];
	char the_filename[4096], show_file_name[4096];
	NedMainWindow *main_window = (NedMainWindow *) data;
	char pathfoldername[4096];
	DIR *the_directory;

	GtkWidget *save_dialog;
	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	char *filename;
	bool ok = FALSE;
	bool extend = FALSE;
	char *cptr;
	char *folder = NULL;

	if (NedResource::isPlaying()) return;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.ntd (Nted-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.ntd");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	save_dialog = gtk_file_chooser_dialog_new (_("Save score"),
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(save_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(save_dialog), file_filter2);
	if (NedResource::m_last_folder != NULL) {
		if ((the_directory = opendir(NedResource::m_last_folder)) != NULL) { // avoid gtk error message
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(save_dialog), NedResource::m_last_folder);
			closedir(the_directory);
		}
		else {
			NedResource::m_last_folder = NULL;
		}
	}

	if (gtk_dialog_run (GTK_DIALOG (save_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (save_dialog));
		strcpy(the_filename, filename);
		folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (save_dialog));
		g_free (filename);
		ok = TRUE;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (save_dialog))) == file_filter1) {
		extend = TRUE;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				extend = FALSE;
			}
		}
		if (extend) {
			strcat(the_filename, ".ntd");
		}
	}
	gtk_widget_destroy (save_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = FALSE;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (ok) {
		sprintf(fnamenew, "%s.new", the_filename);
		if ((fp = fopen(fnamenew, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), fnamenew);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
		}
	}

	if (ok) {
		main_window->storeScore(fp);
		if (rename(fnamenew, the_filename) < 0) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot rename %s to %s"), fnamenew, the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
		}
		if ((cptr = strrchr(the_filename, '/')) != NULL) {
			strncpy(pathfoldername, the_filename, cptr - the_filename);
			pathfoldername[cptr - the_filename] = '\0';
			NedResource::m_last_folder = strdup(pathfoldername);
		}
		else {
			NedResource::m_last_folder = strdup(folder);
		}
		strcpy(main_window->m_current_filename, the_filename);
		if ((cptr = strrchr(the_filename, '/')) != NULL) {
			cptr++;
		}
		else {
			cptr = the_filename;
		}
		sprintf(show_file_name, "Nted - %s", cptr);
		gtk_window_set_title (GTK_WINDOW (main_window->m_main_window), show_file_name);
		NedResource::addToRecentFiles(main_window->m_current_filename);
		main_window->updateRecentFiles();
	}
	if (folder != NULL) {
		g_free(folder);
	}
}

void NedMainWindow::storeScore(FILE *fp) {
	int i;
	GList *lptr;

	NedResource::prepareAddrStruct();
	fprintf(fp, "NTED version 24\n");
	fprintf(fp, "HEAD\n");
	if (m_score_info->title != NULL && strlen(m_score_info->title->getText()) > 0) {
		fprintf(fp, "TITLE : %s\n", m_score_info->title->getText());
	}
	if (m_score_info->subject != NULL && strlen(m_score_info->subject->getText()) > 0) {
		fprintf(fp, "SUBJECT : %s\n", m_score_info->subject->getText());
	}
	if (m_score_info->composer != NULL && strlen(m_score_info->composer->getText()) > 0) {
		fprintf(fp, "COMPOSER : %s\n", m_score_info->composer->getText());
	}
	if (m_score_info->arranger != NULL && strlen(m_score_info->arranger->getText()) > 0) {
		fprintf(fp, "ARRANGER : %s\n", m_score_info->arranger->getText());
	}
	if (m_score_info->copyright != NULL && strlen(m_score_info->copyright->getText()) > 0) {
		fprintf(fp, "COPYRIGHT : %s\n", m_score_info->copyright->getText());
	}
	fprintf(fp, "MEDIA %s\n", m_current_paper->name);
	fprintf(fp, "ORIENTATION %s\n", m_portrait ? "Portrait" : "Landscape");
	fprintf(fp, "SPACEMENT %f\n", m_global_spacement_fac);
	fprintf(fp, "SCALE %d\n", m_current_scale);
	fprintf(fp, "NUMERATOR %d\n", m_numerator);
	fprintf(fp, "DENOMINATOR %d\n", m_denominator);
	fprintf(fp, "TIMESIG_SYMBOL %d\n", m_timesig_symbol);
	fprintf(fp, "TEMPOINVERSE %d\n", (int) m_midi_tempo_inverse);
	fprintf(fp, "UPBEAT_INVERSE %u\n", m_upbeat_inverse);
	fprintf(fp, "MEASURE_NUMBERS %s\n", gtk_toggle_action_get_active(m_show_measure_numbers_action) ? "On" : "Off");
	fprintf(fp, "STAFFS %d\n", ((NedPage *) g_list_first(m_pages)->data)->getNumberOfStaffs());
	for (i = 0; i < ((NedPage *) g_list_first(m_pages)->data)->getNumberOfStaffs(); i++) {
		fprintf(fp, "STAFF %d: ", i);
		if (m_staff_contexts[i].m_staff_name == NULL) {
			fprintf(fp, "NAME: 0 ");
		}
		else if (strlen(m_staff_contexts[i].m_staff_name->getText()) < 1) {
			fprintf(fp, "NAME: 0 ");
		}
		else {
			fprintf(fp, "NAME: %zd %s\n", strlen(m_staff_contexts[i].m_staff_name->getText()), m_staff_contexts[i].m_staff_name->getText());
		}
		if (m_staff_contexts[i].m_staff_short_name == NULL) {
			fprintf(fp, "NAME_SHORT_NAME: 0 ");
		}
		else if (strlen(m_staff_contexts[i].m_staff_short_name->getText()) < 1) {
			fprintf(fp, "NAME_SHORT_NAME: 0 ");
		}
		else {
			fprintf(fp, "NAME_SHORT_NAME: %zd %s\n", strlen(m_staff_contexts[i].m_staff_short_name->getText()), m_staff_contexts[i].m_staff_short_name->getText());
		}
		if (m_staff_contexts[i].m_group_name == NULL) {
			fprintf(fp, "GROUP_NAME: 0 ");
		}
		else if (strlen(m_staff_contexts[i].m_group_name->getText()) < 1) {
			fprintf(fp, "GROUP_NAME: 0 ");
		}
		else {
			fprintf(fp, "GROUP_NAME: %zd %s\n", strlen(m_staff_contexts[i].m_group_name->getText()), m_staff_contexts[i].m_group_name->getText());
		}
		if (m_staff_contexts[i].m_group_short_name == NULL) {
			fprintf(fp, "GROUP_SHORT_NAME: 0 ");
		}
		else if (strlen(m_staff_contexts[i].m_group_short_name->getText()) < 1) {
			fprintf(fp, "GROUP_SHORT_NAME: 0 ");
		}
		else {
			fprintf(fp, "GROUP_SHORT_NAME: %zd %s\n", strlen(m_staff_contexts[i].m_group_short_name->getText()), m_staff_contexts[i].m_group_short_name->getText());
		}
		fprintf(fp, " KEY: %d CLEF: ", m_staff_contexts[i].m_key_signature_number);
		switch (m_staff_contexts[i].m_clef_number) {
			case TREBLE_CLEF: fprintf(fp, "TREBLE "); break;
			case BASS_CLEF: fprintf(fp, "BASS "); break;
			case SOPRAN_CLEF: fprintf(fp, "SOPRAN "); break; 
			case TENOR_CLEF: fprintf(fp, "TENOR "); break;
			case NEUTRAL_CLEF1: fprintf(fp, "NEUTRAL1 "); break;
			case NEUTRAL_CLEF2: fprintf(fp, "NEUTRAL2 "); break;
			case NEUTRAL_CLEF3: fprintf(fp, "NEUTRAL3 "); break;
			default: fprintf(fp, "ALTO "); break;
		}
		fprintf(fp, "%d ", m_staff_contexts[i].m_clef_octave_shift);
		fprintf(fp, "CHANNEL: %d DIFFERENT: %s VOL: %d %d %d %d MIDIPGM: %d %d %d %d PAN: %d %d %d %d CHORUS: %d REVERB: %d",
			m_staff_contexts[i].m_midi_channel,
			m_staff_contexts[i].m_different_voices ? "On" : "Off", 
			m_staff_contexts[i].voices[0].m_midi_volume,
			m_staff_contexts[i].voices[1].m_midi_volume,
			m_staff_contexts[i].voices[2].m_midi_volume,
			m_staff_contexts[i].voices[3].m_midi_volume,
			m_staff_contexts[i].voices[0].m_midi_program,
			m_staff_contexts[i].voices[1].m_midi_program,
			m_staff_contexts[i].voices[2].m_midi_program,
			m_staff_contexts[i].voices[3].m_midi_program,
			m_staff_contexts[i].voices[0].m_midi_pan,
			m_staff_contexts[i].voices[1].m_midi_pan,
			m_staff_contexts[i].voices[2].m_midi_pan,
			m_staff_contexts[i].voices[3].m_midi_pan,
			m_staff_contexts[i].m_midi_chorus, m_staff_contexts[i].m_midi_reverb);
		fprintf(fp, " FLAGS: %x", m_staff_contexts[i].m_flags);
		putc('\n', fp);
		fprintf(fp, "PLAY_TRANSPOSED: %d\n", m_staff_contexts[i].m_play_transposed);
	}
	if (m_special_measures != NULL) {
		fprintf(fp, "SPEC_MEASURES\n");
		for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
			fprintf(fp, "( %d ", ((SpecialMeasure *) lptr->data)->measure_number);
			switch (((SpecialMeasure *) lptr->data)->type & REP_TYPE_MASK) {
				case REPEAT_OPEN: fprintf(fp, "REPEAT_OPEN "); break;
				case REPEAT_CLOSE: fprintf(fp, "REPEAT_CLOSE %s ", ((SpecialMeasure *) lptr->data)->hide_following ? "HIDE_FOLLOWING": "SHOW_FOLLOWING"); break;
				case REPEAT_OPEN_CLOSE: fprintf(fp, "REPEAT_OPEN_CLOSE "); break;
				case DOUBLE_BAR: fprintf(fp, "DOUBLE_BAR "); break;
				case END_BAR: fprintf(fp, "END_BAR %s ", ((SpecialMeasure *) lptr->data)->hide_following ? "HIDE_FOLLOWING": "SHOW_FOLLOWING"); break;
				default: fprintf(fp, "REPEAT_NONE "); break;
			}
			switch(((SpecialMeasure *) lptr->data)->type & START_TYPE_MASK) {
				case REP1START: fprintf(fp, " , REP1START "); break;
				case REP2START: fprintf(fp, " , REP2START "); break;
			}
			switch(((SpecialMeasure *) lptr->data)->type & END_TYPE_MASK) {
				case REP1END: fprintf(fp, " , REP1END "); break;
				case REP2END: fprintf(fp, " , REP2END "); break;
			}
			switch(((SpecialMeasure *) lptr->data)->type & TIMESIG_TYPE_MASK) {
				case TIMESIG: fprintf(fp, " , TIMSIG ( %d, %d, %d ) ",
				     ((SpecialMeasure *) lptr->data)->numerator, ((SpecialMeasure *) lptr->data)->denominator,
				     	((SpecialMeasure *) lptr->data)->symbol); break;
			}
			fprintf(fp, ")\n");
		}
	}
	fprintf(fp, "MUSIC\n");
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->savePage(fp);
	}
	fclose(fp);
	m_config_changed = FALSE;
	m_command_history->setSavePoint();
}

void NedMainWindow::print_file(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	char print_cmd[4096];
	char fname[4096];
	int fd;
	FILE *psfile;
	char Str[1024];

	strcpy(fname, "/tmp/nted_tmp-XXXXXX");

	if ((fd = mkstemp(fname)) < 0) {
		sprintf(Str, "Cannot open %s for writing(1)", fname);
		NedResource::Warning(Str);
		return;
	}
	if ((psfile = fdopen(fd, "w")) == NULL) {
		sprintf(Str, "Cannot open %s for writing(2)", fname);
		NedResource::Warning(Str);
		return;
	}
	main_window->do_export_to_stream(psfile, STREAM_FILE_TYPE_PS);
	fclose(psfile);
	sprintf(print_cmd, NedResource::m_print_cmd, fname);
	system(print_cmd);
	NedResource::appendTempFileName(fname);
}

void NedMainWindow::setAndUpdateClefTypeAndKeySig() {
	GList *lptr;
	bool first = true;
	int i;
	int clef_and_key_array[3 * m_staff_count];

	for (i = 0; i < m_staff_count; i++) {
		clef_and_key_array[3 * i + 0] = m_staff_contexts[i].m_clef_number;
		clef_and_key_array[3 * i + 1] = m_staff_contexts[i].m_clef_octave_shift;
		clef_and_key_array[3 * i + 2] = m_staff_contexts[i].m_key_signature_number;
	}
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->setAndUpdateClefTypeAndKeySig(clef_and_key_array, first);
		first = false;
	}
}


void NedMainWindow::copy_data(GtkWidget *widget, void *data) {
	GList *lptr;

	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::m_main_clip_board != NULL) {
		g_list_free(NedResource::m_main_clip_board);
		NedResource::m_main_clip_board = NULL;
	}
	for (lptr = g_list_first(main_window->m_selected_group); lptr; lptr = g_list_next(lptr)) {
		NedResource::m_main_clip_board = g_list_append(NedResource::m_main_clip_board, lptr->data);
	}
}

void NedMainWindow::importRecorded(GtkWidget *widget, void *data) {
	GList *lptr;

	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (NedResource::m_recorded_staff == NULL) {
		NedResource::Info("No recorded staff found");
		return;
	}
	if (NedResource::m_main_clip_board != NULL) {
		g_list_free(NedResource::m_main_clip_board);
		NedResource::m_main_clip_board = NULL;
	}

	for (lptr = g_list_first(NedResource::m_recorded_staff->getMainWindow()->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->copyDataOfWholeStaff(NedResource::m_recorded_staff->getStaffNumber());
	}
	NedResource::m_number_of_first_selected_staff = NedResource::m_number_of_last_selected_staff = 0;
	paste_data(main_window->m_main_window, data);
}

void NedMainWindow::paste_data(GtkWidget *widget, void *data) {
	int start_staff;
	unsigned long long midi_time;
	unsigned int meas_duration;
	NedSystem *system;
	int i;
	int nelems, num_elems;
	GList *c_board;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (NedResource::m_main_clip_board == NULL) return;

	if (main_window->m_selected_chord_or_rest == NULL) {
		NedResource::Info(_("Please select a note or rest!"));
		return;
	}
	start_staff = main_window->m_selected_chord_or_rest->getStaff()->getStaffNumber();
	if (main_window->m_selected_chord_or_rest->getTupletVal() != 0 && !NedResource::fittingPosition(main_window->m_selected_chord_or_rest->getMidiTime())) {
		NedResource::Info(_("You select a note in an tuplet"));
		return;
	}
	if (NedResource::test_for_incomplete_tuplets(NedResource::m_main_clip_board)) {
		NedResource::Info(_("You cannot copy incomplete tuplets!"));
		return;
	}

	system = main_window->m_selected_chord_or_rest->getSystem();
	midi_time = main_window->m_selected_chord_or_rest->getMidiTime();
	c_board = NedResource::clone_chords_and_rests(NedResource::m_main_clip_board, true);
	nelems = 0;
	num_elems = g_list_length(c_board);
	meas_duration = main_window->m_numerator * WHOLE_NOTE / main_window->m_denominator;
	while (nelems != num_elems && system != NULL) {
		for (i = 0; i <= NedResource::m_number_of_last_selected_staff - NedResource::m_number_of_first_selected_staff &&
			start_staff + i < main_window->m_staff_count; i++) {

			if (system->hasTupletConflict(meas_duration, &(c_board), NedResource::m_number_of_first_selected_staff + i, start_staff + i, midi_time)) {
					NedResource::Info(_("Paste not possible: A tuplet would cross a measure"));
					return;
			}
		}
		system = system->getPage()->getNextSystem(system);
		midi_time = 0;
		num_elems = nelems;
		nelems = g_list_length(c_board);
	}
	g_list_free(c_board);

	c_board = NedResource::clone_chords_and_rests(NedResource::m_main_clip_board, true);
	system = main_window->m_selected_chord_or_rest->getSystem();
	midi_time = main_window->m_selected_chord_or_rest->getMidiTime();
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_tie_forward = NULL;
	main_window->m_selected_note = NULL;
	main_window->m_selected_spec_measure = NULL;
	NedCommandList *command_list = new NedCommandList(main_window);
	nelems = 0;
	num_elems = g_list_length(c_board);
	while (nelems != num_elems && system != NULL) {
		for (i = 0; i <= NedResource::m_number_of_last_selected_staff - NedResource::m_number_of_first_selected_staff &&
			start_staff + i < main_window->m_staff_count; i++) {
			system->pasteElements(command_list, &(c_board),
			NedResource::m_number_of_first_selected_staff + i, start_staff + i, midi_time);

		}
		system = system->getPage()->getNextSystem(system, command_list);
		midi_time = 0;
		num_elems = nelems;
		nelems = g_list_length(c_board);
	}
	g_list_free(c_board);
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	command_list->setFullReposit();
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::delete_block(GtkWidget *widget, void *data) {
	NedPage *min_page = NULL, *max_page = NULL;
	NedSystem *min_sys = NULL, *max_sys = NULL;
	GList *lptr;
	GList *start_page_ptr, *end_page_ptr;
	unsigned long long min_time, max_time;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (NedResource::m_main_clip_board != NULL) {
		g_list_free(NedResource::m_main_clip_board);
		NedResource::m_main_clip_board = NULL;
	}
	for (lptr = g_list_first(main_window->m_selected_group); lptr; lptr = g_list_next(lptr)) {
		NedResource::m_main_clip_board = g_list_append(NedResource::m_main_clip_board, lptr->data);
	}

	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->findFromTo(NedResource::m_main_clip_board, &min_page, &max_page, &min_sys, &max_sys);
	}
	if (min_page != NULL && max_page!= NULL) {
		if (min_sys == NULL || max_sys == NULL) {
			NedResource::Abort("NedMainWindow::delete_block(1)");
		}
		if ((start_page_ptr = g_list_find(main_window->m_pages, min_page)) == NULL) {
			NedResource::Abort("NedMainWindow::delete_block(2)");
		}
		if ((end_page_ptr = g_list_find(main_window->m_pages, max_page)) == NULL) {
			NedResource::Abort("NedMainWindow::delete_block(3)");
		}
		min_time = (1 << 30);
		min_time = min_time * (1 << 30);
		max_time = 0;
		if (!min_sys->findStartMeasureLimits(NedResource::m_main_clip_board, &min_time)) return;
		if (!max_sys->findEndMeasureLimits(NedResource::m_main_clip_board, &max_time)) return;
		NedCommandList *command_list = new NedCommandList(main_window);
		while (1) {
			if (start_page_ptr == NULL) {
				NedResource::Abort("NedMainWindow::delete_block(4)");
			}
			((NedPage *) start_page_ptr->data)->deleteItemsFromTo(command_list, start_page_ptr->data == min_page,
				start_page_ptr->data == max_page, min_sys, max_sys, min_time, max_time);
			if (start_page_ptr == end_page_ptr) break;
			start_page_ptr = g_list_next(start_page_ptr);
		}
		main_window->m_selection_rect.width = main_window->m_selection_rect.height = 0;
		main_window->setAllUnpositioned();
		main_window->reposit(command_list);
		command_list->setFullReposit();
		main_window->m_command_history->addCommandList(command_list);
		main_window->repaint();
	}
}

void NedMainWindow::append_system(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list = new NedCommandList(main_window);
	NedPage *last_page;

	last_page = (NedPage *) g_list_last(main_window->m_pages)->data;

	NedAppendSystemCommand *app_sys_cmd = new NedAppendSystemCommand(last_page);
	app_sys_cmd->execute();
	command_list->addCommand(app_sys_cmd);
	command_list->setFullReposit();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::insert_block(GtkWidget *widget, void *data) {
	int blockcount;
	bool state;
	NedMeasure *measure;
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (main_window->m_selected_chord_or_rest == NULL) {
		NedResource::Info(_("Please select a note or rest!"));
		return;
	}
	NedInsertBlockDialog *dialog = new NedInsertBlockDialog(GTK_WINDOW(main_window->m_main_window));
	dialog->getValues(&state, &blockcount);
	delete dialog;
	if (!state) return;
	//measure = main_window->m_selected_chord_or_rest->getSystem()->getMeasure(main_window->m_selected_chord_or_rest->getMidiTime());
	measure = main_window->m_selected_chord_or_rest->m_measure;
	NedCommandList *command_list = new NedCommandList(main_window);
	main_window->m_selected_chord_or_rest->getSystem()->insertBlocks(command_list, blockcount, measure->midi_end);
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	command_list->setFullReposit();
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::empty_block(GtkWidget *widget, void *data) {
	NedPage *min_page = NULL, *max_page = NULL;
	NedSystem *min_sys = NULL, *max_sys = NULL;
	GList *lptr;
	GList *start_page_ptr, *end_page_ptr;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::selection_has_uncomplete_tuplets(main_window->m_selected_group)) {
		NedResource::Info(_("Empty not possible because of incomplete tuplets"));
		return;
	}

	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->findFromTo(main_window->m_selected_group, &min_page, &max_page, &min_sys, &max_sys);
	}

	if (min_page != NULL && max_page!= NULL) {
		if (min_sys == NULL || max_sys == NULL) {
			NedResource::Abort("NedMainWindow::empty_block(1)");
		}
		if ((start_page_ptr = g_list_find(main_window->m_pages, min_page)) == NULL) {
			NedResource::Abort("NedMainWindow::empty_block(2)");
		}
		if ((end_page_ptr = g_list_find(main_window->m_pages, max_page)) == NULL) {
			NedResource::Abort("NedMainWindow::empty_block(3)");
		}
		NedCommandList *command_list = new NedCommandList(main_window);
		while (1) {
			if (start_page_ptr == NULL) {
				NedResource::Abort("NedMainWindow::empty_block(4)");
			}
			((NedPage *) start_page_ptr->data)->removeNotesFromTo(command_list, main_window->m_selected_group, start_page_ptr->data == min_page,
				start_page_ptr->data == max_page, min_sys, max_sys);
			if (start_page_ptr == end_page_ptr) break;
			start_page_ptr = g_list_next(start_page_ptr);
		}
		main_window->m_selection_rect.width = main_window->m_selection_rect.height = 0;
		main_window->setAllUnpositioned();
		main_window->reposit(command_list);
		command_list->setFullReposit();
		main_window->m_command_history->addCommandList(command_list);
		main_window->repaint();
	}
}

void NedMainWindow::reposit_all(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list = new NedCommandList(main_window);

	main_window->setAllUnpositioned();

	main_window->reposit(command_list);

	if (command_list->getNumberOfCommands() > 0) {
		main_window->m_command_history->addCommandList(command_list);
	}
	main_window->repaint();
}

void NedMainWindow::export_midi(GtkWidget  *widget, void *data) {
	int i, j;
	FILE *fp;
	GList *lptr;
	char the_filename[4096];
	NedMainWindow *main_window = (NedMainWindow *) data;

	GtkWidget *midi_export_dialog;
	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	char *filename;
	bool ok = FALSE;
	bool extend = FALSE;
	char *cptr;
	int last_rep_open = 1, last_alternative1 = -1;
	SpecialMeasure *spec_ptr;

	if (NedResource::isPlaying()) return;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.mid (MIDI-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.mid");
	gtk_file_filter_add_pattern(file_filter1, "*.midi");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	midi_export_dialog = gtk_file_chooser_dialog_new ("Export MIDI",
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(midi_export_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(midi_export_dialog), file_filter2);
	if (main_window->m_current_filename[0] != '\0') {
		strcpy(the_filename, main_window->m_current_filename);
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				*cptr = '\0';
			}
		}
		strcat(the_filename, ".mid");
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(midi_export_dialog), the_filename);
	}

	if (gtk_dialog_run (GTK_DIALOG (midi_export_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (midi_export_dialog));
		strcpy(the_filename, filename);
		g_free (filename);
		ok = TRUE;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (midi_export_dialog))) == file_filter1) {
		extend = TRUE;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".mid") || !strcmp(cptr, ".midi")) {
				extend = FALSE;
			}
		}
		if (extend) {
			strcat(the_filename, ".mid");
		}
	}
	gtk_widget_destroy (midi_export_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = FALSE;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (ok) {
		if ((fp = fopen(the_filename, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			return;
		}
		for (i = 0; i < main_window->m_staff_count; i++) {
			main_window->m_staff_contexts[i].m_effective_channel = main_window->m_staff_contexts[i].m_midi_channel;
			main_window->m_staff_contexts[i].m_pitch_offs = 0;
			if (main_window->m_staff_contexts[i].m_different_voices) {
				for (j = 0; j < VOICE_COUNT; j++) {
					main_window->m_staff_contexts[i].voices[j].m_current_midi_program = main_window->m_staff_contexts[i].voices[j].m_midi_program;
					NedResource::setStartVolume(main_window->m_staff_contexts[i].m_midi_channel, j,
						 main_window->m_staff_contexts[i].voices[j].m_midi_volume);
				}
			}
			else {
				for (j = 0; j < VOICE_COUNT; j++) {
					main_window->m_staff_contexts[i].voices[j].m_current_midi_program = main_window->m_staff_contexts[i].voices[j].m_midi_program;
					NedResource::setStartVolume(main_window->m_staff_contexts[i].m_midi_channel, j,
						 main_window->m_staff_contexts[i].voices[0].m_midi_volume);
				}
			}
		}
		for (i = 0; i < main_window->m_staff_count; i++) {
			NedResource::MidiCtrl(MIDI_CTL_MSB_PAN, main_window->m_staff_contexts[i].m_midi_channel, main_window->m_staff_contexts[i].voices[0].m_midi_pan);
			NedResource::MidiCtrl(MIDI_CTL_LSB_PAN, main_window->m_staff_contexts[i].m_midi_channel, 0);
			NedResource::MidiCtrl(MIDI_CTL_E1_REVERB_DEPTH, main_window->m_staff_contexts[i].m_midi_channel, main_window->m_staff_contexts[i].m_midi_reverb);
			NedResource::MidiCtrl(MIDI_CTL_E3_CHORUS_DEPTH, main_window->m_staff_contexts[i].m_midi_channel, main_window->m_staff_contexts[i].m_midi_chorus);
		}

		NedResource::prepareMidiEventList(main_window->m_midi_tempo_inverse);
		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->prepareReplay(true);
		}
		NedResource::correctFermataTempo();
		for (lptr = g_list_first(main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) {
			spec_ptr = (SpecialMeasure *) lptr->data;
			switch (spec_ptr->type & REP_TYPE_MASK) {
				case REPEAT_OPEN: last_rep_open = spec_ptr->measure_number; break;
				case REPEAT_CLOSE: NedResource::copyAllBetweenMeasures(last_rep_open, last_alternative1, spec_ptr->measure_number);
							last_alternative1 = -1; break;
				case REPEAT_OPEN_CLOSE: NedResource::copyAllBetweenMeasures(last_rep_open, -1, spec_ptr->measure_number);
							last_rep_open = spec_ptr->measure_number; 
							last_alternative1 = -1;
							break;
			}
			switch (spec_ptr->type & START_TYPE_MASK) {
				case REP1START: last_alternative1 = spec_ptr->measure_number; break;
			}
		}
		NedResource::handleSegnos();
		NedResource::startMidiExport(fp, main_window);
		fclose(fp);
	}
}

void NedMainWindow::export_lily(GtkWidget  *widget, void *data) {
	FILE *fp;
	char the_filename[4096];
	NedMainWindow *main_window = (NedMainWindow *) data;

	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	NedLilyPondExportDialog *lily_export_dialog;
	bool state, with_break, ok = FALSE;
	bool extend = FALSE;
	char *cptr;
	bool *selected_staves = NULL;
	bool *keep_beams = NULL;

	if (NedResource::isPlaying()) return;

	the_filename[0] = '\0';
	if (main_window->m_current_filename[0] != '\0') {
		strcpy(the_filename, main_window->m_current_filename);
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				*cptr = '\0';
			}
		}
		strcat(the_filename, ".ly");
	}

	selected_staves = (bool *) alloca(main_window->m_staff_count * sizeof(bool));
	for (int i = 0; i < main_window->m_staff_count; selected_staves[i++] = true);
	keep_beams = (bool *) alloca(main_window->m_staff_count * sizeof(bool));
	for (int i = 0; i < main_window->m_staff_count; keep_beams[i++] = false);

	lily_export_dialog = new NedLilyPondExportDialog(GTK_WINDOW(main_window->m_main_window), main_window, the_filename, selected_staves, keep_beams);

	lily_export_dialog->getValues(&state, &with_break);
	delete lily_export_dialog;
	if (!state) return;
	extend = TRUE;
	if ((cptr = strrchr(the_filename, '.')) != NULL) {
		if (!strcmp(cptr, ".ly")) {
			extend = FALSE;
		}
	}
	if (extend) {
		strcat(the_filename, ".ly");
	}
	ok = true;
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = FALSE;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (ok) {
		if ((fp = fopen(the_filename, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			return;
		}
			
		main_window->do_lily_export(fp, with_break, selected_staves, keep_beams);
		fclose(fp);
	}
}

int NedMainWindow::computeLilySize() {
#define LILY_MIN_SIZE 10
#define LILY_MAX_SIZE 27
	double m = (double) (LILY_MAX_SIZE - LILY_MIN_SIZE) / ((SCALE_MAX - SCALE_MIN) * (double) SCALE_GRANULARITY);
	double n = (double) LILY_MIN_SIZE - SCALE_MIN * (double) SCALE_GRANULARITY * m;
	double ly_size = m * (double) m_current_scale + n;

	if (ly_size < 12.0) return 11;
	if (ly_size < 13.5) return 13;
	if (ly_size < 15.0) return 14;
	if (ly_size < 17.0) return 16;
	if (ly_size < 19.0) return 18;
	if (ly_size < 22.5) return 20;
	if (ly_size < 24.5) return 23;
	return 26;
}

void NedMainWindow::do_lily_export(FILE *fp, bool with_break, bool *selected_staves, bool *keep_beams) {
	GList *lptr;
	int i, j, k;
	unsigned int m, voice_mask[m_staff_count];
	int the_voice = 0, voice_count[m_staff_count];
	int last_line;
	unsigned int midi_len;
	NedSystem *last_system = NULL, *dummy = NULL;
	unsigned long long end_time = 0, dummy2;
	unsigned int partial = 0;
	bool open_repeat = false;
	bool go_on;
	int tabs;
	bool in_alternative;
	unsigned int lyrics_map[m_staff_count], lm_dummy, *lm;
	int clef_number;
	int sil_count;
	NedSlur *lily_slur;
	bool all_staves = true;
	bool guitar_chordnames[m_staff_count];
	bool chordnames[m_staff_count];
	int breath_script;
	char Str[1024];

	computeLilySize();

	for (i = 0; i < m_staff_count; i++) {
		guitar_chordnames[i] = false;
		chordnames[i] = false;
		if (!selected_staves[i]) {
			all_staves = false;
			break;
		}
	}

	fprintf(fp, "\\header {\n");
	if (m_score_info->title != NULL && strlen(m_score_info->title->getText()) > 1) {
		NedPangoCairoText::makeLilyString(m_score_info->title->getText(), Str);
		fprintf(fp, "\ttitle = \\markup \\center-align {%s}\n", Str);
	}
	if (m_score_info->subject != NULL && strlen(m_score_info->subject->getText()) > 1) {
		NedPangoCairoText::makeLilyString(m_score_info->subject->getText(), Str);
		fprintf(fp, "\tsubtitle = \\markup \\center-align {%s}\n", Str);
	}
	if (m_score_info->composer != NULL && strlen(m_score_info->composer->getText()) > 1) {
		NedPangoCairoText::makeLilyString(m_score_info->composer->getText(), Str);
		fprintf(fp, "\tcomposer = %s\n", Str);
	}
	if (m_score_info->arranger != NULL && strlen(m_score_info->arranger->getText()) > 1) {
		NedPangoCairoText::makeLilyString(m_score_info->arranger->getText(), Str);
		fprintf(fp, "\tarranger = %s\n", Str);
	}
	if (m_score_info->copyright != NULL && strlen(m_score_info->copyright->getText()) > 1) {
		NedPangoCairoText::makeLilyString(m_score_info->copyright->getText(), Str);
		fprintf(fp, "\tcopyright = %s\n", Str);
	}
	fprintf(fp, "}\n\n");

	if (!strcmp(m_current_paper->name, "A3")) {
		if (m_portrait) {
			fprintf(fp, "#(set-default-paper-size \"a3\")\n");
		}
		else {
			fprintf(fp, "#(set-default-paper-size \"a3\" 'landscape)\n");
		}
	}
	else if (!strcmp(m_current_paper->name, "A5")) {
		if (m_portrait) {
			fprintf(fp, "#(set-default-paper-size \"a5\")\n");
		}
		else {
			fprintf(fp, "#(set-default-paper-size \"a5\" 'landscape)\n");
		}
	}
	else {
		if (m_portrait) {
			fprintf(fp, "#(set-default-paper-size \"a4\")\n");
		}
		else {
			fprintf(fp, "#(set-default-paper-size \"a4\" 'landscape)\n");
		}
	}
	if (m_current_scale != SCALE_GRANULARITY) {
		fprintf(fp, "#(set-global-staff-size %d)\n", computeLilySize());
	}

	for (go_on = true, lptr = g_list_first(m_special_measures); go_on && lptr; lptr = g_list_next(lptr)) {
		switch (((SpecialMeasure *) lptr->data)->type & REP_TYPE_MASK) {
			case REPEAT_OPEN_CLOSE: 
			case REPEAT_CLOSE: open_repeat = true; go_on = false; break;
			case REPEAT_OPEN: if (((SpecialMeasure *) lptr->data)->measure_number == 1) open_repeat = true; go_on = false; break;
		}
	}
		

	if (m_upbeat_inverse != 0) {
		partial = m_numerator * WHOLE_NOTE / m_denominator - m_upbeat_inverse;
		partial /= NOTE_64;
	}
	for (i = 0; i < m_staff_count; i++) {
		voice_mask[i] = 0x0;
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->detectVoices(i, &(voice_mask[i]), &last_system, &end_time);
		}
	}
	for (i = 0; i < m_staff_count; i++) {
		if (!selected_staves[i]) continue;
		voice_mask[i] = 0x0;
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->detectVoices(i, &(voice_mask[i]), &dummy, &dummy2);
		}
		clef_number = m_staff_contexts[i].m_clef_number;
		voice_count[i] = 0;
		the_voice = 0;
		for (j = 0; j < VOICE_COUNT; j++) {
			m = (1 << j);
			if ((m & voice_mask[i]) != 0) {
				voice_count[i]++;
				the_voice = j;
			}
		}

		lyrics_map[i] = 0;
		lm = &(lyrics_map[i]);
		for (j = 0; j < VOICE_COUNT; j++) {
			m = (1 << j);
			if ((m & voice_mask[i]) == 0) continue;
			if (m_staff_contexts[i].m_clef_number == NEUTRAL_CLEF3) {
				fprintf(fp, "Staff%cVoice%c = \\drummode  {\n", 'A' + i, 'A' + j);
			}
			else {
				fprintf(fp, "Staff%cVoice%c = \\relative c' {\n", 'A' + i, 'A' + j);
			}
			if (keep_beams[i]) {
				fprintf(fp, " \\autoBeamOff ");
			}
			if (open_repeat) {
				fprintf(fp, "\t\\repeat volta 2 {\n");
			}
			putc('\t', fp);
			last_line = NedResource::determineLastLine(-2, clef_number);
			midi_len = 0;
			in_alternative = false;
			lily_slur = NULL;
			breath_script = LILY_BREAH_SCRIPT_RCOMMA;
			for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
				if (!((NedPage *) lptr->data)->exportLilyPond(fp, i, j, &last_line, &midi_len, last_system,
						end_time, &in_alternative, &lily_slur, lm, with_break, &(guitar_chordnames[i]), &(chordnames[i]), &breath_script, keep_beams[i])) break;
			}
			if (in_alternative) { // last measure is REP2END
				fprintf(fp, "\n\t}\n\t}\n");
			}
			fprintf(fp, "\n}\n\n");
			if (j == 0 && lyrics_map[i] != 0) {
				for (k = 0; k < MAX_LYRICS_LINES; k++) {
					if ((lyrics_map[i] & (1 << k)) == 0) continue;
					sil_count = 0;
					fprintf(fp, "Staff%cLyrics%c = \\lyricmode{\n\t", 'A' + i, 'A' + k);
					for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
						if (!((NedPage *) lptr->data)->exportLilyLyrics(fp, i, 0, k, last_system, end_time, &sil_count)) {
							break;
						}
					}
					fprintf(fp, "\n}\n");
				}
			}
			lm_dummy = 0;
			lm = &lm_dummy;
		}
		if (m_staff_contexts[i].m_clef_number == NEUTRAL_CLEF3) {
			fprintf(fp, "Staff%c = \\new DrumStaff { ", 'A' + i);
		}
		else {
			fprintf(fp, "Staff%c = \\new Staff \\relative c' {", 'A' + i);
		}
		if (m_staff_contexts[i].m_staff_name != NULL && strlen(m_staff_contexts[i].m_staff_name->getText()) > 0) {
			fprintf(fp, " \\set Staff.instrumentName = \"%s \"", m_staff_contexts[i].m_staff_name->getText());
		}
		if (m_staff_contexts[i].m_staff_short_name != NULL && strlen(m_staff_contexts[i].m_staff_short_name->getText()) > 0) {
			fprintf(fp, " \\set Staff.shortInstrumentName = \"%s \"", m_staff_contexts[i].m_staff_short_name->getText());
		}
		fprintf(fp, NedResource::getLilyPondClefName(m_staff_contexts[i].m_clef_number));
		fprintf(fp, NedResource::getLilyPondKeySigName(m_staff_contexts[i].m_key_signature_number));
		fprintf(fp, " \\time %d/%d", m_numerator, m_denominator);
		if (m_upbeat_inverse != 0) {
			fprintf(fp, " \\partial 64*%d", partial);
		}
		fprintf(fp, "\n");
		fprintf(fp, "\t<<\n");
		for (j = 0; j < VOICE_COUNT; j++) {
			m = (1 << j);
			if ((m & voice_mask[i]) == 0) continue;
			if (m_staff_contexts[i].m_clef_number == NEUTRAL_CLEF3) {
				fprintf(fp, "\\drummode{\n\t\t\\Staff%cVoice%c", 'A' + i, 'A' + j);
			}
			else {
				fprintf(fp, "\t\t\\new Voice = \"");
				switch(j) {
					case 0: if (voice_count[i] > 1) {
							fprintf(fp, "one\" { \\voiceOne ");
						}
						else {
							fprintf(fp, "one\" { ");
						}
						 break;
					case 1: fprintf(fp, "two\" { \\voiceTwo "); break;
					case 2: fprintf(fp, "three\" { \\voiceThree "); break;
					case 3: fprintf(fp, "four\" { \\voiceFour "); break;
				}
				fprintf(fp, "\\Staff%cVoice%c }", 'A' + i, 'A' + j);
			} 

			fprintf(fp, " \n");
			if (m_staff_contexts[i].m_clef_number == NEUTRAL_CLEF3) {
				fprintf(fp, "\t\t}\n");
			}
		}
		fprintf(fp, "\t>>\n");
		fprintf(fp, "\n}\n\n");
		if (guitar_chordnames[i]) {
			fprintf(fp, "Staff%cGuitarChords = \\new ChordNames {\n", 'A' + i);
			fprintf(fp, "\t\\chordmode {\n\t\t");
			if (open_repeat) {
				fprintf(fp, "\t\\repeat volta 2 {\n");
			}
			in_alternative = false;
			for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
				if (!((NedPage *) lptr->data)->exportLilyGuitarChordnames(fp, i, &last_line, &midi_len, last_system,
						end_time, &in_alternative, with_break)) break;
			}
			if (in_alternative) { // last measure is REP2END
				fprintf(fp, "\n\t}\n\t}\n");
			}
			fprintf(fp, "\n\t}\n");
			fprintf(fp, "}\n\n");
		}
		if (chordnames[i]) {
			fprintf(fp, "Staff%cChords = \\new ChordNames {\n", 'A' + i);
			fprintf(fp, "\t\\chordmode {\n\t\t");
			if (open_repeat) {
				fprintf(fp, "\t\\repeat volta 2 {\n");
			}
			in_alternative = false;
			for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
				if (!((NedPage *) lptr->data)->exportLilyFreeChordName(fp, i, &last_line, &midi_len, last_system,
						end_time, &in_alternative, with_break)) break;
			}
			if (in_alternative) { // last measure is REP2END
				fprintf(fp, "\n\t}\n\t}\n");
			}
			fprintf(fp, "\n\t}\n");
			fprintf(fp, "}\n\n");
		}
	}
	fprintf(fp, "\\score {\n");
	fprintf(fp, "\t<<\n");
	tabs = 1;
	for (i = 0; i < m_staff_count; i++) {
		if (!selected_staves[i]) continue;
		if ((all_staves && m_staff_contexts[i].m_flags & BRACKET_START) != 0) {
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, "\\new StaffGroup <<\n\n");
			tabs++;
		}
		if (all_staves && (m_staff_contexts[i].m_flags & BRACE_START) != 0) {
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, "\\new PianoStaff <<\n");
			tabs++;
		}
		if (guitar_chordnames[i]) {
			tabs++;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, "<<\n");
			tabs++;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, "\\Staff%cGuitarChords\n", 'A' + i);
			tabs--;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, ">>\n");
		}
		if (chordnames[i]) {
			tabs++;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, "<<\n");
			tabs++;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, "\\Staff%cChords\n", 'A' + i);
			tabs--;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, ">>\n");
		}
		for (j = 0; j < tabs; j++) putc('\t', fp);
		fprintf(fp, "\\Staff%c\n", 'A' + i);
		for (j = 0; j < MAX_LYRICS_LINES; j++) {
			m = (1 << j);
			if ((lyrics_map[i] & m) == 0) continue;
			for (k = 0; k < tabs; k++) putc('\t', fp);
			fprintf(fp, "\\new Lyrics \\lyricsto \"one\" { \\Staff%cLyrics%c }\n", 'A' + i, 'A' + j);
		}
			
		if (all_staves && ((m_staff_contexts[i].m_flags & BRACKET_END) != 0)) {
			tabs--;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, ">>\n");
		}
		if (all_staves && ((m_staff_contexts[i].m_flags & BRACE_END) != 0)) {
			tabs--;
			for (j = 0; j < tabs; j++) putc('\t', fp);
			fprintf(fp, ">>\n");
		}
	}
	fprintf(fp, "\t>>\n");
	fprintf(fp, "\t\\layout { }\n");
	fprintf(fp, "}\n");
}

/*
void NedMainWindow::append_page(GtkWidget *widget, void *data) {
	NedCommandList *command_list;
	int len;
	unsigned int measure_number;

	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	measure_number = ((NedPage *) g_list_last(main_window->m_pages)->data)->getNumberOfLastMeasure();
	command_list = new NedCommandList(main_window, NULL);
	len = g_list_length(main_window->m_pages);
	command_list->addCommand(new NedAppendNewPageCommand(main_window, len, measure_number + 1));
	command_list->execute();
	command_list->setFullReposit();
	command_list->setRenumber();
	main_window->m_command_history->addCommandList(command_list);
	main_window->renumberMeasures(NULL, 1, true);
}

void NedMainWindow::insert_page(GtkWidget *widget, void *data) {
	NedCommandList *command_list;
	int pos;
	unsigned int measure_number;

	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_selected_chord_or_rest == NULL) {
		NedResource::Info(_("Please select a note or rest!"));
		return;
	}
	command_list = new NedCommandList(main_window, NULL);
	if ((pos = g_list_index(main_window->m_pages, main_window->m_selected_chord_or_rest->getPage())) < 0) {
		NedResource::Abort("NedMainWindow::insert_page");
	}
	measure_number = main_window->m_selected_chord_or_rest->getPage()->getNumberOfLastMeasure();
	main_window->m_selected_chord_or_rest->getPage()->testForPageBackwardTies(command_list);
	command_list->addCommand(new NedInsertNewPageCommand(main_window, pos, measure_number + 1));
	command_list->execute();
	main_window->m_command_history->addCommandList(command_list);
}

*/
void NedMainWindow::renumberPages() {
	int number = 0;
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->setPageNumber(number++);
	}
}

void NedMainWindow::checkForElementsToSplit(NedCommandList *command_list) {
	GList *lptr;
	int measure_number = 1;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->checkForElementsToSplit(command_list, &measure_number);
	}
}

#define ABOUT_WIDTH 700
#define ABOUT_HEIGHT 250

void NedMainWindow::show_about(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	GtkWidget *about_dialog;
	NedMainWindow *main_window = (NedMainWindow *) data;

	about_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
		(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		_("NtEd version %s by Joerg Anders\nSend bug reports to %s\n\nInfos at http://vsr.informatik.tu-chemnitz.de/staff/jan/nted/nted.xhtml\n\nAuthor:\n               J.Anders <ja@informatik.tu-chemnitz.de>\n\nTranslation: <no translation>"), PACKAGE_VERSION, PACKAGE_BUGREPORT);
	gtk_dialog_run (GTK_DIALOG (about_dialog));
	gtk_widget_destroy (about_dialog);	
}

#ifdef YELP_PATH
void NedMainWindow::show_docu(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	char arg[1024];
	if (main_window->m_docu == NULL || access(YELP_PATH, X_OK)) {
		NedResource::Info(_("Excuse! The documentation is not available\n"
				"due to an installation error"));
		return;
	}
	switch (fork()) {
		case 0: 
			sprintf(arg, "ghelp:%s", main_window->m_docu);
			fprintf(stderr, "YELP_PATH: %s\narg: %s\n", YELP_PATH, arg);
			if (execl(YELP_PATH, YELP_PATH, arg, NULL) < 0) {
				perror("execl");
				NedResource::Info(_("Excuse! Cannot exec"));
				return;
			}
			exit(0);
		case -1: 
			NedResource::Info(_("Excuse! Cannot fork"));
			return;
		default:
			return;
	}
}
#endif

void NedMainWindow::show_license(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedResource::showLicense(GTK_WINDOW(main_window->m_main_window));
}

void NedMainWindow::remove_page(GtkWidget *widget, void *data) {
	NedCommandList *command_list;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (g_list_length(main_window->m_pages) < 2) return;
	command_list = new NedCommandList(main_window, NULL);
	command_list->addCommand(new NedRemoveLastPageCommand(main_window));
	command_list->setFullReposit();
	command_list->execute();
	main_window->m_command_history->addCommandList(command_list);
}

void NedMainWindow::remove_empty_pages(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->deleteEmptyPagesAtEnd();
	main_window->updatePageCounter();
	main_window->setAllUnpositioned();
	main_window->reposit();
	main_window->repaint();
}

void NedMainWindow::realizeNewSpacement(double spacement) {
	m_global_spacement_fac = spacement;
	setNewMeasureSpread(spacement);
	setAllUnpositioned();
	NedCommandList *command_list = new NedCommandList(this);
	reposit(command_list); 
	repaint();
}

void NedMainWindow::change_spacement(GtkWidget *widget, void *data) {
	double spacement;
	bool state;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedSpacementDialog *dialog = new NedSpacementDialog (GTK_WINDOW(main_window->m_main_window), main_window->m_global_spacement_fac);
	dialog->getValues(&state, &spacement);
	delete dialog;
	if (state) {
		NedCommandList *command_list = new NedCommandList(main_window);
		NedChangeSpacementCommand *new_spacement_cmd = new NedChangeSpacementCommand(main_window, spacement);
		new_spacement_cmd->execute();
		command_list->addCommand(new_spacement_cmd);
		main_window->m_command_history->addCommandList(command_list);

		/* --- this is all done during new_spacement_cmd->execute(); see void NedMainWindow::realizeNewSpacement(double spacement) */
		//command_list->setFullReposit();
		//main_window->setAllUnpositioned();
		//NedCommandList *command_list2 = new NedCommandList(main_window);
		//main_window->reposit(command_list2); 
		//main_window->repaint();
	}
}

void NedMainWindow::popup_tools(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedResource::popup_tools((NedMainWindow*) data);
}


void NedMainWindow::set_upbeat_measure(GtkWidget *widget, void *data) {
	unsigned int measure_length;
	NedCommandList *command_list;
	NedChangeUpbeadCommand *change_up_beat_cmd;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_selected_chord_or_rest == NULL) return;
	
	measure_length = main_window->m_numerator * WHOLE_NOTE / main_window->m_denominator;
	if (main_window->m_selected_chord_or_rest->getMidiTime() >= measure_length) return;
	change_up_beat_cmd = new NedChangeUpbeadCommand(main_window, measure_length - main_window->m_selected_chord_or_rest->getMidiTime());
	change_up_beat_cmd->execute();
	command_list = new NedCommandList(main_window);
	command_list->addCommand(change_up_beat_cmd);
	main_window->computeSystemIndent();
	main_window->reposit(command_list);
	//((NedPage *) g_list_first(main_window->m_pages)->data)->getFirstSystem()->assignMidiTimes();
	main_window->checkForElementsToSplit(command_list);
	//((NedPage *) g_list_first(main_window->m_pages)->data)->getFirstSystem()->assignMidiTimes();
	main_window->m_command_history->addCommandList(command_list);
	command_list->setFullReposit();
	main_window->setAndUpdateClefTypeAndKeySig();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	main_window->repaint();
}

void NedMainWindow::set_upbeat_start(GtkWidget *widget, void *data) {
	unsigned int measure_length;
	NedCommandList *command_list;
	unsigned int upbeat_inverse;
	NedChangeUpbeadCommand *change_up_beat_cmd;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_selected_chord_or_rest == NULL) return;
	upbeat_inverse = main_window->m_selected_chord_or_rest->getMidiTime();
	
	measure_length = main_window->m_numerator * WHOLE_NOTE / main_window->m_denominator;
	if (main_window->m_selected_chord_or_rest->getMidiTime() >= measure_length) return;
	command_list = new NedCommandList(main_window);
	if (!((NedPage *) g_list_first(main_window->m_pages)->data)->getFirstSystem()->truncateAtStart(command_list, main_window->m_selected_chord_or_rest->getMidiTime())) {
		NedResource::Info("Upbeat not possible here because note structures must be destroyed");
		return;
	}
	command_list->execute();
	((NedPage *) g_list_first(main_window->m_pages)->data)->getFirstSystem()->assignMidiTimes();
	change_up_beat_cmd = new NedChangeUpbeadCommand(main_window, upbeat_inverse);
	change_up_beat_cmd->execute();
	command_list->addCommand(change_up_beat_cmd);
	main_window->computeSystemIndent();
	//((NedPage *) g_list_first(main_window->m_pages)->data)->getFirstSystem()->assignMidiTimes();
	//main_window->checkForElementsToSplit(command_list);
	((NedPage *) g_list_first(main_window->m_pages)->data)->getFirstSystem()->assignMidiTimes();
	main_window->m_command_history->addCommandList(command_list);
	command_list->setFullReposit();
	main_window->setAndUpdateClefTypeAndKeySig();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	main_window->repaint();
}

void NedMainWindow::edit_score_info(GtkWidget *widget, void *data) {
	bool state;
	NedMainWindow *main_window = (NedMainWindow *) data;

	NedScoreInfoConfigDialog *dialog = new NedScoreInfoConfigDialog(GTK_WINDOW(main_window->m_main_window), main_window, main_window->m_score_info);
	dialog->getValues(&state);
	delete dialog;
	if (!state) return;
	main_window->computeScoreTextExtends();
	NedCommandList *command_list = new NedCommandList(main_window);
	command_list->setFullReposit();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	if (command_list->getNumberOfCommands() > 0) {
		main_window->m_command_history->addCommandList(command_list);
	}
	main_window->repaint();
}

void NedMainWindow::mute_staves(GtkWidget *widget, void *data) {
	int i, j;
	bool state;
	NedMainWindow *main_window = (NedMainWindow *) data;
	bool mutes[main_window->m_staff_count];

	NedMuteDialog *dialog = new NedMuteDialog(GTK_WINDOW(main_window->m_main_window), mutes, main_window->m_staff_contexts, main_window->m_staff_count);
	dialog->getValues(&state);
	if (state) {
		for (i = 0; i < main_window->m_staff_count; i++) {
			for (j = 0; j < VOICE_COUNT; j++) {
				main_window->m_staff_contexts[i].m_muted[j] = mutes[i];
			}
		}
	}
	delete dialog;
}

void NedMainWindow::transpose(GtkWidget *widget, void *data) {
	NedTransposeCommand *transpose_cmd;
	int pitchdist;
	bool state, selected;
	NedMainWindow *main_window = (NedMainWindow *) data;
	bool staff_list[main_window->m_staff_count];

	selected = main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM;
	if (selected) {
		if (g_list_length(main_window->m_selected_group) < 1) selected = false;
	}
	NedSelectorDialog *dialog = new NedSelectorDialog(GTK_WINDOW(main_window->m_main_window), true, staff_list, selected, main_window->m_staff_contexts, main_window->m_staff_count);
	dialog->getValues(&state, &pitchdist, &selected);
	delete dialog;
	if (!state) return;
	NedCommandList *command_list = new NedCommandList(main_window);
	if (main_window->m_selected_group != NULL) {
		NedChordOrRest::handleOpenTies(command_list, main_window->m_selected_group);
	}
	transpose_cmd = new NedTransposeCommand(main_window, pitchdist, main_window->m_staff_count, staff_list, selected ? main_window->m_selected_group : NULL);
	command_list->addCommand(transpose_cmd);
	transpose_cmd->execute();
	command_list->setFullReposit();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::do_transpose(int pitchdist, bool *staff_list, GList *selected_group) {
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->transpose(pitchdist, staff_list, selected_group);
	}
}

void NedMainWindow::set_all_halfs_to_sharp(GtkWidget *widget, void *data) {
	GList *lptr;
	bool state, selected;
	int dummy;
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;
	bool staff_list[main_window->m_staff_count];
	selected = main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM;
	if (selected) {
		if (g_list_length(main_window->m_selected_group) < 1) selected = false;
	}
	NedSelectorDialog *dialog = new NedSelectorDialog(GTK_WINDOW(main_window->m_main_window), false, staff_list, selected, main_window->m_staff_contexts, main_window->m_staff_count);
	dialog->getValues(&state, &dummy, &selected);
	delete dialog;
	if (!state) return;
	
	NedCommandList *command_list = new NedCommandList(main_window);

	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->changeAccidentals(command_list, 1, staff_list, selected ? main_window->m_selected_group : NULL);
	}
	command_list->setFullReposit();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::set_all_halfs_to_flat(GtkWidget *widget, void *data) {
	GList *lptr;
	bool state, selected;
	int dummy;
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;
	bool staff_list[main_window->m_staff_count];
	selected = main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM;
	if (selected) {
		if (g_list_length(main_window->m_selected_group) < 1) selected = false;
	}
	NedSelectorDialog *dialog = new NedSelectorDialog(GTK_WINDOW(main_window->m_main_window), false, staff_list, selected, main_window->m_staff_contexts, main_window->m_staff_count);
	dialog->getValues(&state, &dummy, &selected);
	delete dialog;
	if (!state) return;
	
	NedCommandList *command_list = new NedCommandList(main_window);

	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->changeAccidentals(command_list, -1, staff_list, selected ? main_window->m_selected_group : NULL);
	}
	command_list->setFullReposit();
	main_window->setAllUnpositioned();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::hide_rests(GtkWidget *widget, void *data) {
	int current_voice_nr;
	NedChordOrRest *element;
	int staff_number = -1;
	GList *lptr;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list = new NedCommandList(main_window);
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window->m_v2bu))) {
		current_voice_nr = 1;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window->m_v3bu))) {
		current_voice_nr = 2;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window->m_v4bu))) {
		current_voice_nr = 3;
	}
	else {
		current_voice_nr = 0;
	}
	if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM && 
			g_list_length(main_window->m_selected_group) > 0) {
		for (lptr = g_list_first(main_window->m_selected_group); lptr; lptr = g_list_next(lptr)) {
			element = (NedChordOrRest *) lptr->data;
			if (element->getVoice()->getVoiceNumber() != current_voice_nr) continue;
			element->hideRest(command_list, false);
		}
	}
	else {
		if (main_window->m_selected_chord_or_rest != NULL) {
			staff_number = main_window->m_selected_chord_or_rest->getStaff()->getStaffNumber();
		}
		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->hideRests(command_list, false, staff_number, current_voice_nr);
		}
	}
	main_window->setAllUnpositioned();
	command_list->setFullReposit();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}


void NedMainWindow::unhide_rests(GtkWidget *widget, void *data) {
	int current_voice_nr;
	NedChordOrRest *element;
	int staff_number = -1;
	GList *lptr;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list = new NedCommandList(main_window);
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window->m_v2bu))) {
		current_voice_nr = 1;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window->m_v3bu))) {
		current_voice_nr = 2;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(main_window->m_v4bu))) {
		current_voice_nr = 3;
	}
	else {
		current_voice_nr = 0;
	}
	if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM && 
			g_list_length(main_window->m_selected_group) > 0) {
		for (lptr = g_list_first(main_window->m_selected_group); lptr; lptr = g_list_next(lptr)) {
			element = (NedChordOrRest *) lptr->data;
			if (element->getVoice()->getVoiceNumber() != current_voice_nr) continue;
			element->hideRest(command_list, true);
		}
	}
	else {
		if (main_window->m_selected_chord_or_rest != NULL) {
			staff_number = main_window->m_selected_chord_or_rest->getStaff()->getStaffNumber();
		}
		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->hideRests(command_list, true, staff_number, current_voice_nr);
		}
	}
	main_window->setAllUnpositioned();
	command_list->setFullReposit();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::hide_rests_of_all_voices(GtkWidget *widget, void *data) {
	NedChordOrRest *element;
	GList *lptr;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list = new NedCommandList(main_window);
	if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM && 
			g_list_length(main_window->m_selected_group) > 0) {
		for (lptr = g_list_first(main_window->m_selected_group); lptr; lptr = g_list_next(lptr)) {
			element = (NedChordOrRest *) lptr->data;
			element->hideRest(command_list, false);
		}
	}
	main_window->setAllUnpositioned();
	command_list->setFullReposit();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::unhide_rests_of_all_voices(GtkWidget *widget, void *data) {
	NedChordOrRest *element;
	GList *lptr;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list = new NedCommandList(main_window);
	if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM && 
			g_list_length(main_window->m_selected_group) > 0) {
		for (lptr = g_list_first(main_window->m_selected_group); lptr; lptr = g_list_next(lptr)) {
			element = (NedChordOrRest *) lptr->data;
			element->hideRest(command_list, false);
		}
	}
	main_window->setAllUnpositioned();
	command_list->setFullReposit();
	main_window->reposit(command_list);
	main_window->m_command_history->addCommandList(command_list);
	main_window->repaint();
}

void NedMainWindow::shiftNotes(unsigned long long start_time, int linedist, NedPage *start_page, NedSystem *start_system, int staff_number) {
	GList *lptr;

	if (start_page == NULL) {
		lptr = g_list_first(m_pages);
	}
	else {
		if ((lptr = g_list_find(m_pages, start_page)) == NULL) {
			NedResource::Abort("NedMainWindow::shiftNotes");
		}
	}

	for (; lptr; lptr = g_list_next(lptr), start_system = NULL) {
		if (!((NedPage *) lptr->data)->shiftNotes(start_time, linedist, start_system, staff_number)) return;
	}
}

void NedMainWindow::computeScoreTextExtends() {
	cairo_t *cr = NULL;
	m_first_page_yoffs = 0.0;
	if (m_score_info->title != NULL) {
		m_score_info->title_extends.width = m_score_info->title->getWidth() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_score_info->title_extends.height = m_score_info->title->getHeight() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_first_page_yoffs += m_score_info->title_extends.height + SCORE_INFO_EXTRA_DIST / ((double) m_current_scale / (double) SCALE_GRANULARITY);
	}
	if (m_score_info->subject != NULL) {
		m_score_info->subject_extends.width = m_score_info->subject->getWidth() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_score_info->subject_extends.height = m_score_info->subject->getHeight() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_first_page_yoffs += m_score_info->subject_extends.height ;
	}
	if (m_score_info->composer != NULL) {
		m_score_info->composer_extends.width = m_score_info->composer->getWidth() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_score_info->composer_extends.height = m_score_info->composer->getHeight() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_first_page_yoffs += m_score_info->composer_extends.height + SCORE_INFO_EXTRA_DIST / ((double) m_current_scale / (double) SCALE_GRANULARITY);
	}
	if (m_score_info->arranger != NULL) {
		m_score_info->arranger_extends.width = m_score_info->arranger->getWidth() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_score_info->arranger_extends.height = m_score_info->arranger->getHeight() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_first_page_yoffs += m_score_info->arranger_extends.height + SCORE_INFO_EXTRA_DIST / ((double) m_current_scale / (double) SCALE_GRANULARITY);
	}
	if (m_score_info->copyright != NULL) {
		m_score_info->copyright_extends.width = m_score_info->copyright->getWidth() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_score_info->copyright_extends.height = m_score_info->copyright->getHeight() / m_current_zoom / ((double) m_current_scale / (double) SCALE_GRANULARITY);
		m_first_page_yoffs += m_score_info->copyright_extends.height + SCORE_INFO_EXTRA_DIST / ((double) m_current_scale / (double) SCALE_GRANULARITY);
	}

	if (cr != NULL) {
		cairo_destroy (cr);
		cr = NULL;
	}
}

	
void NedMainWindow::quit_app(GtkWidget  *widget, void *data) {

	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->closeApplication();
}

void NedMainWindow::do_remove_last_staff() {
	GList *lptr;

	m_staff_count--;
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->removeLastStaff();
	}
	reposit();
	repaint();
}

void NedMainWindow::insert_keysig(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	int key_signature_number;
	bool state, do_adjust;

	NedKeysigDialog *keysigdialog = new NedKeysigDialog(GTK_WINDOW(main_window->m_main_window), 0);
	keysigdialog->getValues(&state, &key_signature_number, &do_adjust);
	delete keysigdialog;
	if (state) {
		main_window->m_special_type = (TYPE_KEYSIG | (do_adjust << 24));
		main_window->m_special_sub_type.m_special_sub_type_int = key_signature_number;
		main_window->prepareInsertion();
	}
}

void NedMainWindow::insert_spacer(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	gtk_toggle_action_set_active(main_window->m_show_hidden_action, TRUE);
	main_window->m_special_type = TYPE_SPACER;
	main_window->prepareInsertion();
}

void NedMainWindow::insert_lines(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	bool state;
	int line_type;

	NedLinesDialog *linedialog = new NedLinesDialog(GTK_WINDOW(main_window->m_main_window));
	linedialog->getValues(&state, &line_type);
	delete linedialog;
	if (state) {
		main_window->prepareInsertion();
		switch (line_type) {
			case LINE_CRESCENDO: 
				main_window->m_special_type = TYPE_LINE;
				main_window->m_special_sub_type.m_special_sub_type_int = LINE_CRESCENDO;
				break;
			case LINE_DECRESCENDO: 
				main_window->m_special_type = TYPE_LINE;
				main_window->m_special_sub_type.m_special_sub_type_int = LINE_DECRESCENDO;
				break;
			case LINE_OCTAVATION1:
				main_window->m_special_type = TYPE_LINE;  main_window->m_special_sub_type.m_special_sub_type_int = LINE_OCTAVATION1; break;
			case LINE_OCTAVATION_1:
				main_window->m_special_type = TYPE_LINE;  main_window->m_special_sub_type.m_special_sub_type_int = LINE_OCTAVATION_1; break;
			case LINE_OCTAVATION2:
				main_window->m_special_type = TYPE_LINE;  main_window->m_special_sub_type.m_special_sub_type_int = LINE_OCTAVATION2;  break;
			case LINE_OCTAVATION_2:
				main_window->m_special_type = TYPE_LINE;  main_window->m_special_sub_type.m_special_sub_type_int = LINE_OCTAVATION_2; break;
		}
	}
}

void NedMainWindow::insert_signs(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	bool state;
	int sign_type;

	NedSignsDialog *signsdialog = new NedSignsDialog(GTK_WINDOW(main_window->m_main_window));
	signsdialog->getValues(&state, &sign_type);
	delete signsdialog;
	if (state) {
		main_window->prepareInsertion();
		switch (sign_type) {
			case SIGN_SEGNO: 
				main_window->m_special_type = TYPE_SIGN;
				main_window->m_special_sub_type.m_special_sub_type_int = SIGN_SEGNO;
				break;
			case SIGN_CODA: 
				main_window->m_special_type = TYPE_SIGN;
				main_window->m_special_sub_type.m_special_sub_type_int = SIGN_CODA;
				break;
			case SIGN_BREATH1: 
				main_window->m_special_type = TYPE_STAFF_SIGN;
				main_window->m_special_sub_type.m_special_sub_type_int = SIGN_BREATH1;
				break;
			case SIGN_BREATH2: 
				main_window->m_special_type = TYPE_STAFF_SIGN;
				main_window->m_special_sub_type.m_special_sub_type_int = SIGN_BREATH2;
				break;
		}
	}
}

void NedMainWindow::insert_chord(GtkWidget *widget, void *data) {
	bool state;
	int chordnum;
	unsigned int status;
	struct chord_str *chord_ptr;

	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;
	NedChordDialog *dialog = new NedChordDialog (GTK_WINDOW(main_window->m_main_window), main_window, NULL, (GUITAR_CHORD_DRAW_DIAGRAM | GUITAR_CHORD_DRAW_TEXT));

	dialog->getValues(&state, &chord_ptr, &chordnum, &status);
	if (!state) return;
	main_window->m_special_type = TYPE_GUITAR_CHORD;
	main_window->m_special_sub_type.m_chord_info.chord_ptr = chord_ptr;
	main_window->m_special_sub_type.m_chord_info.chord_name_num = chordnum;
	main_window->m_special_sub_type.m_chord_info.status = status;
	delete dialog;
	main_window->prepareInsertion();

}

void NedMainWindow::setChorNameInfo(char *rootname, char *uptext, char *downtext, double fontsize, double ydist) {
	m_special_type = TYPE_CHORDNAME;
	m_special_sub_type.m_chordname_info.root_name = rootname;
	m_special_sub_type.m_chordname_info.up_name = uptext;
	m_special_sub_type.m_chordname_info.down_name = downtext;
	m_special_sub_type.m_chordname_info.fontsize = fontsize;
	m_special_sub_type.m_chordname_info.ydist = ydist;
	prepareInsertion();
}

void NedMainWindow::insert_chord_name(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	if (NedChordNameDialog::isAlreadyActive()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_lyrics_mode >= 0) return;
	new NedChordNameDialog (main_window, GTK_WINDOW(main_window->m_main_window), NULL, NULL,NULL, -1.0, -1.0, true);
}

void NedMainWindow::insert_slur(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	GList *lptr;
	NedStaff *staff = NULL;
	NedPage *page;
	int voice_nr = 0;

	copy_data(widget, data);

	lptr = g_list_first(NedResource::m_main_clip_board);
	while (lptr) {
		if (((NedChordOrRest *) lptr->data)->getType() != TYPE_CHORD) {
			NedResource::m_main_clip_board = g_list_delete_link(NedResource::m_main_clip_board, lptr);
			lptr = g_list_first(NedResource::m_main_clip_board);
		}
		else {
			if (staff == NULL) {
				staff = ((NedChordOrRest *) lptr->data)->getStaff();
				voice_nr = ((NedChordOrRest *) lptr->data)->getVoice()->getVoiceNumber();
			}
			else if (staff != ((NedChordOrRest *) lptr->data)->getStaff() ||
				voice_nr != ((NedChordOrRest *) lptr->data)->getVoice()->getVoiceNumber()) {
				NedResource::m_main_clip_board = g_list_delete_link(NedResource::m_main_clip_board, lptr);
				lptr = g_list_first(NedResource::m_main_clip_board);
			}
			else {
				lptr = g_list_next(lptr);
			}
		}
	}

	if (main_window->m_selection_rect.width < MIN_SELECTION_RECTANGLE_DIM || main_window->m_selection_rect.height < MIN_SELECTION_RECTANGLE_DIM) {
		NedResource::Info("Please create a selection rectangle first");
		return;
	}

	page = NULL;
	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		if (((NedPage *) lptr->data)->isXOnPage(main_window->m_selection_rect.x)) {
			page = (NedPage *) lptr->data;
			break;
		}
	}

	if (page == NULL) {
		NedResource::Abort("No page found");
	}
	if (g_list_length(NedResource::m_main_clip_board) < 2) {
		g_list_free(NedResource::m_main_clip_board);
		NedResource::m_main_clip_board = NULL;
	}
	else {
		NedResource::m_main_clip_board = g_list_sort(NedResource::m_main_clip_board,
		 	(gint (*)(const void*, const void*)) NedChordOrRest::compareMidiTimes);
	}
	page->insertSlur(&(main_window->m_selection_rect));
/*
	main_window->m_special_type = TYPE_SLUR;
	main_window->m_special_sub_type.m_special_sub_type_int = 0;
	main_window->prepareInsertion();
*/
	main_window->m_selection_rect.width =  main_window->m_selection_rect.height = 0.0;

}

void NedMainWindow::insert_accelerando(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_LINE3;
	main_window->m_special_sub_type.m_special_sub_type_int = LINE_ACCELERANDO;
	main_window->prepareInsertion();
}

void NedMainWindow::insert_ritardando(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_special_type = TYPE_LINE3;
	main_window->m_special_sub_type.m_special_sub_type_int = LINE_RITARDANDO;
	main_window->prepareInsertion();
}

void NedMainWindow::append_staff(GtkWidget *widget, void *data) {
	unsigned int staff_map = 0;
	int midi_channel = -1;
	int i;
	GList *lptr;
	staff_context_str staff_context;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list;

	if (NedResource::isPlaying()) return;

	command_list = new NedCommandList(main_window);

	for (i = 0; i < main_window->m_staff_count; i++) {
		staff_map |= (1 << main_window->m_staff_contexts[i].m_midi_channel);
	}
	for (i = 0; i < 16; i++) {
		if (i == 9) continue;
		if ((staff_map & (1 << i)) == 0) {
			midi_channel = i;
			break;
		}
	}
	if (midi_channel < 0) {
		NedResource::DbgMsg(DBG_TESTING, "To many staffs: no free MIDI channel! I choose channel 1\n"); 
		midi_channel = 0;
	}
		
	staff_context.m_key_signature_number = 0;
	staff_context.m_clef_number = 0;
	staff_context.m_clef_octave_shift = 0;
	staff_context.m_different_voices = false;
	for (i = 0; i < VOICE_COUNT; i++) {
		staff_context.voices[i].m_midi_volume = 64;
		staff_context.voices[i].m_midi_pan = 64;
		staff_context.voices[i].m_midi_program = 0;
		staff_context.m_muted[i] = false;
	}
	staff_context.m_staff_name = NULL;
	staff_context.m_staff_short_name = NULL;
	staff_context.m_group_name = NULL;
	staff_context.m_group_short_name = NULL;
	staff_context.m_midi_chorus = 0;
	staff_context.m_play_transposed = 0;
	staff_context.m_midi_reverb = 0;
	staff_context.m_midi_channel = midi_channel;
	staff_context.m_flags = 0;
	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->appendStaff(command_list);
	}
	command_list->addCommand(new NedSetStaffContext(main_window, &staff_context, &(main_window->m_staff_count)));


	main_window->m_command_history->addCommandList(command_list);
	command_list->execute();
	command_list->setFullReposit();
	main_window->setAllUnpositioned();
	command_list->setRenumber(NULL, 0, true);
	main_window->reposit(command_list);
	main_window->renumberMeasures(NULL, 1, true);
	main_window->repaint();
}

void NedMainWindow::insert_tuplet(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	bool state;
	int tuplet_val;
	int method;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (!main_window->m_selected_chord_or_rest) {
		NedResource::Info(_("please select a note or rest first"));
		return;
	}
	if (main_window->m_selected_chord_or_rest->getDotCount() != 0) {
		NedResource::Info(_("you cannot select an element with dot"));
		return;
	}

	NedTupletDialog *dialog = new NedTupletDialog(GTK_WINDOW(main_window->m_main_window));
	dialog->getValues(&state, &tuplet_val, &method);
	delete dialog;
	if (!state) return;
	if (!main_window->m_selected_chord_or_rest->getVoice()->
		tryConvertToTuplet(method, tuplet_val, main_window->m_selected_chord_or_rest)) {
		NedResource::Info(_("I'm not able to create a tuplet here"));
	}
}

void NedMainWindow::remove_tuplet(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (!main_window->m_selected_chord_or_rest) {
		NedResource::Info(_("please select a note or rest first"));
		return;
	}
	if (main_window->m_selected_chord_or_rest->getTupletVal() == 0) {
		NedResource::Info(_("selected element is not inside a tuplet"));
		return;
	}
	main_window->m_selected_chord_or_rest->getVoice()->removeTuplet(main_window->m_selected_chord_or_rest);
}

void NedMainWindow::insert_clef(GtkWidget *widget, void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	int clef_type, octave_shift;
	bool state, do_adjust;

	NedClefConfigDialog *dialog = new NedClefConfigDialog (GTK_WINDOW(main_window->m_main_window), TREBLE_CLEF, 0);
	dialog->getValues(&state, &clef_type, &octave_shift, &do_adjust);
	delete dialog;
	main_window->m_special_type = (TYPE_CLEF | (do_adjust << 24));
	main_window->m_special_sub_type.m_special_sub_type_int = clef_type | ((octave_shift + 100) << 16);
	main_window->prepareInsertion();
}


void NedMainWindow::set_brace_system_delimiter(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::m_number_of_first_selected_staff < 0 ||
		NedResource::m_number_of_last_selected_staff < 0) return;
	//if (NedResource::m_number_of_last_selected_staff - NedResource::m_number_of_first_selected_staff < 1) return;


	main_window->deleteEnclosedBras(BRACE_START, BRACE_END);

	main_window->m_staff_contexts[NedResource::m_number_of_first_selected_staff].m_flags |= BRACE_START;
	main_window->m_staff_contexts[NedResource::m_number_of_last_selected_staff].m_flags |= BRACE_END;
	main_window->computeSystemIndent();
	main_window->reposit();
	main_window->repaint();

}
void NedMainWindow::set_bracket_system_delimiter(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::m_number_of_first_selected_staff < 0 ||
		NedResource::m_number_of_last_selected_staff < 0) return;
	//if (NedResource::m_number_of_last_selected_staff - NedResource::m_number_of_first_selected_staff < 1) return;

	if (main_window->deleteEnclosedBras(BRACE_START|BRACKET_START, BRACE_END|BRACKET_END)) {
		main_window->m_staff_contexts[NedResource::m_number_of_first_selected_staff].m_flags |= BRACKET_START;
		main_window->m_staff_contexts[NedResource::m_number_of_last_selected_staff].m_flags |= BRACKET_END;

		main_window->computeSystemIndent();
		main_window->reposit();
		main_window->repaint();
	}
}

void NedMainWindow::set_connected_bar_line(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::m_number_of_first_selected_staff < 0 ||
		NedResource::m_number_of_last_selected_staff < 0) return;
	if (NedResource::m_number_of_last_selected_staff - NedResource::m_number_of_first_selected_staff < 1) return;

	if (main_window->deleteEnclosedConnections()) {
		main_window->m_staff_contexts[NedResource::m_number_of_first_selected_staff].m_flags |= CON_BAR_START;
		main_window->m_staff_contexts[NedResource::m_number_of_last_selected_staff].m_flags |= CON_BAR_END;
		main_window->repaint();
	}
}

void NedMainWindow::remove_system_delimiter(GtkWidget *widget, void *data) {
	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::m_number_of_first_selected_staff < 0 ||
		NedResource::m_number_of_last_selected_staff < 0) return;

	main_window->deleteEnclosedBras(BRACE_START|BRACKET_START, BRACE_END|BRACKET_END);
	main_window->deleteEnclosedConnections();
	main_window->computeSystemIndent();
	main_window->reposit();
	main_window->repaint();
}


bool NedMainWindow::deleteEnclosedBras(unsigned int starts, unsigned int ends) {
	int i, j;

	for (i = 0; i < m_staff_count; i++) {
		if (i < NedResource::m_number_of_first_selected_staff && (m_staff_contexts[i].m_flags & starts) != 0) {
			for (j = i + 1; j < m_staff_count; j++) {
				if ((m_staff_contexts[j].m_flags & ends) != 0) {
					if (j > NedResource::m_number_of_last_selected_staff) return false;
					break;
				}
			}
		}
	}

	for (i = 0; i < m_staff_count; i++) {
		if ((m_staff_contexts[i].m_flags & starts) != 0) {
			if (i >= NedResource::m_number_of_first_selected_staff && i <= NedResource::m_number_of_last_selected_staff) {
				for (j = i + 1; j < m_staff_count; j++) {
					if ((m_staff_contexts[j].m_flags & ends) != 0) {
						m_staff_contexts[j].m_flags &= (~ends);
						break;
					}
				}
				m_staff_contexts[i].m_flags &= (~starts);
			}
		}
		if ((m_staff_contexts[i].m_flags & ends) != 0) {
			if (i >= NedResource::m_number_of_first_selected_staff && i <= NedResource::m_number_of_last_selected_staff) {
				for (j = i - 1; j >= 0; j--) {
					if ((m_staff_contexts[j].m_flags & starts) != 0) {
						m_staff_contexts[j].m_flags &= (~starts);
						break;
					}
				}
			}
		}
	}
	return true;
}

bool NedMainWindow::deleteEnclosedConnections() {
	int i, j;

	for (i = 0; i < m_staff_count; i++) {
		if (i < NedResource::m_number_of_first_selected_staff && (m_staff_contexts[i].m_flags & CON_BAR_START) != 0) {
			for (j = i + 1; j < m_staff_count; j++) {
				if ((m_staff_contexts[j].m_flags & CON_BAR_END) != 0) {
					if (j > NedResource::m_number_of_last_selected_staff) return false;
					break;
				}
			}
		}
	}

	for (i = 0; i < m_staff_count; i++) {
		if ((m_staff_contexts[i].m_flags & CON_BAR_START) != 0) {
			if (i >= NedResource::m_number_of_first_selected_staff && i <= NedResource::m_number_of_last_selected_staff) {
				for (j = i + 1; j < m_staff_count; j++) {
					if ((m_staff_contexts[j].m_flags & CON_BAR_END) != 0) {
						m_staff_contexts[j].m_flags &= (~(CON_BAR_END));
						break;
					}
				}
				m_staff_contexts[i].m_flags &= (~(CON_BAR_START));
			}
		}
		if ((m_staff_contexts[i].m_flags & CON_BAR_END) != 0) {
			if (i >= NedResource::m_number_of_first_selected_staff && i <= NedResource::m_number_of_last_selected_staff) {
				for (j = i - 1; j >= 0; j--) {
					if ((m_staff_contexts[j].m_flags & CON_BAR_START) != 0) {
						m_staff_contexts[j].m_flags &= (~(CON_BAR_START));
						break;
					}
				}
			}
		}
	}
	return true;
}


gboolean NedMainWindow::handle_delete(GtkWidget *widget, GdkEvent *event, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->closeApplication();
	return TRUE;
}

void NedMainWindow::handle_page_request(GtkSpinButton *spinbutton, gpointer data) {
	int page;
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_avoid_feedback) return;
	
	page = (int) gtk_spin_button_get_value(spinbutton);
	main_window->setToPage(page);
}

void NedMainWindow::closeApplication() {
	GtkWidget *unsaved_dialog;
	bool stop_closing, do_save;
	GList *lptr;
	NedMainWindow *main_window;


	if (NedResource::isPlaying()) return;

	for (lptr = g_list_first(NedResource::m_main_windows); lptr; lptr = g_list_next(lptr)) {
		main_window = (NedMainWindow *) lptr->data;
		stop_closing = false; do_save = false;

		if (main_window->m_config_changed || main_window->m_command_history->unsavedOperations()) {
			unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
					GTK_MESSAGE_WARNING,
					GTK_BUTTONS_YES_NO,
					_("Save Changes ?"));
			gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
			switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
				case GTK_RESPONSE_YES: do_save = TRUE; break;
				case GTK_RESPONSE_REJECT: stop_closing = TRUE; break;
			}
			gtk_widget_destroy (unsaved_dialog);	
			if (stop_closing) {
				return;
			}
			if (do_save) {
				save_score(main_window->m_main_window, main_window);
			}
		}
		if (g_list_length(NedResource::m_main_windows) == 1) {
			NedResource::cleanup();
		}
		//delete main_window;
	}
	/*
	g_list_free(NedResource::m_main_windows);
	NedResource::m_main_windows = NULL;
	*/
	gtk_main_quit();	
}


void NedMainWindow::restore_score(GtkWidget  *widget, void *data) {
	FILE *fp;
	NedMainWindow *main_window = (NedMainWindow *) data;
	DIR *the_directory;

	char pathfoldername[4096], *cptr;
	GtkWidget *open_dialog;
	GtkWidget *error_dialog;
	GtkWidget *unsaved_dialog;
	char *filename, *folder = NULL;
	bool ok = FALSE, stop_opening = FALSE, do_save = FALSE;

	if (NedResource::isPlaying()) return;

	if (main_window->m_config_changed || main_window->m_command_history->unsavedOperations()) {
		unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				_("Save Changes ?"));
		gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
			case GTK_RESPONSE_YES: do_save = TRUE; break;
			case GTK_RESPONSE_REJECT: stop_opening = TRUE; break;
		}
		gtk_widget_destroy (unsaved_dialog);	
		if (stop_opening) {
			return;
		}
		if (do_save) {
			main_window->save_score(widget, data);
		}
	}

	open_dialog = gtk_file_chooser_dialog_new (_("Open score"),
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.ntd (Nted-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.ntd");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_dialog), file_filter2);


	if (NedResource::m_last_folder != NULL) {
		if ((the_directory = opendir(NedResource::m_last_folder)) != NULL) { // avoid gtk error message
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(open_dialog), NedResource::m_last_folder);
			closedir(the_directory);
		}
		else {
			NedResource::m_last_folder = NULL;
		}
	}

	if (gtk_dialog_run (GTK_DIALOG (open_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (open_dialog));
		folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (open_dialog));
		ok = TRUE;
	}
	gtk_widget_destroy (open_dialog);
	if (ok) {
		if ((fp = fopen(filename, "r")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for reading"), filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
		}
	}
	if (ok) {
		main_window->do_restore(fp, filename);
		if ((cptr = strrchr(filename, '/')) != NULL) {
			strncpy(pathfoldername, filename, cptr - filename);
			pathfoldername[cptr - filename] = '\0';
			NedResource::m_last_folder = strdup(pathfoldername);
		}
		else {
			NedResource::m_last_folder = strdup(folder);
		}
		g_free (filename);
		fclose(fp);
	}
	if (folder != NULL) {
		g_free(folder);
	}
}


void NedMainWindow::do_restore(FILE *fp, char *filename) {
	int i, j;
	int score_info_idx;
	int midi_channel = 0;
	unsigned int upbeat_inverse = 0;
	int pan, chorus, reverb;
	int pagenum;
	int tsig_sym;
	NedPangoCairoText **sc_ptr;
	NedPage *page;
	GList *lptr, *newpages = NULL;
	int num, denom, tempo_inverse;
	char the_filename[4096], *cptr, show_file_name[4096];
	int play_transposed;
	staff_context_str staff_contexts[MAX_STAFFS];
	unsigned int flags;
	NedResource::m_error_message = NULL;
	NedResource::m_input_line = 1;
	char buffer[128];
	ScoreInfo *newscoreinfo = new ScoreInfo();
	NedResource::prepareAddrStruct();
	NedResource::m_avoid_immadiate_play = TRUE;
	GList *new_spec_measures = NULL;
	if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "NTED")) {
		NedResource::m_error_message = "NTED expected";
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "version")) {
			NedResource::m_error_message = "version expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readInt(fp, &m_file_version) || m_file_version < 1 || m_file_version > 24) {
			NedResource::m_error_message = "version number expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "HEAD")) {
			NedResource::m_error_message = "HEAD expected";
		}
	}
	while(NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "MEDIA, SUBJECT, COMPOSER, ARRANGER, COPYRIGHT OR MEDIA expected";
		}
		if (!strcmp(buffer, "MEDIA")) {
			NedResource::unreadWord(buffer);
			break;
		}
		else if (!strcmp(buffer, "TITLE")) {
			sc_ptr = &(newscoreinfo->title);
			score_info_idx = 1;
		}
		else if (!strcmp(buffer, "SUBJECT")){ 
			sc_ptr = &(newscoreinfo->subject);
			score_info_idx = 2;
		}
		else if (!strcmp(buffer, "COMPOSER")){ 
			sc_ptr = &(newscoreinfo->composer);
			score_info_idx = 3;
		}
		else if (!strcmp(buffer, "ARRANGER")) {
			sc_ptr = &(newscoreinfo->arranger);
			score_info_idx = 4;
		}
		else if (!strcmp(buffer, "COPYRIGHT")) {
			sc_ptr = &(newscoreinfo->copyright);
			score_info_idx = 5;
		}
		else {
			NedResource::m_error_message = "MEDIA, SUBJECT, COMPOSER, ARRANGER, COPYRIGHT OR MEDIA expected";
			break;
		}
		if (*sc_ptr != NULL) {
			NedResource::m_error_message = "double information";
			break;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
			NedResource::m_error_message = ": expected";
			break;
		}
		if (!NedResource::readTillEnd(fp, buffer)) { 
			NedResource::m_error_message = "value expected";
			break;
		}
		switch (score_info_idx) {
			case 1:
			newscoreinfo->title = new NedPangoCairoText(m_drawing_area->window, 
				buffer, TITLE_FONT, TITLE_FONT_SLANT, TITLE_FONT_WEIGHT, TITLE_FONT_SIZE, m_current_zoom, 1.0, false);
				break;
			case 2:
			newscoreinfo->subject = new NedPangoCairoText(m_drawing_area->window, 
				buffer, SUBJECT_FONT, SUBJECT_FONT_SLANT, SUBJECT_FONT_WEIGHT, SUBJECT_FONT_SIZE, m_current_zoom, 1.0, false);
				break;
			case 3:
			newscoreinfo->composer = new NedPangoCairoText(m_drawing_area->window, 
				buffer, COMPOSER_FONT, COMPOSER_FONT_SLANT, COMPOSER_FONT_WEIGHT, COMPOSER_FONT_SIZE, m_current_zoom, 1.0, false);
				break;
			case 4:
			newscoreinfo->arranger = new NedPangoCairoText(m_drawing_area->window, 
				buffer, ARRANGER_FONT, ARRANGER_FONT_SLANT, ARRANGER_FONT_WEIGHT, ARRANGER_FONT_SIZE, m_current_zoom, 1.0, false);
				break;
			case 5:
			newscoreinfo->copyright = new NedPangoCairoText(m_drawing_area->window, 
				buffer, STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT, STAFF_NAME_FONT_WEIGHT, STAFF_NAME_FONT_SIZE, m_current_zoom, 1.0, false);
				break;
		}
			
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "MEDIA")) {
			NedResource::m_error_message = "MEDIA expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readString(fp, buffer)) {
			NedResource::m_error_message = "MEDIA value expected";
		}
		if ((m_current_paper = NedResource::getPaperInfo(buffer)) == NULL) {
			NedResource::m_error_message = "wrong media";
			NedResource::DbgMsg(DBG_CRITICAL, "buffer = %s\n", buffer);
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "ORIENTATION")) {
			NedResource::m_error_message = "ORIENTATION expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "ORIENTATION value expected";
		}
		if (!strcmp(buffer, "Portrait")) {
			m_portrait = TRUE;
		}
		else if (!strcmp(buffer, "Landscape")) {
			m_portrait = FALSE;
		}
		else {
			NedResource::m_error_message = "Wrong orientation value";
		}
	}
	if (m_file_version > 9) {
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "SPACEMENT")) {
				NedResource::m_error_message = "SPACEMENT expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readFloat(fp, &m_global_spacement_fac) || m_global_spacement_fac < MEASURE_MIN_SPREAD  || m_global_spacement_fac > MEASURE_MAX_SPREAD)  {
				NedResource::m_error_message = "SPACEMENT value expected";
			}
		}
	}
	if (m_file_version > 13) {
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "SCALE")) {
				NedResource::m_error_message = "SCALE expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &m_current_scale) || m_current_scale < (int) (SCALE_MIN * (double) SCALE_GRANULARITY - 2.0)  ||
				m_current_scale > (int) (SCALE_MAX * (double) SCALE_GRANULARITY + 2.0))  {
				NedResource::m_error_message = "SCALE value expected";
			}
			if (m_current_scale > (int) (SCALE_MAX * (double) SCALE_GRANULARITY + 2.0)) {
				m_current_scale = (int) (SCALE_MAX * (double) SCALE_GRANULARITY + 2.0);
			}
			if (m_current_scale < (int) (SCALE_MIN * (double) SCALE_GRANULARITY - 2.0)) {
				m_current_scale = (int) (SCALE_MIN * (double) SCALE_GRANULARITY - 2.0);
			}
		}
	}
	else {
		m_current_scale = SCALE_GRANULARITY;
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "NUMERATOR")) {
			NedResource::m_error_message = "NUMERATOR expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readInt(fp, &num) || num > 20 || num < 0) {
			NedResource::m_error_message = "NUMERATOR value expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "DENOMINATOR")) {
			NedResource::m_error_message = "DENOMINATOR expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readInt(fp, &denom) || (denom != 2 && denom != 4 && denom != 8 && denom != 16)) {
			NedResource::m_error_message = "DENOMINATOR value expected";
		}
	}
	tsig_sym = TIME_SYMBOL_NONE;
	if (m_file_version > 16) {
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "TIMESIG_SYMBOL")) {
				NedResource::m_error_message = "TIMESIG_SYMBOL expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &tsig_sym) || ( tsig_sym < TIME_SYMBOL_NONE || tsig_sym > TIME_SYMBOL_CUT_TIME)) {
				NedResource::m_error_message = "TIMSIG_SYMBOL value expected";
			}
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "TEMPOINVERSE")) {
			NedResource::m_error_message = "TEMPO_INVERSE expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readInt(fp, &tempo_inverse) || tempo_inverse > 2900 || tempo_inverse < 200) {
			NedResource::m_error_message = "TEMPO INVERSE VALUE value expected";
		}
	}
	if (m_file_version > 2) {
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "UPBEAT_INVERSE")) {
				NedResource::m_error_message = "UPBEAT_INVERSE expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readUnsignedInt(fp, &upbeat_inverse)) {
				NedResource::m_error_message = "UPBEAT_INVERSE VALUE value expected";
			}
		}
	}
	if (m_file_version > 18) {
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "MEASURE_NUMBERS")) {
				NedResource::m_error_message = "MEASURE_NUMBERS expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || (strcmp(buffer, "On") && strcmp(buffer, "Off"))) {
				NedResource::m_error_message = "On or Off expected";
			}
			if (!strcmp(buffer, "On")) {
				gtk_toggle_action_set_active(m_show_measure_numbers_action, TRUE);
			}
			else {
				gtk_toggle_action_set_active(m_show_measure_numbers_action, FALSE);
			}
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "STAFFS")) {
			NedResource::m_error_message = "STAFFS expected";
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readInt(fp, & m_pre_staffcount) ||  m_pre_staffcount > MAX_STAFFS ||  m_pre_staffcount < 1) {
			NedResource::m_error_message = "STAFF COUNT value expected";
		}
	}	

	for (i = 0; i < MAX_STAFFS; i++) {
		staff_contexts[i].m_staff_name = NULL;
		staff_contexts[i].m_staff_short_name = NULL;
		staff_contexts[i].m_group_name = NULL;
		staff_contexts[i].m_group_short_name = NULL;
	}
	for (i = 0; NedResource::m_error_message == NULL && i < m_pre_staffcount; i++) {
		int key, staffnum, channel, vol, midi_pgm;
		staff_contexts[i].m_midi_channel = midi_channel++;
		for (j = 0; j < VOICE_COUNT; j++) {
			staff_contexts[i].voices[j].m_midi_program = 0;
			staff_contexts[i].voices[j].m_midi_pan = 64;
			staff_contexts[i].voices[j].m_midi_volume = 64;
			staff_contexts[i].m_muted[j] = false;
		}
		staff_contexts[i].m_flags = 0;
		if (staff_contexts[i].m_staff_name != NULL) {
			delete staff_contexts[i].m_staff_name;
		}
		staff_contexts[i].m_staff_short_name = NULL;
		if (staff_contexts[i].m_staff_short_name != NULL) {
			 delete staff_contexts[i].m_staff_short_name;
		}
		staff_contexts[i].m_staff_short_name = NULL;
		if (staff_contexts[i].m_group_name != NULL) {
			delete staff_contexts[i].m_group_name;
		}
		staff_contexts[i].m_group_name = NULL;
		if (staff_contexts[i].m_group_short_name != NULL) {
			 delete staff_contexts[i].m_group_short_name;
		}
		staff_contexts[i].m_group_short_name = NULL;
		staff_contexts[i].m_midi_chorus = 0;
		staff_contexts[i].m_play_transposed = 0;
		staff_contexts[i].m_midi_reverb = 0;
	
		if (midi_channel == 9) {
			midi_channel++;
		}
		if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "STAFF")) {
			NedResource::m_error_message = "STAFF expected(1)";
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &staffnum) || staffnum != i) {
				NedResource::m_error_message = "STAFF_NUM value expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": (1)expected";
			}
		}
		if (m_file_version > 1) {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "NAME")) {
					NedResource::m_error_message = "NAME expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (2)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWordOfLength(fp, buffer)) {
					NedResource::m_error_message = "STAFF NAME value expected";
				}
				else {
					if (buffer[0] != '\0') {
						if (staff_contexts[i].m_staff_name != NULL) {
							delete staff_contexts[i].m_staff_name;
						}
						staff_contexts[i].m_staff_name = new NedPangoCairoText(m_drawing_area->window, buffer,
							STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT, STAFF_NAME_FONT_WEIGHT,
								STAFF_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
					}
				}
			}
		}
		if (m_file_version > 10) {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "NAME_SHORT_NAME")) {
					NedResource::m_error_message = "NAME_SHORT_NAME expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (2)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWordOfLength(fp, buffer)) {
					NedResource::m_error_message = "STAFF SHORT NAME value expected";
				}
				else {
					if (buffer[0] != '\0') {
						if (staff_contexts[i].m_staff_short_name != NULL) {
							delete staff_contexts[i].m_staff_short_name;
						}
						staff_contexts[i].m_staff_short_name = new NedPangoCairoText(m_drawing_area->window, buffer,
							STAFF_SHORT_NAME_FONT, STAFF_SHORT_NAME_FONT_SLANT, STAFF_SHORT_NAME_FONT_WEIGHT,
								STAFF_SHORT_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
					}
				}
			}
		}
		if (m_file_version > 19) {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "GROUP_NAME")) {
					NedResource::m_error_message = "GROUP_NAME expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (8)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWordOfLength(fp, buffer)) {
					NedResource::m_error_message = "GROUP_NAME value expected";
				}
				else {
					if (buffer[0] != '\0') {
						if (staff_contexts[i].m_group_name != NULL) {
							delete staff_contexts[i].m_group_name;
						}
						staff_contexts[i].m_group_name = new NedPangoCairoText(m_drawing_area->window, buffer,
							STAFF_SHORT_NAME_FONT, STAFF_SHORT_NAME_FONT_SLANT, STAFF_SHORT_NAME_FONT_WEIGHT,
								STAFF_SHORT_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
					}
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "GROUP_SHORT_NAME")) {
					NedResource::m_error_message = "GROUP_SHORT_NAME expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (9)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWordOfLength(fp, buffer)) {
					NedResource::m_error_message = "GROUP_SHORT_NAME value expected";
				}
				else {
					if (buffer[0] != '\0') {
						if (staff_contexts[i].m_group_short_name != NULL) {
							delete staff_contexts[i].m_group_short_name;
						}
						staff_contexts[i].m_group_short_name = new NedPangoCairoText(m_drawing_area->window, buffer,
							STAFF_SHORT_NAME_FONT, STAFF_SHORT_NAME_FONT_SLANT, STAFF_SHORT_NAME_FONT_WEIGHT,
								STAFF_SHORT_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
					}
				}
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "KEY")) {
				NedResource::DbgMsg(DBG_CRITICAL, "gelesen: %s\n", buffer); 
				NedResource::m_error_message = "KEY expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": (2)expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &key) || key < -6 || key > 6) {
				NedResource::m_error_message = "KEY value expected";
			}
			else {
				staff_contexts[i].m_key_signature_number = key;
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "CLEF")) {
				NedResource::DbgMsg(DBG_CRITICAL, "GOT: *%s*\n", buffer);
				NedResource::m_error_message = "CLEF expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = "(3): expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWordWithNum(fp, buffer)) {
				NedResource::m_error_message = "CLEF value expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!strcmp(buffer, "TREBLE")) {
				staff_contexts[i].m_clef_number = TREBLE_CLEF;
			}
			else if (!strcmp(buffer, "BASS")) {
				staff_contexts[i].m_clef_number = BASS_CLEF;
			}
			else if (!strcmp(buffer, "ALTO")) {
				staff_contexts[i].m_clef_number = ALTO_CLEF;
			}
			else if (!strcmp(buffer, "TENOR")) {
				staff_contexts[i].m_clef_number = TENOR_CLEF;
			}
			else if (!strcmp(buffer, "SOPRAN")) {
				staff_contexts[i].m_clef_number = SOPRAN_CLEF;
			}
			else if (!strcmp(buffer, "NEUTRAL1")) {
				staff_contexts[i].m_clef_number = NEUTRAL_CLEF1;
			}
			else if (!strcmp(buffer, "NEUTRAL2")) {
				staff_contexts[i].m_clef_number = NEUTRAL_CLEF2;
			}
			else if (!strcmp(buffer, "NEUTRAL3")) {
				staff_contexts[i].m_clef_number = NEUTRAL_CLEF3;
			}
			else {
				NedResource::m_error_message = "CLEF value expected";
			}
		}
		staff_contexts[i].m_clef_octave_shift = 0;
		if (m_file_version > 3) {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readInt(fp, &(staff_contexts[i].m_clef_octave_shift)) ||
					(staff_contexts[i].m_clef_octave_shift != -12 && staff_contexts[i].m_clef_octave_shift != 0 && staff_contexts[i].m_clef_octave_shift != 12)) {
					NedResource::m_error_message = "Octve shift expected";
				}
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "CHANNEL")) {
				NedResource::m_error_message = "CHANNEL expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": (2)expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &channel) || channel < 0 || channel > 15) {
				NedResource::m_error_message = "CHANNEL number expected";
			}
			else {
				staff_contexts[i].m_midi_channel = channel;
			}
		}
		staff_contexts[i].m_different_voices = false;
		if (m_file_version > 23) {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "DIFFERENT")) {
					NedResource::m_error_message = "DIFFERENT expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (2)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer)) {
					NedResource::m_error_message = "On/Off expected";
				}
				if (!strcmp(buffer, "On")) {
					staff_contexts[i].m_different_voices = true;
				}
				else if (!strcmp(buffer, "Off")) {
					staff_contexts[i].m_different_voices = false;
				}
				else {
					NedResource::m_error_message = "On/Off(2) expected";
				}
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "VOL")) {
				NedResource::m_error_message = "VOL expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": (2)expected";
			}
		}
		if (m_file_version > 23) {
			for (j = 0; j < VOICE_COUNT; j++) {
				if (NedResource::m_error_message == NULL) {
					if (!NedResource::readInt(fp, &vol) || vol < 0 || channel > 127) {
						NedResource::m_error_message = "volume valueexpected";
					}
					else {
						staff_contexts[i].voices[j].m_midi_volume = vol;
					}
				}
			}
		}
		else {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readInt(fp, &vol) || vol < 0 || channel > 127) {
					NedResource::m_error_message = "volume valueexpected";
				}
				else {
					for (j = 0; j < VOICE_COUNT; j++) {
						staff_contexts[i].voices[j].m_midi_volume = vol;
					}
				}
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "MIDIPGM")) {
				NedResource::m_error_message = "MIDIPGM expected";
			}
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
				NedResource::m_error_message = ": (2)expected";
			}
		}
		if (m_file_version > 23) {
			if (NedResource::m_error_message == NULL) {
				for (j = 0; j < VOICE_COUNT; j++) {
					if (!NedResource::readInt(fp, &midi_pgm) || midi_pgm < 0 || midi_pgm >= NedResource::getNumInstruments()) {
						NedResource::m_error_message = "PROGRAM number expected";
					}
					else {
						staff_contexts[i].voices[j].m_midi_program = midi_pgm;
					}
				}
			}
		}
		else {
			if (!NedResource::readInt(fp, &midi_pgm) || midi_pgm < 0 || midi_pgm >= NedResource::getNumInstruments()) {
				NedResource::m_error_message = "PROGRAM number expected";
			}
			else {
				for (j = 0; j < VOICE_COUNT; j++) {
					staff_contexts[i].voices[j].m_midi_program = midi_pgm;
				}
			}
		}
		if (m_file_version > 1) {
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "PAN")) {
					NedResource::m_error_message = "PAN expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (3)expected";
				}
			}
			if (m_file_version > 23) {
				for (j = 0; j < VOICE_COUNT; j++) {
					if (NedResource::m_error_message == NULL) {
						if (!NedResource::readInt(fp, (int *) &pan)) {
							NedResource::m_error_message = "pan value expected";
						}
						else {
							staff_contexts[i].voices[j].m_midi_pan = pan;
						}
					}
				}
			}
			else {
				if (NedResource::m_error_message == NULL) {
					if (!NedResource::readInt(fp, (int *) &pan)) {
						NedResource::m_error_message = "pan value expected";
					}
					else {
						for (j = 0; j < VOICE_COUNT; j++) {
							staff_contexts[i].voices[j].m_midi_pan = pan;
						}
					}
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "CHORUS")) {
					NedResource::m_error_message = "CHORUS expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (4)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readInt(fp, (int *) &chorus)) {
					NedResource::m_error_message = "chorus value expected";
				}
				else {
					staff_contexts[i].m_midi_chorus = chorus;
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "REVERB")) {
					NedResource::m_error_message = "REVERB expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (5)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readInt(fp, (int *) &reverb)) {
					NedResource::m_error_message = "reverb value expected";
				}
				else {
					staff_contexts[i].m_midi_reverb = reverb;
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "FLAGS")) {
					NedResource::m_error_message = "FLAGS expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
					NedResource::m_error_message = ": (6)expected";
				}
			}
			if (NedResource::m_error_message == NULL) {
				if (!NedResource::readHex(fp, (int *) &flags)) {
					NedResource::m_error_message = "flags value expected";
				}
				else {
					staff_contexts[i].m_flags = flags;
				}
			}
			if (m_file_version > 7 ) {
				if (NedResource::m_error_message == NULL) {
					if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "PLAY_TRANSPOSED")) {
						NedResource::m_error_message = "PLAY_TRANSPOSED expected";
					}
				}
				if (NedResource::m_error_message == NULL) {
					if (!NedResource::readWord(fp, buffer) || strcmp(buffer, ":")) {
						NedResource::m_error_message = ": (7)expected";
					}
				}
				if (NedResource::m_error_message == NULL) {
					if (!NedResource::readInt(fp, &play_transposed) || play_transposed < -24 || play_transposed > 24) {
						NedResource::m_error_message = "play transposed value expected";
					}
					else {
						staff_contexts[i].m_play_transposed = play_transposed;
					}
				}
			}
		}
	}
	if (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "MUSIC or SPEC_MEASURES expected";
		}
		if (NedResource::m_error_message == NULL) {
			if (!strcmp(buffer, "SPEC_MEASURES")) {
				readSpecMeasures(fp, &new_spec_measures);
			}
			else if (strcmp(buffer, "MUSIC")) {
				NedResource::m_error_message = "MUSIC expected";
			}
		}
	}
	i = 0;
	while (NedResource::m_error_message == NULL) {
		if (!NedResource::readWord(fp, buffer)) {
			if (feof(fp)) {
				break;
			}
		}
		if (strcmp(buffer, "PAGE")) {
			NedResource::m_error_message = "PAGE expected";
		}
		if (NedResource::m_error_message == NULL) {
			if (!NedResource::readInt(fp, &pagenum) || pagenum != i) {
				NedResource::m_error_message = "Pagenum expected expected";
			}
			i++;
			double w = m_portrait ? m_current_paper->width : m_current_paper->height;
			double h = m_portrait ? m_current_paper->height : m_current_paper->width;
			w /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
			h /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
			page = new NedPage(this, w, h, pagenum, 0 /* dummy is reset during "renumberMeasures()" at the end of "reposit()" */, FALSE);
			newpages = g_list_append(newpages, page);
			page->restorePage(fp);

		}
	}

	if (NedResource::m_error_message == NULL) {
		NedResource::DbgMsg(DBG_TESTING, "%s successfully loaded\n", filename);
		m_numerator = num;
		m_denominator = denom;
		m_timesig_symbol = tsig_sym;
		m_staff_count = m_pre_staffcount;
		for (i = 0; i < m_staff_count; i++) {
			if (m_staff_contexts[i].m_staff_name != NULL) {
				delete m_staff_contexts[i].m_staff_name;
			}
			if (m_staff_contexts[i].m_staff_short_name != NULL) {
				delete m_staff_contexts[i].m_staff_short_name;
			}
			if (m_staff_contexts[i].m_group_name != NULL) {
				delete m_staff_contexts[i].m_group_name;
			}
			if (m_staff_contexts[i].m_group_short_name != NULL) {
				delete m_staff_contexts[i].m_group_short_name;
			}
			m_staff_contexts[i] = staff_contexts[i];
		}
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			delete ((NedPage *) lptr->data);
		}
		g_list_free(m_pages);
		m_pages = newpages;
		for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
			free(lptr->data);
		}
		g_list_free(m_special_measures);
		m_special_measures = new_spec_measures;
		m_command_history->reset();
		m_selected_note = NULL;
		m_upbeat_inverse = upbeat_inverse;
		m_selected_chord_or_rest = NULL;
		m_selected_tie_forward = NULL;
		m_selected_spec_measure = NULL;
		resetLyricsMode();
		m_midi_tempo_inverse = tempo_inverse;
		m_config_changed = FALSE;
		resetButtons();
		m_selection_rect.width = 0;
		if (m_selected_group != NULL) {
			g_list_free(m_selected_group);
			m_selected_group = NULL;
		}
		m_selection_rect.width = m_selection_rect.height = 0;
		delete m_score_info;
		m_score_info = newscoreinfo;
		computeSystemIndent();
		computeScoreTextExtends();
		m_topy = m_leftx = 0;
		setAndUpdateClefTypeAndKeySig();
		reposit();
		updatePageCounter();
		zoomFreeReplaceables(m_current_zoom, getCurrentScale());
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), 1);
		strcpy(m_current_filename, filename);
		strcpy(the_filename, filename);
		if ((cptr = strrchr(the_filename, '/')) != NULL) {
			cptr++;
		}
		else {
			cptr = the_filename;
		}
		sprintf(show_file_name, "Nted - %s", cptr);
		gtk_window_set_title (GTK_WINDOW (m_main_window), show_file_name);
		NedResource::addToRecentFiles(filename);
		updateRecentFiles();
		setAllUnpositioned();
		NedCommandList *cm = new NedCommandList(this);
		reposit(cm, NULL, NULL, true);
		deleteEmptyPagesAtEnd();
		setAllUnpositioned();
		updatePageCounter();
		repaint();
	}
	else {
		delete newscoreinfo;
		NedResource::DbgMsg(DBG_CRITICAL, "Error loading %s: line %d: %s\n", filename, NedResource::m_input_line, NedResource::m_error_message);
	}
	NedResource::m_avoid_immadiate_play = FALSE;
}

void NedMainWindow::import_musicxml(GtkWidget  *widget, void *data) {
	DIR *the_directory;
	char *filename = NULL, *folder = NULL;
	GtkWidget *import_dialog;
	GtkWidget *unsaved_dialog;
	bool stop_opening = FALSE, do_save = FALSE;
	bool ok=false;

	if (NedResource::isPlaying()) return; 
	NedMusicXMLImport musicxmlimport;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (main_window->m_config_changed || main_window->m_command_history->unsavedOperations()) {
		unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				_("Save Changes ?"));
		gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
			case GTK_RESPONSE_YES: do_save = TRUE; break;
			case GTK_RESPONSE_REJECT: stop_opening = TRUE; break;
		}
		gtk_widget_destroy (unsaved_dialog);	
		if (stop_opening) {
			return;
		}
		if (do_save) {
			main_window->save_score(main_window->m_main_window, (void *) main_window);
		}
	}

	import_dialog = gtk_file_chooser_dialog_new (_("Import MusicXML"),
			      GTK_WINDOW(main_window->m_main_window),
			      GTK_FILE_CHOOSER_ACTION_OPEN,
			      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			      NULL);
	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, ".xml (XML-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.xml");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(import_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(import_dialog), file_filter2);


	if (NedResource::m_last_xml_dir != NULL) {
		if ((the_directory = opendir(NedResource::m_last_xml_dir)) != NULL) { // avoid gtk error message
			gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(import_dialog), NedResource::m_last_xml_dir);
			closedir(the_directory);
		}
	}

	if (gtk_dialog_run (GTK_DIALOG (import_dialog)) == GTK_RESPONSE_ACCEPT) {
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (import_dialog));
		folder = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER (import_dialog));
		ok = TRUE;
	}
	gtk_widget_destroy (import_dialog);
	if (!ok) return;
	if (filename == NULL) return;
	if (strlen(filename) < 1) return;
	if (NedResource::m_last_xml_dir != NULL) {
		free(NedResource::m_last_xml_dir);
		NedResource::m_last_xml_dir = NULL;
	}
	if (folder != NULL) {
		if (strlen(folder) > 0) {
			NedResource::m_last_xml_dir = strdup(folder);
		}
	}

	main_window->import_from_other(&musicxmlimport, filename, true);
}


void NedMainWindow::import_midi(GtkWidget  *widget, void *data) {
	if (NedResource::isPlaying()) return; 
	NedMainWindow *main_window = (NedMainWindow *) data;

	main_window->do_import_midi(false);
}

void NedMainWindow::import_midi2(GtkWidget  *widget, void *data) {
	if (NedResource::isPlaying()) return; 
	NedMainWindow *main_window = (NedMainWindow *) data;

	main_window->do_import_midi(true);
}

void NedMainWindow::do_import_midi(bool use_id_ed) {
	bool state, with_triplets;
	GtkWidget *unsaved_dialog;
	bool stop_opening = FALSE, do_save = FALSE;
	bool force_piano, sort_instruments, dont_split;
	int tempo_change_density, volume_change_density;
	char *filename = NULL;

	if (m_config_changed || m_command_history->unsavedOperations()) {
		unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				_("Save Changes ?"));
		gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
			case GTK_RESPONSE_YES: do_save = TRUE; break;
			case GTK_RESPONSE_REJECT: stop_opening = TRUE; break;
		}
		gtk_widget_destroy (unsaved_dialog);	
		if (stop_opening) {
			return;
		}
		if (do_save) {
			save_score(m_main_window, (void *) this);
		}
	}

	NedMidiImportDialog *dialog = new NedMidiImportDialog (GTK_WINDOW(m_main_window), &NedResource::m_last_midi_dir, &filename);
	dialog->getValues(&state, &with_triplets,  &tempo_change_density, &volume_change_density, &force_piano, &sort_instruments, &dont_split);
	delete dialog;
	if (!state) return;
	if (filename == NULL) return;
	if (strlen(filename) < 1) return;
	NedMidiImporter *midiimport = new NedMidiImporter(with_triplets, tempo_change_density, volume_change_density,
			force_piano, sort_instruments, dont_split, use_id_ed);

	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "Cannot open %s\n", filename);
		return;
	}
	midiimport->setImportParameters(this, fp);

	if (use_id_ed) {
		if (!midiimport->import()) {
			if (midiimport->getErrorString()) {
				NedResource::Info(midiimport->getErrorString());
			}
		}
	}
	else {
		import_from_other(midiimport, filename, true);
	}
	fclose(fp);
	if (!use_id_ed) {
		delete midiimport;
	}
}

void NedMainWindow::deleteEmptyPagesAtEnd() {
	GList *lptr;
	bool deleted;

	if (g_list_length(m_pages) < 2) return;

	do {
		deleted = false;
		lptr = g_list_last(m_pages);
		if (((NedPage *) lptr->data)->hasOnlyRests()) {
			deleted = true;
			m_pages = g_list_delete_link(m_pages, lptr);
		}
	}
	while (deleted && g_list_length(m_pages) > 0);
}

void NedMainWindow::deletePagesWithoutSystem() {
	GList *lptr;
	bool deleted;

	do {
		deleted = false;
		lptr = g_list_last(m_pages);
		if (((NedPage *) lptr->data)->getSystemCount() < 1) {
			deleted = true;
			m_pages = g_list_delete_link(m_pages, lptr);
		}
		if (g_list_length(m_pages) < 1) {
			NedResource::Abort("NedMainWindow::deletePagesWithoutSystem");
		}
	}
	while (deleted);
}
	

bool NedMainWindow::import_from_other(NedImporter *im, char *filename, bool execute_import) {
	FILE *fp = NULL;

	GtkWidget *error_dialog;
#ifdef ASSIGN1
	char pathfoldername[4096], *cptr;
#endif
	int i;
	GList *lptr;
	GList *old_spec_measures;
	GList *old_pages;
	bool old_portrait;
	double old_tempo_inverse;
	double old_spacement_fac;
	bool old_config_changed;
	int old_numerator;
	int old_denominator;
	unsigned int old_timesig_symbol;
	int old_staff_count;
	int old_scale;
	struct paper_info_struct *old_paper_type;

	unsigned int old_upbeat_inverse;
	staff_context_str old_staff_contexts[MAX_STAFFS];
	double old_first_page_yoffs;
	ScoreInfo *old_score_info;
	bool state, portrait;
	const char *paper;
	bool ok = true;

	for (i = 0; i < MAX_STAFFS; i++) {
		old_staff_contexts[i].m_staff_name = NULL;
		old_staff_contexts[i].m_staff_short_name = NULL;
		old_staff_contexts[i].m_group_name = NULL;
		old_staff_contexts[i].m_group_short_name = NULL;
	}

	if (filename != NULL) {
		if ((fp = fopen(filename, "r")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for reading"), filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
		}
	}
	if (ok) {
		im->setImportParameters(this, fp);
		old_spec_measures = m_special_measures;
		m_special_measures = NULL;
		old_pages = m_pages;
		old_portrait = m_portrait;
		old_paper_type = m_current_paper;
		old_tempo_inverse = m_midi_tempo_inverse;
		old_numerator = m_numerator;
		old_scale = m_current_scale;
		old_denominator = m_denominator;
		old_timesig_symbol = m_timesig_symbol;
		old_staff_count = m_staff_count;
		old_upbeat_inverse = m_upbeat_inverse;
		old_spacement_fac = m_global_spacement_fac;
		old_config_changed = m_config_changed;
		for (i = 0; i < old_staff_count; i++) {
			old_staff_contexts[i] = m_staff_contexts[i];
		}
		old_first_page_yoffs = m_first_page_yoffs;
		old_score_info = m_score_info;

		m_staff_count = 1;
		m_current_scale = SCALE_GRANULARITY;
		m_selected_note = NULL;
		m_selected_chord_or_rest = NULL;
		m_selected_tie_forward = NULL;
		m_selected_spec_measure = NULL;
		m_config_changed = TRUE;
		m_selection_rect.width = 0;
		resetLyricsMode();
		m_midi_tempo_inverse = START_TEMPO_INVERSE;

		if (m_selected_group != NULL) {
			g_list_free(m_selected_group);
			m_selected_group = NULL;
		}
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_v1bu), TRUE);
		gtk_toggle_action_set_active(m_show_hidden_action, FALSE);
		m_score_info = new ScoreInfo();
		computeScoreTextExtends();
		resetButtons();
		ok = true;
		if (execute_import) {
			ok = im->import();
		}
		ok = ok && im->convertToScore();
		if (ok) {
			NedPaperConfigDialog *dialog = new NedPaperConfigDialog (GTK_WINDOW(m_main_window),
						m_current_paper->name, m_portrait, true);
			dialog->getValues(&state, &paper, &portrait);
			delete dialog;
			ok = state;
		}
		if (ok) {
			struct paper_info_struct *paper_type = NedResource::getPaperInfo(paper);
			m_current_paper = paper_type;
			m_portrait = portrait;
			if (do_import_from_other(im)) {
#ifdef ASSIGN1
				if (filename != NULL) {
					if ((cptr = strrchr(filename, '/')) != NULL) {
						strncpy(pathfoldername, filename, cptr - filename);
						pathfoldername[cptr - filename] = '\0';
						*last_dir = strdup(pathfoldername);
					}
					else {
						*last_dir = strdup(folder);
					}
				}
#endif
#define FREE1
#ifdef FREE1
				if (filename != NULL) {
					g_free(filename);
				}
				for (lptr = g_list_first(old_pages); lptr; lptr = g_list_next(lptr)) {
					delete ((NedPage *) lptr->data);
				}
				g_list_free(old_pages);
				if (old_spec_measures != NULL) {
					for (lptr = g_list_first(old_spec_measures); lptr; lptr = g_list_next(lptr)) {
						delete ((SpecialMeasure *) lptr->data);
					}
					g_list_free(old_spec_measures);
				}
	
				delete old_score_info;
#endif
				m_upbeat_inverse = im->m_upbeat_inverse;
				m_global_spacement_fac = 1.0;
				m_midi_tempo_inverse = im->m_tempo_inverse;
				m_command_history->reset();
				m_current_filename[0] = '\0';
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), 1);
				m_topy = m_leftx = 0;
				gtk_window_set_title (GTK_WINDOW (m_main_window), "Nted");
				for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
					((NedPage *) lptr->data)->handleStaffElements();
				}
				computeSystemIndent();
				setAndUpdateClefTypeAndKeySig();
				updatePageCounter();
				zoomFreeReplaceables(m_current_zoom, getCurrentScale());
				NedCommandList *command_list = new NedCommandList(this);
				renumberMeasures(NULL, 1, true);
/*
				removeAllBeams();
*/
				reposit(command_list, NULL, NULL, true);
				for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
					((NedPage *) lptr->data)->testTies();
				}
				for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
					((NedPage *) lptr->data)->cutEmptyVoices();
				}
				for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
					((NedPage *) lptr->data)->handleEmptyMeasures();
				}
				for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
					((NedPage *) lptr->data)->removeUnneededAccidentals();
				}
				setAllUnpositioned();
				removeEmptyInteriourPages();
				reposit(command_list, NULL, NULL, true);
				deleteEmptyPagesAtEnd();
				renumberMeasures(NULL, 1, true);
				updatePageCounter();
				//delete command_list;
			}
			else {
#define FREE2
#ifdef FREE2
				for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
					delete ((NedPage *) lptr->data);
				}
				g_list_free(m_pages);
				m_pages = NULL;
				if (m_special_measures != NULL) {
					for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
						free(lptr->data);
					}
					g_list_free(m_special_measures);
				}
				delete m_score_info;
#endif

				m_special_measures = old_spec_measures;
				m_pages = old_pages;
				m_upbeat_inverse = old_upbeat_inverse;
				m_global_spacement_fac = old_spacement_fac;
				m_portrait = old_portrait;
				m_current_paper = old_paper_type;
				m_midi_tempo_inverse = old_tempo_inverse;
				m_numerator = old_numerator;
				m_current_scale = old_scale;
				m_denominator = old_denominator;
				m_timesig_symbol = old_timesig_symbol;
				m_staff_count = old_staff_count;
				m_config_changed =	old_config_changed;
				for (i = 0; i < old_staff_count; i++) {
					m_staff_contexts[i] = old_staff_contexts[i];
				}
				m_first_page_yoffs = old_first_page_yoffs;
				m_score_info = old_score_info;
				computeSystemIndent();
			}
			repaint();
		}
		else {
			if (im->getErrorString()) {
				NedResource::Info(im->getErrorString());
			}
		}
		if (filename != NULL) {
			fclose(fp);
		}
	}
	return ok;
}


bool NedMainWindow::do_import_from_other(NedImporter *im) {
	int i, j;
	bool first = true;
	bool first_measure = true;
	bool overflow;
	NedPage *current_page;
	NedSystem *current_system;
	int page_nr = 0;
	unsigned int measure_duration;
	int num, denom;
	unsigned int meas_num = 1;

	if (im->m_partcount < 1) return false;
	double w = m_portrait ? m_current_paper->width : m_current_paper->height;
	double h = m_portrait ? m_current_paper->height : m_current_paper->width;
	w /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
	h /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
	
	m_staff_count = im->m_partcount;
	m_numerator = im->m_parts[0].numerator;
	m_denominator = im->m_parts[0].denominator;
	m_timesig_symbol = TIME_SYMBOL_NONE;
	for (i = 0; i < m_staff_count; i++) {
		if (m_staff_contexts[i].m_staff_name != NULL) {
			delete m_staff_contexts[i].m_staff_name;
			m_staff_contexts[i].m_staff_name = NULL;
		}
		if (im->m_parts[i].name != NULL && strlen(im->m_parts[i].name) > 0) {
			m_staff_contexts[i].m_staff_name = new NedPangoCairoText(m_drawing_area->window, im->m_parts[i].name, STAFF_NAME_FONT, STAFF_NAME_FONT_SLANT,
				STAFF_NAME_FONT_WEIGHT, STAFF_NAME_FONT_SIZE, m_current_zoom, getCurrentScale(), false);
		}
		if (m_staff_contexts[i].m_staff_short_name != NULL) {
			delete m_staff_contexts[i].m_staff_short_name;
		}
		m_staff_contexts[i].m_staff_short_name = NULL;
		if (m_staff_contexts[i].m_group_name != NULL) {
			delete m_staff_contexts[i].m_group_name;
		}
		m_staff_contexts[i].m_group_name = NULL;
		if (m_staff_contexts[i].m_group_short_name != NULL) {
			delete m_staff_contexts[i].m_group_short_name;
		}
		m_staff_contexts[i].m_group_short_name = NULL;
		m_staff_contexts[i].m_key_signature_number = im->m_parts[i].key_signature;
		m_staff_contexts[i].m_clef_number = im->m_parts[i].clef;
		m_staff_contexts[i].m_clef_octave_shift = im->m_parts[i].clef_octave;
		m_staff_contexts[i].m_different_voices = false;
		for (j = 0; j < VOICE_COUNT; j++) {
			m_staff_contexts[i].voices[j].m_midi_volume = im->m_parts[i].volume;
			m_staff_contexts[i].voices[j].m_midi_pan = 64;
			m_staff_contexts[i].voices[j].m_midi_program = im->m_parts[i].midi_pgm;
			m_staff_contexts[i].m_muted[j] = false;
		}
		m_staff_contexts[i].m_midi_chorus = 0;
		m_staff_contexts[i].m_play_transposed = 0;
		m_staff_contexts[i].m_midi_reverb = 0;
		m_staff_contexts[i].m_midi_channel = im->m_parts[i].midi_channel;
		m_staff_contexts[i].m_flags = 0;
	}
	// current_page = (NedPage *) g_list_first(m_pages)->data;
	m_pages = NULL;
	current_page = new NedPage(this, w, h, page_nr++, 0, false);
	m_pages = g_list_append(m_pages, current_page);
	current_system = current_page->appendSystem();
/*
	NedCommandList *command_list = new NedCommandList(this);
	for (i = 1; i < m_staff_count; i++) {
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			((NedPage *) lptr->data)->appendStaff(command_list, i);
		}
	}
	command_list->execute();
*/

	current_system->empty();
	current_system->prepareForImport();
	computeSystemIndent();
	num = getNumerator(meas_num);
	denom = getDenominator(meas_num);
	while(!im->is_eof() && !(overflow = current_system->testLineOverflow())) {
		if (first_measure) {
			first_measure = false;
			measure_duration = num * WHOLE_NOTE / denom - m_upbeat_inverse;
		}
		else {
			measure_duration = num * WHOLE_NOTE / denom;

		}
		current_system->appendElementsOfMeasureLength(im->m_parts, measure_duration, meas_num);
		meas_num++;
		num = getNumerator(meas_num);
		denom = getDenominator(meas_num);
	}
	if (overflow) {
		im->resetTaken();
		current_system->removeLastImported();
		meas_num--;
		num = getNumerator(meas_num);
		denom = getDenominator(meas_num);
	}
	if (!current_system->handleImportedTuplets()) {
		NedResource::Info("Problems setting Tuplets");
		return false;;
	}
	if (!im->is_eof()) {
		if (current_page->isPageOverflow()) {
			current_page->removeSystem(current_system);
			current_page = new NedPage(this, w, h, page_nr++, 0, false);
			m_pages = g_list_append(m_pages, current_page);
			current_page->insertSystem(current_system);
		}
		current_system = current_page->appendSystem();
		first = false;
	}
	while (!im->is_eof()) {
		current_system->empty();
		current_system->prepareForImport();
		overflow = false;
		while(!im->is_eof() && !overflow) {
			measure_duration = num * WHOLE_NOTE / denom;
			current_system->appendElementsOfMeasureLength(im->m_parts, num * WHOLE_NOTE / denom, meas_num);
			meas_num++;
			num = getNumerator(meas_num);
			denom = getDenominator(meas_num);
			overflow = current_system->testLineOverflow();
		}
		if (overflow) {
			im->resetTaken();
			current_system->removeLastImported();
			NedResource::DbgMsg(DBG_TESTING, "Stelle 3: meas_num = %d\n", meas_num);
			meas_num--;
			NedResource::DbgMsg(DBG_TESTING, "Stelle 4: meas_num = %d\n", meas_num);
			num = getNumerator(meas_num);
			denom = getDenominator(meas_num);
		}
		if (!current_system->handleImportedTuplets()) {
			NedResource::Info("Problems setting Tuplets");
			return false;
		}
		if (current_page->isPageOverflow()) {
			current_page->removeSystem(current_system);
			current_page = new NedPage(this, w, h, page_nr++, 0, false);
			m_pages = g_list_append(m_pages, current_page);
			current_page->insertSystem(current_system);
		}
		if (!im->is_eof()) {
			current_system = current_page->appendSystem();
		}
			
	}
	current_system->fill_up(NULL);
	return true;
}

void NedMainWindow::readSpecMeasures(FILE *fp, GList **new_spec_measures) {
	int measnum, type;
	int tsig_sym = TIME_SYMBOL_NONE;
	bool hide_following;
	int num, denom;
	char buffer[1024], buffer2[1024];
	SpecialMeasure *spec;

	if (!NedResource::readWord(fp, buffer) || strcmp(buffer, "(")) {
		NedResource::m_error_message = "( expected";
		return;
	}
	while (1) {
		hide_following = false;
		if (!NedResource::readInt(fp, &measnum) || measnum < 0 || measnum > 100000) {
			NedResource::m_error_message = "measure number expected";
			return;
		}
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "REPEAT_OPEN, REPEAT_CLOSE, REPEAT_OPEN_CLOSE, END_BAR, DOUBLE_BAR or REPEAT_NONE expected";
			return;
		}
		if (!strcmp(buffer, "REPEAT_OPEN")) {
			type = REPEAT_OPEN;
		}
		else if (!strcmp(buffer, "REPEAT_CLOSE")) {
			type = REPEAT_CLOSE;
			if (m_file_version > 15) {
				if (!NedResource::readWord(fp, buffer)) {
					NedResource::m_error_message = "HIDE_FOLLOWING or SHOW_FOLLOWING expected";
					return;
				}
				if (!strcmp(buffer, "HIDE_FOLLOWING")) {
					hide_following = true;
				}
				else if (!strcmp(buffer, "SHOW_FOLLOWING")) {
					hide_following = false;
				}
				else {
					NedResource::m_error_message = "HIDE_FOLLOWING or SHOW_FOLLOWING expected";
					return;
				}
			}
		}
		else if (!strcmp(buffer, "REPEAT_OPEN_CLOSE")) {
			type = REPEAT_OPEN_CLOSE;
		}
		else if (!strcmp(buffer, "END_BAR")) {
			type = END_BAR;
			if (m_file_version > 12) {
				if (!NedResource::readWord(fp, buffer)) {
					NedResource::m_error_message = "HIDE_FOLLOWING or SHOW_FOLLOWING expected";
					return;
				}
				if (!strcmp(buffer, "HIDE_FOLLOWING")) {
					hide_following = true;
				}
				else if (!strcmp(buffer, "SHOW_FOLLOWING")) {
					hide_following = false;
				}
				else {
					NedResource::m_error_message = "HIDE_FOLLOWING or SHOW_FOLLOWING expected";
					return;
				}
			}
		}
		else if (!strcmp(buffer, "DOUBLE_BAR")) {
			type = DOUBLE_BAR;
		}
		else if (!strcmp(buffer, "REPEAT_NONE")) {
			type = 0;
		}
		else {
			NedResource::m_error_message = "REPEAT_NONE, REPEAT_OPEN, REPEAT_CLOSE, REPEAT_OPEN_CLOSE, END_BAR, DOUBLE_BAR,  REPEAT_NONE or TIMESIG expected";
			return;
		}
		do {
			if (!NedResource::readWord(fp, buffer)) {
				NedResource::m_error_message = ") or , expected";
				return;
			}
			if (!strcmp(buffer, ",")) {
				if (!NedResource::readWordWithNum(fp, buffer)) {
					NedResource::m_error_message = "REP1START, REP1END, REP2START or REP2END expected(1)";
					return;
				}
				if (!strcmp(buffer, "REP1START")) {
					type |= REP1START;
				}
				else if (!strcmp(buffer, "REP2START")) {
					type |= REP2START;
				}
				else if (!strcmp(buffer, "REP1END")) {
					type |= REP1END;
				}
				else if (!strcmp(buffer, "REP2END")) {
					type |= REP2END;
				}
				else if (!strcmp(buffer, "TIMSIG")) {
					type |= TIMESIG;
					if (!NedResource::readWord(fp, buffer2) || strcmp(buffer2, "(")) {
						NedResource::m_error_message = "( expected";
						return;
					}
					if (!NedResource::readInt(fp, &num) || num < 1 || num > 20) {
						NedResource::m_error_message = "numerator expected";
						return;
					}
					if (!NedResource::readWord(fp, buffer2) || strcmp(buffer2, ",")) {
						NedResource::m_error_message = ", expected";
						return;
					}
					if (!NedResource::readInt(fp, &denom) || (denom  != 2 && denom != 4 && denom != 8 && denom != 16)) {
						NedResource::m_error_message = "denominator expected";
						return;
					}
					if (m_file_version > 16) {
						if (!NedResource::readWord(fp, buffer2) || strcmp(buffer2, ",")) {
							NedResource::m_error_message = ", expected";
							return;
						}
						if (!NedResource::readInt(fp, &tsig_sym) || (tsig_sym < TIME_SYMBOL_NONE || tsig_sym > TIME_SYMBOL_CUT_TIME)) {
							NedResource::m_error_message = "timesig symbol value expected";
							return;
						}
					}
					if (!NedResource::readWord(fp, buffer2) || strcmp(buffer2, ")")) {
						NedResource::m_error_message = ") expected";
						return;
					}
				}
				else {
					NedResource::DbgMsg(DBG_CRITICAL, "buffer = %s\n", buffer);
					NedResource::m_error_message = "REP1START, REP1END, REP2START or REP2END expected";
					return;
				}
			}
		}
		while (strcmp(buffer, ")"));
		spec = new SpecialMeasure();
		spec->measure_number = measnum;
		spec->hide_following = hide_following;
		spec->type = type;
		if ((type & TIMESIG) != 0) {
			spec->setTimeSignature(num, denom, tsig_sym);
		}
		*new_spec_measures = g_list_insert_sorted(*new_spec_measures, spec, compare_spec_measures_according_measnum);
		if (!NedResource::readWord(fp, buffer)) {
			NedResource::m_error_message = "( or MUSIC expected";
			return;
		}
		if (!strcmp(buffer, "MUSIC")) {
			//NedResource::unreadWord(buffer);
			return;
		}
		if (strcmp(buffer, "(")) {
			NedResource::m_error_message = "( or MUSIC expected";
			return;
		}
	}
}

void NedMainWindow::testAlternativeState(NedCommandList *command_list, NedMeasure *measure, bool alter1, bool alter2) {
	GList *lptr;
	NedChangeMeasureTypeCommand *change_measure_command;

	if ((measure->getSpecial() & REP_TYPE_MASK) != REPEAT_CLOSE) return;
	if (!alter1 && ((measure->getSpecial() & END_TYPE_MASK) == REP1END)) {
		if ((lptr = g_list_find(m_special_measures, measure->m_special_descr /* friend!! */)) == NULL) {
			NedResource::Abort("NedMainWindow::testAlternativeState(1)");
		}
		for (lptr = g_list_previous(lptr); lptr; lptr = g_list_previous(lptr)) {
			if (((((SpecialMeasure *) lptr->data)->type & START_TYPE_MASK)) == REP1START) {
				change_measure_command = new NedChangeMeasureTypeCommand(this, ((SpecialMeasure *) lptr->data)->measure_number,
					(((SpecialMeasure *) lptr->data)->type) & (~(REP1START)), false);
				command_list->addCommand(change_measure_command);
				change_measure_command->execute();
				break;
			}
		}
	}
	if (!alter2 && ((measure->getSpecial() & START_TYPE_MASK) == REP2START)) {
		if ((lptr = g_list_find(m_special_measures, measure->m_special_descr /* friend!! */)) == NULL) {
			NedResource::Abort("NedMainWindow::testAlternativeState(2)");
		}
		for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
			if (((((SpecialMeasure *) lptr->data)->type & END_TYPE_MASK)) == REP2END) {
				change_measure_command = new NedChangeMeasureTypeCommand(this, ((SpecialMeasure *) lptr->data)->measure_number,
					(((SpecialMeasure *) lptr->data)->type) & (~(REP2END)), false);
				command_list->addCommand(change_measure_command);
				change_measure_command->execute();
				break;
			}
		}
	}
}

bool NedMainWindow::conflictWithOhterSpecMeasure(int measnum, int dir) {
	GList *lptr;
	int measnum2;
	measnum2 = measnum + dir;
	if (measnum2 < 2) return true;
	if (dir > 0) {
		for (lptr = g_list_first(m_special_measures); lptr ; lptr = g_list_next(lptr)) {
			if (((SpecialMeasure *) lptr->data)->measure_number > measnum2) return false;
			if (((SpecialMeasure *) lptr->data)->measure_number <= measnum) continue;
			if ((((SpecialMeasure *) lptr->data)->type & (REP_TYPE_MASK | START_TYPE_MASK | END_TYPE_MASK)) == 0) continue;
			if ((((SpecialMeasure *) lptr->data)->type &  REP_TYPE_MASK) == DOUBLE_BAR) continue;
			if ((((SpecialMeasure *) lptr->data)->type &  REP_TYPE_MASK) == END_BAR) continue;
			return true;
		}
	}
	else {
		for (lptr = g_list_last(m_special_measures); lptr ; lptr = g_list_previous(lptr)) {
			if (((SpecialMeasure *) lptr->data)->measure_number < measnum2) return false;
			if (((SpecialMeasure *) lptr->data)->measure_number >= measnum) continue;
			if ((((SpecialMeasure *) lptr->data)->type & (REP_TYPE_MASK | START_TYPE_MASK | END_TYPE_MASK)) == 0) continue;
			if ((((SpecialMeasure *) lptr->data)->type &  REP_TYPE_MASK) == DOUBLE_BAR) continue;
			if ((((SpecialMeasure *) lptr->data)->type &  REP_TYPE_MASK) == END_BAR) continue;
			return true;
		}
	}
	return false;
}
			




void NedMainWindow::moveSpecMeasure(SpecialMeasure *spec, int dir) {
	GList *lptr;
	NedMoveSpecMeasureCommand *spec_meas_cmd;
	NedCommandList *command_list;
	if ((spec->type & START_TYPE_MASK) == REP1START) {
		if (conflictWithOhterSpecMeasure(spec->measure_number, dir)) {
			return;
		}
		command_list = new NedCommandList(this);
		spec_meas_cmd = new NedMoveSpecMeasureCommand(this, spec->measure_number, dir);
		spec_meas_cmd->execute();
		command_list->addCommand(spec_meas_cmd);
		command_list->setRenumber(NULL, 0, true);
		m_command_history->addCommandList(command_list);
		renumberMeasures(NULL, 1, true);
		repaint();
	}
	else if ((spec->type & START_TYPE_MASK) == REP2START) {
		if ((lptr = g_list_find(m_special_measures, spec)) == NULL) {
			NedResource::Abort("NedMainWindow::move_spec_measure_to_left");
		}
		for (lptr = g_list_next(lptr); lptr; lptr = g_list_next(lptr)) {
			spec = (SpecialMeasure *) lptr->data;
			if ((spec->type & START_TYPE_MASK) != 0) break;
			if ((spec->type & END_TYPE_MASK) == REP2END) break;
		}
		if (lptr == NULL) return;
		if ((spec->type & END_TYPE_MASK) == REP2END) {
			if (conflictWithOhterSpecMeasure(spec->measure_number, dir)) {
				return;
			}
			command_list = new NedCommandList(this);
			spec_meas_cmd = new NedMoveSpecMeasureCommand(this, spec->measure_number, dir);
			spec_meas_cmd->execute();
			command_list->addCommand(spec_meas_cmd);
			command_list->setRenumber(NULL, 0, true);
			m_command_history->addCommandList(command_list);
			renumberMeasures(NULL, 1, true);
			repaint();
		}
	}
}

void NedMainWindow::open_recent(GtkAction *action, gpointer data) {
	int idx;
	FILE *fp;
	NedMainWindow *main_window = (NedMainWindow *) data;
	char pathfoldername[4096], *cptr;
	GtkWidget *error_dialog;
	bool ok = TRUE;
	GtkWidget *unsaved_dialog;
	bool stop_opening = FALSE, do_save = FALSE;

	if (NedResource::isPlaying()) return;

	if (main_window->m_config_changed || main_window->m_command_history->unsavedOperations()) {
		unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				_("Save Changes ?"));
		gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
			case GTK_RESPONSE_YES: do_save = TRUE; break;
			case GTK_RESPONSE_REJECT: stop_opening = TRUE; break;
		}
		gtk_widget_destroy (unsaved_dialog);	
		if (stop_opening) {
			return;
		}
		if (do_save) {
			main_window->save_score(main_window->m_main_window, main_window);
		}
	}

	if (sscanf(gtk_action_get_name(action), "open_recent_action_%d", &idx) != 1) {
		NedResource::Warning("NedMainWindow::open_recent: cannot get index");
		return;
	}

	if (NedResource::m_recent_files[idx] == NULL) {
		NedResource::Warning("NedMainWindow::open_recent: NedResource::m_recent_files[idx] == NULL");
		return;
	}
	if ((fp = fopen(NedResource::m_recent_files[idx], "r")) == NULL) {
		error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			_("Cannot open %s for reading"), NedResource::m_recent_files[idx]);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
	}
	if (ok) {
		main_window->do_restore(fp, NedResource::m_recent_files[idx]);
		if ((cptr = strrchr(NedResource::m_recent_files[idx], '/')) != NULL) {
			strncpy(pathfoldername, NedResource::m_recent_files[idx], cptr - NedResource::m_recent_files[idx]);
			pathfoldername[cptr - NedResource::m_recent_files[idx]] = '\0';
			NedResource::m_last_folder = strdup(pathfoldername);
		}
		fclose(fp);
	}

}

void NedMainWindow::updateRecentFiles() {
	int i;
	GError *error = NULL;


	static const char *uiadd_start = "<ui>"    
	"  <menubar name='MainMenu'>"
	"    <menu action='FileMenu'>"
	"      <menu action='OpenRecentMenu'>";
	static const char *uiadd_end = "</menu>"
	"    </menu>"
	"  </menubar>"
	"</ui>";

	char Str[4096], str[4096], ac_name[128];

	if (m_recentFileMergeId >= 0) {
		gtk_ui_manager_remove_ui(m_ui_manager, m_recentFileMergeId);
		m_recentFileMergeId = -1;
	}

	strcpy(Str, uiadd_start);
	for (i = 0; i < MAX_RECENT_FILES; i++) {
		if (NedResource::m_recent_files[i] != NULL) {
			sprintf(str, "<menuitem action='open_recent_action_%d'/>", i);
			sprintf(ac_name, "open_recent_action_%d", i);
			gtk_action_group_remove_action(m_open_recent_action_group, m_recent_actions[i]);
			g_object_ref(m_recent_actions[i]);
			m_recent_actions[i] = gtk_action_new(ac_name, NedResource::m_recent_files[i], NULL, NULL);
			g_signal_connect (m_recent_actions[i], "activate", G_CALLBACK (open_recent), (void *) this);
			gtk_action_group_add_action(m_open_recent_action_group, m_recent_actions[i]);
			strcat(Str, str);
		}
	}
	strcat(Str, uiadd_end);
	m_recentFileMergeId = gtk_ui_manager_add_ui_from_string(m_ui_manager, Str, strlen(Str), &error);
}

		



void NedMainWindow::config_midi_out(GtkWidget  *widget, void *data) {
	GList *midi_ports, *lptr;
	int idx;

	if (!NedResource::alsaSequencerOutOk()) {
		NedResource::Info(_("Cannot list midi ports because could not open ALSA sequencer"));
		return;
	}
	NedMainWindow *main_window = (NedMainWindow *) data;
	midi_ports = NedResource::listMidiOutDevices(&idx);

	NedPortChooseDialog *dialog = new NedPortChooseDialog(GTK_WINDOW(main_window->m_main_window), midi_ports, idx, NedResource::m_midi_echo, true);
	idx = dialog->getSelection();
	if (idx < 0) {
		NedResource::DbgMsg(DBG_TESTING, "nothing selected\n");
	}
	else {
		if ((lptr = g_list_nth(midi_ports, idx)) == NULL) {
			NedResource::Abort("NedMainWindow::do_configure");
		}
		NedResource::subscribe_out(idx);
		NedResource::m_midi_echo =  dialog->getMidiEcho();
	}
	delete dialog;
}

void NedMainWindow::config_midi_in(GtkWidget  *widget, void *data) {
	GList *midi_ports, *lptr;
	int idx;

	if (!NedResource::alsaSequencerInOk()) {
		NedResource::Info(_("Cannot list midi ports because could not open ALSA sequencer"));
		return;
	}
	NedMainWindow *main_window = (NedMainWindow *) data;
	midi_ports = NedResource::listMidiInDevices(&idx);

	NedPortChooseDialog *dialog = new NedPortChooseDialog(GTK_WINDOW(main_window->m_main_window), midi_ports, idx, false, false);
	idx = dialog->getSelection();
	if (idx < 0) {
		NedResource::DbgMsg(DBG_TESTING, "nothing selected\n");
	}
	else {
		if ((lptr = g_list_nth(midi_ports, idx)) == NULL) {
			NedResource::Abort("NedMainWindow::do_configure");
		}
		gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(main_window->m_midi_input_action), FALSE);
		NedResource::subscribe_in(idx);
	}
	delete dialog;
}

void NedMainWindow::reconfig_paper(bool do_reposit, struct paper_info_struct *paper_type, bool portrait, int scale) {
	GList *lptr;

	m_current_scale = scale;
	m_current_paper = paper_type;
	m_portrait = portrait;
	double w = m_portrait ? m_current_paper->width : m_current_paper->height;
	double h = m_portrait ? m_current_paper->height : m_current_paper->width;
	w /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;
	h /= PS_ZOOM * (double) m_current_scale / (double) SCALE_GRANULARITY;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->reconfig_paper(w, h);
	}

	adjustAfterZoom();
	computeScoreTextExtends();
	/*
	NedCommandList *dummy = new NedCommandList(this);
	if (do_reposit) {
		reposit(dummy);
	}
	*/
	//delete dummy;
	repaint();
}

void NedMainWindow::config_print_cmd(GtkWidget *widget, void *data) {
	bool state;
	const char *new_print_command;

	NedMainWindow *main_window = (NedMainWindow *) data;

	NedPrintConfigDialog *dialog = new NedPrintConfigDialog (GTK_WINDOW(main_window->m_main_window), NedResource::m_print_cmd);
	dialog->getValues(&state, &new_print_command);
	if (state) {
		NedResource::m_print_cmd = new_print_command;
	}
	/*
	else {
		free(new_print_command);
	}
	*/
	delete dialog;
}

void NedMainWindow::config_meas_num_font(GtkWidget *widget, void *data) {
	GtkWidget* fontdialog;
	gchar *fontname;

	fontdialog = gtk_font_selection_dialog_new(_("Font selection - Measure numbers"));
	gtk_dialog_run(GTK_DIALOG(fontdialog));
	fontname = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(fontdialog));
	NedResource::DbgMsg(DBG_TESTING, "Font name = %s\n", fontname); 
	gtk_widget_destroy (GTK_WIDGET(fontdialog));
}

void NedMainWindow::config_paper(GtkWidget  *widget, void *data) {
	bool state, portrait;
	const char *paper;
	int w, h;
	bool p;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	NedPaperConfigDialog *dialog = new NedPaperConfigDialog (GTK_WINDOW(main_window->m_main_window),
				main_window->m_current_paper->name, main_window->m_portrait, true);
	dialog->getValues(&state, &paper, &portrait);
	delete dialog;
	if (state) {
		struct paper_info_struct *paper_type = NedResource::getPaperInfo(paper);
		w = main_window->m_current_paper->width;
		h = main_window->m_current_paper->height;
		p = main_window->m_portrait;
		if (main_window->m_portrait != portrait || paper_type->width != w || paper_type->height != h) {
			//NedCommandList *dummy = new NedCommandList(main_window);
			NedCommandList *command_list = new NedCommandList(main_window);
			NedPaperConfigCommand *page_config_command = new NedPaperConfigCommand(main_window, main_window->m_current_paper,
				main_window->m_portrait, paper_type, portrait, main_window->m_current_scale, main_window->m_current_scale);
			page_config_command->execute();
			command_list->addCommand(page_config_command);
			command_list->setFullReposit();
			main_window->setAllUnpositioned();
			main_window->reposit(command_list, NULL, NULL, true);
			main_window->repaint();
			main_window->m_command_history->addCommandList(command_list);
			//delete dummy; // crashes shouldn't !!!
		}
	}
}

void NedMainWindow::change_scale(GtkWidget  *widget, void *data) {
	bool state;
	int newscale;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	NedScaleDialog *dialog = new NedScaleDialog (GTK_WINDOW(main_window->m_main_window), main_window->m_current_scale);
	dialog->getValues(&state, &newscale);
	delete dialog;
	if (state && newscale != main_window->m_current_scale) {
		//NedCommandList *dummy = new NedCommandList(main_window);
		NedCommandList *command_list = new NedCommandList(main_window);
		NedPaperConfigCommand *page_config_command = new NedPaperConfigCommand(main_window, main_window->m_current_paper,
			main_window->m_portrait, main_window->m_current_paper, main_window->m_portrait, main_window->m_current_scale, newscale);
		page_config_command->execute();
		command_list->addCommand(page_config_command);
		command_list->setFullReposit();
		main_window->setAllUnpositioned();
		main_window->reposit(command_list, NULL, NULL, true);
		main_window->repaint();
		main_window->m_command_history->addCommandList(command_list);
		//delete dummy; // crashes shouldn't !!!
	}
}

void NedMainWindow::config_record(GtkWidget  *widget, void *data) {
	bool state;
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (main_window->m_selected_chord_or_rest != NULL) {
		NedStaff *staff = main_window->m_selected_chord_or_rest->getStaff();
		main_window->m_record_midi_numerator = main_window->m_numerator;
		main_window->m_record_midi_denominator = main_window->m_denominator;
		main_window->m_record_midi_keysig = main_window->m_staff_contexts[staff->getStaffNumber()].m_key_signature_number;
		main_window->m_record_midi_pgm = main_window->m_staff_contexts[staff->getStaffNumber()].voices[0].m_midi_program;
	}

	NedMidiRecordConfigDialog *dialog = new NedMidiRecordConfigDialog (GTK_WINDOW(main_window->m_main_window),
			main_window->m_record_midi_numerator, main_window->m_record_midi_denominator, main_window->m_record_midi_pgm,
			main_window->m_record_midi_tempo_inverse, main_window->m_record_midi_metro_volume, main_window->m_record_midi_triplet_recognition, main_window->m_record_midi_f_piano, main_window->m_record_midi_dnt_split,
			main_window->m_record_midi_keysig);
	dialog->getValues(&state, &(main_window->m_record_midi_numerator), &(main_window->m_record_midi_denominator), 
			&(main_window->m_record_midi_pgm),
	                &(main_window->m_record_midi_tempo_inverse), &(main_window->m_record_midi_metro_volume), &(main_window->m_record_midi_triplet_recognition), &(main_window->m_record_midi_f_piano), &(main_window->m_record_midi_dnt_split),
			&(main_window->m_record_midi_keysig));
	delete dialog;
}

void NedMainWindow::config_drums(GtkWidget  *widget, void *data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedDrumConfigDialog NedDrumConfigDialog(GTK_WINDOW(main_window->m_main_window));
}

void NedMainWindow::toggle_show_hidden(GtkAction *action, gpointer data) {
	if (NedResource::isPlaying()) return;
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->repaint();
}
	
void NedMainWindow::toggle_midi_input(GtkAction *action, gpointer data) {
	if (NedResource::isPlaying()) return;


	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION (action))) {
		NedMainWindow *main_window = (NedMainWindow *) data;
		gtk_toggle_action_set_active(main_window->m_midi_record_action, FALSE);
		NedResource::startMidiInput();
	}
	else {
		NedResource::stopMidiInput();
	}
}

int NedMainWindow::setChannelFromCursor() {
	GdkDisplay *display = NULL;
	NedStaff *staff;
	double dummy1;
	int dummy2;
	gint x=0, y=0;
	GList *lptr;
	bool found = false;
	GdkScreen *screen = NULL;

	gtk_widget_get_pointer(m_drawing_area, &x, &y);
	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		if (((NedPage *) lptr->data)->findLine(x, y, &dummy1, &dummy2, &dummy1, &staff)) {
			found = true; break;
		}
	}
	if (found) {
		return m_staff_contexts[staff->getStaffNumber()].m_midi_channel;
	}
	return 0;
}

void NedMainWindow::toggle_record_midi(GtkAction *action, gpointer data) {
	if (NedResource::isPlaying()) return;
	GList *lptr;
	bool ok;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION (action))) {
		gtk_toggle_action_set_active(main_window->m_midi_input_action, FALSE);
		gtk_toggle_action_set_active(main_window->m_keyboard_insert_action, FALSE);
		gtk_toggle_action_set_active(main_window->m_insert_erease_mode_action, FALSE);
		if (main_window->m_selected_chord_or_rest != NULL) {
			NedStaff *staff = main_window->m_selected_chord_or_rest->getStaff();
			main_window->m_record_midi_numerator = main_window->m_numerator;
			main_window->m_record_midi_denominator = main_window->m_denominator;
			main_window->m_record_midi_keysig = main_window->m_staff_contexts[staff->getStaffNumber()].m_key_signature_number;
			main_window->m_record_midi_pgm = main_window->m_staff_contexts[staff->getStaffNumber()].voices[0].m_midi_program;
		}

		main_window->m_midi_recorder = new NedMidiRecorder(main_window->m_record_midi_numerator, main_window->m_record_midi_denominator, main_window->m_record_midi_keysig,
				main_window->m_record_midi_pgm, main_window->m_record_midi_tempo_inverse, main_window->m_record_midi_metro_volume,
					main_window->m_record_midi_triplet_recognition, main_window->m_record_midi_f_piano, main_window->m_record_midi_dnt_split);
		main_window->m_midi_recorder->startMetronom();
	}
	else {
		main_window->m_midi_recorder->stopMetronom();
		NedMainWindow *ned_main_window = new NedMainWindow(); // the new window registers itself in NedResource::m_main_windows list
#ifdef YELP_PATH
		ned_main_window->createLayout(NULL, NedResource::m_yelparg);
#else
		ned_main_window->createLayout(NULL, NULL);
#endif


		ok = ned_main_window->import_from_other(main_window->m_midi_recorder, NULL, true);
		delete main_window->m_midi_recorder;
		ned_main_window->m_midi_recorder = NULL;
		if (!ok) {
			if ((lptr = g_list_find(NedResource::m_main_windows, ned_main_window)) == NULL) {
				NedResource::Abort("NedMainWindow::toggle_record_midi(1)");
			}
			if (g_list_length(NedResource::m_main_windows) < 1) {
				NedResource::Abort("NedMainWindow::toggle_record_midi(2)");
			}
			NedResource::m_main_windows = g_list_delete_link(NedResource::m_main_windows, lptr);
			delete ned_main_window;
		}
		else {
			NedResource::m_recorded_staff = ((NedPage *) g_list_first(ned_main_window->m_pages)->data)->getFirstSystem()->getStaff(0);
		}
	}
}

void NedMainWindow::zoom_in(GtkWidget  *widget, void *data) {
	double midx;
	double old_zoom;
	NedMainWindow *main_window = (NedMainWindow *) data;
	
	old_zoom = main_window->getCurrentZoomFactor();
	main_window->m_current_zoom_level++;
	if (main_window->m_current_zoom_level >= ZOOMLEVELS) {
		main_window->m_current_zoom_level = ZOOMLEVELS - 1;
	}
	
	main_window->m_selection_rect.width = main_window->m_selection_rect.height = 0;
	
	main_window->m_current_zoom = NedResource::getZoomFactor(main_window->m_current_zoom_level);
	main_window->adjustAfterZoom();
	main_window->repaint();
	midx = main_window->m_leftx + main_window->m_drawing_area->allocation.width / 2;
	midx *= main_window->m_current_zoom / old_zoom;
	main_window->m_leftx = midx - main_window->m_drawing_area->allocation.width / 2;
	main_window->adjustView();

}
	
void NedMainWindow::zoom_out(GtkWidget  *widget, void *data) {
	double midx;
	double old_zoom;
	NedMainWindow *main_window = (NedMainWindow *) data;
	
	old_zoom = main_window->getCurrentZoomFactor();
	main_window->m_current_zoom_level--;
	if (main_window->m_current_zoom_level < 0) {
		main_window->m_current_zoom_level = 0;
	}
	main_window->m_current_zoom = NedResource::getZoomFactor(main_window->m_current_zoom_level);
	main_window->adjustAfterZoom();
	main_window->m_selection_rect.width = main_window->m_selection_rect.height = 0;
	if (main_window->m_selected_group != NULL) {
		g_list_free(main_window->m_selected_group);
		main_window->m_selected_group = NULL;
	}
	
	midx = main_window->m_leftx + main_window->m_drawing_area->allocation.width / 2;
	midx *= main_window->m_current_zoom / old_zoom;
	main_window->m_leftx = midx - main_window->m_drawing_area->allocation.width / 2;
	main_window->adjustView();
	main_window->repaint();
}

void NedMainWindow::adjustAfterZoom() {
	int i;
	for (i = 0; i < m_staff_count; i++) {
		if (m_staff_contexts[i].m_staff_name != NULL) {
			m_staff_contexts[i].m_staff_name->setZoom(m_current_zoom, getCurrentScale());
		}
		if (m_staff_contexts[i].m_staff_short_name != NULL) {
			m_staff_contexts[i].m_staff_short_name->setZoom(m_current_zoom, getCurrentScale());
		}
		if (m_staff_contexts[i].m_group_name != NULL) {
			m_staff_contexts[i].m_group_name->setZoom(m_current_zoom, getCurrentScale());
		}
		if (m_staff_contexts[i].m_group_short_name != NULL) {
			m_staff_contexts[i].m_group_short_name->setZoom(m_current_zoom, getCurrentScale());
		}
	}
	rezoomScoreInfo();
	zoomFreeReplaceables(m_current_zoom, getCurrentScale());
	computeSystemIndent();
	computeScoreTextExtends();
	//reposit();
}

void NedMainWindow::rezoomScoreInfo() {
	if (m_score_info->title != NULL) {
		m_score_info->title->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->subject != NULL) {
		m_score_info->subject->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->composer != NULL) {
		m_score_info->composer->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->arranger != NULL) {
		m_score_info->arranger->setZoom(m_current_zoom, 1.0);
	}
	if (m_score_info->copyright != NULL) {
		m_score_info->copyright->setZoom(m_current_zoom, 1.0);
	}
}

NedPage *NedMainWindow::getLastPage() {
	return (NedPage *) g_list_last(m_pages)->data;
}

void NedMainWindow::find_new_cursor_pos(guint keyval, int *x, int *y) {
	GList *lptr;
	bool found = false;

	if (m_last_staff_during_key_insertion == NULL) {
		for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->find_staff_and_line(*x, *y, &m_last_staff_during_key_insertion, &m_last_line_during_key_insertion)) {
				found = true;
				break;
			}
		}
	}
	else {
		found = true;
	}

	if (!found) {
		return;
	}
	switch (keyval) {
		case GDK_Up: m_last_line_during_key_insertion--; break;
		case GDK_Down: m_last_line_during_key_insertion++; break;
		default: return;
	}

	*y = (int) m_last_staff_during_key_insertion->getRealYPosOfLine(m_last_line_during_key_insertion);
}

bool NedMainWindow::find_new_cursor_pos_at_measure(guint keyval, int *x, int *y) {
	GList *lptr;
	double newx, newy;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		if (((NedPage *) lptr->data)->findXposInOtherMeasure(keyval, *x, *y, &newx, &newy)) {
			*x = (int) newx;
			*y = (int) newy;
			return true;
		}
	}
	return false;
}
			


gboolean NedMainWindow::key_press_handler (GtkWidget *widget,  GdkEventKey *event,
                                                        gpointer  data)  {
	int method, ret;
	unsigned int nl;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedCommandList *command_list;
	int tuplet_val = 0;
	NedRemoveFreePlaceableCommand *rem_displaceable;
	GList *lptr;
	NedChordOrRest *newObj, *newObj1;
	int wx, wy;
	int page_nr;
	double newx;
	gint x=0, y=0;
	bool keyboard_insertion = gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(main_window->m_keyboard_insert_action));
	NedPage *page;
	NedChangeMeasureSpreadCommand *change_spread_cmd;
	GdkDisplay *display = NULL;
	GdkScreen *screen = NULL;
	char utf8string[10];
	guint32 unichar = gdk_keyval_to_unicode(event->keyval);
	int ll = g_unichar_to_utf8(unichar, utf8string);
	utf8string[ll] = '\0';

	if (NedResource::isPlaying()) return FALSE;

	if (main_window->m_lyrics_mode >= 0) {
		if (event->keyval == GDK_Escape) {
			main_window->resetLyricsMode();
			main_window->repaint();
			return TRUE;
		}
		if (main_window->m_selected_chord_or_rest == NULL) return TRUE;
		if (event->keyval == GDK_Left) {
			if (!main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_LEFT)) {
				main_window->m_selected_chord_or_rest->selectPreviousChord(true, main_window->m_lyrics_mode);
				if (main_window->m_selected_chord_or_rest != NULL) {
					main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_END);
				}
			}
			main_window->repaint();
			return TRUE;
		}
		else if (event->keyval == GDK_Right) {
			if (!main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_RIGHT)) {
				main_window->m_selected_chord_or_rest->selectNextChord(true, main_window->m_lyrics_mode);
				if (main_window->m_selected_chord_or_rest != NULL) {
					main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_START);
				}
			}
			main_window->repaint();
			return TRUE;
		}
		else if (event->keyval == GDK_BackSpace) {
			command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
			bool not_empty = main_window->m_selected_chord_or_rest->deleteLyricsLetter(command_list, main_window->m_lyrics_mode);
			main_window->reposit(command_list, main_window->m_selected_chord_or_rest->getPage(),
				main_window->m_selected_chord_or_rest->getSystem());
			main_window->repaint();
			if (!not_empty) {
				main_window->m_selected_chord_or_rest->selectPreviousChord(main_window->m_lyrics_mode, true);
				if (main_window->m_selected_chord_or_rest != NULL) {
					main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_END);
				}
			}
			if (command_list->getNumberOfCommands() > 0) {
				main_window->m_command_history->addCommandList(command_list);
			}
			return TRUE;
		}

		if (!strcmp(event->string, " ") || event->keyval == GDK_Return) {
			main_window->m_selected_chord_or_rest->selectNextChord(false, main_window->m_lyrics_mode);
			if (main_window->m_selected_chord_or_rest != NULL) {
				main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_START);
			}
			return TRUE;
		}
#ifdef KEYDEBUG
		printf("KEY = %c(0x%x)\n", event->keyval, event->keyval); fflush(stdout);
#endif
		gtk_im_context_filter_keypress(main_window->m_im_context, event);
		return TRUE;
	}

	/* ------------------------------------- */
	if (keyboard_insertion) {

		gtk_widget_get_pointer(main_window->m_drawing_area, &x, &y);
		display = gdk_display_get_default ();
		screen = gdk_display_get_default_screen (display);
		
		switch (event->keyval) {
			case GDK_Up:
			case GDK_Down:
				gtk_widget_get_pointer(main_window->m_drawing_area, &x, &y);
				main_window->find_new_cursor_pos(event->keyval, &x, &y);
				gtk_widget_translate_coordinates(main_window->m_drawing_area, main_window->m_main_window,
					x, y, &x, &y);
				gdk_window_get_position(GDK_WINDOW(main_window->m_main_window->window), &wx, &wy);
				gdk_display_warp_pointer (display, screen, wx + x, wy + y);
				break;
			case GDK_Right:
			case GDK_Left:
				gtk_widget_get_pointer(main_window->m_drawing_area, &x, &y);
				if (!main_window->find_new_cursor_pos_at_measure(event->keyval, &x, &y)) break;
				gtk_widget_translate_coordinates(main_window->m_drawing_area, main_window->m_main_window,
					x, y, &x, &y);
				gdk_window_get_position(GDK_WINDOW(main_window->m_main_window->window), &wx, &wy);
				gdk_display_warp_pointer (display, screen, wx + x, wy + y);
				break;
			case GDK_BackSpace:
				if (main_window->m_last_obj == NULL) break;
				if (main_window->m_last_cursor_x < 0) break;
				if (main_window->m_last_cursor_y < 0) break;
				command_list = new NedCommandList(main_window, main_window->m_last_obj->getSystem());
				main_window->m_last_obj->testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
				if (main_window->m_last_obj->getType() == TYPE_GRACE) {
					command_list->addCommand(new NedEreaseChordOrRestCommand(main_window->m_last_obj));
				}
				else {
					command_list->addCommand(new NedDeleteChordCommand(main_window->m_last_obj));
				}
				command_list->execute();
				main_window->m_command_history->addCommandList(command_list);
				main_window->reposit(command_list, main_window->m_last_obj->getPage(), main_window->m_last_obj->getSystem());
				gtk_widget_translate_coordinates(main_window->m_drawing_area, main_window->m_main_window,
					main_window->m_last_cursor_x, main_window->m_last_cursor_y, &x, &y);
				gdk_window_get_position(GDK_WINDOW(main_window->m_main_window->window), &wx, &wy);
				gdk_display_warp_pointer (display, screen, wx + x, wy + y);
				main_window->m_last_obj = NULL;
				main_window->m_last_cursor_x = main_window->m_last_cursor_y = -1;
				main_window->repaint();
				return TRUE;
		}
	}
	/*----------------------------------------- */

	if (event->keyval == GDK_Control_L || event->keyval == GDK_Control_R) {
		main_window->m_keyboard_ctrl_mode = TRUE;
		main_window->m_pointer_xpos = -1.0;
		main_window->repaint();
	}
	else if (event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R) {
		gdk_window_set_cursor (GDK_WINDOW(main_window->m_drawing_area->window), main_window->m_hand);
	}
	else if (!keyboard_insertion && event->keyval == GDK_Down) {
		if (main_window->m_selected_note != NULL) {
			if (event->state & GDK_SHIFT_MASK) {
				if (!main_window->m_selected_chord_or_rest->testMeasureSpread(-1)) return FALSE;
				command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
				change_spread_cmd = new NedChangeMeasureSpreadCommand(main_window->m_selected_chord_or_rest->m_measure, -1);
				change_spread_cmd->execute();
				command_list->addCommand(change_spread_cmd);
				main_window->setAllUnpositioned();
				main_window->reposit(command_list);
				command_list->setFullReposit();
				main_window->m_command_history->addCommandList(command_list);
				main_window->repaint();
			}
			else if (main_window->m_selected_note->testRelativeMove(-1)) {
				command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
				main_window->m_selected_note->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD | BREAK_TIE_FORWARD);
				main_window->m_selected_chord_or_rest->testForDeleteableConstraints(command_list);
				command_list->addCommand(new NedMoveNoteRelativeCommand(main_window->m_selected_note, -1));
				command_list->execute();
				main_window->reposit(command_list, NULL, main_window->m_selected_chord_or_rest->getSystem());
				main_window->m_command_history->addCommandList(command_list);
			}
		}
		else if (main_window->m_selected_chord_or_rest != NULL) { // rest
			command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
			command_list->addCommand(new NedRestMoveCommand(main_window->m_selected_chord_or_rest, main_window->m_selected_chord_or_rest->getLine(),
				main_window->m_selected_chord_or_rest->getLine() - 1));
			command_list->execute();
			main_window->reposit(command_list, NULL, main_window->m_selected_chord_or_rest->getSystem());
			main_window->m_command_history->addCommandList(command_list);
		}
		else if (main_window->m_selected_tie_forward != NULL) {
			if (main_window->m_selected_tie_forward->test_tie_increment(D_TIE)) {
				command_list = new NedCommandList(main_window, main_window->m_selected_tie_forward->getSystem());
				command_list->addCommand(new NedChangeTieCommand(main_window->m_selected_tie_forward, D_TIE));
				command_list->execute();
				main_window->m_command_history->addCommandList(command_list);
				main_window->repaint();
			}
		}
		return TRUE;
	}
	else if (!keyboard_insertion && event->keyval == GDK_Up) {
		if (main_window->m_selected_note != NULL) {
			if (event->state & GDK_SHIFT_MASK) {
				if (!main_window->m_selected_chord_or_rest->testMeasureSpread(1)) return FALSE;
				command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
				change_spread_cmd = new NedChangeMeasureSpreadCommand(main_window->m_selected_chord_or_rest->m_measure, 1);
				change_spread_cmd->execute();
				command_list->addCommand(change_spread_cmd);
				main_window->setAllUnpositioned();
				main_window->reposit(command_list);
				command_list->setFullReposit();
				main_window->m_command_history->addCommandList(command_list);
				main_window->repaint();
			}
			else if (main_window->m_selected_note->testRelativeMove(1)) {
				command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
				main_window->m_selected_note->testForTiesToDelete(command_list, BREAK_TIE_BACKWARD | BREAK_TIE_FORWARD);
				main_window->m_selected_chord_or_rest->testForDeleteableConstraints(command_list);
				command_list->addCommand(new NedMoveNoteRelativeCommand(main_window->m_selected_note, 1));
				command_list->execute();
				main_window->reposit(command_list, NULL, main_window->m_selected_chord_or_rest->getSystem());
				main_window->m_command_history->addCommandList(command_list);
			}
		}
		else if (main_window->m_selected_chord_or_rest != NULL) { // rest
			command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
			command_list->addCommand(new NedRestMoveCommand(main_window->m_selected_chord_or_rest, main_window->m_selected_chord_or_rest->getLine(),
				main_window->m_selected_chord_or_rest->getLine() + 1));
			command_list->execute();
			main_window->reposit(command_list, NULL, main_window->m_selected_chord_or_rest->getSystem());
			main_window->m_command_history->addCommandList(command_list);
		}
		else if (main_window->m_selected_tie_forward != NULL) {
			if (main_window->m_selected_tie_forward->test_tie_increment(-D_TIE)) {
				command_list = new NedCommandList(main_window, main_window->m_selected_tie_forward->getSystem());
				command_list->addCommand(new NedChangeTieCommand(main_window->m_selected_tie_forward,  -D_TIE));
				command_list->execute();
				main_window->m_command_history->addCommandList(command_list);
				main_window->reposit(NULL, main_window->m_selected_tie_forward->getPage(), main_window->m_selected_tie_forward->getSystem());
				main_window->repaint();
			}
		}
		return TRUE;
	}
	else if (!keyboard_insertion && event->keyval == GDK_Left) {
		if (main_window->m_selected_chord_or_rest != NULL) {
			main_window->m_selected_chord_or_rest = main_window->m_selected_chord_or_rest->getPreviousChordOrRest();
			if (main_window->m_selected_chord_or_rest != NULL) {
				if (main_window->m_selected_chord_or_rest->isRest()) {
					main_window->m_selected_note = NULL;
				}
				else {
					main_window->m_selected_note = main_window->m_selected_chord_or_rest->getFirstNote();
				}
			}
			else {
				main_window->m_selected_note = NULL;
			}
				
			main_window->setButtons();
			main_window->repaint();
		}
		else if (main_window->m_selected_spec_measure) {
			main_window->moveSpecMeasure(main_window->m_selected_spec_measure, -1);
		}
		else if (main_window->m_score_info->title != NULL && strlen(main_window->m_score_info->title->getText()) > 0) {
			main_window->m_score_info->title->moveCursor(-1);
			main_window->repaint();
		}
		return TRUE;
	}
	else if (!keyboard_insertion && event->keyval == GDK_Right) {
		if (main_window->m_selected_chord_or_rest != NULL) {
			main_window->m_selected_chord_or_rest = main_window->m_selected_chord_or_rest->getNextChordOrRest();
			if (main_window->m_selected_chord_or_rest != NULL) {
				if (main_window->m_selected_chord_or_rest->isRest()) {
					main_window->m_selected_note = NULL;
				}
				else {
					main_window->m_selected_note = main_window->m_selected_chord_or_rest->getFirstNote();
				}
			}
			else {
				main_window->m_selected_note = NULL;
			}
			main_window->setButtons();
			main_window->repaint();
		}
		else if (main_window->m_selected_spec_measure) {
			main_window->moveSpecMeasure(main_window->m_selected_spec_measure, 1);
		}
		else if (main_window->m_score_info->title != NULL && strlen(main_window->m_score_info->title->getText()) > 0) {
			main_window->m_score_info->title->moveCursor(1);
			main_window->repaint();
		}
		return TRUE;
	}
	else if (!keyboard_insertion && event->keyval == GDK_BackSpace) {
		if (main_window->m_selected_chord_or_rest != NULL && ((main_window->m_selected_chord_or_rest->getType() & (TYPE_CLEF | TYPE_STAFF_SIGN | TYPE_KEYSIG)) != 0)) {
			if (main_window->m_selected_chord_or_rest->getType() & TYPE_STAFF_SIGN) {
				ret = GTK_RESPONSE_NO;
			}
			else {
				GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(main_window->m_main_window), GTK_DIALOG_DESTROY_WITH_PARENT,
		                              GTK_MESSAGE_QUESTION, GTK_BUTTONS_YES_NO, _("Adjust notes ?"));
				ret = gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (dialog);
			}
			command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
			NedStaff *staff = main_window->m_selected_chord_or_rest->getStaff();
			NedRemoveStaffElemCommand *rem_staff_el_cmd = new NedRemoveStaffElemCommand(main_window, staff->getStaffNumber(), main_window->m_selected_chord_or_rest,
					ret == GTK_RESPONSE_YES);
			rem_staff_el_cmd->execute();
			command_list->setFullReposit();
			command_list->addCommand(rem_staff_el_cmd);
			main_window->setAllUnpositioned();
			main_window->reposit(command_list);
			main_window->m_command_history->addCommandList(command_list);
			main_window->m_selected_chord_or_rest = NULL;
			main_window->m_selected_note = NULL;
		}
		else if (main_window->m_selected_free_replaceable != NULL) {
			command_list = new NedCommandList(main_window);
			if (main_window->m_selected_free_replaceable->getType() == FREE_PLACEABLE_SLUR_POINT) {
				NedSlurPoint **sp;
				sp = ((NedSlurPoint *) main_window->m_selected_free_replaceable)->getSlurPoints();
				rem_displaceable = new NedRemoveFreePlaceableCommand(sp[0]);
				command_list->addCommand(rem_displaceable);
				rem_displaceable->execute();
				rem_displaceable = new NedRemoveFreePlaceableCommand(sp[1]);
				command_list->addCommand(rem_displaceable);
				rem_displaceable->execute();
				rem_displaceable = new NedRemoveFreePlaceableCommand(sp[2]);
				command_list->addCommand(rem_displaceable);
				rem_displaceable->execute();
			}
			else if (main_window->m_selected_free_replaceable->getType() == FREE_PLACEABLE_LINE_POINT) {
				NedLinePoint *startp, *endp, *midp;
				startp = ((NedLinePoint *) main_window->m_selected_free_replaceable)->getLine()->getLineStartPoint();
				endp = ((NedLinePoint *) main_window->m_selected_free_replaceable)->getLine()->getLineEndPoint();
				rem_displaceable = new NedRemoveFreePlaceableCommand(startp);
				command_list->addCommand(rem_displaceable);
				rem_displaceable->execute();
				if ((((NedLinePoint *) main_window->m_selected_free_replaceable)->getLine()->getLineType() & LINE_TEMPO_CHANGE) != 0) {
					midp = ((NedLine3 *) ((NedLinePoint *) main_window->m_selected_free_replaceable)->getLine())->getLineMidPoint();
					rem_displaceable = new NedRemoveFreePlaceableCommand(midp);
					command_list->addCommand(rem_displaceable);
					rem_displaceable->execute();
				}
				rem_displaceable = new NedRemoveFreePlaceableCommand(endp);
				command_list->addCommand(rem_displaceable);
				rem_displaceable->execute();
			}
			else {
				NedRemoveFreePlaceableCommand *rem_displaceable = new NedRemoveFreePlaceableCommand(main_window->m_selected_free_replaceable);
				rem_displaceable->execute();
				command_list->addCommand(rem_displaceable);
			}
			main_window->m_command_history->addCommandList(command_list);
			main_window->repaint();
		}
		else if (main_window->m_selected_chord_or_rest != NULL && !main_window->m_selected_chord_or_rest->isRest()) {
			command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
			main_window->m_selected_chord_or_rest->testForTiesToDelete(command_list, BREAK_TIE_FORWARD | BREAK_TIE_BACKWARD);
			GList *already_deleted_free_displaceables = NULL;
			main_window->m_selected_chord_or_rest->testForFreeReplaceablesToDelete(command_list, &already_deleted_free_displaceables);
			g_list_free(already_deleted_free_displaceables);
			if (main_window->m_selected_chord_or_rest->getType() == TYPE_GRACE) {
				command_list->addCommand(new NedEreaseChordOrRestCommand(main_window->m_selected_chord_or_rest));
			}
			else {
				command_list->addCommand(new NedDeleteChordCommand(main_window->m_selected_chord_or_rest));
			}
			main_window->m_selected_chord_or_rest = main_window->m_selected_chord_or_rest->getPreviousChordOrRest();
			if (main_window->m_selected_chord_or_rest != NULL) {
				if (main_window->m_selected_chord_or_rest->isRest()) {
					main_window->m_selected_note = NULL;
				}
				else {
					main_window->m_selected_note = main_window->m_selected_chord_or_rest->getFirstNote();
				}
			}
			command_list->execute();
			main_window->m_command_history->addCommandList(command_list);
			main_window->m_selected_spec_measure = NULL;
			if (main_window->m_selected_chord_or_rest != NULL) {
				main_window->reposit(command_list, main_window->m_selected_chord_or_rest->getPage(), main_window->m_selected_chord_or_rest->getSystem());
			}
			else {
				main_window->reposit(command_list);
			}
			main_window->repaint();
		}
	}
	else if (event->keyval == GDK_Page_Up) {
		page_nr = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window->m_page_selector));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window->m_page_selector), page_nr - 1);
	}
	else if (event->keyval == GDK_Page_Down) {
		page_nr = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window->m_page_selector));
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window->m_page_selector), page_nr + 1);
	}
	else if (event->keyval >= GDK_2 && event->keyval <=  GDK_9 && ( event->state & GDK_CONTROL_MASK) ) {
		method = (event->state & GDK_MOD1_MASK)  ? 1 : 0;
		if (!main_window->m_selected_chord_or_rest) {
			NedResource::Info(_("please select a note or rest first"));
			return TRUE;
		}
		if (main_window->m_selected_chord_or_rest->getDotCount() != 0) {
			NedResource::Info(_("you cannot select an element with dot"));
			return TRUE;
		}
		switch (event->keyval) {
			case GDK_2: tuplet_val = 2; break;
			case GDK_3: tuplet_val = 3; break;
			case GDK_4: tuplet_val = 4; break;
			case GDK_5: tuplet_val = 5; break;
			case GDK_6: tuplet_val = 6; break;
			case GDK_7: tuplet_val = 7; break;
			case GDK_8: tuplet_val = 8; break;
			case GDK_9: tuplet_val = 9; break;
		}
		if (!main_window->m_selected_chord_or_rest->getVoice()->
			tryConvertToTuplet(method, tuplet_val, main_window->m_selected_chord_or_rest)) {
			NedResource::Info(_("I'm not able to create a tuplet here"));
			return TRUE;
		}
	}
	else if ((event->keyval >= GDK_1 && event->keyval <= GDK_8) || (event->keyval >= GDK_KP_1 && event->keyval <= GDK_KP_8)) {
		switch (event->keyval) {
			case GDK_1: nl = WHOLE_NOTE; break;
			case GDK_2: nl = NOTE_2; break;
			case GDK_4: nl = NOTE_4; break;
			case GDK_5: nl = NOTE_8; break;
			case GDK_6: nl = NOTE_16; break;
			case GDK_7: nl = NOTE_32; break;
			case GDK_8: nl = NOTE_64; break;
			default: nl = NOTE_4; break;
		}
		NedResource::setNoteLengthButtons(nl);
		if (keyboard_insertion) {
			gtk_widget_get_pointer(main_window->m_drawing_area, &x, &y);
			display = gdk_display_get_default ();
			screen = gdk_display_get_default_screen (display);
			main_window->m_last_cursor_x = x; main_window->m_last_cursor_y = y;
			gtk_widget_get_pointer(main_window->m_drawing_area, &x, &y);
			main_window->find_new_cursor_pos(0, &x, &y);
			main_window->m_last_obj = NULL;
			for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
				if (((NedPage *) lptr->data)->tryInsertOrErease(x, y, 0, NULL /* no pitch given */, &newObj, (event->state & GDK_MOD1_MASK))) {
					page = (NedPage *) lptr->data;
					if (newObj != NULL) {
						main_window->m_last_obj = newObj;
						newObj1 = newObj->getNextChordOrRest();
						if (newObj1 == NULL) {
							newObj1 = newObj->getFirstObjectInNextSystem();
						}
						if (newObj1 != NULL) {
							main_window->m_last_staff_during_key_insertion = newObj1->getStaff();
							page = newObj1->getPage();
							newx = page->convertX(newObj1->getXPos());
							y = (int) main_window->m_last_staff_during_key_insertion->getRealYPosOfLine(main_window->m_last_line_during_key_insertion);
							gtk_widget_translate_coordinates(main_window->m_drawing_area, main_window->m_main_window,
								(gint)newx, y, &x, &y);
							gdk_window_get_position(GDK_WINDOW(main_window->m_main_window->window), &wx, &wy);
							gdk_display_warp_pointer (display, screen, (int) (x + wx), y + wy);
						}
					}
					main_window->resetSomeButtons();
				}
			}
		}
	}
	else  {
		switch(event->keyval) {
			case GDK_KP_Divide:  NedResource::toggleButton(1); break;
			case GDK_KP_Multiply:  NedResource::toggleButton(2); break;
			case GDK_KP_Subtract:  NedResource::toggleButton(3); break;
			case GDK_KP_Home:  NedResource::toggleButton(4); break;
			case GDK_KP_Up:  NedResource::toggleButton(5); break;
			case GDK_KP_Prior:  NedResource::toggleButton(6); break;
			case GDK_KP_Add:  NedResource::toggleButton(7); break;
			case GDK_KP_Left:  NedResource::toggleButton(8); break;
			case GDK_KP_Begin:  NedResource::toggleButton(9); break;
			case GDK_KP_Right:  NedResource::toggleButton(10); break;
			case GDK_KP_End:  NedResource::toggleButton(11); break;
			case GDK_KP_Down:  NedResource::toggleButton(12); break;
			case GDK_KP_Next:  NedResource::toggleButton(13); break;
			case GDK_KP_Enter:  NedResource::toggleButton(14); break;
			case GDK_KP_Insert:  NedResource::toggleButton(15); break;
			case GDK_KP_Delete:  NedResource::toggleButton(16); break;
			case GDK_R: 
			case GDK_r:  NedResource::toggleRest(); break;
			case GDK_comma: main_window->tryTieBackward(); break;
			case GDK_underscore:  NedResource::toggleTie(); break;
			case GDK_period:  NedResource::toggleDot(); break;
			case GDK_numbersign: NedResource::toggleSharp(); break;
			case GDK_equal: NedResource::toggleNatural(); break;
			case GDK_minus: NedResource::toggleFlat(); break;
			case GDK_Delete: if (main_window->m_selected_note == NULL) break;
					 if (main_window->m_selected_chord_or_rest == NULL) break;
					 main_window->m_selected_chord_or_rest->ereaseNote(main_window->m_selected_note); 
					 break;
		}
	}

		
	return TRUE;
}

void NedMainWindow::OnCommit(GtkIMContext *context, const gchar  *str,  void *data) {
	NedMainWindow *main_window = (NedMainWindow *) data;

#ifdef KEYDEBUG
	printf("str = %s\n", str); fflush(stdout);
#endif
	NedCommandList *command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
	if (!strcmp(str, "_")) {
		main_window->m_selected_chord_or_rest->appendToLyrics(command_list, main_window->m_lyrics_mode, " ");
	}
	else {
		main_window->m_selected_chord_or_rest->appendToLyrics(command_list, main_window->m_lyrics_mode, str);
	}
	main_window->reposit(command_list, main_window->m_selected_chord_or_rest->getPage(),
		main_window->m_selected_chord_or_rest->getSystem());
	if (!strcmp(str, "-")) {
		main_window->m_selected_chord_or_rest->selectNextChord(false, main_window->m_lyrics_mode);
		if (main_window->m_selected_chord_or_rest != NULL) {
			main_window->m_selected_chord_or_rest->setLyricsCursor(main_window->m_lyrics_mode, LYR_CURSOR_START);
		}
	}
	main_window->repaint();
	if (command_list->getNumberOfCommands() > 0) {
		main_window->m_command_history->addCommandList(command_list);
	}
}

void NedMainWindow::tryTieBackward() {
	if (gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(m_keyboard_insert_action))) { // keyboard insertion mode
		if (m_last_obj == NULL) return;
		m_last_obj->tryTieBackward();
		return;
	}
	if (m_selected_chord_or_rest == NULL || m_selected_note == NULL) return;
	m_selected_note->tryTieBackward();
}

gboolean NedMainWindow::key_release_handler (GtkWidget *widget,  GdkEventKey *event,
                                                        gpointer  data)  {
	NedMainWindow *main_window = (NedMainWindow *) data;
	if (event->keyval == GDK_Control_L || event->keyval == GDK_Control_R) {
		main_window->m_keyboard_ctrl_mode = false;
		main_window->m_selected_note = NULL;
		main_window->m_selected_chord_or_rest = NULL;
		main_window->m_selected_spec_measure = NULL;
		main_window->m_selected_tie_forward = NULL;
		main_window->repaint();
	}
	else if (event->keyval == GDK_Shift_L || event->keyval == GDK_Shift_R) {
		main_window->setCursor();
	}
	return FALSE;
}

gboolean NedMainWindow::window_leave_handler (GtkWidget *widget,  GdkEventCrossing *event,
                                                        gpointer  data)  {
	return FALSE;
}


gboolean NedMainWindow::window_enter_handler (GtkWidget *widget,  GdkEventCrossing *event,
                                                        gpointer  data)  {
	gtk_widget_grab_focus (widget);
	NedResource::setMainwWindowWithFocus((NedMainWindow *) data);
	return FALSE;
}

void NedMainWindow::OnSetFocus(GtkWindow *window, GtkWidget *widget, gpointer data) {
}

gboolean NedMainWindow::handle_button_press (GtkWidget *widget,
	 GdkEventButton *event, gpointer data)
{
	int staff_num, mtype, measnum, mtypeold;
	bool state;
	bool alter1, alter2, allow_alter;
	bool oldalter1, oldalter2, old_hide_following;
	GList *lptr, *lptr2;
	NedStaff *staff = NULL;
	NedMainWindow *main_window = (NedMainWindow *) data;
	NedNote *note;
	NedMeasure *measure;
	bool hide_following;
	int newnum, newdenom;
	int num, denom;
	unsigned int symbol, newsymbol;
	bool time_sig_state, delete_timesig;
	NedCommandList *command_list = NULL;
	NedChangeMeasureTimeSignatureCommand *change_sig_command;

	if (NedResource::isPlaying()) return FALSE;

	main_window->m_last_staff_during_key_insertion = NULL;
	main_window->m_pointer_ypos = event->y;

	if (event->button == 3) {
		if (g_list_length(main_window->m_selected_group) > 0 && main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM) {
			for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
				if ((staff = ((NedPage *) lptr->data)->findStaff(event->x,  event->y, &measure /* dummy */)) != NULL) {
					staff_num = staff->getStaffNumber();
					lptr2 = g_list_first(main_window->m_selected_group);
					while (lptr2) {
						if (((NedChordOrRest *) lptr2->data)->getStaff()->getStaffNumber() == staff_num) {
							main_window->m_selected_group = g_list_delete_link(main_window->m_selected_group, lptr2);
							lptr2 = g_list_first(main_window->m_selected_group);
						}
						else {
							lptr2 = g_list_next(lptr2);
						}
					}
					main_window->repaint();
					return FALSE;
				}
			}
			return FALSE;
		}

		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->trySelect(event->x, event->y, true)) {
				if (main_window->m_selected_free_replaceable != NULL) {
					main_window->m_selected_free_replaceable->startContextDialog(main_window->m_main_window);
					return FALSE;
				}
			}
		}

		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			measure = NULL;
			if ((staff = ((NedPage *) lptr->data)->findStaff(event->x,  event->y, &measure)) != NULL) {
				if (measure != NULL) {
					mtype = measure->getSpecial();
					measnum = measure->getMeasureNumber();
					mtypeold = mtype;
					oldalter1 = ((mtype & REP_TYPE_MASK) == REPEAT_CLOSE) && ((mtype & END_TYPE_MASK) == REP1END);
					oldalter2 = ((mtype & REP_TYPE_MASK) == REPEAT_CLOSE) && ((mtype & START_TYPE_MASK) == REP2START);
					allow_alter = oldalter1 || oldalter2 || !main_window->conflictWithOhterSpecMeasure(measure->getMeasureNumber(), -1);
					if ((mtype & REP_TYPE_MASK) & (END_BAR | REPEAT_CLOSE)) {
						old_hide_following = measure->m_hide_following;
					}
					else {
						old_hide_following = true;
					}

					num = main_window->getNumeratorOfThisMeasure(measure->getMeasureNumber());
					denom = main_window->getDenominatorOfThisMeasure(measure->getMeasureNumber());
					symbol = main_window->getTimesigSymbolOfThisMeasure(measure->getMeasureNumber());
					NedMeasureConfigDialog *dialog = 
						new NedMeasureConfigDialog(GTK_WINDOW(main_window->m_main_window), mtype & REP_TYPE_MASK, oldalter1, oldalter2, allow_alter, 
						old_hide_following, num, denom, symbol);
					dialog->getValues(&state, &mtype, &alter1, &alter2, &hide_following, &time_sig_state, &delete_timesig, &newnum, &newdenom, &newsymbol);
					if (state && (mtypeold != mtype || oldalter1 != alter1 || oldalter2 != alter2 || old_hide_following != hide_following)) {
						command_list = new NedCommandList(main_window, staff->getSystem());
						main_window->testAlternativeState(command_list, measure, alter1, alter2);
						alter1 = alter1 && ((mtype & REP_TYPE_MASK) == REPEAT_CLOSE);
						alter2 = alter2 && ((mtype & REP_TYPE_MASK) == REPEAT_CLOSE);
						if (alter1) {
							mtype |= REP1END;
						}
						if (alter2) {
							mtype |= REP2START;
						}
						if ((mtypeold & END_TYPE_MASK) == REP2END) {
							mtype |= REP2END;
						}
						/*
						if ((mtypeold & END_TYPE_MASK) == REP1END) {
							mtype |= REP1END;
						}
						*/
						NedChangeMeasureTypeCommand *change_measure_command = new
							NedChangeMeasureTypeCommand(main_window, measnum, mtype, hide_following);
						command_list->addCommand(change_measure_command);
						change_measure_command->execute();
						if (alter1 && !oldalter1) {
							if (measnum > 0) {
								change_measure_command = new NedChangeMeasureTypeCommand(main_window, measnum - 1, REP1START, hide_following);
								command_list->addCommand(change_measure_command);
								change_measure_command->execute();
							}
						}
						if (alter2 && !oldalter2) {
							change_measure_command = new NedChangeMeasureTypeCommand(main_window, measnum + 1, REP2END, hide_following);
							command_list->addCommand(change_measure_command);
							change_measure_command->execute();
						}
					}
					num = main_window->getNumerator(measure->getMeasureNumber());
					denom = main_window->getDenominator(measure->getMeasureNumber());

					if (time_sig_state && ((newnum != num || newdenom != denom || newsymbol != symbol) || delete_timesig)) {
						if (command_list == NULL) {
							command_list = new NedCommandList(main_window, staff->getSystem());
						}
						if (delete_timesig) {
							change_sig_command = new NedChangeMeasureTimeSignatureCommand(main_window, measnum, -1, -1, -1);
						}
						else {
							change_sig_command = new NedChangeMeasureTimeSignatureCommand(main_window, measnum, newnum, newdenom, newsymbol);
						}
						command_list->addCommand(change_sig_command);
						change_sig_command->execute();
						command_list->setFullReposit();
						main_window->setAllUnpositioned();
						main_window->reposit(command_list);
						main_window->checkForElementsToSplit(command_list);
					}
					if (command_list != NULL) {
						command_list->setFullReposit();
						main_window->setAllUnpositioned();
						main_window->m_command_history->addCommandList(command_list);
						main_window->reposit(command_list);
					}
					delete dialog;
				}
				else {
					main_window->do_staff_config(staff->getStaffNumber(), staff->getSystem());
				}
				return FALSE;
			}
		}
		return FALSE;
	}

	main_window->m_mouse_x = event->x;
	main_window->m_mouse_y = event->y;
	main_window->m_leftx0 = main_window->m_leftx;
	main_window->m_topy0 = main_window->m_topy;
	main_window->m_selection_rect.x = (int) event->x;
	main_window->m_selection_rect.y = (int) event->y;
	main_window->m_selection_rect.width = 0;
	main_window->m_selection_rect.height = 0;
	if (main_window->m_selected_group != NULL) {
		g_list_free(main_window->m_selected_group);
		main_window->m_selected_group = NULL;
	}

	if (main_window->m_selected_chord_or_rest != NULL && ((main_window->getMainWindowStatus() & INSERT_EREASE_MODE) == 0)) {
		if (main_window->m_selected_chord_or_rest->hitStemEnd(event->x, event->y)) {
			main_window->m_selected_stem = main_window->m_selected_chord_or_rest;
			return FALSE;
		}
	}

	if (NedResource::getTiedMode()) {
		if (main_window->m_selected_note) {
			note = main_window->m_selected_note;
			bool sel2 = FALSE;
			for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
				if (((NedPage *) lptr->data)->trySelect(event->x, event->y)) {
					sel2 = TRUE;
					break;
				}
			}
			if (main_window->m_selected_note) {
				main_window->m_selected_note->do_tie(note);
				main_window->m_selected_note = NULL;
				main_window->m_selected_chord_or_rest = NULL;
				main_window->m_selected_spec_measure = NULL;
				main_window->m_selected_tie_forward = NULL;
			}
			if (!sel2) {
				main_window->resetSomeButtons();
			}
		}
		else {
			for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
				if (((NedPage *) lptr->data)->trySelect(event->x, event->y)) {
					main_window->repaint();
					return FALSE;
				}
			}
			main_window->resetSomeButtons();
		}
		main_window->repaint();
		return FALSE;
	}

	if ((main_window->getMainWindowStatus() & SHIFT_MODE) != 0 || ((event->state & GDK_SHIFT_MASK) != 0)) return FALSE;


	main_window->m_selected_note = NULL;
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_spec_measure = NULL;
	main_window->m_selected_free_replaceable = NULL;
	main_window->m_selected_tie_forward = NULL;
		
	if (main_window->m_lyrics_mode < 0 && (main_window->getMainWindowStatus() & INSERT_EREASE_MODE) != 0 && ((event->state & GDK_CONTROL_MASK) == 0)) {
		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->tryInsertOrErease(event->x, event->y, 0, NULL /* no pitch given */)) {
				main_window->resetSomeButtons();
				main_window->repaint();
				return FALSE;
			}
		}
	}
	else {
		for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
			if (((NedPage *) lptr->data)->trySelect(event->x, event->y)) {
				main_window->setButtons();
				main_window->repaint();
				return FALSE;
			}
		}
		for (lptr = g_list_first(main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) {
			if (((SpecialMeasure *) lptr->data)->isNear(event->x, event->y)) {
				main_window->m_selected_spec_measure = (SpecialMeasure *) lptr->data;
			}
		}
		main_window->resetSomeButtons();
	}

	if (main_window->m_selected_chord_or_rest == NULL && main_window->m_selected_note == NULL && main_window->m_selected_spec_measure == NULL && 
		main_window->m_selected_free_replaceable != NULL) {
	}
	main_window->repaint();
	return FALSE;
}


void NedMainWindow::insertNoteWithPitch(int num_midi_input_notes, int *midi_input_chord) {
	GList *lptr;
	NedChordOrRest *newObj, *newObj1;
	GdkDisplay *display = NULL;
	GdkScreen *screen = NULL;
	double newx;
	gint x=0, y=0;
	NedPage *page;
	int wx, wy;
	bool found = false;
	double dummy1;
	int dummy2;
	NedStaff *staff;

	if (num_midi_input_notes == 0) return;

	gtk_widget_get_pointer(m_drawing_area, &x, &y);
	display = gdk_display_get_default ();
	screen = gdk_display_get_default_screen (display);
	m_last_cursor_x = x; m_last_cursor_y = y;
	gtk_widget_get_pointer(m_drawing_area, &x, &y);
	find_new_cursor_pos(0, &x, &y);
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		if (((NedPage *) lptr->data)->findLine(x, y, &dummy1, &dummy2, &dummy1, &staff)) {
			found = true; break;
		}
	}
	if (!found) return;


	if (!gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(m_keyboard_insert_action)) ||
		!gtk_toggle_action_get_active(GTK_TOGGLE_ACTION(m_insert_erease_mode_action))) {
			return;
	}


	m_last_obj = NULL;
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		if (((NedPage *) lptr->data)->tryInsertOrErease(x, y, num_midi_input_notes, midi_input_chord, &newObj)) {
			page = (NedPage *) lptr->data;
			if (newObj != NULL) {
				m_last_obj = newObj;
				newObj1 = newObj->getNextChordOrRest();
				if (newObj1 == NULL) {
					newObj1 = newObj->getFirstObjectInNextSystem();
				}
				if (newObj1 != NULL) {
					m_last_staff_during_key_insertion = newObj1->getStaff();
					page = newObj1->getPage();
					newx = page->convertX(newObj1->getXPos());
					y = (int) m_last_staff_during_key_insertion->getRealYPosOfLine(m_last_line_during_key_insertion);
					gtk_widget_translate_coordinates(m_drawing_area, m_main_window,
						(gint)newx, y, &x, &y);
					gdk_window_get_position(GDK_WINDOW(m_main_window->window), &wx, &wy);
					gdk_display_warp_pointer (display, screen, (int) (x + wx), y + wy);
				}
			}
			resetSomeButtons();
			
		}
	}
}

gint NedMainWindow::compare_spec_measures_according_measnum(gconstpointer ptr1, gconstpointer ptr2) {
	SpecialMeasure *spec_meas1 = (SpecialMeasure *) ptr1;
	SpecialMeasure *spec_meas2 = (SpecialMeasure *) ptr2;

	if (spec_meas1->measure_number < spec_meas2->measure_number) return -1;
	if (spec_meas1->measure_number > spec_meas2->measure_number) return  1;
	return 0;
}

int NedMainWindow::setSpecial(int measnum, int mtype, bool hide, bool *oldhide, bool overwrite) {
	GList *lptr;
	SpecialMeasure *spec_meas;
	int ret = 0;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == measnum)  {
			ret = (((SpecialMeasure *) lptr->data)->type & (REP_TYPE_MASK | START_TYPE_MASK | END_TYPE_MASK));
		/*
			if (mtype == 0) {
				((SpecialMeasure *) lptr->data)->type &= (~(REP_TYPE_MASK | START_TYPE_MASK | END_TYPE_MASK));
				if (((SpecialMeasure *) lptr->data)->type == 0) {
					free(lptr->data);
					m_special_measures = g_list_delete_link(m_special_measures, lptr); 
				}
			}
			else {
				((SpecialMeasure *) lptr->data)->type |= mtype;
			}
			*/
			if (overwrite) {
				((SpecialMeasure *) lptr->data)->type &= (~(REP_TYPE_MASK | START_TYPE_MASK | END_TYPE_MASK));
			}
			((SpecialMeasure *) lptr->data)->type |= mtype;
			if (oldhide != NULL) {
				*oldhide = ((SpecialMeasure *) lptr->data)->hide_following;
			}
			((SpecialMeasure *) lptr->data)->hide_following = hide;
			if (((SpecialMeasure *) lptr->data)->type == 0) {
				free(lptr->data);
				m_special_measures = g_list_delete_link(m_special_measures, lptr); 
			}
			repaint();
			return ret;
		}
	}

	if (mtype == 0) return 0;
	spec_meas = new SpecialMeasure();
	spec_meas->type = mtype;
	if (oldhide != NULL) {
		*oldhide = false;
	}
	spec_meas->hide_following = hide;
	spec_meas->measure_number = measnum;
	m_special_measures = g_list_insert_sorted(m_special_measures, spec_meas, compare_spec_measures_according_measnum);
	repaint();
	return 0;
}

void NedMainWindow::setInternalPitches() {
	GList *lptr;

	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->setInternalPitches();
	}
}

void NedMainWindow::adjustAccidentals(int staff_nr) {
	GList *lptr;
	setAndUpdateClefTypeAndKeySig();
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->adjustAccidentals(staff_nr);
	}
	for (lptr = g_list_first(m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->removeUnneededAccidentals(staff_nr);
	}
}
	
void NedMainWindow::setSpecialTimesig(int measnum, int num, int denom, unsigned int newsym, int *oldnum, int *olddenom, unsigned int *oldsym, bool do_reposit /* = true */) {
	GList *lptr;
	SpecialMeasure *spec_meas;
	*oldnum = *olddenom = -1;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == measnum)  {
			if ((((SpecialMeasure *) lptr->data)->type & TIMESIG_TYPE_MASK) != 0) {
				*oldnum = ((SpecialMeasure *) lptr->data)->numerator;
				*olddenom = ((SpecialMeasure *) lptr->data)->denominator;
				*oldsym = ((SpecialMeasure *) lptr->data)->symbol;
			}
			if (num < 0 || denom < 0) {
				((SpecialMeasure *) lptr->data)->type &= (~(TIMESIG));
				if (((SpecialMeasure *) lptr->data)->type == 0) {
					free(lptr->data);
					m_special_measures = g_list_delete_link(m_special_measures, lptr); 
				}
			}
			else {
				((SpecialMeasure *) lptr->data)->setTimeSignature(num, denom, newsym);
				((SpecialMeasure *) lptr->data)->type |= TIMESIG;
			}
			if (do_reposit) {
				renumberMeasures(NULL, 1, true);
				reposit();
				repaint();
			}
			return;
		}
	}

	if (num < 0 || denom < 0) return;
	spec_meas = new SpecialMeasure();
	spec_meas->type = TIMESIG;
	spec_meas->measure_number = measnum;
	spec_meas->setTimeSignature(num, denom, newsym);
	m_special_measures = g_list_insert_sorted(m_special_measures, spec_meas, compare_spec_measures_according_measnum);
	if (do_reposit) {
		renumberMeasures(NULL, 1, true);
		reposit();
		repaint();
	}
}

unsigned int NedMainWindow::getSpecial(int meas_num) {
	GList *lptr;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == meas_num)  {
			return ((SpecialMeasure *) lptr->data)->type;
		}
	}
	return 0;
}

SpecialMeasure *NedMainWindow::getSpecialMeasure(int meas_num) {
	GList *lptr;

	for (lptr = g_list_first(m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if (((SpecialMeasure *) lptr->data)->measure_number == meas_num)  {
			return (SpecialMeasure *) lptr->data;
		}
	}
	return NULL;
}

gboolean NedMainWindow::handle_motion (GtkWidget      *widget,
       GdkEventMotion  *event,
       gpointer	       data)
{
	GList *lptr;
	if (NedResource::isPlaying()) return FALSE;
	NedBbox sel_rect;
	struct timeval now;
	NedSystem *first_selected_system, *last_selected_system;
	NedPage *first_selected_page, *last_selected_page;

	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->m_last_staff_during_key_insertion = NULL;
	if (!(event->state & GDK_BUTTON1_MASK)) {
		if (((main_window->getMainWindowStatus() & INSERT_EREASE_MODE) != 0) && (!main_window->m_keyboard_ctrl_mode) && ((main_window->getMainWindowStatus() & SHIFT_MODE) == 0)) {
			main_window->m_pointer_xpos = event->x; 
			main_window->m_pointer_ypos = event->y; 
			main_window->repaint();
		}
		return FALSE;
	}
	main_window->m_pointer_xpos = -1.0;
	if ((main_window->getMainWindowStatus() & SHIFT_MODE) != 0 || ((event->state & GDK_SHIFT_MASK) != 0)) {
		main_window->m_selection_rect.width = main_window->m_selection_rect.height = 0;
		if (main_window->m_selected_group != NULL) {
			g_list_free(main_window->m_selected_group);
			main_window->m_selected_group = NULL;
		}
		main_window->setTopLeft(event->x, event->y);
		main_window->repaint();
		return FALSE;
	}
	if (main_window->m_selected_stem != NULL && ((main_window->getMainWindowStatus() & INSERT_EREASE_MODE) == 0)) {
		gettimeofday(&now, NULL);
		if (NedResource::subtime(&now, &(main_window->m_last_motion_call)) > 50000) {
			if (main_window->m_selected_stem->scaleStem(event->y, main_window->m_pointer_ypos)) {
				main_window->m_pointer_ypos = event->y;
			}
			main_window->repaint();
			gettimeofday(&(main_window->m_last_motion_call), NULL);
		}
		return FALSE;
	}
	if (NedResource::getTiedMode()) {
		return FALSE;
	}
	if (main_window->m_selected_chord_or_rest != NULL && ((main_window->m_selected_chord_or_rest->getType() & (TYPE_CLEF | TYPE_STAFF_SIGN | TYPE_KEYSIG)) != 0)) {
		return FALSE;
	}
	if (main_window->m_selected_chord_or_rest != NULL && (((main_window->getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (main_window->m_keyboard_ctrl_mode) != 0)) {
		main_window->m_selected_chord_or_rest->testXShift(event->x);
		main_window->repaint();
	}
	else if (main_window->m_selected_free_replaceable != NULL && (((main_window->getMainWindowStatus() & INSERT_EREASE_MODE) == 0) || (main_window->m_keyboard_ctrl_mode) != 0)) {
		main_window->m_selected_free_replaceable->shift(event->x, event->y, main_window->m_current_zoom, main_window->getCurrentScale(), main_window->m_leftx, main_window->m_topy);
		main_window->repaint();
		return FALSE;
	}
	if (main_window->m_selected_note == NULL || (((main_window->getMainWindowStatus() & INSERT_EREASE_MODE) != 0) && (!main_window->m_keyboard_ctrl_mode))) {
		gettimeofday(&now, NULL);
		if (NedResource::subtime(&now, &(main_window->m_last_motion_call)) > 50000) {
			if (main_window->m_selected_chord_or_rest == NULL) {
				int w = (int) (event->x - main_window->m_mouse_x);
				int h = (int) (event->y - main_window->m_mouse_y);
				if (w > 0) {
					main_window->m_selection_rect.width = w;
				}
				else {
					main_window->m_selection_rect.width = -w;
					main_window->m_selection_rect.x = (int) (main_window->m_mouse_x + w);
				}
				if (h > 0) {
					main_window->m_selection_rect.height = h;
				}
				else {
					main_window->m_selection_rect.height = -h;
					main_window->m_selection_rect.y = (int) (main_window->m_mouse_y + h);
				}
				if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM) {
					sel_rect.x = (main_window->m_leftx + main_window->m_selection_rect.x / main_window->getCurrentScale()) / main_window->m_current_zoom;
					sel_rect.y = (main_window->m_topy + main_window->m_selection_rect.y / main_window->getCurrentScale()) / main_window->m_current_zoom;
					sel_rect.width = main_window->m_selection_rect.width / main_window->m_current_zoom / main_window->getCurrentScale();
					sel_rect.height = main_window->m_selection_rect.height / main_window->m_current_zoom / main_window->getCurrentScale();
					if (main_window->m_selected_group != NULL) {
						g_list_free(main_window->m_selected_group);
						main_window->m_selected_group = NULL;
					}
					first_selected_system = last_selected_system = NULL;
					first_selected_page = last_selected_page = NULL;
					NedResource::m_number_of_first_selected_staff = NedResource::m_number_of_last_selected_staff = -1;
					for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
						((NedPage *) lptr->data)->findSelectedFirstsLasts(&sel_rect, &(NedResource::m_number_of_first_selected_staff), 
						&(NedResource::m_number_of_last_selected_staff), &first_selected_system, &last_selected_system,
						&first_selected_page, &last_selected_page);
					}
					for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
						((NedPage *) lptr->data)->collectSelectionRectangleElements(&sel_rect,
							&(main_window->m_selected_group), first_selected_system, last_selected_system,
							first_selected_page, last_selected_page);
					}
				}
				main_window->repaint();
			}
			gettimeofday(&(main_window->m_last_motion_call), NULL);
		}
		return FALSE;
	}
	if (main_window->m_selected_note->testYShift(event->y)) {
		main_window->repaint();
	}
	return FALSE;
}

NedChordOrRest* NedMainWindow::findAndBeamElementsOfCurrentVoice() {
	GList *lptr;
	NedChordOrRest *element = NULL;
	NedCommandList *command_list;

	for (lptr = g_list_first(m_selected_group); lptr; lptr = g_list_next(lptr)) {
		if ((((NedChordOrRest *) lptr->data)->getType() & TYPE_CHORD) && 
		    (((NedChordOrRest *) lptr->data)->getVoice()->getVoiceNumber() == getCurrentVoice())){ 
			element= (NedChordOrRest *) lptr->data;
			break;
		}
	}
	if (element == NULL) return NULL;
	command_list = new NedCommandList(this, element->getSystem());
	element->getVoice()->setBeamFlagsOf(command_list, m_selected_group, element);
	if (command_list->getNumberOfCommands() > 0) {
		m_command_history->addCommandList(command_list);
	}
	else {
		delete command_list;
	}
	return element;
}

gboolean NedMainWindow::handle_button_release (GtkWidget *widget,
	 GdkEventButton *event, gpointer data)
{
	double x;
	int i;
	GList *lptr;
	NedCommandList *commandList;

	if (NedResource::isPlaying()) return FALSE;


	NedMainWindow *main_window = (NedMainWindow *) data;
	if (main_window->m_selected_stem != NULL) {
		NedCommandList *command_list = new NedCommandList(main_window, main_window->m_selected_stem->getSystem());
		NedFixIntermediateCommand *fix_intermdiate_cmd = new NedFixIntermediateCommand(main_window->m_selected_stem);
		fix_intermdiate_cmd->execute();
		command_list->addCommand(fix_intermdiate_cmd);
		main_window->reposit(NULL, main_window->m_selected_note->getPage(), main_window->m_selected_note->getSystem());
		main_window->m_command_history->addCommandList(command_list);
	}
	main_window->m_selected_stem = NULL;

	if (main_window->m_selection_rect.width > MIN_SELECTION_RECTANGLE_DIM && main_window->m_selection_rect.height > MIN_SELECTION_RECTANGLE_DIM) {
		gtk_tool_button_set_stock_id (GTK_TOOL_BUTTON(main_window->m_customize_beam_button), "groupbeams-icon");
	}
	else {
		gtk_tool_button_set_stock_id (GTK_TOOL_BUTTON(main_window->m_customize_beam_button), "isolate-icon");
	}

	if (event->button != 1) {
		return FALSE;
	}

	
	if (main_window->getMainWindowStatus() & SHIFT_MODE) {
		x = main_window->m_leftx / main_window->m_current_zoom;
		for (i = 0, lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr), i++) {
			if (((NedPage *) lptr->data)->getXPos() > x) {
				break;
			}
		}
		main_window->m_avoid_feedback = TRUE;
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(main_window->m_page_selector), i);
		main_window->m_avoid_feedback = FALSE;
		return FALSE;
	}
	if (main_window->getMainWindowStatus() & INSERT_EREASE_MODE && !main_window->m_keyboard_ctrl_mode) return FALSE;
	if (main_window->m_selected_chord_or_rest != NULL) {
		main_window->m_selected_chord_or_rest->ReleaseTmpValue();
		main_window->repaint();
	}
	if (main_window->m_selected_free_replaceable != NULL) {
		main_window->m_selected_free_replaceable->toRel();
		commandList = new NedCommandList(main_window);
		commandList->addCommand(new NedFixMovedFreePlaceableCommand(main_window->m_selected_free_replaceable));
		if ((main_window->m_selected_free_replaceable->getType() & (FREE_PLACEABLE_CHORD | FREE_PLACEABLE_SPACER | FREE_PLACEABLE_CHORDNAME)) != 0) {
			main_window->setAllUnpositioned();
			main_window->reposit(commandList);
		}
		main_window->m_command_history->addCommandList(commandList);
		main_window->repaint();
		return FALSE;
	}
	if (main_window->m_selected_note == NULL) return FALSE;
	int new_line =  main_window->m_selected_note->getTempLine();
	if (new_line == UNKNOWN_LINE) return FALSE;
	int old_line =  main_window->m_selected_note->getLine();
	if (main_window->m_selected_note->noConflict()) {
		commandList = new NedCommandList(main_window, main_window->m_selected_note->getSystem());
		main_window->m_selected_note->testForTiesToDelete(commandList, BREAK_TIE_BACKWARD | BREAK_TIE_FORWARD);
		commandList->addCommand(new NedNoteMoveCommand(main_window->m_selected_note,
			old_line, new_line));
		commandList->execute();
		main_window->m_command_history->addCommandList(commandList);
	}
	main_window->m_selected_note->handleRelease();
	main_window->reposit(NULL, main_window->m_selected_note->getPage(), main_window->m_selected_note->getSystem());
	main_window->repaint();

	return FALSE;

}

gboolean NedMainWindow::scroll_event_handler(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
#define SCROLL_INCREMENT 40.0

	if (NedResource::isPlaying()) return FALSE;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if (event->state & GDK_SHIFT_MASK) {
		if (main_window->m_selected_chord_or_rest == NULL) return FALSE;
		int dir = (event->direction == GDK_SCROLL_UP) ? 1 : -1;
		if (!main_window->m_selected_chord_or_rest->testMeasureSpread(dir)) return FALSE;
		NedCommandList *command_list;
		NedChangeMeasureSpreadCommand *change_spread_cmd;
		command_list = new NedCommandList(main_window, main_window->m_selected_chord_or_rest->getSystem());
		change_spread_cmd = new NedChangeMeasureSpreadCommand(main_window->m_selected_chord_or_rest->m_measure, dir);
		change_spread_cmd->execute();
		command_list->addCommand(change_spread_cmd);
		main_window->setAllUnpositioned();
		main_window->reposit(command_list);
		command_list->setFullReposit();
		main_window->m_command_history->addCommandList(command_list);
		main_window->repaint();
		return FALSE;
	}
		

	if (event->direction == GDK_SCROLL_UP) {
		main_window->setTopRelative(-SCROLL_INCREMENT);
	}
	else if (event->direction == GDK_SCROLL_DOWN) {
		main_window->setTopRelative(SCROLL_INCREMENT);
	}
	return FALSE;
}

int NedMainWindow::getSorting(NedPage *this_page, NedStaff *this_staff, NedPage *other_page, NedStaff *other_staff) {
	int pos0, pos1;

	if (this_staff->getStaffNumber() != other_staff->getStaffNumber()) {
		return SORTING_NONE;
	}
	if ((pos0 = g_list_index(m_pages, this_page)) < 0) {
		NedResource::Warning("NedMainWindow::getSorting: didn't foind page 0");
		return SORTING_NONE;
	}
	if ((pos1 = g_list_index(m_pages, other_page)) < 0) {
		NedResource::Warning("NedMainWindow::getSorting: didn't foind page 1");
		return SORTING_NONE;
	}
	if (pos0 + 1 == pos1) {
		return SORTING_GREAT;
	}
	if (pos1 + 1 == pos0) {
		return SORTING_LESS;
	}
	return SORTING_NONE;
}
void NedMainWindow::size_change_handler(GtkWidget *widget, GtkRequisition *allocation, gpointer data) {
	NedMainWindow *main_window = (NedMainWindow *) data;
	main_window->adjustView();
}

void NedMainWindow::setButtons() {
	if (m_selected_chord_or_rest == NULL) return;
	m_avoid_feedback_action = TRUE;
	NedResource::setButtons(m_selected_chord_or_rest->getLength(), m_selected_chord_or_rest->getDotCount(), m_selected_chord_or_rest->getStatus(),
			(m_selected_chord_or_rest->isRest() || m_selected_note == NULL) ? 0 : m_selected_note->getStatus(),
			m_selected_chord_or_rest->isRest(), (m_selected_note == NULL) ? false : m_selected_note->getTieForward() != NULL,
			(m_selected_note == NULL) ? NORMAL_NOTE : m_selected_note->getNoteHead());
}

void NedMainWindow::resetButtons() {
	NedResource::resetButtons();
}

void NedMainWindow::prepareInsertion() {
	gtk_toggle_action_set_active (GTK_TOGGLE_ACTION (m_insert_erease_mode_action), TRUE);
	g_main_context_iteration (NULL, FALSE);
	NedResource::resetTieButton();
}

		

void NedMainWindow::setTopLeft(double x, double y) {
	NedPage *page;
	m_leftx +=  SHIFT_ACCEL * (m_mouse_x - x);
	m_topy +=  SHIFT_ACCEL *  (m_mouse_y - y);
	page = (NedPage *) g_list_first(m_pages)->data;
	if (m_topy > page->getTopPos() -  m_drawing_area->allocation.height / getCurrentScale()) {
		m_topy = page->getTopPos() - m_drawing_area->allocation.height / getCurrentScale();
	}
	page = (NedPage *) g_list_last(m_pages)->data;
	if (m_leftx > page->getLeftPos() - m_drawing_area->allocation.width / getCurrentScale()) {
		m_leftx = page->getLeftPos() - m_drawing_area->allocation.width / getCurrentScale();
	}
	if (m_leftx < 0) {m_leftx = 0; }
	if (m_topy < 0) {m_topy = 0;}
	m_mouse_x = x;
	m_mouse_y = y;
	if (m_selected_group != NULL) {
		g_list_free(m_selected_group);
		m_selected_group = NULL;
	}
	m_selection_rect.width = m_selection_rect.height = 0;
}

void NedMainWindow::setTopRelative(double y_incr) {
	NedPage *page;
	bool next_page_allowed = FALSE;
	page = (NedPage *) g_list_first(m_pages)->data;
	if (m_topy < 0.001 || m_topy > page->getTopPos() -  m_drawing_area->allocation.height / getCurrentScale() - 0.001) {
		next_page_allowed = TRUE;
	}
	m_topy += y_incr;
	if (m_topy > page->getTopPos() -  m_drawing_area->allocation.height / getCurrentScale()) {
		if (next_page_allowed) {
			if (setPageRelative(1)) {
				m_topy = 0.0;
			}
			else {
				m_topy = page->getTopPos() -  m_drawing_area->allocation.height / getCurrentScale();
			}
		}
		else {
			m_topy = page->getTopPos() -  m_drawing_area->allocation.height / getCurrentScale();
		}
	}
	if (m_topy < 0) {
		if (next_page_allowed) {
			if (setPageRelative(-1)) {
				m_topy = page->getTopPos() -  m_drawing_area->allocation.height / getCurrentScale();
			}
			else {
				m_topy = 0;
			}
		}
		else {
			m_topy = 0;
		}
	}
	if (m_selected_group != NULL) {
		g_list_free(m_selected_group);
		m_selected_group = NULL;
	}
	m_selection_rect.width = m_selection_rect.height = 0;
	repaint();
}

void NedMainWindow::setToPage(int page_nr) {
	GList *lptr;

	if ((lptr = g_list_nth (m_pages, page_nr - 1)) == NULL) {
		return;
	}
	m_topy = 0;
	m_leftx = ((NedPage *) lptr->data)->getXPos() * m_current_zoom;
	if (m_selected_group != NULL) {
		g_list_free(m_selected_group);
		m_selected_group = NULL;
	}
	m_selection_rect.width = m_selection_rect.height = 0;
	repaint();
}

bool NedMainWindow::setPageRelative(int y_incr) {
	GList *lptr;

	int page_nr;

	page_nr = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_page_selector));
	page_nr += y_incr;

	if ((lptr = g_list_nth (m_pages, page_nr - 1)) == NULL) {
		return FALSE;
	}
	if (m_selected_group != NULL) {
		g_list_free(m_selected_group);
		m_selected_group = NULL;
	}
	m_selection_rect.width = m_selection_rect.height = 0;
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_page_selector), page_nr);
	m_leftx = ((NedPage *) lptr->data)->getXPos() * m_current_zoom;
	return TRUE;
}



void NedMainWindow::adjustView() {
	if (m_pages == NULL) return;
	NedPage *page;
	page = (NedPage *) g_list_first(m_pages)->data;
	if (m_topy > page->getTopPos() -  m_drawing_area->allocation.height) {
		m_topy = page->getTopPos() -  m_drawing_area->allocation.height;
	}
	page = (NedPage *) g_list_last(m_pages)->data;
	if (m_leftx > page->getLeftPos() - m_drawing_area->allocation.width) {
		m_leftx = page->getLeftPos() -  m_drawing_area->allocation.width;
	}
	if (m_selected_group != NULL) {
		g_list_free(m_selected_group);
		m_selected_group = NULL;
	}
	m_selection_rect.width = m_selection_rect.height = 0;
	if (m_leftx < 0) m_leftx = 0;
	if (m_topy < 0) m_topy = 0;
}



void NedMainWindow::write_png(GtkWidget *widget, void *data) {
	GList *lptr;
	gchar *selected_filename;
	char the_filename[4096];
	GtkWidget *png_file_dialog;
	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	char *cptr;
	bool ok = false;
	bool extend;
	int page_nr;
	NedPage *page;
	char page_num_str[128];
	cairo_surface_t *surface;
	cairo_t *cr;
	bool old_paint_colored;
	NedNote *old_selected_note;

	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	page_nr = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window->m_page_selector));

	sprintf(page_num_str, "_%d", page_nr);

	if ((lptr = g_list_nth (main_window->m_pages, page_nr - 1)) == NULL) {
		lptr = g_list_first(main_window->m_pages);
	}

	page = (NedPage *) lptr->data;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.png (PNG-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.png");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	png_file_dialog = gtk_file_chooser_dialog_new (_("Create PNG"),
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(png_file_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(png_file_dialog), file_filter2);
	if (main_window->m_current_filename[0] != '\0') {
		strcpy(the_filename, main_window->m_current_filename);
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				*cptr = '\0';
			}
		}
		strcat(the_filename, page_num_str);
		strcat(the_filename, ".png");
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(png_file_dialog), the_filename);
	}

	if (gtk_dialog_run (GTK_DIALOG (png_file_dialog)) == GTK_RESPONSE_ACCEPT) {
		selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (png_file_dialog));
		strcpy(the_filename, selected_filename);
		g_free (selected_filename);
		ok = true;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (png_file_dialog))) == file_filter1) {
		extend = true;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".png")) {
				extend = false;
			}
		}
		if (extend) {
			strcat(the_filename, page_num_str);
			strcat(the_filename, ".png");
		}
	}
	gtk_widget_destroy (png_file_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = false;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (!ok) return;

	NedChordOrRest *old_selected_chord_or_rest = main_window->m_selected_chord_or_rest;
	old_selected_note = main_window->m_selected_note;
	old_paint_colored = main_window->m_paint_colored;
	main_window->m_paint_colored = false;
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_note = NULL;
	main_window->m_draw_postscript = true;

	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
	                       (int) (page->getWidth()  * main_window->m_current_zoom * main_window->getCurrentScale()), 
	                       (int) (page->getHeight() * main_window->m_current_zoom * main_window->getCurrentScale()));
	
	cr = cairo_create (surface);

#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(main_window->m_current_zoom_level));
	cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	
	page->draw(cr, gtk_toggle_action_get_active(main_window->m_show_measure_numbers_action),
		page->getWidth()  * main_window->m_current_zoom * main_window->getCurrentScale(),
		page->getHeight() * main_window->m_current_zoom * main_window->getCurrentScale());

	
	if (cairo_surface_write_to_png (surface, the_filename) != CAIRO_STATUS_SUCCESS) {
		error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			_("Cannot open %s for writing"), the_filename);
		gtk_dialog_run (GTK_DIALOG (error_dialog));
		gtk_widget_destroy (error_dialog);	
		ok = false;
	}
	
	cairo_surface_destroy (surface);
	cairo_destroy (cr);
	main_window->m_draw_postscript = false;
	main_window->m_paint_colored = old_paint_colored;
	main_window->m_selected_chord_or_rest = old_selected_chord_or_rest;
	main_window->m_selected_note = old_selected_note;
}

#ifdef CAIRO_HAS_SVG_SURFACE
void NedMainWindow::write_svg(GtkWidget *widget, void *data) {
	FILE *svgfile;
	GList *lptr;
	gchar *selected_filename;
	char the_filename[4096];
	GtkWidget *svg_file_dialog;
	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	char *cptr;
	bool ok = false;
	bool extend;
	int page_nr;
	NedPage *page;
	char page_num_str[128];
	cairo_surface_t *surface;
	cairo_t *cr;
	bool old_paint_colored;
	NedNote *old_selected_note;

	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	page_nr = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(main_window->m_page_selector));

	sprintf(page_num_str, "_%d", page_nr);

	if ((lptr = g_list_nth (main_window->m_pages, page_nr - 1)) == NULL) {
		lptr = g_list_first(main_window->m_pages);
	}

	page = (NedPage *) lptr->data;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.svg (SVG-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.svg");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	svg_file_dialog = gtk_file_chooser_dialog_new (_("Create SVG"),
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(svg_file_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(svg_file_dialog), file_filter2);
	if (main_window->m_current_filename[0] != '\0') {
		strcpy(the_filename, main_window->m_current_filename);
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				*cptr = '\0';
			}
		}
		strcat(the_filename, page_num_str);
		strcat(the_filename, ".svg");
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(svg_file_dialog), the_filename);
	}

	if (gtk_dialog_run (GTK_DIALOG (svg_file_dialog)) == GTK_RESPONSE_ACCEPT) {
		selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (svg_file_dialog));
		strcpy(the_filename, selected_filename);
		g_free (selected_filename);
		ok = true;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (svg_file_dialog))) == file_filter1) {
		extend = true;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".svg")) {
				extend = false;
			}
		}
		if (extend) {
			strcat(the_filename, page_num_str);
			strcat(the_filename, ".svg");
		}
	}
	gtk_widget_destroy (svg_file_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = false;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (ok) {
		if ((svgfile = fopen(the_filename, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = false;
		}
	}
	if (!ok) return;

	NedChordOrRest *old_selected_chord_or_rest = main_window->m_selected_chord_or_rest;
	old_selected_note = main_window->m_selected_note;
	old_paint_colored = main_window->m_paint_colored;
	main_window->m_paint_colored = false;
	main_window->m_selected_chord_or_rest = NULL;
	main_window->m_selected_note = NULL;
	main_window->m_draw_postscript = true;

	surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 
                       (int) (page->getWidth()  * main_window->m_current_zoom * main_window->getCurrentScale()), 
                       (int) (page->getHeight() * main_window->m_current_zoom * main_window->getCurrentScale()));

        surface = cairo_svg_surface_create_for_stream (writefunc, (void *) svgfile, 
                       (int) (page->getWidth()  * main_window->m_current_zoom * main_window->getCurrentScale()),
                       (int) (page->getHeight() * main_window->m_current_zoom * main_window->getCurrentScale()));

	cr = cairo_create (surface);

#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(main_window->m_current_zoom_level));
	cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	
	page->draw(cr, gtk_toggle_action_get_active(main_window->m_show_measure_numbers_action),
		page->getWidth()  * main_window->m_current_zoom * main_window->getCurrentScale(),
		page->getHeight() * main_window->m_current_zoom * main_window->getCurrentScale());
	
	cairo_surface_destroy (surface);
	cairo_destroy (cr);
	fclose(svgfile);
	main_window->m_draw_postscript = false;
	main_window->m_paint_colored = old_paint_colored;
	main_window->m_selected_chord_or_rest = old_selected_chord_or_rest;
	main_window->m_selected_note = old_selected_note;
}
#endif

void NedMainWindow::write_ps(GtkWidget  *widget, void *data) {
	FILE *psfile;
	gchar *selected_filename;
	char the_filename[4096];
	GtkWidget *ps_file_dialog;
	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	char *cptr;
	bool ok = FALSE;
	bool extend;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::isPlaying()) return;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.ps (PostScript-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.ps");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	ps_file_dialog = gtk_file_chooser_dialog_new (_("Create Postscript"),
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(ps_file_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(ps_file_dialog), file_filter2);
	if (main_window->m_current_filename[0] != '\0') {
		strcpy(the_filename, main_window->m_current_filename);
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				*cptr = '\0';
			}
		}
		strcat(the_filename, ".ps");
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(ps_file_dialog), the_filename);
	}

	if (gtk_dialog_run (GTK_DIALOG (ps_file_dialog)) == GTK_RESPONSE_ACCEPT) {
		selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (ps_file_dialog));
		strcpy(the_filename, selected_filename);
		g_free (selected_filename);
		ok = TRUE;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (ps_file_dialog))) == file_filter1) {
		extend = TRUE;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ps")) {
				extend = FALSE;
			}
		}
		if (extend) {
			strcat(the_filename, ".ps");
		}
	}
	gtk_widget_destroy (ps_file_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = FALSE;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (ok) {
		if ((psfile = fopen(the_filename, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
		}
	}
	if (!ok) return;
	main_window->do_export_to_stream(psfile, STREAM_FILE_TYPE_PS);
	fclose(psfile);

}

#ifdef CAIRO_HAS_PDF_SURFACE
void NedMainWindow::write_pdf(GtkWidget  *widget, void *data) {
	FILE *pdffile;
	gchar *selected_filename;
	char the_filename[4096];
	GtkWidget *pdf_file_dialog;
	GtkWidget *access_dialog;
	GtkWidget *error_dialog;
	char *cptr;
	bool ok = FALSE;
	bool extend;
	NedMainWindow *main_window = (NedMainWindow *) data;

	if (NedResource::isPlaying()) return;

	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.pdf (PDF-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.pdf");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	pdf_file_dialog = gtk_file_chooser_dialog_new (_("Create PDF"),
				      GTK_WINDOW(main_window->m_main_window),
				      GTK_FILE_CHOOSER_ACTION_SAVE,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(pdf_file_dialog), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(pdf_file_dialog), file_filter2);
	if (main_window->m_current_filename[0] != '\0') {
		strcpy(the_filename, main_window->m_current_filename);
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".ntd")) {
				*cptr = '\0';
			}
		}
		strcat(the_filename, ".pdf");
		gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(pdf_file_dialog), the_filename);
	}

	if (gtk_dialog_run (GTK_DIALOG (pdf_file_dialog)) == GTK_RESPONSE_ACCEPT) {
		selected_filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (pdf_file_dialog));
		strcpy(the_filename, selected_filename);
		g_free (selected_filename);
		ok = TRUE;
	}
	if (ok && gtk_file_chooser_get_filter((GTK_FILE_CHOOSER (pdf_file_dialog))) == file_filter1) {
		extend = TRUE;
		if ((cptr = strrchr(the_filename, '.')) != NULL) {
			if (!strcmp(cptr, ".pdf")) {
				extend = FALSE;
			}
		}
		if (extend) {
			strcat(the_filename, ".pdf");
		}
	}
	gtk_widget_destroy (pdf_file_dialog);
	if (ok && access(the_filename, F_OK) == 0) {
		access_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_QUESTION,
				GTK_BUTTONS_YES_NO,
				_("File %s already exists. Overwrite ?"), the_filename);
		if (gtk_dialog_run (GTK_DIALOG (access_dialog)) != GTK_RESPONSE_YES) {
			ok = FALSE;
		}
		gtk_widget_destroy (access_dialog);	
	}
	if (ok) {
		if ((pdffile = fopen(the_filename, "w")) == NULL) {
			error_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
				(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_OK,
				_("Cannot open %s for writing"), the_filename);
			gtk_dialog_run (GTK_DIALOG (error_dialog));
			gtk_widget_destroy (error_dialog);	
			ok = FALSE;
		}
	}
	if (!ok) return;
	main_window->do_export_to_stream(pdffile, STREAM_FILE_TYPE_PDF);
	fclose(pdffile);

}
#endif


cairo_status_t NedMainWindow::writefunc(void *fdv,  const unsigned char *data, unsigned int length) {
	if (fwrite(data, length, 1, (FILE *) fdv) != 1) {
		NedResource::Abort("Fehler in writefunc\n");
	}
	return CAIRO_STATUS_SUCCESS;
}


void NedMainWindow::do_export_to_stream(FILE *psfile, int stream_file_type) {
	cairo_surface_t* stream_file_surface;
	cairo_t *cr;
	char Str[1024];
	int old_zoom_level = m_current_zoom_level;
	NedChordOrRest *old_selected_chord_or_rest = m_selected_chord_or_rest;
	NedNote *old_selected_note = m_selected_note;
	bool old_paint_colored = m_paint_colored;
	m_paint_colored = false;
	m_selected_chord_or_rest = NULL;
	m_selected_note = NULL;
	m_draw_postscript = true;



	if (NedResource::isPlaying()) return;

	if (stream_file_type == STREAM_FILE_TYPE_PS) {
		if (m_portrait) {
			stream_file_surface = cairo_ps_surface_create_for_stream(writefunc, (void *) psfile, m_current_paper->width, m_current_paper->height);
		}
		else {
			stream_file_surface = cairo_ps_surface_create_for_stream(writefunc, (void *) psfile, m_current_paper->height, m_current_paper->width);
	
		}
		cairo_ps_surface_dsc_comment(stream_file_surface, "%%Creator: nted by J.Anders (ja@informatik.tu-chemnitz.de\n");
		if (m_current_filename[0] == '\0') {
			cairo_ps_surface_dsc_comment(stream_file_surface, "%%Title: Music\n");
		}
		else {
			sprintf(Str, "%%Title: %s\n", m_current_filename);
			cairo_ps_surface_dsc_comment(stream_file_surface, Str);
		}
		cairo_ps_surface_dsc_begin_setup(stream_file_surface);
		cairo_ps_surface_dsc_begin_page_setup (stream_file_surface);
	}
	else {
		if (m_portrait) {
			stream_file_surface = cairo_pdf_surface_create_for_stream(writefunc, (void *) psfile, m_current_paper->width, m_current_paper->height);
		}
		else {
			stream_file_surface = cairo_pdf_surface_create_for_stream(writefunc, (void *) psfile, m_current_paper->height, m_current_paper->width);
	
		}
	}
	cr = cairo_create (stream_file_surface);
	m_current_zoom_level = 4;

	m_current_zoom = PS_ZOOM2;
	adjustAfterZoom();
	computeSystemIndent();
	computeScoreTextExtends();
	draw(cr, m_current_paper->width, m_current_paper->height);
	cairo_surface_destroy (stream_file_surface);
	cairo_destroy (cr);

	m_draw_postscript = false;
	m_paint_colored = old_paint_colored;
	m_selected_chord_or_rest = old_selected_chord_or_rest;
	m_selected_note = old_selected_note;
	m_current_zoom_level = old_zoom_level;
	m_current_zoom = NedResource::getZoomFactor(m_current_zoom_level);
	adjustAfterZoom();
	computeSystemIndent();
	computeScoreTextExtends();

}

void NedMainWindow::new_window(GtkWidget  *widget, void *data) {
	NedMainWindow *ned_main_window = new NedMainWindow(); // the new window registers itself in NedResource::m_main_windows list
#ifdef YELP_PATH
	ned_main_window->createLayout(NULL, NedResource::m_yelparg);
#else
	ned_main_window->createLayout(NULL, NULL);
#endif

}

void NedMainWindow::create_excerpt(GtkWidget  *widget, void *data) {
	GList *lptr;
	struct addr_ref_str *addrlist = NULL, *slurlist = NULL, *addr_ptr;
	SpecialMeasure *spec_measure;
	NedMainWindow *main_window = (NedMainWindow *) data;
	bool *staves = (bool *) alloca(main_window->m_staff_count * sizeof(bool));
	bool state, correct_layout = false;
	int i, j;


	NedStaffSelectDialog *staff_select_dialog = new NedStaffSelectDialog(GTK_WINDOW(main_window->m_main_window), _("select staves"), main_window, staves);
	staff_select_dialog->getValues(&state);
	delete staff_select_dialog;
	if (!state)  return;


	NedMainWindow *ned_main_window = new NedMainWindow(); // the new window registers itself in NedResource::m_main_windows list
	ned_main_window->m_staff_count = main_window->m_staff_count;
	ned_main_window->m_global_spacement_fac = main_window->m_global_spacement_fac;
	ned_main_window->m_numerator = main_window->m_numerator;
	ned_main_window->m_denominator = main_window->m_denominator;
	ned_main_window->m_timesig_symbol = main_window->m_timesig_symbol;
	ned_main_window->m_midi_tempo_inverse = main_window->m_midi_tempo_inverse;
	ned_main_window->m_upbeat_inverse = main_window->m_upbeat_inverse;
	ned_main_window->m_score_info = main_window->m_score_info;
	ned_main_window->m_first_page_yoffs = main_window->m_first_page_yoffs;
	ned_main_window->m_current_paper = main_window->m_current_paper;
	ned_main_window->m_current_scale = main_window->m_current_scale;
	ned_main_window->m_portrait = main_window->m_portrait;
	for (lptr = g_list_first(main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) {
		if ((spec_measure = (SpecialMeasure *) g_try_malloc(sizeof(SpecialMeasure))) == NULL) {
			NedResource::Abort("NedMainWindow::create_excerpt: malloc");
		}
		*spec_measure = (*((SpecialMeasure *) lptr->data));
		ned_main_window->m_special_measures = g_list_append(ned_main_window->m_special_measures, spec_measure);
	}
#ifdef YELP_PATH
	ned_main_window->createLayout(NULL, NedResource::m_yelparg);
#else
	ned_main_window->createLayout(NULL, NULL);
#endif
	for (i = j = 0; i < main_window->m_staff_count; i++) {
		if (!staves[i]) {
			ned_main_window->m_staff_count--;
			correct_layout = true;
			continue;
		}
		ned_main_window->m_staff_contexts[j] = main_window->m_staff_contexts[i];
		if (main_window->m_staff_contexts[i].m_staff_name != NULL) {
			ned_main_window->m_staff_contexts[j].m_staff_name = main_window->m_staff_contexts[i].m_staff_name->clone();
		}
		if (ned_main_window->m_staff_contexts[j].m_staff_short_name != NULL) {
			ned_main_window->m_staff_contexts[j].m_staff_short_name = main_window->m_staff_contexts[i].m_staff_short_name->clone();
		}
		if (main_window->m_staff_contexts[i].m_group_name != NULL) {
			ned_main_window->m_staff_contexts[j].m_group_name = main_window->m_staff_contexts[i].m_group_name->clone();
		}
		if (ned_main_window->m_staff_contexts[j].m_group_short_name != NULL) {
			ned_main_window->m_staff_contexts[j].m_group_short_name = main_window->m_staff_contexts[i].m_group_short_name->clone();
		}
		j++;
	}
	delete (NedPage *) g_list_first(ned_main_window->m_pages)->data;
	g_list_free(ned_main_window->m_pages);
	ned_main_window->m_pages = NULL;
	for (lptr = g_list_first(main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		ned_main_window->m_pages = g_list_append(ned_main_window->m_pages, ((NedPage *) lptr->data)->clone(&addrlist, &slurlist, ned_main_window, staves));
	}
	for (lptr = g_list_first(ned_main_window->m_pages); lptr; lptr = g_list_next(lptr)) {
		((NedPage *) lptr->data)->adjust_pointers(addrlist, slurlist);
	}
	while (addrlist != NULL) {
		addr_ptr = addrlist->next;
		g_free(addrlist);
		addrlist = addr_ptr;
	}
	while (slurlist != NULL) {
		addr_ptr = slurlist->next;
		g_free(slurlist);
		slurlist = addr_ptr;
	}
	if (correct_layout) {
		for (i = 0; i < ned_main_window->m_staff_count; i++) {
			ned_main_window->m_staff_contexts[i].m_flags = 0;
		}
	}
	ned_main_window->computeSystemIndent();
	ned_main_window->m_topy = ned_main_window->m_leftx = 0;
	NedCommandList *command_list = new NedCommandList(ned_main_window);
	ned_main_window->setNewMeasureSpread(ned_main_window->m_global_spacement_fac);
	ned_main_window->setAndUpdateClefTypeAndKeySig();
	ned_main_window->setAllUnpositioned();
	ned_main_window->reposit(command_list);
	ned_main_window->zoomFreeReplaceables(ned_main_window->m_current_zoom, ned_main_window->getCurrentScale());
	ned_main_window->setAndUpdateClefTypeAndKeySig();
	ned_main_window->setAllUnpositioned();
	ned_main_window->reposit(command_list);
	ned_main_window->updatePageCounter();
}

bool NedMainWindow::close_window2(GtkWidget *widget, GdkEvent  *event, gpointer data) {
	close_window(widget, data);
	return FALSE;
}

void NedMainWindow::close_window(GtkWidget  *widget, void *data) {
	GList *lptr;
	bool stop_closing = false, do_save = false;
	GtkWidget *unsaved_dialog;


	if (NedResource::isPlaying()) return;

	NedMainWindow *main_window = (NedMainWindow *) data;

	if ((lptr = g_list_find(NedResource::m_main_windows, main_window)) == NULL) {
		NedResource::Abort("NedMainWindow::close_window");
	}

	if (main_window->m_config_changed || main_window->m_command_history->unsavedOperations()) {
		unsaved_dialog = gtk_message_dialog_new (GTK_WINDOW(main_window->m_main_window),
			(GtkDialogFlags) (GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL),
				GTK_MESSAGE_WARNING,
				GTK_BUTTONS_YES_NO,
				_("Save Changes ?"));
		gtk_dialog_add_buttons(GTK_DIALOG (unsaved_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
		switch (gtk_dialog_run (GTK_DIALOG (unsaved_dialog))) {
			case GTK_RESPONSE_YES: do_save = true; break;
			case GTK_RESPONSE_REJECT: stop_closing = true; break;
		}
		gtk_widget_destroy (unsaved_dialog);	
		if (stop_closing) {
			return;
		}
		if (do_save) {
			save_score(main_window->m_main_window, data);
		}
	}

	NedResource::m_main_windows = g_list_delete_link(NedResource::m_main_windows, lptr);
	if (g_list_length(NedResource::m_main_windows) < 1) {
		NedResource::cleanup();
	}
	if (NedResource::m_recorded_staff != NULL && NedResource::m_recorded_staff->getMainWindow() == main_window) {
		NedResource::m_recorded_staff = NULL;
	}
	delete main_window;
	if (g_list_length(NedResource::m_main_windows) < 1) {
		gtk_main_quit();	
	}
}


int main (int argc, char **argv)
{

#ifdef YELP_PATH
	const gchar* const *langnames;
	const gchar* const *cptr;
	char Str[1024];
#endif
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
	NedMainWindow *ned_main_window = new NedMainWindow(); // the new window registers itself in NedResource::m_main_windows list

	if (argc > 2) {
		fprintf(stderr, _("Usage: %s [<filename>.ntd]\n"), argv[0]);
		exit(1);
	}


	gtk_init (&argc, &argv);

#ifdef YELP_PATH
	langnames = g_get_language_names ();
	for (cptr = langnames; *cptr != NULL; cptr++) {
		sprintf(Str, "%s/%s/index.docbook", NTED_HTMLDIR, *cptr);
		if (!access(Str, R_OK)) {
			NedResource::m_yelparg = strdup(Str);
			break;
		}
	}
	if (NedResource::m_yelparg == NULL) {
		sprintf(Str, "%s/en/index.docbook", NTED_HTMLDIR);
		if (!access(Str, R_OK)) {
			NedResource::m_yelparg = strdup(Str);
		}
	}
#endif
	

	
	if (setlocale(LC_NUMERIC, "C") == NULL) {
		if (setlocale(LC_NUMERIC, "POSIX") == NULL) {
			NedResource::Warning(_("unable to set decimal point to '.'. PostScript output will be erroneous. \
			Please set the $LANG environment variable appropriately!"));
		}
	}
	NedResource::initialize();
#ifdef YELP_PATH
	ned_main_window->createLayout((argc == 2) ? argv[1] : NULL, NedResource::m_yelparg);
#else
	ned_main_window->createLayout((argc == 2) ? argv[1] : NULL, NULL);
#endif
	gtk_main ();
    
	return 0;
}


bool SpecialMeasure::isNear(double x, double y) {
	//if (type & REP_TYPE_MASK != REPEAT_CLOSE) return false;
	if ((type & START_TYPE_MASK) == 0) return false;
	if (measure == NULL) {
		NedResource::DbgMsg(DBG_CRITICAL, "0x%x: type = 0x%x, measnum = %d measure = 0x%x, (0x%x)\n", this, type, measure_number, measure, &measure);
		NedResource::Abort("SpecialMeasure::isNear");
	}
	return measure->nearRepeatElement(x, y);
}

int SpecialMeasure::compare_spec_measures(const SpecialMeasure *a, const SpecialMeasure *b) {
	if (a->measure_number < b->measure_number) return -1;
	if (a->measure_number == b->measure_number) return 0;
	return 1;
}
