#########################################################################################
#											#
# This program is free software; you can redistribute it and#or modify it under the	#
# terms of the GNU General Public License as published by the Free Software		#
# Foundation; either version 2 of the License, or (at your option) any later version.	#
#											#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY	#
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A	#
# PARTICULAR PURPOSE. See the GNU General Public License for more details.		#
#											#
# You should have received a copy of the GNU General Public License along with this	#
# program; (See "COPYING"). If not, If not, see <http://www.gnu.org/licenses/>.         #
#											#
#---------------------------------------------------------------------------------------#
#											#
# Copyright	Joerg Anders, TU Chemnitz, Fakultaet fuer Informatik, GERMANY		#
#		ja@informatik.tu-chemnitz.de						#
#											#
#											#
#########################################################################################

SUFFIXES = .png

PNGS = n32ndnote_icon.png  n64thnote_icon.png  eightnote_icon.png  fullnote_icon.png \
	halfnote_icon.png  quarternote_icon.png  sixteenthnote_icon.png \
	shift_mode_icon.png select_mode_icon.png rest_icon.png tied_icon.png \
	dot_icon.png cross_icon.png flat_icon.png natural_icon.png insert_mode_icon.png \
	arrow_down_icon.png arrow_up_icon.png delete_staff_icon.png delete_system_icon.png color_icon.png \
	alto_clef_icon.png  bass_clef_icon.png treble_clef_icon.png lyrics_icon.png \
	stroken_grace_icon.png grace_eighth_icon.png grace_sixth_icon.png \
	normal_note_icon.png drum1_icon.png  drum2_icon.png drum3_icon.png \
	drum4_icon.png drum5_icon.png drum6_icon.png drum7_icon.png drum8_icon.png \
	drum9_icon.png drum10_icon.png drum11_icon.png drum12_icon.png brace_icon.png bracket_icon.png  \
	ddot_icon.png dcross_icon.png dflat_icon.png stacc_icon.png \
	str_pizz_icon.png tenuto_icon.png sforzato_icon.png sforzando_icon.png \
	bow_up_icon.png bow_down_icon.png ped_off_icon.png  ped_on_icon.png \
	trill_icon.png prall_icon.png open_icon.png mordent_icon.png staccatissimo_icon.png \
	lines.png signs.png fermata_icon.png arpeggio_icon.png nted.png key_icon.png \
	nted.png isolate_icon.png groupbeams_icon.png flipstem_icon.png \
	background1.png background2.png guitar_note_icon.png guitar_note_no_stem_icon.png \
	backgroundv1.png backgroundv2.png backgroundh2.png backgroundh1.png \
	arrow_left_icon.png arrow_right_icon.png tool_icon.png comm_icon.png cut_icon.png \
	midikeyboard_icon.png record_icon.png revturn_icon.png  turn_icon.png \
	drum10_icon_small.png  drum12_icon_small.png  drum2_icon_small.png  drum4_icon_small.png  drum6_icon_small.png  drum8_icon_small.png \
	drum11_icon_small.png  drum1_icon_small.png   drum3_icon_small.png  drum5_icon_small.png  drum7_icon_small.png  drum9_icon_small.png \
	normal_note_icon_small.png \
	eightnote_dot_icon.png	halfnote_dot_icon.png  quarternote_dot_icon.png  sixteenthnote_dot_icon.png \
	tremolo_icon1.png  tremolo_icon2.png  tremolo_icon3.png  tremolo_icon4.png





EXTRA_DIST = $(PNGS)


icondir = $(datadir)/pixmaps
dist_icon_DATA = nted.png

IMAGE2HEADER = @IMAGE2HEADER@
PIXMAPHEADERS = $(PNGS:.png=.h)

noinst_HEADERS = $(PIXMAPHEADERS)

.png.h:
	rm -f $@
	$(IMAGE2HEADER) --name $* $< > $@

CLEANFILES = $(PIXMAPHEADERS)
