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
 * @file python_interface.c
 * @brief Functions to interface the plugin and Python
 */

#include "python_interface.h"

/**
 * @brief Reference to the apertiumFiles module
 *
 * Initialized with the pythonInit() function
 */
PyObject *files_module;

/**
 * @brief Reference to the apertiumInterfaceAPY module
 *
 * Initialized with the pythonInit() function
 */
PyObject *iface_module;

/**
 * @brief Initializes the Python environment
 *
 * Loads both the apertiumFiles and apertiumInterfaceAPY modules, which are used by the plugin.<br>
 * All the functions in this file require this to be first called in order to work properly
 * @param filename Name of the file where the preferences for the plugin will be stored
 */
void pythonInit(const char* filename){
    PyObject *pFunc, *pArgs, *addresses;

    Py_SetProgramName(NULL);
    Py_Initialize();

    files_module = PyImport_ImportModule("apertiumpluginutils.apertiumFiles");

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "setFile");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(filename));

            PyObject_CallObject(pFunc, pArgs);
            Py_XDECREF(pArgs);
        }
        else {
            return;
        }
        Py_XDECREF(pFunc);

        pFunc = PyObject_GetAttrString(files_module, "read");

        if (pFunc) {
            pArgs = PyTuple_New(0);
            PyObject_CallObject(pFunc, pArgs);
            Py_XDECREF(pArgs);
        }
        else {
            return;
        }
        Py_XDECREF(pFunc);

        pFunc = PyObject_GetAttrString(files_module, "getKey");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("apyAddress"));

            addresses = PyObject_CallObject(pFunc, pArgs);
            if(addresses == NULL || addresses == Py_None){
                Py_XDECREF(pArgs);
                Py_XDECREF(pFunc);
                return;
            }
            Py_XDECREF(pArgs);
        }
        else {
            return;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error_popup("Failed to load module: \'apertiumFiles\'");
        return;
    }

    iface_module = PyImport_ImportModule("apertiumpluginutils.apertiumInterfaceAPY");

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "setAPYList");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, addresses);

            if(PyObject_CallObject(pFunc, pArgs) == NULL){
                Py_XDECREF(pFunc);
                Py_XDECREF(addresses);
                notify_error_popup("Error on setAPYList(result is null)");
                return;
            }
        }
        else {
            notify_error_popup("Error on setAPYList(pfunc)");
            return;
        }
        Py_XDECREF(pFunc);
        Py_XDECREF(addresses);
    }
    else{
        notify_error_popup("Failed to load module: \'apertiumInterfaceAPY\'");
        return;
    }
}

/**
 * @brief Finalizes the Python environment
 */
void pythonFinalize(void){
    Py_Finalize();
}

/**
 * @brief Retrieves a list with all the APYs
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param list References to a 2-level char array where the apy list will be stored
 * The list parameter must be freed after its use
 * @return The number of addresses returned or -1 if an error occured
 */
int getAPYAddress(char ***list){
    int i,size;
    PyObject *pFunc, *pArgs, *result;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "getAPYList");

        if (pFunc) {
            pArgs = PyTuple_New(0);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {

                size = PyList_GET_SIZE(result);

                *list = malloc(sizeof(char*)*size);
                for(i=0; i<size; i++){
                    (*list)[i] = PyBytes_AsString(PyList_GetItem(result,i));
                }

                Py_XDECREF(pFunc);
                Py_XDECREF(result);
                return size;
            }
            else {
                Py_XDECREF(pFunc);
                return -1;
            }
        }
        else {
            return -1;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return -1;
    }
}

/**
 * @brief Sets the address where the request for the Apertium-APY will be sent
 *
 * The address will not be set if there is no response from an APY server <br>
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param address Pointer to a string with the new address
 * @param port Pointer to a string with the new port. NULL if no port is needed
 * @param order Position this address will take in the list. Negative value to append at the end
 * @param force Any number that is not 0 indicates the function to forcefully change the address,
 * despite not receiving an answer from the server
 * @return 1 if the call was successful and the address was set, or 0 otherwise
 */
int setAPYAddress(char* address, char* port, int order, int force){
    char* msg;
    PyObject *pFunc, *pArg, *pArgs, *new_address;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "setAPYAddress");

        if (pFunc) {
            pArgs = PyTuple_New(4);

            pArg = PyBytes_FromString(address);
            PyTuple_SetItem(pArgs, 0, pArg);

            pArg = port == NULL ? Py_None : PyBytes_FromString(port);
            PyTuple_SetItem(pArgs, 1, pArg);

            PyTuple_SetItem(pArgs, 2, order > -1 ? PyLong_FromLong(order) : Py_None);

            PyTuple_SetItem(pArgs, 3, force == 0 ? Py_False : Py_True);

            new_address = PyObject_CallObject(pFunc, pArgs);
            Py_XDECREF(pArgs);
            Py_XDECREF(pFunc);

            if (new_address != NULL){
                if(new_address != Py_None){
                    if(files_module != NULL){
                        pFunc = PyObject_GetAttrString(files_module, "setKey");

                        if (pFunc) {
                            pArgs = PyTuple_New(2);

                            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("apyAddress"));

                            PyTuple_SetItem(pArgs, 1, new_address);

                            PyObject_CallObject(pFunc, pArgs);

                            Py_XDECREF(new_address);
                        }
                        else{
                            Py_XDECREF(new_address);
                            return 0;
                        }
                    }
                    else{
                        notify_error("Module: \'apertiumFiles\' is not loaded");
                        return 0;
                    }
                    return 1;
                }
                else{
                    msg = malloc(sizeof(char)*(strlen(address)+(port == NULL ? 0 : strlen(port))+100));
                    if(port != NULL){
                        sprintf(msg,"No response from server at %s:%s",address,port);
                    }
                    else{
                        sprintf(msg,"No response from server at %s",address);
                    }
                    notify_error(msg);
                    free(msg);
                    return 0;
                }
            }
            else {
                return 0;
            }
        }
        else {
            return 0;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return 0;
    }
}

/**
 * @brief Removes the APY address at the given position in the APY list
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param position Position in the list of the address to be removed
 * @return 1 on success, or 0 otherwise
 */
int removeAPYAddress(int position){
    PyObject *pFunc, *pArgs, *result;

    if(iface_module != NULL){
        pFunc = PyObject_GetAttrString(iface_module, "removeAPYAddress");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, PyLong_FromLong((long)position));

            result = PyObject_CallObject(pFunc, pArgs);

            Py_XDECREF(pArgs);

            if(result != NULL && result != Py_False){
                Py_XDECREF(result);
                Py_XDECREF(pFunc);

                if(updateFileAddresses()){
                    return 1;
                }
                else{
                    return 0;
                }
            }
            else{
                Py_XDECREF(pFunc);
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else{
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return 0;
    }
}

/**
 * @brief Sets the APY list key in the dictionary
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param list APY list to be set
 * @return 1 on success, or 0 otherwise
 */
int setFileAPYList(PyObject* list){
    PyObject *pFunc, *pArgs;

    if(files_module != NULL){
        pFunc = PyObject_GetAttrString(files_module, "setKey");

        if (pFunc) {
            pArgs = PyTuple_New(2);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("apyAddress"));

            PyTuple_SetItem(pArgs, 1, list);

            PyObject_CallObject(pFunc, pArgs);

            Py_XDECREF(pArgs);
            Py_XDECREF(pFunc);

            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        notify_error("Module: \'apertiumFiles\' is not loaded");
        return 0;
    }
}

/**
 * @brief Writes to the preferences files the latest changes to the APY address list
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @return 1 on success, or 0 otherwise
 */
int updateFileAddresses(void){
    PyObject *pFunc, *pArgs, *list;

    if(iface_module != NULL){
        pFunc = PyObject_GetAttrString(iface_module, "getAPYList");

        if (pFunc) {
            pArgs = PyTuple_New(0);

            list = PyObject_CallObject(pFunc, pArgs);

            Py_XDECREF(pArgs);
            Py_XDECREF(pFunc);

            if(list != NULL){

                if(setFileAPYList(list)){
                    return 1;
                }
                else{
                    return 0;
                }
            }
            else{
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else{
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return 0;
    }
}

/**
 * @brief Returns the value for the display_mode stored in the preferences file
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @return The display_mode value on success or NULL otherwise
 */
const char* getDisplay(void){
    int mode;
    PyObject *pFunc, *pArgs, *result;

    if(files_module != NULL){
        pFunc = PyObject_GetAttrString(files_module, "getKey");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("displayMode"));

            result = PyObject_CallObject(pFunc, pArgs);

            Py_XDECREF(pArgs);

            if(result != NULL && result != Py_None){
                mode = (int)PyLong_AsLong(result);
                Py_XDECREF(result);
                switch(mode){
                    case 0:
                        return "compressed";
                    case 1:
                        return "both";
                    default:
                        return "translation";
                }
            }
            else{
                Py_XDECREF(pFunc);
                return NULL;
            }
        }
        else{
            return NULL;
        }
    }
    else{
        notify_error("Module: \'apertiumFiles\' is not loaded");
        return NULL;
    }
}

/**
 * @brief Sets the display_mode value in the dictionary so that it is store in the preferences file
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param display_mode The display_mode. Must be 'both', 'translation' or 'compressed'
 * @return 1 on success or 0 otherwise
 */
int setDisplay(const char* display_mode){
    int mode;
    PyObject *pFunc, *pArgs;

    if(!strcmp("compressed",display_mode)){
        mode = 0;
    }
    else{
        if(!strcmp("both",display_mode)){
            mode = 1;
        }
        else{
            mode = 2;
        }
    }

    if(files_module != NULL){
        pFunc = PyObject_GetAttrString(files_module, "setKey");

        if (pFunc) {
            pArgs = PyTuple_New(2);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("displayMode"));

            PyTuple_SetItem(pArgs, 1, PyLong_FromLong((long)mode));

            PyObject_CallObject(pFunc, pArgs);

            Py_XDECREF(pArgs);
        }
        else{
            return 0;
        }
        Py_XDECREF(pFunc);
        return 1;
    }
    else{
        notify_error("Module: \'apertiumFiles\' is not loaded");
        return 0;
    }
}

/**
 * @brief Checks whether the dictionary contains language pair information for a given user
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param user Name of the user to look for
 * @param direction Direction to look for the user in ("incoming" or "outgoing")
 * @return 1 if there is a language pair for the user, or 0 otherwise
 */
int dictionaryHasUser(const char* user, const char* direction){
    int has_user;
    PyObject *dictionary;

    if((dictionary = getDictionary()) == Py_None){
        return 0;
    }

    has_user = PyDict_Contains(
                PyDict_GetItemString(dictionary, direction),
                PyUnicode_FromString(user));

    Py_XDECREF(dictionary);
    return has_user;
}

/**
 * @brief Returns the language stored for a user in the preferences file
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param user Name of the user to look for
 * @param direction Direction to look for the user in ("incoming" or "outgoing")
 * @param key Language to look for ("source" or "target")
 * @return The language if the call was successful, or "None" otherwise
 */
char* dictionaryGetUserLanguage(const char *user, const char* direction, const char* key){
    char* user_lang;
    PyObject *dictionary;

    if((dictionary = getDictionary()) == Py_None){
        return "None";
    }

    user_lang = PyBytes_AsString(PyDict_GetItemString(
                    PyDict_GetItemString(PyDict_GetItemString(dictionary, direction), user),key));

    Py_XDECREF(dictionary);
    return user_lang;
}

/**
 * @brief Creates a new entry in the language pairs dictionary
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param user Name of the user to create a new entry for
 * @param direction Direction to create a new entry in ("incoming" or "outgoing")
 * @param source Source language of the language pair
 * @param target Target language of the language pair
 * @return 1 on success, or 0 otherwise
 */
int dictionarySetUserEntry(const char* user, const char* direction, const char* source, const char* target){
    PyObject *pFunc, *pArgs, *result;

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "setLangPair");

        if (pFunc) {
            pArgs = PyTuple_New(4);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(direction));

            PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(user));

            PyTuple_SetItem(pArgs, 2, PyBytes_FromString(source));

            PyTuple_SetItem(pArgs, 3, PyBytes_FromString(target));

            result = PyObject_CallObject(pFunc, pArgs);

            if(result != NULL){
                if(result == Py_True){
                    Py_XDECREF(result);
                    Py_XDECREF(pFunc);
                    Py_XDECREF(pArgs);
                    return 1;
                }
                else{
                    Py_XDECREF(result);
                    Py_XDECREF(pFunc);
                    Py_XDECREF(pArgs);
                    return 0;
                }
            }
            else{
                Py_XDECREF(pFunc);
                Py_XDECREF(pArgs);
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else {
        notify_error("Module: \'apertiumFiles\' is not loaded");
        return 0;
    }
}

/**
 * @brief Removes the specified entry from the dictionary related to the given user
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param user Name of the user whose entry will be removed
 * @param entry Name of the entry that will be removed. Must be either 'incoming' or 'outgoing'
 * @return 1 on success, or 0 otherwise
 */
int dictionaryRemoveUserEntry(const char* user, char* entry){
    PyObject *pFunc, *pArgs, *result;

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "unsetLangPair");

        if (pFunc) {
            pArgs = PyTuple_New(2);

            PyTuple_SetItem(pArgs, 0, PyUnicode_FromString(entry));

            PyTuple_SetItem(pArgs, 1, PyUnicode_FromString(user));

            result = PyObject_CallObject(pFunc, pArgs);

            if(result != NULL){
                if(result == Py_True){
                    Py_XDECREF(result);
                    Py_XDECREF(pFunc);
                    Py_XDECREF(pArgs);
                    return 1;
                }
                else{
                    Py_XDECREF(result);
                    Py_XDECREF(pFunc);
                    Py_XDECREF(pArgs);
                    return 0;
                }
            }
            else{
                Py_XDECREF(pFunc);
                Py_XDECREF(pArgs);
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    else {
        notify_error("Module: \'apertiumFiles\' is not loaded");
        return 0;
    }
}

/**
 * @brief Removes all the entries from the dictionary related to the given user
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param user Name of the user whose entries will be removed
 * @return 1 on success, or 0 otherwise
 */
int dictionaryRemoveUserEntries(const char* user){
    PyObject *dictionary;

    if((dictionary = getDictionary()) == Py_None){
        return 0;
    }

    if(PyDict_Contains(PyDict_GetItemString(dictionary, "incoming"), PyUnicode_FromString(user))){
        PyDict_DelItemString(PyDict_GetItemString(dictionary, "incoming"), user);
    }

    if(PyDict_Contains(PyDict_GetItemString(dictionary, "outgoing"), PyUnicode_FromString(user))){
        PyDict_DelItemString(PyDict_GetItemString(dictionary, "outgoing"), user);
    }

    setDictionary(dictionary);
    Py_XDECREF(dictionary);

    return 1;
}

/**
 * @brief Retrieves the Python dictionary containing the user-language_pair settings
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @return The dictionary (as a PyObject) if the call was successful, or Py_None otherwise
 */
PyObject* getDictionary(void){
    PyObject *pFunc, *pArgs, *result;

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "getDictionary");

        if (pFunc) {
            pArgs = PyTuple_New(0);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                return result;
            }
            else {
                Py_XDECREF(pFunc);
                return Py_None;
            }
        }
        else {
            return Py_None;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumFiles\' is not loaded");
        return Py_None;
    }
}

/**
 * @brief Sets the Python dictionary containing the user-language_pair settings
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param item New dictionary to substitute the old one with
 */
void setDictionary(PyObject* item){
    PyObject *pFunc, *pArgs;

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "setDictionary");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, item);

            PyObject_CallObject(pFunc, pArgs);
        }
    }
    else {
        notify_error("Module: \'apertiumFiles\' is not loaded");
    }
}

/**
 * @brief Saves the Python dictionary containing the user-language_pair settings to a file
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 */
void saveDictionary(void){
    PyObject *pFunc, *pArgs;

    if(getDictionary() != Py_None && getDictionary() != NULL){
        if (files_module != NULL) {
            pFunc = PyObject_GetAttrString(files_module, "save");

            if (pFunc) {
                pArgs = PyTuple_New(0);

                PyObject_CallObject(pFunc, pArgs);
            }
            else {
                return;
            }
            Py_XDECREF(pFunc);
        }
        else {
            notify_error("Module: \'apertiumFiles\' is not loaded");
            return;
        }
    }
}

/**
 * @brief Retrieves a list of all the available language pairs
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded).
 * @param pairList Reference to a 3-level char pointer where the pairs will be stored. <br>
 * Pair 'n' is stored in pairList[n] and its two languages are pairList[n][0] (source) and pairList[n][1] (target). <br>
 * Both pairList[x] and pairList must be freed after its use.
 * @return Number of language pairs if the call was successful, or 0 otherwise<br>
 */
int getAllPairs(char**** pairList){
    int i, size;
    PyObject *pFunc, *pArgs, *result, *list;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "getAllPairs");

        if (pFunc) {
            pArgs = PyTuple_New(0);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                if(PyDict_GetItemString(result,"ok") == Py_True){
                    list = PyDict_GetItemString(result,"result");
                    size = PyList_GET_SIZE(list);

                    *pairList = malloc(sizeof(char**)*size);
                    for(i=0; i<size; i++){
                        (*pairList)[i] = malloc(sizeof(char*)*2);
                        (*pairList)[i][0] = PyBytes_AsString(PyList_GetItem(PyList_GetItem(list,i),0));
                        (*pairList)[i][1] = PyBytes_AsString(PyList_GetItem(PyList_GetItem(list,i),1));
                    }

                    Py_XDECREF(pFunc);
                    Py_XDECREF(result);
                    Py_XDECREF(list);
                    return size;
                }
                else{
                    notify_error(PyBytes_AsString(PyDict_GetItemString(result,"errorMsg")));
                    return 0;
                }
            }
            else {
                Py_XDECREF(pFunc);
                return 0;
            }
        }
        else {
            Py_XDECREF(pFunc);
            return 0;
        }
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return 0;
    }
}

/**
 * @brief Checks if a given language pair is available
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param source String containing the source language
 * @param source String containing the target language
 * @return 1 if the call was successful and the language pair exists, or 0 otherwise
 */
int pairExists(char* source, char* target){
    PyObject *pFunc, *pArgs, *pArg, *result;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "pairExists");

        if (pFunc) {
            pArgs = PyTuple_New(2);

            pArg = PyBytes_FromString(source);
            PyTuple_SetItem(pArgs, 0, pArg);

            pArg = PyBytes_FromString(target);
            PyTuple_SetItem(pArgs, 1, pArg);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                if(PyDict_GetItemString(result,"ok") == Py_True){
                    Py_XDECREF(pFunc);

                    if(PyDict_GetItemString(result,"result") == Py_True){
                        return 1;
                    }
                    else{
                        notify_error("Pair does not exist");
                        return 0;
                    }
                }
                else{
                    Py_XDECREF(pFunc);
                    notify_error(PyBytes_AsString(PyDict_GetItemString(result,"errorMsg")));
                    return 0;
                }
            }
            else {
                Py_XDECREF(pFunc);
                return 0;
            }
        }
        else {
            return 0;
        }
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return 0;
    }
}

/**
 * @brief Translates a given text
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param text String containing the text to be translated
 * @param source String containing the source language to translate the text from
 * @param target String containing the target language to translate the text to
 * @return A string containing the translated text if the call was successful, or NULL otherwise
 */
char* translate(char* text, char* source, char* target){
    char* translation;
    PyObject *pFunc, *pArgs, *pArg, *result;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "translate");

        if (pFunc) {
            pArgs = PyTuple_New(3);

            pArg = PyBytes_FromString(text);
            PyTuple_SetItem(pArgs, 0, pArg);

            PyTuple_SetItem(pArgs, 1, PyBytes_FromString(source));

            PyTuple_SetItem(pArgs, 2, PyBytes_FromString(target));

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                Py_XDECREF(pFunc);
                if(PyDict_GetItemString(result,"ok") == Py_True){
                    translation = PyBytes_AsString(PyDict_GetItemString(result,"result"));
                    Py_XDECREF(result);

                    return translation;
                }
                else{
                    notify_error(PyBytes_AsString(PyDict_GetItemString(result,"errorMsg")));
                    return NULL;
                }
            }
            else {
                Py_XDECREF(pFunc);
                notify_error("There was an error in the translate call");
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return NULL;
    }
}
