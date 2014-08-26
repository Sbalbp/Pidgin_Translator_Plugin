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
 * @brief Describes the different ways in which information text can be displayed
 */
typedef enum {DIALOG, PRINT, NONE} info_display_mode;

/**
 * @brief The plugin handle
 *
 * Its use is needed for plugin notification calls
 */
PurplePlugin *translator_plugin_handle = NULL;

/**
 * @brief The current conversation
 *
 * This is the ocnversation where notification will be displayed on
 */
PurpleConversation *current_conversation = NULL;

/**
 * @brief Indicates how information text is displayed
 */
info_display_mode info_mode = DIALOG;

/**
 * @brief Indicates whether or not error messages should be shown
 */
int errors_on = 1;

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
 * @brief Sets the current conversation variable
 *
 * @param conversation New conversation to assign
 */
void set_conversation(PurpleConversation* conversation){
	current_conversation = conversation;
}

/**
 * @brief Sets the display mode for information
 *
 * @param mode String containing the information display mode to be set
 * @return 1 on success or 0 otherwise
 */
int set_info_display_mode(const char* mode){
	if(strcmp(mode,"dialog") == 0){
		info_mode = DIALOG;
		return 1;
	}
	if(strcmp(mode,"print") == 0){
		info_mode = PRINT;
		return 1;
	}
	if(strcmp(mode,"none") == 0){
		info_mode = NONE;
		return 1;
	}

	return 0;
}

/**
 * @brief Activates error notification messages
 */
void notifications_on(void){
	errors_on = 1;
}

/**
 * @brief Deactivates error notification messages
 */
void notifications_off(void){
	errors_on = 0;
}

/**
 * @brief Displays an information notification on the current conversation
 *
 * The plugin handle must have been set (set_translator_plugin called) before calling this function
 * @param text Text string containing the main body of the notification
 */
void notify_info(const char* text){
	time_t current;
    time (&current);

    if(current_conversation != NULL){
    	purple_conversation_write(current_conversation, "Information", text, PURPLE_MESSAGE_NOTIFY, current);
    }
}

/**
 * @brief Displays an information notification inside a pop-up window
 *
 * The plugin handle must have been set (set_translator_plugin called) before calling this function
 * @param title Text string containing the title for the notification
 * @param text Text string containing the main body of the notification
 */
void notify_info_popup(const char* title, const char* text){
	time_t current;
    time (&current);

	switch(info_mode){
		case DIALOG:
			purple_notify_message (translator_plugin_handle, PURPLE_NOTIFY_MSG_INFO,
				"Apertium plugin information", title, text,NULL,NULL);
			break;
		case PRINT:
			if(current_conversation != NULL){
    			purple_conversation_write(current_conversation, title, text, PURPLE_MESSAGE_NOTIFY, current);
    		}
			break;
		case NONE:
			break;
	}
}

/**
 * @brief Displays an error notification on the current conversation
 *
 * The plugin handle must have been set (set_translator_plugin called) before calling this function
 * @param text Text string containing the main body of the notification
 */
void notify_error(const char* text){
	time_t current;
    time (&current);

    if(current_conversation != NULL && errors_on){
    	purple_conversation_write(current_conversation, "Error", text, PURPLE_MESSAGE_ERROR, current);
	}
}

/**
 * @brief Displays an error notification inside a pop-up window
 *
 * The plugin handle must have been set (set_translator_plugin called) before calling this function
 * @param text Text string containing the main body of the notification (generally, the cause of the error)
 */
void notify_error_popup(const char* text){
	time_t current;
    time (&current);

	if(errors_on){
		switch(info_mode){
			case DIALOG:
				purple_notify_message (translator_plugin_handle, PURPLE_NOTIFY_MSG_ERROR,
					"Apertium plugin error", text, NULL,NULL,NULL);
				break;
			case PRINT:
				if(current_conversation != NULL){
    				purple_conversation_write(current_conversation, "Error", text, PURPLE_MESSAGE_ERROR, current);
				}
				break;
			case NONE:
				break;
		}
	}
}

