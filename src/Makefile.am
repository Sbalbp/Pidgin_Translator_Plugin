#
# Pidgin Translator Plugin.
#
# Copyright (C) 2014 Sergio Balbuena <sbalbp@gmail.com>.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

AM_PYV1=2
AM_PURPLE_GLIB_CFLAGS =`pkg-config --libs --cflags purple`

if HAVE_PDFLATEX
AM_PYTHON_CFLAGS =`python$(AM_PYV1)-config --libs --cflags`
else
AM_PYTHON_CFLAGS =`pkg-config --libs --cflags python$(AM_PYV1)`
endif

AM_PY = $(top_builddir)/python
AM_INC = $(top_builddir)/include
AM_SRC = $(top_builddir)/src
AM_OBJ = $(top_builddir)/obj
AM_SO = $(top_builddir)/so
AM_PLUGIN_DIR = ~/.purple/plugins

all-local: $(AM_PLUGIN_DIR) $(AM_SO) $(AM_SO)/translator.so
	cp -fv $(AM_SO)/translator.so $(AM_PLUGIN_DIR)/translator.so

$(PLUGIN_DIR):
	$(MKDIR_P) $(AM_PLUGIN_DIR)

$(AM_SO)/translator.so: $(AM_SO) $(AM_OBJ) $(AM_SRC)/translator.c $(AM_OBJ)/python_interface.o $(AM_OBJ)/notifications.o
	$(CC) -fPIC $(DEFS) -shared -o $(AM_SO)/translator.so $(AM_SRC)/translator.c $(AM_OBJ)/python_interface.o $(AM_OBJ)/notifications.o -I $(AM_INC) $(AM_PYTHON_CFLAGS) $(AM_PURPLE_GLIB_CFLAGS)

$(AM_SO):
	$(MKDIR_P) $(AM_SO)

$(AM_OBJ):
	$(MKDIR_P) $(AM_OBJ)

$(AM_OBJ)/python_interface.o: $(AM_SRC)/python_interface.c $(AM_INC)/python_interface.h
	$(CC) -fPIC -c -o $(AM_OBJ)/python_interface.o $(AM_SRC)/python_interface.c -I $(AM_INC) $(AM_PYTHON_CFLAGS) $(AM_PURPLE_GLIB_CFLAGS)

$(AM_OBJ)/notifications.o: $(AM_SRC)/notifications.c $(AM_INC)/notifications.h
	$(CC) -fPIC -c -o $(AM_OBJ)/notifications.o $(AM_SRC)/notifications.c -I $(AM_INC) $(AM_PURPLE_GLIB_CFLAGS)

clean-local:
	rm -rf $(AM_SO)
	rm -rf $(AM_OBJ)
