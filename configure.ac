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

#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT(pidgin_apertium_plugin, 0.1.0, sbalbp@gmail.com)
AM_INIT_AUTOMAKE(pidgin_apertium_plugin, 0.1.0)
AC_CONFIG_SRCDIR([src/py2/translator.c],[src/py3/translator.c])

# Checks for programs.
AC_PROG_CC

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AC_CHECK_PROGS([PDFLATEX], [pdflatex])
if test -z "$PDFLATEX";
   then AC_MSG_WARN([pdflatex not found - continuing without pdflatex support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AM_CONDITIONAL([HAVE_PDFLATEX], [test -n "$PDFLATEX"])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memmove strstr])

AC_PROG_MKDIR_P

AM_PROG_CC_C_O

AC_OUTPUT(Makefile src/Makefile doc/Makefile)
