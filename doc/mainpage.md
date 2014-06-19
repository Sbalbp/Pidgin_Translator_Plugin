/**
\mainpage Pidgin Translator Plugin

Pidgin plugin to translate incoming and outgoing messages with the help of the software developed by the <a href="http://www.apertium.org/">Apertium</a> project team.

This plugin is currently still not finished and, therefore, some things are still subject to change.

<h3><b>What it does</b></h3>

When enabled, this plugin keeps track of the user's language preferences for each of their buddies (both incoming and outgoing messages). If the user has set the language pair eng-spa (English -> Spanish) for incoming messages from buddy1, then the plugin will attempt to tranlate all incoming messages from buddy1 to Spanish (assuming they will be in English).

The translating is done by an <a href="http://wiki.apertium.org/wiki/Apy">Apertium-apy</a> that may run locally or on a remote location (its address can be set from within the plugin).

<h3><b>Compilation Requirements</b></h3>

<ul>
<li><b>libpurple.</b> The library containing all the development sources and headers needed for Pidgin Plugins, as well as some example plugins to help new developers get started. You can get a pidgin .tar file with libpurple <a href="http://sourceforge.net/projects/pidgin/">here</a> (don't forget to './configure' and 'make' it, as explained in this <a href="https://developer.pidgin.im/wiki/CHowTo/BasicPluginHowto">tutorial</a>).</li>

<li><b>glib2.0.</b> Install package libglib2.0</li>

<li><b>Python.</b> Install package python-dev</li>

<li><b>(Optional) <a href="http://wiki.apertium.org/wiki/Apy">Apertium-apy</a>.</b> Needed if you intend to run your own apy in your machine.</li>
</ul>

<h3><b>Compiling and installing</b></h3>

To begin with, run

<ul>
<li>./autogen.sh</li>
</ul>

which will generate the Makefile. It will also attempt to find out what your installed version of python is and change the Makefile accordingly. It is assumed by default that your python version is 2.7.

Now run

<ul>
<li>make</li>
</ul>

and the plugin (.so file) will be directly installed to the directory where Pidgin expects it to be (~/.purple/plugins). Note that a few commands from the Makefile are run with sudo.

It will also generate the documentation in the doc folder.

<h3><b>Plugin commands</b></h3>

The following commands are available for now in the plugin:

<ul>
<li><b>/apertium_apy <em>address</em> <em>port</em></b> Sets the address and port(optional) where the apy is running. If no arguments are given, it just shows the current address. The default address is http://localhost:2737.</li>

<li><b>/apertium_check</b> Shows the current language pairs associated with the buddy whose conversation you issued the command on.</li>

<li><b>/apertium_pairs</b> Ask the apy which language pairs are available and shows them.</li>

<li><b>/apertium_set <em>direction</em> <em>source</em> <em>target</em></b> Sets a language pair for the buddy whose conversation the command was issued on. <em>direction</em> must be either 'incoming' (for incoming messages) or 'outgoing' (for messages sent to that buddy). <em>source</em> and <em>target</em> are the source and target languages of the language pair to be set, respectively.</li>

<li><b>/apertium_delete <em>direction</em></b> Delete language pair data for the buddy whose conversation the command was issued on. <em>direction</em> is an optional argument. If present, it must be either 'incoming' or 'outgoing', to delete the language pair bindings for incoming or outgoing messages, respectively. If omitted, all language pair bindings are deleted.</li>

<li><b>/apertium_errors <em>switch</em></b> Turns on/off the error notifications from the plugin. <em>switch</em> must be either 'on' (enable notifications) or 'off' (disable notifications).</li>
</ul>

*/
