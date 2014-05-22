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
 * @file translator.c
 * @brief Functions and signal bindings for the plugin
 */

#define PURPLE_PLUGINS

#define PLUGIN_ID "core-sbalbp-apertium_translator"

#include "python_interface.h"
#include <string.h>
#include <glib.h>
#include "notifications.h"
#include "plugin.h"
#include "debug.h"
#include "signals.h"
#include "request.h"
#include "cmds.h"
#include "version.h"

/**
 * @brief ID for the 'apertium_set' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId set_command_id;

/**
 * @brief ID for the 'apertium_check' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId check_command_id;

/**
 * @brief ID for the 'apertium_pairs' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId pairs_command_id;

/****************************************************************************************************/
/*----------------------------------------------UTILS-----------------------------------------------*/
/****************************************************************************************************/

/**
 * @brief Translates a text message
 *
 * *message is reallocated and modified to contain both the original message and its translation
 * @param message Reference to the text string to be translated
 * @param dictionary PyObject containing the dictionary with the user-language_pair bindings
 * @param buddy Buddy to check user-language_pair binding for
 * @param key String indicating which entry to check in the dictionary. Must be "incoming" or "outgoing"
 */
void translate_message(char **message, PyObject *dictionary, PurpleBuddy *buddy, const char *key){
    PyObject *translation, *user;

    if(PyDict_Contains(PyDict_GetItemString(dictionary, key), PyString_FromString(purple_buddy_get_name(buddy)))){
        char* oldMsg = malloc(sizeof(char)*(strlen(*message)+1));
        sprintf(oldMsg,"%s\0",*message);

        user = PyDict_GetItemString(PyDict_GetItemString(dictionary, key),purple_buddy_get_name(buddy));

        translation = translate(*message,
            PyDict_GetItemString(user,"source"),
            PyDict_GetItemString(user,"target"));

        *message = (char*)realloc(*message, sizeof(char)*(strlen(oldMsg)+strlen(PyString_AsString(translation))+21));
        sprintf(*message,"\n%s\n-----------------\n%s\0",oldMsg,PyString_AsString(translation));
    }
}

/**
 * @brief Parses and returns arguments for 'apertium_set' command
 *
 * This function expects exactly 3 arguments. Passing less than 3 arguments in args will result in an error. Arguments after the third one will be ignored
 * @param args String containing the argument for passed to the command (separated by whitespaces)
 * @param command Reference to a string where the 'command' argument will be stored
 * @param source Reference to a string where the 'source language' argument will be stored
 * @param target Reference to a string where the 'target language' argument will be stored
 * @return 1 on success, or 0 otherwise
 */
int parse_set_arguments(char* args, char **command, char** source, char** target){

    if((*command = strtok(args," ")) == NULL){
        notify_error("No command provided");
        return FALSE;
    }

    if(strcmp(*command,"incoming") && strcmp(*command,"outgoing")){
        notify_error("Command for apertium_set must be \'incoming\' or \'outgoing\'");
        return FALSE;
    }

	if((*source = strtok(NULL," ")) == NULL){
        notify_error("No source language provided");
        return FALSE;
    }
	if((*target = strtok(NULL," ")) == NULL){
        notify_error("No target language provided");
        return FALSE;
    }

    return pairExists(*source, *target);
}

/****************************************************************************************************/
/*----------------------------------COMMAND CALLBACK DEFINITIONS------------------------------------*/
/****************************************************************************************************/

/**
 * @brief Callback for the 'apertium_check' command
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_check_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *title, *text;
    PyObject *dictionary;
    PurpleBuddy *buddy;

    if((dictionary = getDictionary()) == Py_None){
        return PURPLE_CMD_RET_FAILED;
    }

    title = malloc(sizeof(char)*100);
    text = malloc(sizeof(char)*300);

    buddy = purple_find_buddy(purple_conversation_get_account(conv), purple_conversation_get_name(conv));

    sprintf(title,"Pairs for \'%s\'\0", purple_buddy_get_name(buddy));

    if(PyDict_Contains(PyDict_GetItemString(dictionary, "incoming"),PyString_FromString(purple_buddy_get_name(buddy)))){
        sprintf(text,"Incoming messages: %s - %s\n",
            PyString_AsString(PyDict_GetItemString(PyDict_GetItemString(PyDict_GetItemString(dictionary, "incoming"),purple_buddy_get_name(buddy)),"source")),
            PyString_AsString(PyDict_GetItemString(PyDict_GetItemString(PyDict_GetItemString(dictionary, "incoming"),purple_buddy_get_name(buddy)),"target")));
    }
    else{
        sprintf(text,"Incoming messages: None\n");
    }
    if(PyDict_Contains(PyDict_GetItemString(dictionary, "outgoing"),PyString_FromString(purple_buddy_get_name(buddy)))){
        sprintf(text,"%sOutgoing messages: %s - %s\0", text,
            PyString_AsString(PyDict_GetItemString(PyDict_GetItemString(PyDict_GetItemString(dictionary, "outgoing"),purple_buddy_get_name(buddy)),"source")),
            PyString_AsString(PyDict_GetItemString(PyDict_GetItemString(PyDict_GetItemString(dictionary, "outgoing"),purple_buddy_get_name(buddy)),"target")));
    }
    else{
        sprintf(text,"%sOutgoing messages: None\0", text);
    }

    notify_info(title, text);

    free(title);
    free(text);

    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_pairs' command
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_pairs_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    int i;
    char *title, *text;
    PyObject *pairsList;

    if((pairsList = getAllPairs()) == Py_None){
        return PURPLE_CMD_RET_FAILED;
    }

    title = malloc(sizeof(char)*100);
    text = malloc(sizeof(char)*300);

    sprintf(title, "available pairs\0");
    sprintf(text,"");


    for(i=0; i<PyList_GET_SIZE(pairsList); i++){
        sprintf(text,"%s%s - %s\n",text,
            PyString_AsString(PyList_GetItem(PyList_GetItem(pairsList,i),0)),
            PyString_AsString(PyList_GetItem(PyList_GetItem(pairsList,i),1)));
    }

    sprintf(text,"%s\0", text);

    notify_info(title, text);

    free(title);
    free(text);

    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_set' command
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_set_cb(PurpleConversation *conv, const gchar *cmd,
								gchar **args, gchar **error, void *data){
	char *command, *source, *target;
	PyObject *sourceTargetDict, *dictionary;
	PurpleBuddy *buddy;

    if(parse_set_arguments(*args,&command,&source,&target)){

    	buddy = purple_find_buddy(purple_conversation_get_account(conv), purple_conversation_get_name(conv));

    	if((sourceTargetDict = PyDict_New())!=NULL){

    		if((dictionary = getDictionary()) == Py_None){
        		return PURPLE_CMD_RET_FAILED;
			}

    		PyDict_SetItemString(sourceTargetDict,"source",PyString_FromString(source));
    		PyDict_SetItemString(sourceTargetDict,"target",PyString_FromString(target));

    		PyDict_SetItemString(
    			PyDict_GetItemString(dictionary, command),
				purple_buddy_get_name(buddy),sourceTargetDict);

    		setDictionary(dictionary);

			Py_XDECREF(sourceTargetDict);

    		return PURPLE_CMD_RET_OK;
    	}
    	else{
    		return PURPLE_CMD_RET_FAILED;
    	}
	}
	else{
		return PURPLE_CMD_RET_FAILED;
	}
}

/****************************************************************************************************/
/*--------------------------------CONVERSATION CALLBACK DEFINITIONS---------------------------------*/
/****************************************************************************************************/

/**
 * @brief Callback called before sending an IM message
 *
 * Attemps to translate the outgoing message if there exists a recipient-language_pair binding. Refer to the libpurple Conversation Signals documentation for more information
 * @param account The account the message is being sent on
 * @param recipient The username of the receiver
 * @param message Reference to the message string. Can be modified
 * @param handle Plugin handle
 */
void sending_im_msg_cb(PurpleAccount *account, char *recipient, char **message, gpointer handle){

    PurpleBuddy *buddy;
    PyObject *dictionary;

    buddy = purple_find_buddy(account, recipient);

    if((dictionary = getDictionary()) == Py_None){
        return;
    }

    translate_message(message, dictionary, buddy, "outgoing");

    return;
}

/**
 * @brief Callback called before receiving an IM message
 *
 * Attemps to translate the incoming message if there exists a sender-language_pair binding. Refer to the libpurple Conversation Signals documentation for more information
 * @param account The account the message was received on
 * @param sender Reference to a string containing the username of the sender
 * @param message Reference to a string containing the message that was sent
 * @param conv The IM conversation
 * @param flags A pointer to the IM message flags
 * @param handle Plugin handle
 */
gboolean receiving_im_msg_cb(PurpleAccount *account, char **sender,
                            char **message, PurpleConversation *conv,
                            PurpleMessageFlags *flags, gpointer handle){

	PurpleBuddy *buddy;
	PyObject *dictionary;

	buddy = purple_find_buddy(account, *sender);

	if((dictionary = getDictionary()) == Py_None){
		return TRUE;
	}

	translate_message(message, dictionary, buddy, "incoming");

	return FALSE;
}

/****************************************************************************************************/
/*----------------------------------------PLUGIN FUNCTIONS------------------------------------------*/
/****************************************************************************************************/

/**
 * @brief Initializes the plugin
 *
 * Sets both the conversation and command bindings. Initializes the Python embedding
 * @param plugin Plugin handle
 * @return TRUE on success, or FALSE otherwise
 */
gboolean plugin_load(PurplePlugin *plugin){

	void *conv_handle = purple_conversations_get_handle();

	set_translator_plugin(plugin);

	/*
	 * Here we bind the different callbacks to the appropriate events.
	 * The events 'sending' and 'receiving' are both fired before actually
	 * sending/receiving the message, which allows us to manipulate it
	 * on the callback and display/send a different message.
	 */
	purple_signal_connect(conv_handle, "sending-im-msg",
						plugin, PURPLE_CALLBACK(sending_im_msg_cb), plugin);
	purple_signal_connect(conv_handle, "receiving-im-msg",
						plugin, PURPLE_CALLBACK(receiving_im_msg_cb), plugin);


    set_command_id = purple_cmd_register("apertium_set", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM, PLUGIN_ID, apertium_set_cb,
        "apertium_set \'direction\' \'source language\' \'target language\'\nSets the source-target language pair to translate messages from/to this user.\n\'direction\' must be \"incoming\" for received messages, or \"outgoing\" for user-sent messages.\n\'source language\' is the language expected to translate messages from.\n\'target language\' is the language to translate messages to.",                   /* Help message */
        NULL);

    check_command_id = purple_cmd_register("apertium_check", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM, PLUGIN_ID, apertium_check_cb,
        "apertium_check\nShows the current language pairs assigned to this contact for both incoming and outgoing messages.",
        NULL);

    pairs_command_id = purple_cmd_register("apertium_pairs", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_pairs_cb,
        "apertium_pairs\nShows all the available Apertium language pairs that can be used.",
        NULL);

	// Python embedding
	pythonInit();

	return TRUE;
}

/**
 * @brief Finalizes the plugin
 *
 * Called on plugin unload. Unregisters the command bindings and finalizes the Python embedding
 * @param plugin Plugin handle
 * @return TRUE on success, or FALSE otherwise
 */
gboolean plugin_unload(PurplePlugin *plugin){

	saveDictionary();

	purple_signals_disconnect_by_handle(plugin);

	purple_cmd_unregister(set_command_id);
    purple_cmd_unregister(check_command_id);
    purple_cmd_unregister(pairs_command_id);

	pythonFinalize();

	return TRUE;
}

static PurplePluginInfo info =
{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,
	NULL,
	0,
	NULL,
	PURPLE_PRIORITY_DEFAULT,

	PLUGIN_ID,
	"Message translator",
	"1.0",
	"Translates incoming messages using Apertium.",
	"Translates incoming messages using Apertium.",
	"Sergio Balbuena <sbalbp@gmail.com>",
	"",

	plugin_load,
	plugin_unload,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL,

	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin){
}

PURPLE_INIT_PLUGIN(apertium_translator, init_plugin, info)