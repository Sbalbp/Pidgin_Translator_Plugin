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

<li><b>Python 2.7.</b></li>

<li><b>(Optional) <a href="http://wiki.apertium.org/wiki/Apy">Apertium-apy</a>.</b> Needed if you intend to run your own apy in your machine.</li>
</ul>

<h3><b>Compiling and installing</b></h3>

For now, it might be necessary to 'manually' change some settings in the Makefile in order for the plugin to compile. Specifically, the <b>LIBPURPLE</b> and <b>PYTHONDIR</b> variables, which contain the route where your copy of libpurple is located and the route where your python2.7 modules are stored (generally, /usr/lib/pythonX.Y), respectively.

After that, run

<ul>
<li>make</li>
</ul>

and the plugin (.so file) will be directly installed to the directory where Pidgin expects it to be (~/.purple/plugins). Note that a few commands from the Makefile are run with sudo.

<h3><b>Plugin commands</b></h3>

The following commands are available for now in the plugin:

<ul>
<li><b>/apertium_apy <em>address</em> <em>port</em></b> Sets the address and port(optional) where the apy is running. If no arguments are given, it just shows the current address. The default address is http://localhost:2737.</li>

<li><b>/apertium_check</b> Shows the current language pairs associated with the buddy whose conversation you issued the command on.</li>

<li><b>/apertium_pairs</b> Ask the apy which language pairs are available and shows them.</li>

<li><b>/apertium_set <em>direction</em> <em>source</em> <em>target</em></b> Sets a language pair for the buddy whose conversation the command was issued on. <em>direction</em> must be either 'incoming' (for incoming messages) or 'outgoing' (for messages sent to that buddy). <em>source</em> and <em>target</em> are the source and target languages of the language pair to be set, respectively.</li>
</ul>

*/
