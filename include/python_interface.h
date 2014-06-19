/*
 * Pidgin Translator Plugin.
 *
 * Copyright (C) 2014 Sergio Balbuena <sbalbp@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Python.h>
#include "notifications.h"

void pythonInit(const char* filename);

void pythonFinalize(void);

char* getAPYAddress(void);

int setAPYAddress(char* address, char* port, int force);

int dictionaryHasUser(const char* user, const char* direction);

char* dictionaryGetUserLanguage(const char* user, const char* direction, const char* key);

int dictionarySetUserEntry(const char* user, const char* direction, const char* source, const char* target);

int dictionaryRemoveUserEntry(const char* user, char* entry);

int dictionaryRemoveUserEntries(const char* user);

PyObject* getDictionary(void);

void setDictionary(PyObject* item);

void saveDictionary(void);

int getAllPairs(char**** pairList);

int pairExists(char* source, char* target);

char* translate(char* text, char* source, char* target);
