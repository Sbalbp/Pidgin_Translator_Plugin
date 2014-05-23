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
 */
void pythonInit(){
    PyObject *pName, *pFunc, *pArgs;
    char** args = malloc(sizeof(char*));

    Py_SetProgramName(NULL);
    Py_Initialize();
    PySys_SetArgv(0, args);

    pName = (PyObject*)PyString_FromString("apertiumFiles");

    files_module = PyImport_Import(pName);
    Py_DECREF(pName);

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "read");

        if (pFunc) {
            pArgs = PyTuple_New(0);
            PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
        }
        else {
            return;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Failed to load module: \'apertiumFiles\'");
        return;
    }

    pName = (PyObject*)PyString_FromString("apertiumInterfaceAPY");

    iface_module = PyImport_Import(pName);
    Py_DECREF(pName);

    if (iface_module == NULL) {
        notify_error("Failed to load module: \'apertiumInterfaceAPY\'");
        return;
    }
}

/**
 * @brief Finalizes the Python environment
 */
void pythonFinalize(){
    Py_XDECREF(files_module);
    Py_XDECREF(iface_module);
    Py_Finalize();
}

/**
 * @brief Retrieves a Python string containing the Apertium-APY current address
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @return The address (as a PyObject-string) if the call was successful, or Py_None otherwise
 */
PyObject* getAPYAddress(){
    PyObject *pFunc, *pArgs, *result;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "getAPYAddress");

        if (pFunc) {
            pArgs = PyTuple_New(0);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                return result;
            }
            else {
                Py_DECREF(pFunc);
                return Py_None;
            }
        }
        else {
            return Py_None;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return Py_None;
    }
}

/**
 * @brief Sets the address where the request for the Apertium-APY will be sent
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @param address Pointer to a string with the new address
 * @param port Pointer to a string with the new port. NULL if no port is needed
 */
void setAPYAddress(char* address, char* port){
    PyObject *pFunc, *pArg, *pArgs;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "setAPYAddress");

        if (pFunc) {
            pArgs = PyTuple_New(2);

            pArg = PyString_FromString(address);
            PyTuple_SetItem(pArgs, 0, pArg);

            pArg = port == NULL ? Py_None : PyString_FromString(port);
            PyTuple_SetItem(pArgs, 1, pArg);

            PyObject_CallObject(pFunc, pArgs);
        }
        else {
            return;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return;
    }
}

/**
 * @brief Retrieves the Python dictionary containing the user-language_pair settings
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 * @return The dictionary (as a PyObject) if the call was successful, or Py_None otherwise
 */
PyObject* getDictionary(){
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
                Py_DECREF(pFunc);
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
 * @return The dictionary (as a PyObject) if the call was successful, or Py_None otherwise
 */
PyObject* setDictionary(PyObject* item){
    PyObject *pFunc, *pArgs;

    if (files_module != NULL) {
        pFunc = PyObject_GetAttrString(files_module, "setDictionary");

        if (pFunc) {
            pArgs = PyTuple_New(1);

            PyTuple_SetItem(pArgs, 0, item);

            PyObject_CallObject(pFunc, pArgs);
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
 * @brief Saves the Python dictionary containing the user-language_pair settings to a file
 *
 * pythonInit() must have been called before or an error will occur (the module is not loaded)
 */
void saveDictionary(){
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
 * @return A Python list (as a PyObject) if the call was successful, or Py_None otherwise<br>
 * On success, the format of the list is:<br>
 * [<var>pair0, pair1, ...., pairN</var>]<br>
 * Each <var>pairN</var> is a list with the format:<br>
 * [<var>source language</var>, <var>target language</var>]
 */
PyObject* getAllPairs(){
    PyObject *pFunc, *pArgs, *pArg, *result;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "getAllPairs");

        if (pFunc) {
            pArgs = PyTuple_New(0);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                if(PyDict_GetItemString(result,"ok") == Py_True){
                    return PyDict_GetItemString(result,"result");
                }
                else{
                    notify_error(PyString_AsString(PyDict_GetItemString(result,"errorMsg")));
                    return Py_None;
                }
            }
            else {
                Py_DECREF(pFunc);
                return Py_None;
            }
        }
        else {
            Py_XDECREF(pFunc);
            return Py_None;
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return Py_None;
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

            pArg = PyString_FromString(source);
            PyTuple_SetItem(pArgs, 0, pArg);

            pArg = PyString_FromString(target);
            PyTuple_SetItem(pArgs, 1, pArg);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                if(PyDict_GetItemString(result,"ok") == Py_True){
                    if(PyDict_GetItemString(result,"result") == Py_True){
                        return 1;
                    }
                    else{
                        notify_error("Pair does not exist");
                        return 0;
                    }
                }
                else{
                    notify_error(PyString_AsString(PyDict_GetItemString(result,"errorMsg")));
                    return 0;
                }
            }
            else {
                Py_DECREF(pFunc);
                return 0;
            }
        }
        else {
        }
        Py_XDECREF(pFunc);
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
 * @return A Python string (as a PyObject)  containing the translated text if the call was successful, or "Error in translation" otherwise
 */
PyObject* translate(char* text, PyObject *source, PyObject *target){
    PyObject *pFunc, *pArgs, *pArg, *result;

    if (iface_module != NULL) {
        pFunc = PyObject_GetAttrString(iface_module, "translate");

        if (pFunc) {
            pArgs = PyTuple_New(3);

            pArg = PyString_FromString(text);
            PyTuple_SetItem(pArgs, 0, pArg);

            PyTuple_SetItem(pArgs, 1, source);

            PyTuple_SetItem(pArgs, 2, target);

            result = PyObject_CallObject(pFunc, pArgs);

            if (result != NULL) {
                if(PyDict_GetItemString(result,"ok") == Py_True){
                    return PyDict_GetItemString(result,"result");
                }
                else{
                    notify_error(PyString_AsString(PyDict_GetItemString(result,"errorMsg")));
                    return PyString_FromString("Error in translation");
                }
            }
            else {
                Py_DECREF(pFunc);
                notify_error("There was an error in the translate call");
                return PyString_FromString("Error in translation");
            }
        }
        else {
        }
        Py_XDECREF(pFunc);
    }
    else {
        notify_error("Module: \'apertiumInterfaceAPY\' is not loaded");
        return PyString_FromString("Error in translation");
    }
}
