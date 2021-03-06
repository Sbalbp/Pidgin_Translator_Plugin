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

#include "notify.h"

void set_translator_plugin(PurplePlugin* plugin);

void set_conversation(PurpleConversation* conversation);

int set_info_display_mode(const char* mode);

void notifications_on(void);

void notifications_off(void);

void notify_info(const char* text);

void notify_info_popup(const char* title, const char* text);

void notify_error(const char* text);

void notify_error_popup(const char* text);

