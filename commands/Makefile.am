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

AM_CPPFLAGS = $(FT2_CFLAGS) -I$(top_srcdir) $(CAIRO_CFLAGS) $(GTK_CFLAGS)

noinst_LIBRARIES = libcommands.a
libcommands_a_SOURCES = commandhistory.cpp commandlist.cpp notemovecommand.cpp \
	deletechordcommand.cpp insertchordorrestcommand.cpp ereasechordorrestcommand.cpp \
	addnoteatcommand.cpp movechordsandrestscommand.cpp  untieforwardcommand.cpp \
	ereasenotecommand.cpp changenotestatus.cpp changechordorrestlength.cpp \
	movechordsandrestsreversecommand.cpp changexpositioncommand.cpp \
	deletesystemcommand.cpp movesystemcommand.cpp changespacement.cpp\
	deletechordorrestgroupcommand.cpp tienotescommand.cpp appendnewpagecommand.cpp appendsystemcommand.cpp \
	getsystemfromnextpagecmmand.cpp removelastpagecommand.cpp appendstaffcommand.cpp deletestaffcommand.cpp \
	shiftstaffcommand.cpp paperconfigcommand.cpp notemoverelativecommand.cpp settotupletcommand.cpp \
	setstaffcontext.cpp insertchordorrestgroupcommand.cpp insertnewpagecommand.cpp changemeasuretypecommand.cpp \
	movespecmeasurecommand.cpp changemeasuretimsigcommand.cpp inserttiedelementcommand.cpp \
	changetimsigcommand.cpp removestaffelemcommand.cpp insertstaffelemcommand.cpp \
	newlyricscommand.cpp deletelyricscommand.cpp changenotehead.cpp removepagecommand.cpp \
	changechordorreststatus.cpp changeupbeatcommand.cpp insertfreeplaceablecommand.cpp fixmovedfreeplaceablecommand.cpp \
	removefreeplaceablecommand.cpp changetie.cpp fixintermediatecommand.cpp flipvaluescommand.cpp \
	removeconstraintscommand.cpp changemeasurespread.cpp changestaffkeysigcmd.cpp transposecommand.cpp \
	changestaffclefcmd.cpp restmovecommand.cpp changeaccidentalcmd.cpp changelyricscommand.cpp

libcommands_a_SOURCES += addnoteatcommand.h appendnewpagecommand.h appendstaffcommand.h appendsystemcommand.h \
	changechordorrestlength.h changechordorreststatus.h changemeasuretimsigcommand.h changemeasuretypecommand.h \
	changenotehead.h changenotestatus.h changetimsigcommand.h changeupbeatcommand.h changexpositioncommand.h command.h \
	commandhistory.h commandlist.h deletechordcommand.h deletechordorrestgroupcommand.h deletelyricscommand.h \
	deletestaffcommand.h deletesystemcommand.h ereasechordorrestcommand.h ereasenotecommand.h fixmovedfreeplaceablecommand.h \
	getsystemfromnextpagecmmand.h insertchordorrestcommand.h insertchordorrestgroupcommand.h insertfreeplaceablecommand.h \
	insertnewpagecommand.h insertstaffelemcommand.h inserttiedelementcommand.h changetie.h movechordsandrestscommand.h \
	movechordsandrestsreversecommand.h movespecmeasurecommand.h movesystemcommand.h newlyricscommand.h notemovecommand.h \
	notemoverelativecommand.h paperconfigcommand.h removefreeplaceablecommand.h removelastpagecommand.h removepagecommand.h \
	removestaffelemcommand.h setstaffcontext.h settotupletcommand.h shiftstaffcommand.h tienotescommand.h \
	untieforwardcommand.h fixintermediatecommand.h flipvaluescommand.h removeconstraintscommand.h changemeasurespread.h \
	changespacement.h changestaffkeysigcmd.h transposecommand.h changestaffclefcmd.h restmovecommand.h changeaccidentalcmd.h \
	changelyricscommand.h
