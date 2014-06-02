"""
 Pidgin Translator Plugin.

 Copyright (C) 2014 Sergio Balbuena <sbalbp@gmail.com>.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 3 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
"""

#!/usr/bin/env python

## @file apertiumFiles.py
# Manages the files and dictionary used with the plugin

import pickle
import os.path

## Dictionary which contains the user-language_pair bindings
#
# The dictionary has the following structure:<br>
# { <b>'apyAddress'</b> : 'address to make requests to' , <b>'incoming'</b> : { <var>user1, user2, ...., userN</var> } , <b>'outgoing'</b>: { <var>user1, user2, ...., userN</var> } }<br>
# The structure for each <var>userN</var>:<br>
# { <b>'name'</b> : { <b>'source'</b> : <var>source_language_str</var> , <b>'target'</b> : <var>target_language_str</var> } }
dictionary = None

## Name of the file where the dictionary will be stored
fileName = 'apertium_plugin_pairs_preferences.pkl'

## Creates a new empty dictionary and stores it in a file
#
# If the file to store the dictionary in does not exist, it is created
def createDictionary():
	global dictionary

	dictionary = {'apyAddress':'http://localhost:2737', 'incoming':{}, 'outgoing':{}}

	file1 = open(fileName, 'wb')

	pickle.dump(dictionary, file1)
	file1.close()

## Reads the dictionary from a file
#
# If the file to store the dictionary in does not exist, a new file and a dictionary are created with createDictionary()
# @return The dictionary
def read():
	global dictionary

	if(not os.path.isfile(fileName)):
		createDictionary()

	file1 = open(fileName, 'rb')

	dictionary = pickle.load(file1)
	file1.close()

	return dictionary

## Saves the current state of the dictionary to a file
def save():
	global dictionary

	file1 = open(fileName, 'wb')

	pickle.dump(dictionary, file1)
	file1.close()

## Retrieves the current dictionary
#
# @return The dictionary
def getDictionary():
	global dictionary

	if(dictionary is None):
		read()

	return dictionary

## Overrides the current dictionary with a new one
#
# @param newDictionary Dictionary to replace the old one with
def setDictionary(newDictionary):
	global dictionary

	dictionary = newDictionary
