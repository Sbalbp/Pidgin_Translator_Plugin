Pidgin Translator Plugin
========================

Pidgin plugin to translate incoming and outgoing messages with the help of the software developed by the [Apertium](http://www.apertium.org/ "Apertium") project team.

This plugin is currently still not finished and, therefore, some things are still subject to change.

###What it does

When enabled, this plugin keeps track of the user's language preferences for each of their buddies (both incoming and outgoing messages). If the user has set the language pair eng-spa (English -> Spanish) for incoming messages from buddy1, then the plugin will attempt to tranlate all incoming messages from buddy1 to Spanish (assuming they will be in English).

The translating is done by an [Apertium-apy](http://wiki.apertium.org/wiki/Apy "Apertium-apy") that may run locally or on a remote location (its address can be set from within the plugin).

###Compilation Requirements

* **libpurple.** The library containing all the development sources and headers needed for Pidgin Plugins, as well as some example plugins to help new developers get started. You can get a pidgin .tar file with libpurple [here](http://sourceforge.net/projects/pidgin/ "here") (don't forget to './configure' and 'make' it, as explained in this [tutorial](https://developer.pidgin.im/wiki/CHowTo/BasicPluginHowto "tutorial") ).
* **glib2.0.** Install package libglib2.0
* **Python.** Install package python-dev
* **(Optional)[Apertium-apy](http://wiki.apertium.org/wiki/Apy "Apertium-apy").** Needed if you intend to run your own apy in your machine.

###Compiling and installing

To begin with, run

* ./autogen.sh

which will generate the Makefile. It will also attempt to find out what your installed version of python is and change the Makefile accordingly. It is assumed by default that your python version is 2.7.

Now run

* make

and the plugin (.so file) will be directly installed to the directory where Pidgin expects it to be (~/.purple/plugins). Note that a few commands from the Makefile are run with sudo.

It will also generate the documentation in the doc folder.

###Plugin commands

The following commands are available for now in the plugin:

* **/apertium_apy _address_ _port_** Sets the address and port(optional) where the apy is running. If no arguments are given, it just shows the current address. The default address is http://localhost:2737.
* **/apertium_check** Shows the current language pairs associated with the buddy whose conversation you issued the command on.
* **/apertium_pairs** Ask the apy which language pairs are available and shows them.
* **/apertium_set _direction_ _source_ _target_** Sets a language pair for the buddy whose conversation the command was issued on. *direction* must be either 'incoming' (for incoming messages) or 'outgoing' (for messages sent to that buddy). *source* and *target* are the source and target languages of the language pair to be set, respectively.
* **/apertium_delete** Delete all language pair data for the buddy whose conversation the command was issued on.
* **/apertium_errors _switch_** Turns on/off the error notifications from the plugin. *switch* must be either 'on' (enable notifications) or 'off' (disable notifications).
