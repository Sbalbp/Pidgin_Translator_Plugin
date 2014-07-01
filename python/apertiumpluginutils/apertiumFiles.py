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
import sys
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

## Sets the name for the file where the dictionary will be stored
#
# @param newFileName Name of the file
def setFile(newFileName):
	global fileName

	fileName = newFileName

## Creates a new empty dictionary and stores it in a file
#
# If the file to store the dictionary in does not exist, it is created
def createDictionary():
	global dictionary

	dictionary = {'version':sys.version_info[0], 'apyAddress':'http://localhost:2737', 'incoming':{}, 'outgoing':{}}

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

## Retrieves the value pointed by a key in the dictionary
#
# @param key Entry in the dictionary to look for
# @return Value pointed by key if it exists, or None otherwise
def getKey(key):
	global dictionary

	if(dictionary is None):
		read()

	if(key in dictionary.keys()):
		if(isinstance(dictionary[key],str) and sys.version_info[0] >= 3):
			return dictionary[key].encode('utf-8')
		else:
			return dictionary[key]
	else:
		return None

## Sets the value pointed by a key in the dictionary
#
# @param key Entry in the dictionary to set
# @param value Value to be assigned to the key
def setKey(key, value):
	global dictionary

	if(dictionary is None):
		read()

	dictionary[key] = value

## Sets the language pair to be associated with an user in a direction
#
# @param direction String containing 'incoming' or 'outgoing'
# @param user User the language pair will be associated to
# @param source Source language of the pair
# @param target Source language of the pair
# @return True on success, or Flase otherwise
def setLangPair(direction, user, source, target):
	global dictionary

	newDict = {}
	newDict['source'] = source
	newDict['target'] = target

	try:
		dictionary[direction][user]=newDict
		return True
	except:
		return False

## Removes the language pair associated with an user in a direction
#
# @param direction String containing 'incoming' or 'outgoing'
# @param user User whose language pair binding is to be removed
# @return True on success, or False otherwise
def unsetLangPair(direction, user):
	global dictionary

	if(direction in dictionary.keys() and user in dictionary[direction]):
		del dictionary[direction][user]
		return True
	else:
		return False

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
