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

/**
 * @file notifications.c
 * @brief Functions for notifying information or errors to the user
 */

#include "notifications.h"

/**
 * @brief The plugin handle
 *
 * Its use is needed for plugin notification calls
 */
PurplePlugin *translator_plugin_handle = NULL;

/**
 * @brief Sets the plugin handle variable
 *
 * This function must be called before any other in this file, or else the handle will be NULL, producing an error
 * @param plugin Plugin handle to assign
 */
void set_translator_plugin(PurplePlugin* plugin){
	translator_plugin_handle = plugin;
}

/**
 * @brief Displays an information notification
 *
 * The plugin handle must have been set (set_translator_plugin called) before calling this function
 * @param title Text string containing the title for the notification
 * @param text Text string containing the main body of the notification
 */
void notify_info(const char* title, const char* text){
	purple_notify_message (translator_plugin_handle, PURPLE_NOTIFY_MSG_INFO,
			"Apertium plugin information", title, text,NULL,NULL);
}

/**
 * @brief Displays an error notification
 *
 * The plugin handle must have been set (set_translator_plugin called) before calling this function
 * @param text Text string containing the main body of the notification (generally, the cause of the error)
 */
void notify_error(const char* text){
	purple_notify_message (translator_plugin_handle, PURPLE_NOTIFY_MSG_ERROR,
			"Apertium plugin error", text, NULL,NULL,NULL);
}