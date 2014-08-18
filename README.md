Pidgin Translator Plugin
========================

Pidgin plugin to translate incoming and outgoing messages with the help of the software developed by the [Apertium](http://www.apertium.org/ "Apertium") project team.

This plugin is currently still not finished and, therefore, some things are still subject to change.

###What it does

When enabled, this plugin keeps track of the user's language preferences for each of their buddies (both incoming and outgoing messages). If the user has set the language pair eng-spa (English -> Spanish) for incoming messages from buddy1, then the plugin will attempt to tranlate all incoming messages from buddy1 to Spanish (assuming they will be in English).

The translating is done by an [Apertium-apy](http://wiki.apertium.org/wiki/Apy "Apertium-apy") that may run locally or on a remote location (its address can be set from within the plugin).

The plugin is able to use several APY instances, as it stores an ordered APY list. The first APY in the list takes priority when the plugin need to make a request to an APY. If the first APY is unreachable or unable to give an answer, the plugin will attempt to make the same request to the second APY in the list, and so on.

###Compilation Requirements

* **libpurple.** The library containing all the development sources and headers needed for Pidgin Plugins, as well as some example plugins to help new developers get started. You can get a pidgin .tar file with libpurple [here](http://sourceforge.net/projects/pidgin/ "here") (don't forget to './configure' and 'make' it, as explained in this [tutorial](https://developer.pidgin.im/wiki/CHowTo/BasicPluginHowto "tutorial") ).

	Alternatively, you can install the pidgin-dev package.
* **glib2.0.** Install package libglib2.0-dev
* **Python.** Install package python-dev
* **(Optional)[Apertium-apy](http://wiki.apertium.org/wiki/Apy "Apertium-apy").** Needed if you intend to run your own apy in your machine.

###Compiling and installing

For this plugin to work, it is first necessary to install the Python module included in this repository under the 'Apertium_Plugin_Utils' folder, as it is used by the plugin.

If you have just cloned this repository you will need to first update the submodule:

* ./updateSub.sh

this will always fetch the latest version of the Python module, so it can be used to keep it up to date.

Now that you have the Python module, you might want to install it. First enter the submodule directory

* cd Apertium_Plugin_Utils

and install the module. You can choose between a global installation with

* sudo python setup.py install

or you can install the module to a chosen folder (refer to this [documentation](https://docs.python.org/2/install/ "documentation") for different alternatives). In this case you will have to add the path where you installed the module (up to the site-packages folder) to your PYTHONPATH environment variable.

Now, to compile the plugin, go back to the main repository directory and run

* ./autogen.sh

which will generate the Makefile. It will also attempt to find out what your installed version of python is and change the Makefile accordingly. It is assumed by default that your python version is 2.7.

Now run

* make

and the plugin (.so file) will be directly installed to the directory where Pidgin expects it to be (~/.purple/plugins).

It will also generate the documentation in the doc folder.

Lastly, in order to use the plugin, you must first activate it in Pidgin. From the Pidgin plugin installation [page](https://developer.pidgin.im/wiki/ThirdPartyPlugins "page"): *You can manage available plugins by accessing the "Tools" menu from the Buddy List window and selecting "Plugins."*. This plugin will be listed as 'Message Translator'. If there was an error during plugin load, an error would be thrown.

###Plugin commands

The following commands are available for now in the plugin:

* **/apertium_apy _position_ _address_ _port_** Adds a new APY *address* to the APY list. *port* number is optional. *position* (optional) tells the plugin which position the new APY should take in the list (last if it exceeds the APY list length). If no arguments are given, it just shows the current address list.

	For example, if the APY list looks like [address1, address2, address3] and we issue the command '/apertium_apy 1 http://localhost 2737', it will insert the address http://localhost:2737 to the APY list in the position number 1, pushing back ay other APY, which results in the following APY list: [address1, http://localhost:2737, address2, address3]. This means that the new address wil take priority over address2 and address3 when issuing a command that makes a request to an APY, but it will always be asked after address1.

	The default list only address is http://localhost:2737. The address http://apy.projectjj.com can be added to the list. This address, however, is not guaranteed to work 100% of the times, as it is still in test stage.

* **/apertium_apyremove _position_** Removes the APY address located at the given *position* in the APY list.
* **/apertium_check** Shows the current language pairs associated with the buddy whose conversation you issued the command on.
* **/apertium_pairs** Ask the apy which language pairs are available and shows them.
* **/apertium_bind _direction_ _source_ _target_** Sets a language pair for the buddy whose conversation the command was issued on. *direction* must be either 'incoming' (for incoming messages) or 'outgoing' (for messages sent to that buddy). *source* and *target* are the source and target languages of the language pair to be set, respectively.
* **/apertium_unbind _direction_** Delete language pair data for the buddy whose conversation the command was issued on. *direction* is an optional argument. If present, it must be either 'incoming' or 'outgoing', to delete the language pair bindings for incoming or outgoing messages, respectively. If omitted, all language pair bindings are deleted.
* **/apertium_display _displayMode_** Selects how the messages should be displayed. *displayMode* (optional) can be 'both' (the translation and the original message are both displayed), 'translation' (only the translated message is displayed) or 'compressed' (both the translation and the original message are shown, in a compressed 2-line way). If no argument is passed, the current display mode is shown. The default display mode is 'compressed'.
* **/apertium_errors _switch_** Turns on/off the error notifications from the plugin. *switch* must be either 'on' (enable notifications) or 'off' (disable notifications).
