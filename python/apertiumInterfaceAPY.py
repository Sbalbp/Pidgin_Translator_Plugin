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

## @package apertiumInterfaceAPY
# Acts as an interface with an Apertium-APY

import urllib2
import json

## Address of the Apertium-APY
apyAddress = 'http://localhost:2737'

## Retrieves the current address where the APY requests are sent
#
# @return A string with the current APY address
def getAPYAddress():
	return apyAddress

## Changes the address where request to the APY will be sent
#
# @param newAddress New address for the APY
# @param newPort Port for the APY. None if no port is needed
def setAPYAddress(newAddress, newPort):
	global apyAddress

	if(newPort is not None):
		newAddress = newAddress+':'+newPort

	apyAddress = newAddress

## Retrieves a list with all the available language pairs
#
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> List with the language pairs. Only present if <b>'ok'</b> is True<br>
# Each element of the result list is a list with two string elements: the source and the target languages of the pair, respectively
def getAllPairs():
	pairs = []

	try:
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'}

	if(request.getcode() < 300):
		jsonObj = json.load(request)

		for pair in jsonObj['responseData']:
			pairs.append([ pair['sourceLanguage'].encode("utf-8"), pair['targetLanguage'].encode("utf-8") ])

		return {'ok':True, 'result':pairs}

	else:
		return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}

## Retrieves a list with all the available language pairs that share a common source language
#
# @param source String with the source language that the returned pairs must share
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> List with the language pairs. Only present if <b>'ok'</b> is True<br>
# Each element of the result list is a list with two string elements: the source and the target languages of the pair, respectively
def getPairsBySource(source):
	pairs = []

	try:
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'}

	if(request.getcode() < 300):
		jsonObj = json.load(request)

		for pair in jsonObj['responseData']:
			if(pair['sourceLanguage'] == source):
				pairs.append([ pair['sourceLanguage'].encode("utf-8"), pair['targetLanguage'].encode("utf-8") ])

		return {'ok':True, 'result':pairs}

	else:
		return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}

## Retrieves a list with all the available language pairs that share a common target language
#
# @param target String with the target language that the returned pairs must share
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> List with the language pairs. Only present if <b>'ok'</b> is True<br>
# Each element of the result list is a list with two string elements: the source and the target languages of the pair, respectively
def getPairsByTarget(target):
	pairs = []

	try:
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'}

	if(request.getcode() < 300):
		jsonObj = json.load(request)

		for pair in jsonObj['responseData']:
			if(pair['targetLanguage'] == target):
				pairs.append([ pair['sourceLanguage'].encode("utf-8"), pair['targetLanguage'].encode("utf-8") ])

		return {'ok':True, 'result':pairs}

	else:
		return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}

## Checks if a given language pair is available
#
# @param source String with the source language of the pair to be checked
# @param target String with the target language of the pair to be checked
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> True if the pair exists, False otherwise. Only present if <b>'ok'</b> is True
def pairExists(source, target):
	pairs = []

	try:
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'}

	if(request.getcode() < 300):
		jsonObj = json.load(request)

		for pair in jsonObj['responseData']:
			if(pair['sourceLanguage'] == source and pair['targetLanguage'] == target):
				return {'ok':True, 'result':True}

		return {'ok':True, 'result':False}

	else:
		return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}

## Translates a given text
#
# @param text String to be translated
# @param source String with the language to translate the text from
# @param target String with the language to translate the text to
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> A string with the translated text. Only present if <b>'ok'</b> is True
def translate(text, source, target):
	result = pairExists(source, target)

	if(result['ok']):
		if(result['result']):

			try:
				request = urllib2.urlopen((apyAddress+'/translate?q='+text+'&langpair='+source+'|'+target).replace(' ','%20'))
			except urllib2.URLError:
				return {'ok':False, 'errorMsg':'Error on connection'}
			except urllib2.HTTPError:
				return {'ok':False, 'errorMsg':'Error on connection'}

			if(request.getcode() < 300):
				jsonObj = json.load(request)

				return {'ok':True, 'result':jsonObj['responseData']['translatedText']}

			else:
				return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}

		else:
			return {'ok':False, 'errorMsg':'Pair '+source+'-'+target+' does not exist'}
	else:
		return result
