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
AC_INIT([pidgin_apertium_plugin], [0.1.0], [sbalbp@gmail.com])
AC_CONFIG_SRCDIR([src/translator.c])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

# Checks for programs.
AC_PROG_CC

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([doxygen not found - continuing without Doxygen support])
fi

AC_CHECK_PROGS([PDFLATEX], [pdflatex])
if test -z "$PDFLATEX";
   then AC_MSG_WARN([pdflatex not found - continuing without pdflatex support])
fi

AC_CHECK_PROGS([PYTHONCNF], [python2.7-config])
if test -z "$PYTHONCNF";
   then AC_MSG_WARN([python-config not found - continuing without python-config])
fi

AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])

AM_CONDITIONAL([HAVE_PDFLATEX], [test -n "$PDFLATEX"])

AM_CONDITIONAL([HAVE_PYTHONCNF], [test -n "$PYTHONCNF"])

# Checks for libraries.
AC_CHECK_LIB(purple,main,,AC_MSG_ERROR(Cannot find required library purple.))
AC_CHECK_LIB(glib-2.0,main,,AC_MSG_ERROR(Cannot find required library glib-2.0.))
AC_CHECK_LIB(python2.7,main,,AC_MSG_ERROR(Cannot find required library python2.7.))

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
