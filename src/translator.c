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
 * @brief Describes the different ways in which a translated message can be shown
 */
typedef enum {BOTH, TRANSLATION, COMPRESSED} display_mode;

/**
 * @brief Variable containing the display_mode value that tell the plugin how messages should be shown
 */
display_mode display = COMPRESSED;

/**
 * @brief ID for the 'apertium_bind' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId bind_command_id;

/**
 * @brief ID for the 'apertium_unbind' (without arguments) command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId unbind_noargs_command_id;

/**
 * @brief ID for the 'apertium_unbind' (with arguments) command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId unbind_args_command_id;

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

/**
 * @brief ID for the 'apertium_apy' (without arguments) command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId apy_noargs_command_id;

/**
 * @brief ID for the 'apertium_apy' (with arguments) command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId apy_args_command_id;

/**
 * @brief ID for the 'apertium_apyremove' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId apyremove_command_id;

/**
 * @brief ID for the 'apertium_display' (without arguments) command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId display_noargs_command_id;

/**
 * @brief ID for the 'apertium_display' (with arguments) command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId display_args_command_id;

/**
 * @brief ID for the 'apertium_infodisplay' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId info_display_command_id;

/**
 * @brief ID for the 'apertium_errors' command
 *
 * Used to unregister the command on plugin unload
 */
PurpleCmdId errors_command_id;

/****************************************************************************************************/
/*----------------------------------------------UTILS-----------------------------------------------*/
/****************************************************************************************************/

/**
 * @brief Translates a text message
 *
 * *message is reallocated and modified to contain both the original message and its translation
 * @param message Reference to the text string to be translated
 * @param buddy Buddy to check user-language_pair binding for
 * @param key String indicating which entry to check in the dictionary. Must be "incoming" or "outgoing"
 */
void translate_message(char **message, PurpleBuddy *buddy, const char *key){
    const char *username;
    char *translation;

    username = purple_buddy_get_name(buddy);

    if(dictionaryHasUser(username, key)){
        char* oldMsg = malloc(sizeof(char)*(strlen(*message)+1));
        sprintf(oldMsg,"%s",*message);

        translation = translate(*message,
            dictionaryGetUserLanguage(username, key, "source"),
            dictionaryGetUserLanguage(username, key, "target"));

        if(translation != NULL){
            switch(display){
                case BOTH:
                    *message = (char*)realloc(*message, sizeof(char)*(strlen(oldMsg)+strlen(translation)+100));
                    sprintf(*message,"\n-- Original:\n%s\n-- Translation:\n%s",oldMsg,translation);
                    break;
                case TRANSLATION:
                    *message = (char*)realloc(*message, sizeof(char)*(strlen(translation)+100));
                    sprintf(*message,"%s",translation);
                    break;
                case COMPRESSED:
                    *message = (char*)realloc(*message, sizeof(char)*(strlen(oldMsg)+strlen(translation)+100));
                    sprintf(*message,"%s\n-- Translation: %s",oldMsg,translation);
                    break;
            }
        }

        free(oldMsg);
    }
}

/**
 * @brief Parses and returns arguments for 'apertium_apy' command
 *
 * This function expects 1 to 3 arguments. Passing less than 1 argument in args will result in an error. Arguments after the third one will be ignored
 * @param args String containing the arguments passed to the command (separated by whitespaces)
 * @param order Reference to a integer where the 'order' argument (optional) will be stored
 * @param address Reference to a string where the 'address' argument will be stored
 * @param port Reference to a string where the 'port' argument (optional) will be stored
 * @return 1 on success, or 0 otherwise
 */
int parse_apy_arguments(char* args, int *order, char **address, char **port){

    char *str;

    if((*address = strtok(args," ")) == NULL){
        notify_error("No 'address' argument provided");
        return 0;
    }

    str = malloc(sizeof(char)*100);
    sprintf(str, "%d", atoi(*address));

    if(strcmp(str, *address) == 0){
        free(str);
        *order = atoi(*address);

        if((*address = strtok(NULL," ")) == NULL){
            notify_error("No 'address' argument provided");
            return 0;
        }
    }
    else{
        free(str);
        *order = -1;
    }

    *port = strtok(NULL," ");

    return 1;
}

/**
 * @brief Parses and returns arguments for 'apertium_bind' command
 *
 * This function expects exactly 3 arguments. Passing less than 3 arguments in args will result in an error. Arguments after the third one will be ignored
 * @param args String containing the arguments passed to the command (separated by whitespaces)
 * @param command Reference to a string where the 'command' argument will be stored
 * @param source Reference to a string where the 'source language' argument will be stored
 * @param target Reference to a string where the 'target language' argument will be stored
 * @return 1 on success, or 0 otherwise
 */
int parse_bind_arguments(char* args, char **command, char** source, char** target){

    if((*command = strtok(args," ")) == NULL){
        notify_error("No command provided");
        return 0;
    }

    if(strcmp(*command,"incoming") && strcmp(*command,"outgoing")){
        notify_error("Command for apertium_set must be \'incoming\' or \'outgoing\'");
        return 0;
    }

	if((*source = strtok(NULL," ")) == NULL){
        notify_error("No source language provided");
        return 0;
    }
	if((*target = strtok(NULL," ")) == NULL){
        notify_error("No target language provided");
        return 0;
    }

    return pairExists(*source, *target);
}

/**
 * @brief Parses and returns arguments for 'apertium_unbind' command
 *
 * This function expects 1 argument in args. Additional arguments will be ignored
 * @param args String containing the argument passed to the command
 * @param command Reference to a string where the 'command' argument will be stored
 * @return 1 on success, or 0 otherwise
 */
int parse_unbind_arguments(char* args, char **command){

    if((*command = strtok(args," ")) == NULL){
        return 0;
    }

    if(strcmp(*command,"incoming") && strcmp(*command,"outgoing")){
        notify_error("Command for apertium_delete must be null, \'incoming\' or \'outgoing\'");
        return 0;
    }

    return 1;
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
    const char *username;
    PurpleBuddy *buddy;

    set_conversation(conv);

    title = malloc(sizeof(char)*100);
    text = malloc(sizeof(char)*300);

    buddy = purple_find_buddy(purple_conversation_get_account(conv), purple_conversation_get_name(conv));
    username = purple_buddy_get_name(buddy);

    sprintf(title,"Pairs for \'%s\'", purple_buddy_get_name(buddy));

    if(dictionaryHasUser(username, "incoming")){
        sprintf(text,"Incoming messages: %s - %s\n",
            dictionaryGetUserLanguage(username, "incoming", "source"),
            dictionaryGetUserLanguage(username, "incoming", "target"));
    }
    else{
        sprintf(text,"Incoming messages: None\n");
    }
    if(dictionaryHasUser(username, "outgoing")){
        sprintf(text,"%sOutgoing messages: %s - %s", text,
            dictionaryGetUserLanguage(username, "outgoing", "source"),
            dictionaryGetUserLanguage(username, "outgoing", "target"));
    }
    else{
        sprintf(text,"%sOutgoing messages: None", text);
    }

    notify_info_popup(title, text);

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
    int i, size;
    char *title, *text, ***pairsList;

    set_conversation(conv);

    if(!(size = getAllPairs(&pairsList))){
        return PURPLE_CMD_RET_FAILED;
    }

    title = malloc(sizeof(char)*100);
    text = malloc(sizeof(char)*size*50);

    sprintf(title, "available pairs");
    sprintf(text, " ");

    for(i=0; i<size; i++){
        sprintf(text,"%s%s - %s", text, pairsList[i][0], pairsList[i][1]);
        if(i%3 == 2){
            sprintf(text,"%s\n",text);
        }
        else{
            sprintf(text,"%s\t",text);
        }
    }

    sprintf(text,"%s", text);

    notify_info_popup(title, text);

    for(i=0; i<size; i++){
        free(pairsList[i]);
    }
    free(pairsList);
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
PurpleCmdRet apertium_bind_cb(PurpleConversation *conv, const gchar *cmd,
								gchar **args, gchar **error, void *data){
    const char *username;
    char *command, *source, *target, *msg;
	PurpleBuddy *buddy;

    set_conversation(conv);

    if(parse_bind_arguments(*args,&command,&source,&target)){
    	buddy = purple_find_buddy(purple_conversation_get_account(conv), purple_conversation_get_name(conv));
        username = purple_buddy_get_name(buddy);

    	if(dictionarySetUserEntry(username,command,source,target)){
            msg = malloc(sizeof(char)*(strlen(source)+strlen(target)+strlen(command)+strlen(username)+100));
            sprintf(msg, "%s pair for %s successfully set to %s-%s",command,username,source,target);
            notify_info(msg);
            free(msg);
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

/**
 * @brief Callback for the 'apertium_delete' command when no arguments are passed
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_unbind_noargs_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    const char *username;
    char *msg;
    PurpleBuddy *buddy;

    set_conversation(conv);

    buddy = purple_find_buddy(purple_conversation_get_account(conv), purple_conversation_get_name(conv));
    username = purple_buddy_get_name(buddy);

    if(dictionaryRemoveUserEntries(username)){
        msg = malloc(sizeof(char)*(strlen(username)+100));
        sprintf(msg, "Successfully removed data for %s",username);
        notify_info(msg);
        free(msg);
        return PURPLE_CMD_RET_OK;
    }
    else{
        return PURPLE_CMD_RET_FAILED;
    }
}

/**
 * @brief Callback for the 'apertium_delete' command when arguments are passed
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_unbind_args_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    const char *username;
    char *command, *msg;
    PurpleBuddy *buddy;

    set_conversation(conv);

    if(parse_unbind_arguments(*args,&command)){
        buddy = purple_find_buddy(purple_conversation_get_account(conv), purple_conversation_get_name(conv));
        username = purple_buddy_get_name(buddy);

        if(dictionaryRemoveUserEntry(username, command)){
            msg = malloc(sizeof(char)*(strlen(username)+strlen(command)+100));
            sprintf(msg, "Successfully removed %s data for %s",command,username);
            notify_info(msg);
            free(msg);
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

/**
 * @brief Callback for the 'apertium_apy' command when no arguments are passed
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_apy_noargs_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char **addresses, *msg;
    int i,size;

    set_conversation(conv);

    if((size = getAPYAddress(&addresses)) == -1){
        return PURPLE_CMD_RET_FAILED;
    }
    else{
        msg = malloc(sizeof(char)*(17+50*size));
        sprintf(msg,"APY address list:");
        for(i=0; i<size; i++){
            sprintf(msg,"%s\n%d - %s",msg,i+1,addresses[i]);
        }

        notify_info_popup("APY addresses", msg);
        free(addresses);
        free(msg);

        return PURPLE_CMD_RET_OK;
    }
}

/**
 * @brief Callback for the 'apertium_apy' command when arguments are passed
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_apy_args_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *address, *port;
    int order;

    set_conversation(conv);

    if(parse_apy_arguments(*args, &order, &address, &port)){
        if(!setAPYAddress(address, port, order, 0)){
            return PURPLE_CMD_RET_FAILED;
        }
    }
    else{
        return PURPLE_CMD_RET_FAILED;
    }

    notify_info("APY address successfully set");
    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_apyremove' command
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_apyremove_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *position_str;
    int position;

    set_conversation(conv);

    if((position_str = strtok(*args," ")) == NULL){
        notify_error("No 'position' argument provided");
        return PURPLE_CMD_RET_FAILED;
    }
    else{
        position = atoi(position_str);
        if(!removeAPYAddress(position)){
            notify_error("Couldn't remove the APY address");
            return PURPLE_CMD_RET_FAILED;
        }
    }

    notify_info("APY address successfully removed");
    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_display' command when no arguments are passed
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_display_noargs_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *msg;

    set_conversation(conv);

    msg = malloc(sizeof(char)*150);

    switch(display){
        case COMPRESSED:
            sprintf(msg,"\"Compressed\"\nBoth the original message and its translation are displayed, in 2 lines");
            break;
        case BOTH:
            sprintf(msg,"\"Both\"\nBoth the original message and its translation are displayed");
            break;
        case TRANSLATION:
            sprintf(msg,"\"Translation\"\nOnly the translated message is displayed");
            break;
    }

    notify_info_popup("Current display mode",msg);

    free(msg);

    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_display' command when arguments are passed
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_display_args_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *mode;

    set_conversation(conv);

    if((mode = strtok(*args," ")) == NULL){
        notify_error("No mode argument provided");
        return PURPLE_CMD_RET_FAILED;
    }
    else{
        if(!strcmp(mode,"both")){
            display = BOTH;
            setDisplay("both");
        }
        else{
            if(!strcmp(mode,"translation")){
                display = TRANSLATION;
                setDisplay("translation");
            }
            else{
                if(!strcmp(mode,"compressed")){
                    display = COMPRESSED;
                    setDisplay("compressed");
                }
                else{
                    notify_error("mode argument must be \"both\", \"translation\" or \"compressed\"");
                    return PURPLE_CMD_RET_FAILED;
                }
            }
        }
    }

    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_infodisplay' command
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_infodisplay_args_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *mode;

    set_conversation(conv);

    if((mode = strtok(*args," ")) == NULL){
        notify_error("No mode argument provided");
        return PURPLE_CMD_RET_FAILED;
    }
    else{
        if(!strcmp(mode,"dialog")){
            set_info_display_mode("dialog");
        }
        else{
            if(!strcmp(mode,"print")){
                set_info_display_mode("print");
            }
            else{
                if(!strcmp(mode,"none")){
                    set_info_display_mode("none");
                }
                else{
                    notify_error("mode argument must be \"dialog\", \"print\" or \"none\"");
                    return PURPLE_CMD_RET_FAILED;
                }
            }
        }
    }

    return PURPLE_CMD_RET_OK;
}

/**
 * @brief Callback for the 'apertium_errors' command
 *
 * Refer to the libpurple Commands API documentation for more information
 * @param conv Conversation where the command was used
 * @param cmd String containing the command
 * @param args String containing the arguments passed to the command
 * @param error
 * @param data Additional data passed
 * @return PURPLE_CMD_RET_OK on success, or PURPLE_CMD_RET_FAILED otherwise
 */
PurpleCmdRet apertium_errors_cb(PurpleConversation *conv, const gchar *cmd,
                                gchar **args, gchar **error, void *data){
    char *swtch;

    set_conversation(conv);

    if((swtch = strtok(*args," ")) == NULL){
        notify_error("No switch value provided");
        return PURPLE_CMD_RET_FAILED;
    }

    if(!strcmp(swtch,"on")){
        notifications_on();
        notify_info("Error messages enabled");
        return PURPLE_CMD_RET_OK;
    }
    if(!strcmp(swtch,"off")){
        notifications_off();
        notify_info("Error messages disabled");
        return PURPLE_CMD_RET_OK;
    }

    notify_error("Argument for this command must be either \"on\" or \"off\"");
    return PURPLE_CMD_RET_FAILED;
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

    buddy = purple_find_buddy(account, recipient);

    translate_message(message, buddy, "outgoing");

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

	buddy = purple_find_buddy(account, *sender);

	translate_message(message, buddy, "incoming");

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


    bind_command_id = purple_cmd_register("apertium_bind", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM, PLUGIN_ID, apertium_bind_cb,
        "apertium_bind \'direction\' \'source language\' \'target language\'\nSets the source-target language pair to translate messages from/to this user.\n\'direction\' must be \"incoming\" for received messages, or \"outgoing\" for user-sent messages.\n\'source language\' is the language expected to translate messages from.\n\'target language\' is the language to translate messages to.",
        NULL);

    unbind_noargs_command_id = purple_cmd_register("apertium_unbind", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM, PLUGIN_ID, apertium_unbind_noargs_cb,
        "apertium_unbind\nRemoves the stored language pair data for this buddy.",
        NULL);

    unbind_args_command_id = purple_cmd_register("apertium_unbind", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM, PLUGIN_ID, apertium_unbind_args_cb,
        "apertium_unbind \'direction\'\nRemoves the stored language pair data for this buddy.\n\'direction\' must be \"incoming\" or \"outgoing\".",
        NULL);

    check_command_id = purple_cmd_register("apertium_check", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM, PLUGIN_ID, apertium_check_cb,
        "apertium_check\nShows the current language pairs assigned to this contact for both incoming and outgoing messages.",
        NULL);

    pairs_command_id = purple_cmd_register("apertium_pairs", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_pairs_cb,
        "apertium_pairs\nShows all the available Apertium language pairs that can be used.",
        NULL);

    apy_noargs_command_id = purple_cmd_register("apertium_apy", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_apy_noargs_cb,
        "apertium_apy\nShows a list with all the APYs that will be requested for answers in order.",
        NULL);

    apy_args_command_id = purple_cmd_register("apertium_apy", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_apy_args_cb,
        "apertium_apy \'position\' \'address\' \'port\'\nInserts a new APY address to the APY list at the given position.\nThe \'port\' argument is optional\nThe \'position\' argument is optional and defaults to the end of the list",
        NULL);

    apyremove_command_id = purple_cmd_register("apertium_apyremove", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_apyremove_cb,
        "apertium_apyremove \'position\'\nRemoves the APY address located at the given position in the APY list",
        NULL);

    display_noargs_command_id = purple_cmd_register("apertium_display", "", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_display_noargs_cb,
        "apertium_display\nShows the current diplay mode.",
        NULL);

    display_args_command_id = purple_cmd_register("apertium_display", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_display_args_cb,
        "apertium_display \'display_mode\'\nSets the display mode for translated messages.\nThe \'display_mode\' argument must be \"both\" (displays the original message and its translation), \"translation\" (displays only the translation) or \"compressed\" (displays both the original message and translation, but does so in 2 lines)",
        NULL);

    info_display_command_id = purple_cmd_register("apertium_infodisplay", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_infodisplay_args_cb,
        "apertium_infodisplay \'info_display_mode\'\nSets the display mode for information text.\nThe \'info_display_mode\' argument must be \"dialog\" (displays the information in a dialog window), \"print\" (prints the information to the conversation) or \"none\" (no information is displayed)",
        NULL);

    errors_command_id = purple_cmd_register("apertium_errors", "s", PURPLE_CMD_P_HIGH,
        PURPLE_CMD_FLAG_IM | PURPLE_CMD_FLAG_CHAT, PLUGIN_ID, apertium_errors_cb,
        "apertium_errors \'switch\'\nTurns on/off the error notification messages.\nThe \'switch\' argument must be either \"on\" or \"off\"",
        NULL);

	// Python embedding
	pythonInit("apertium_pidgin_plugin_preferences.pkl");

    // Retrieving the displayMode
    const char* mode = getDisplay();

    if(mode != NULL){
        if(!strcmp(mode,"both")){
            display = BOTH;
        }
        else{
            if(!strcmp(mode,"compressed")){
                display = COMPRESSED;
            }
            else{
                display = TRANSLATION;
            }
        }
    }

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

	purple_cmd_unregister(bind_command_id);
    purple_cmd_unregister(unbind_noargs_command_id);
    purple_cmd_unregister(unbind_args_command_id);
    purple_cmd_unregister(check_command_id);
    purple_cmd_unregister(pairs_command_id);
    purple_cmd_unregister(apy_noargs_command_id);
    purple_cmd_unregister(apy_args_command_id);
    purple_cmd_unregister(apyremove_command_id);
    purple_cmd_unregister(display_noargs_command_id);
    purple_cmd_unregister(display_args_command_id);
    purple_cmd_unregister(info_display_command_id);
    purple_cmd_unregister(errors_command_id);

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
	"0.1.0",
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