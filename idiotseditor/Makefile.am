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

AM_CPPFLAGS = $(FT2_CFLAGS) -I$(top_srcdir) -I$(top_srcdir)/commands $(CAIRO_CFLAGS) $(GTK_CFLAGS)

noinst_LIBRARIES = libidiotseditor.a
libidiotseditor_a_SOURCES = idiotseditor.cpp idcommandhistory.cpp idcommandlist.cpp changeidnotecommand.cpp
libidiotseditor_a_SOURCES += idiotseditor.h idcommandhistory.h idcommandlist.h changeidnotecommand.h
