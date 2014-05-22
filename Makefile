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

GLIB_CFLAGS = `pkg-config --libs --cflags glib-2.0`
LIBPURPLE = -I ~/development/pidgin-2.10.9/libpurple
LIBPYTHON = `pkg-config --libs --cflags python2`
INC = include
SRC = src
OBJ = obj
SO = so
PLUGIN_DIR = ~/.purple/plugins

all: $(PLUGIN_DIR) $(SO) $(SO)/translator.so
	cp -fv $(SO)/translator.so $(PLUGIN_DIR)/translator.so

$(PLUGIN_DIR):
	mkdir -p $(PLUGIN_DIR)

$(SO):
	mkdir -p $(SO)

$(SO)/translator.so: $(OBJ) $(SRC)/translator.c $(OBJ)/python_interface.o $(OBJ)/notifications.o
	gcc -shared -o $(SO)/translator.so $(SRC)/translator.c $(OBJ)/python_interface.o $(OBJ)/notifications.o -I $(INC) $(LIBPYTHON) $(LIBPURPLE) $(GLIB_CFLAGS)

$(OBJ):
	mkdir -p $(OBJ)

$(OBJ)/python_interface.o: $(SRC)/python_interface.c $(INC)/python_interface.h
	gcc -c -o $(OBJ)/python_interface.o $(SRC)/python_interface.c -I $(INC) $(LIBPYTHON)


$(OBJ)/notifications.o: $(SRC)/notifications.c $(INC)/notifications.h
	gcc -c -o $(OBJ)/notifications.o $(SRC)/notifications.c -I $(INC) $(LIBPURPLE) $(GLIB_CFLAGS)

