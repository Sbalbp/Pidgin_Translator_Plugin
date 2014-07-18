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

## @file apertiumInterfaceAPY.py
# Acts as an interface with an Apertium-APY

try:
	import urllib.parse as parse
except:
	pass
try:
    import urllib.request as urllib2
except:
	import urllib2
try:
    import html.parser as HTMLParser
except:
	import HTMLParser
import sys
import json

## Python version running the module
pyVersion = sys.version_info[0]

## Address of the Apertium-APY
apyAddress = ['http://localhost:2737']

## Parser that will unescape APY responses
parser = HTMLParser.HTMLParser()

## Checks whether an APY server is running in the given address or not
#
# @param address Address to be checked
# @return True if there was a response from the server, False otherwise
def checkAPY(address):

	if(pyVersion >= 3):
		try:
			address.decode('utf-8')
		except:
			pass

	try:
		request = urllib2.urlopen(address+'/listPairs')
	except urllib2.URLError:
		return False
	except urllib2.HTTPError:
		return False
	except ValueError:
		return False

	return True

## Retrieves the length of the APY list
#
# @return The number of APYs currently in the list
def getAPYListSize():
	return len(apyAddress)

## Retrieves an APY from the current APY list
#
# @param index Position of the APY to be retrieved from the list. 0 if omitted
# @return A string with the current APY address on success, or None otherwise
def getAPYAddress(index=0):
	if(len(apyAddress) <= index or index < 0):
		return None
	else:
		if(pyVersion >= 3):
			return apyAddress[index].encode('utf-8')
		else:
			return apyAddress[index]


## Adds a new address to the APY addresses list
#
# @param newAddress New address for the APY
# @param newPort Port for the APY. None if no port is needed
# @param order Position this address will take in the list. None (appends address) by default
# @param force Forces the address to be set even if there was no response
# @return The new address if it was set, or None otherwise
def setAPYAddress(newAddress, newPort=None, order=None, force=False):
	global apyAddress

	if(pyVersion >= 3):
		try:
			newAddress = newAddress.decode('utf-8')
		except:
			pass
		if(newPort is not None):
			try:
				newPort = newPort.decode('utf-8')
			except:
				pass

	if(newPort is not None):
		newAddress = newAddress+':'+newPort

	if(checkAPY(newAddress) or force):
		if(order == None):
			apyAddress.append(newAddress)
		else:
			apyAddress.insert(order, newAddress)

		if(pyVersion >= 3):
			return newAddress.encode('utf-8')
		else:
			return newAddress
	else:
		return None

## Removes an APY from the APY list
#
# @param index Index of the address to remove in the list
# @return True on success or False otherwise
def removeAPYAddress(index):
	global apyAddress

	if(index >= len(apyAddress)):
		return False
	else:
		apyAddress.pop(index)
		return True

## Retrieves the list of APY addresses
#
# @return The list of APY addresses
def getAPYList():
	addrList = []

	for address in apyAddress:
		if(pyVersion >= 3):
			addrList.append(address.encode('utf-8'))
		else:
			addrList.append(address)

	return addrList

## Sets a list of APY addresses as the address list
#
# @param newList List ocntaining the addresses to be added
# @return The actual number of APY addresses added
def setAPYList(newList):
	global apyAddress

	apyAddress = []
	success = 0

	for address in newList:
		if(setAPYAddress(address, force=True) != None):
			success = success+1

	return success

## Retrieves a list with all the available language pairs
#
# @param index Optional integer indicating the position of the address in the list the request should be sent to. <br>
# Defaults to -1. Used to keep the function from iterating through all the addresses
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> List with the language pairs. Only present if <b>'ok'</b> is True<br>
# Each element of the result list is a list with two string elements: the source and the target languages of the pair, respectively
def getAllPairs(index=-1):
	if(index > -1 and index < len(apyAddress)):
		apyList = [apyAddress[index]]
		last = 0
	else:
		apyList = apyAddress
		last = len(apyAddress)-1

	for it,address in enumerate(apyList):
		pairs = []

		try:
			request = urllib2.urlopen(address+'/listPairs')
		except urllib2.URLError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue
		except urllib2.HTTPError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue

		if(request.getcode() < 300):
			if(pyVersion >= 3):
				request = request.read().decode("utf-8")
				jsonObj = json.loads(request)
			else:
				jsonObj = json.load(request)

			for pair in jsonObj['responseData']:
				pairs.append([ pair['sourceLanguage'].encode("utf-8"), pair['targetLanguage'].encode("utf-8") ])

			return {'ok':True, 'result':pairs}

		else:
			if(it == last):
				return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}
			else:
				continue

## Retrieves a list with all the available language pairs that share a common source language
#
# @param source String with the source language that the returned pairs must share
# @param index Optional integer indicating the position of the address in the list the request should be sent to. <br>
# Defaults to -1. Used to keep the function from iterating through all the addresses
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> List with the language pairs. Only present if <b>'ok'</b> is True<br>
# Each element of the result list is a list with two string elements: the source and the target languages of the pair, respectively
def getPairsBySource(source, index=-1):
	if(index > -1 and index < len(apyAddress)):
		apyList = [apyAddress[index]]
		last = 0
	else:
		apyList = apyAddress
		last = len(apyAddress)-1

	for it,address in enumerate(apyList):
		pairs = []

		if(pyVersion >= 3):
			try:
				source = source.decode("utf-8")
			except:
				pass

		try:
			request = urllib2.urlopen(address+'/listPairs')
		except urllib2.URLError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue
		except urllib2.HTTPError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue

		if(request.getcode() < 300):
			if(pyVersion >= 3):
				request = request.read().decode("utf-8")
				jsonObj = json.loads(request)
			else:
				jsonObj = json.load(request)

			for pair in jsonObj['responseData']:
				if(pair['sourceLanguage'] == source):
					pairs.append([ pair['sourceLanguage'].encode("utf-8"), pair['targetLanguage'].encode("utf-8") ])

			return {'ok':True, 'result':pairs}

		else:
			if(it == last):
				return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}
			else:
				continue

## Retrieves a list with all the available language pairs that share a common target language
#
# @param target String with the target language that the returned pairs must share
# @param index Optional integer indicating the position of the address in the list the request should be sent to. <br>
# Defaults to -1. Used to keep the function from iterating through all the addresses
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> List with the language pairs. Only present if <b>'ok'</b> is True<br>
# Each element of the result list is a list with two string elements: the source and the target languages of the pair, respectively
def getPairsByTarget(target, index=-1):
	if(index > -1 and index < len(apyAddress)):
		apyList = [apyAddress[index]]
		last = 0
	else:
		apyList = apyAddress
		last = len(apyAddress)-1

	for it,address in enumerate(apyList):
		pairs = []

		if(pyVersion >= 3):
			try:
				target = target.decode("utf-8")
			except:
				pass

		try:
			request = urllib2.urlopen(address+'/listPairs')
		except urllib2.URLError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue
		except urllib2.HTTPError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue

		if(request.getcode() < 300):
			if(pyVersion >= 3):
				request = request.read().decode("utf-8")
				jsonObj = json.loads(request)
			else:
				jsonObj = json.load(request)

			for pair in jsonObj['responseData']:
				if(pair['targetLanguage'] == target):
					pairs.append([ pair['sourceLanguage'].encode("utf-8"), pair['targetLanguage'].encode("utf-8") ])

			return {'ok':True, 'result':pairs}

		else:
			if(it == last):
				return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}
			else:
				continue

## Checks if a given language pair is available
#
# @param source String with the source language of the pair to be checked
# @param target String with the target language of the pair to be checked
# @param index Optional integer indicating the position of the address in the list the request should be sent to. <br>
# Defaults to -1. Used to keep the function from iterating through all the addresses
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> True if the pair exists, False otherwise. Only present if <b>'ok'</b> is True
def pairExists(source, target, index=-1):
	if(index > -1 and index < len(apyAddress)):
		apyList = [apyAddress[index]]
		last = 0
	else:
		apyList = apyAddress
		last = len(apyAddress)-1

	for it,address in enumerate(apyList):
		pairs = []

		if(pyVersion >= 3):
			try:
				source = source.decode('utf-8')
			except:
				pass
			try:
				target = target.decode('utf-8')
			except:
				pass

		try:
			request = urllib2.urlopen(address+'/listPairs')
		except urllib2.URLError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue
		except urllib2.HTTPError:
			if(it == last):
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			else:
				continue

		if(request.getcode() < 300):
			if(pyVersion >= 3):
				request = request.read().decode("utf-8")
				jsonObj = json.loads(request)
			else:
				jsonObj = json.load(request)

			for pair in jsonObj['responseData']:
				if(pair['sourceLanguage'] == source and pair['targetLanguage'] == target):
					return {'ok':True, 'result':True}

			if(it == last):
				return {'ok':True, 'result':False}
			else:
				continue

		else:
			if(it == last):
				return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}
			else:
				continue

## Translates a given text
#
# @param text String to be translated
# @param source String with the language to translate the text from
# @param target String with the language to translate the text to
# @param index Optional integer indicating the position of the address in the list the request should be sent to. <br>
# Defaults to -1. Used to keep the function from iterating through all the addresses
# @return A dictionary with the following fields:<br>
# <b>'ok':</b> True if the call was successful, False otherwise<br>
# <b>'errorMsg':</b> String with the cause of the error. Only present if <b>'ok'</b> is False<br>
# <b>'result':</b> A string with the translated text. Only present if <b>'ok'</b> is True
def translate(text, source, target, index=-1):
	if(index > -1 and index < len(apyAddress)):
		apyList = [apyAddress[index]]
		last = 0
	else:
		apyList = apyAddress
		last = len(apyAddress)-1

	for it,address in enumerate(apyList):
		result = pairExists(source, target, it)

		if(pyVersion >= 3):
			try:
				text = text.decode("utf-8")
			except:
				pass
			try:
				source = source.decode("utf-8")
			except:
				pass
			try:
				target = target.decode("utf-8")
			except:
				pass

		if(result['ok']):
			if(result['result']):

				try:
					if(pyVersion >= 3):
						request = urllib2.urlopen(parse.quote_plus((address+'/translate?q='+text+'&langpair='+source+'|'+target),safe=':/=?&|',encoding=None,errors=None))
					else:
						request = urllib2.urlopen((address+'/translate?q='+text+'&langpair='+source+'|'+target).replace(' ','%20'))
				except urllib2.URLError:
					if(it == last):
						return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
					else:
						continue
				except urllib2.HTTPError:
					if(it == last):
						return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
					else:
						continue

				if(request.getcode() < 300):
					if(pyVersion >= 3):
						request = request.read().decode("utf-8")
						jsonObj = json.loads(request)
					else:
						jsonObj = json.load(request)

					return {'ok':True, 'result':parser.unescape(jsonObj['responseData']['translatedText']).replace('%20',' ').encode('utf-8')}

				else:
					if(it == last):
						return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}
					else:
						continue

			else:
				if(it == last):
					return {'ok':False, 'errorMsg':('Pair '+source+'-'+target+' does not exist').encode('utf-8')}
				else:
					continue
		else:
			if(it == last):
				return result
			else:
				continue
