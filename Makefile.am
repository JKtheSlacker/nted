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

SUBDIRS = commands idiotseditor dialogs chords icons . po datafiles doc man

dist_doc_DATA =
dist_doc_DATA += AUTHORS
dist_doc_DATA += ABOUT_THE_EXAMPLES.TXT
dist_doc_DATA += FAQ

AM_CPPFLAGS =
AM_CPPFLAGS += -DNTED_HTMLDIR=\"${htmldir}\"
AM_CPPFLAGS += -DNTED_DATADIR=\"${datarootdir}/${PACKAGE_TARNAME}\"
AM_CPPFLAGS += -DLOCALEDIR=\"${localedir}\"
AM_CPPFLAGS += -I$(top_srcdir)/dialogs -I$(top_srcdir)/commands -I$(top_srcdir)/chords -I$(top_srcdir)/idiotseditor
AM_CPPFLAGS += $(FT2_CFLAGS) $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(PANGO_CFLAGS)

bin_PROGRAMS = nted
nted_SOURCES = mainwindow.cpp resource.cpp page.cpp system.cpp staff.cpp \
	voice.cpp chordorrest.cpp note.cpp musicxmlimport.cpp \
	beam.cpp tuplet.cpp clipboard.cpp midiexport.cpp measure.cpp \
	freereplaceable.cpp volumesign.cpp temposign.cpp slurpoint.cpp \
	slur.cpp linepoint.cpp line.cpp crescendo.cpp octavation.cpp \
	line3.cpp acceleration.cpp freetext.cpp freesign.cpp \
	importer.cpp positionarray.cpp freechord.cpp freespacer.cpp \
	pangocairotext.cpp freechordname.cpp midiimporter.cpp midirecorder.cpp

nted_SOURCES += acceleration.h beam.h beaming.h chordorrest.h \
	clipboard.h crescendo.h freereplaceable.h freesign.h freetext.h \
	importer.h line3.h line.h linepoint.h localization.h \
	mainwindow.h measure.h midiexport.h musicxmlimport.h note.h\
	octavation.h page.h resource.h slur.h slurpoint.h staff.h system.h \
	temposign.h tuplet.h voice.h volumesign.h positionarray.h dynarray.h COPYING.GFDL COPYING.FONT.TXT \
	freechord.h freespacer.h pangocairotext.h freechordname.h midiimporter.h midirecorder.h

nted_LDADD = commands/libcommands.a dialogs/libdialogs.a chords/libchords.a idiotseditor/libidiotseditor.a $(CAIRO_LIBS) $(PANGO_LIBS) $(GTK_LIBS) $(X11_LIBS) -lasound


cleanup: maintainer-clean cleanup-local

distclean-local:
	rm -f po/stamp-po

# Clean up all generated files. You can re-create them with autoreconf.
cleanup-local:
	find . -name Makefile | xargs rm -f
	find $(srcdir) -name Makefile.in | xargs rm -f
	rm -f ./config.status ./config.log
	cd $(srcdir) && rm -f config.sub config.guess config.rpath
	cd $(srcdir) && rm -f aclocal.m4 configure config.h.in
	cd $(srcdir) && rm -f depcomp install-sh missing mkinstalldirs
	rm -f ./config.h
	find . -type d -name .deps -exec rm -fr {} \; -prune
	rm -f stamp-h1
	rm -rf $(srcdir)/autom4te.cache
	cd $(srcdir) && rm -rf m4
	cd $(srcdir)/po && rm -f boldquot.sed en@boldquot.header en@quot.header insert-header.sin quot.sed remove-potcdate.sin Rules-quot
	cd $(srcdir)/po && rm -f Makefile.in.in Makevars.template
	cd $(srcdir) && rm -f ABOUT-NLS

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath INSTALL README
