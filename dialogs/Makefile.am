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

AM_CPPFLAGS = $(FT2_CFLAGS) -I$(top_srcdir) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(X11_CFLAGS)

noinst_LIBRARIES = libdialogs.a
libdialogs_a_SOURCES = staffcontextdialog.cpp  portchoosedialog.cpp paperconfigdialog.cpp \
		printconfigdialog.cpp tupletdialog.cpp insertblockdialog.cpp measureconfigdialog.cpp \
		keysigdialog.cpp scoreinfodialog.cpp clefconfigdialog.cpp volumedialog.cpp \
		tempodialog.cpp mutedialog.cpp linesdialog.cpp signsdialog.cpp textdialog.cpp \
		lilypondexportdialog.cpp staffselectdialog.cpp spacementdialog.cpp selectordialog.cpp \
		tools.cpp scaledialog.cpp chordnamedialog.cpp midiimportdialog.cpp  \
		midirecordconfigdialog.cpp druminfo.cpp drumconfigdialog.cpp lyricseditor.cpp
libdialogs_a_SOURCES += clefconfigdialog.h insertblockdialog.h keysigdialog.h linesdialog.h measureconfigdialog.h \
			mutedialog.h paperconfigdialog.h portchoosedialog.h printconfigdialog.h scoreinfodialog.h signsdialog.h \
			staffcontextdialog.h tempodialog.h textdialog.h tupletdialog.h volumedialog.h lilypondexportdialog.h staffselectdialog.h \
			spacementdialog.h selectordialog.h tools.h scaledialog.h chordnamedialog.h midiimportdialog.h \
			midirecordconfigdialog.h druminfo.h drumconfigdialog.h lyricseditor.h
