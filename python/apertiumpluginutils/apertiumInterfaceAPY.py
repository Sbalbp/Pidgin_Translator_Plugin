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
    import urllib.request as urllib2
except:
	import urllib2
import sys
import json

## Python version running the module
pyVersion = sys.version_info[0]

## Address of the Apertium-APY
apyAddress = 'http://localhost:2737'

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

## Retrieves the current address where the APY requests are sent
#
# @return A string with the current APY address
def getAPYAddress():
	if(pyVersion >= 3):
		return apyAddress.encode('utf-8')
	else:
		return apyAddress


## Changes the address where requests to the APY will be sent
#
# The address is only changed if there was a postive checkAPY() response for it
# @param newAddress New address for the APY
# @param newPort Port for the APY. None if no port is needed
# @return The new address if it was set, or None otherwise
def setAPYAddress(newAddress, newPort=None):
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

	if(checkAPY(newAddress)):
		apyAddress = newAddress

		return apyAddress
	else:
		return None

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
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}

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
		return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}

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

	if(pyVersion >= 3):
		try:
			source = source.decode("utf-8")
		except:
			pass

	try:
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}

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
		return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}

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

	if(pyVersion >= 3):
		try:
			target = target.decode("utf-8")
		except:
			pass

	try:
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}

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
		return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}

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
		request = urllib2.urlopen(apyAddress+'/listPairs')
	except urllib2.URLError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
	except urllib2.HTTPError:
		return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}

	if(request.getcode() < 300):
		if(pyVersion >= 3):
			request = request.read().decode("utf-8")
			jsonObj = json.loads(request)
		else:
			jsonObj = json.load(request)

		for pair in jsonObj['responseData']:
			if(pair['sourceLanguage'] == source and pair['targetLanguage'] == target):
				return {'ok':True, 'result':True}

		return {'ok':True, 'result':False}

	else:
		return {'ok':False, 'errorMsg':('Response '+str(request.getcode())+' from APY').encode('utf-8')}

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
				request = urllib2.urlopen((apyAddress+'/translate?q='+text+'&langpair='+source+'|'+target).replace(' ','%20'))
			except urllib2.URLError:
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}
			except urllib2.HTTPError:
				return {'ok':False, 'errorMsg':'Error on connection'.encode('utf-8')}

			if(request.getcode() < 300):
				if(pyVersion >= 3):
					request = request.read().decode("utf-8")
					jsonObj = json.loads(request)
				else:
					jsonObj = json.load(request)

				return {'ok':True, 'result':jsonObj['responseData']['translatedText'].encode('utf-8')}

			else:
				return {'ok':False, 'errorMsg':'Response '+str(request.getcode())+' from APY'}

		else:
			return {'ok':False, 'errorMsg':('Pair '+source+'-'+target+' does not exist').encode('utf-8')}
	else:
		return result
