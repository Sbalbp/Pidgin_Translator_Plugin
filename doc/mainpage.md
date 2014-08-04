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

<li><b>glib2.0.</b> Install package libglib2.0-dev</li>

<li><b>Python.</b> Install package python-dev</li>

<li><b>(Optional) <a href="http://wiki.apertium.org/wiki/Apy">Apertium-apy</a>.</b> Needed if you intend to run your own apy in your machine.</li>
</ul>

<h3><b>Compiling and installing</b></h3>

For this plugin to work, it is first necessary to install the Python module included in this repository under the 'Apertium_Plugin_Utils' folder, as it is used by the plugin.

If you have just cloned this repository you will need to first update the submodule:

<ul>
<li>./updateSub.sh</li>
</ul>

this will always fetch the latest version of the Python module, so it can be used to keep it up to date.

Now that you have the Python module, you might want to install it:

<ul>
<li>python setup.py install</li>
</ul>

Now, to compile the plugin, run:

<ul>
<li>./autogen.sh</li>
</ul>

which will generate the Makefile. It will also attempt to find out what your installed version of python is and change the Makefile accordingly. It is assumed by default that your python version is 2.7.

Now run

<ul>
<li>make</li>
</ul>

and the plugin (.so file) will be directly installed to the directory where Pidgin expects it to be (~/.purple/plugins).

It will also generate the documentation in the doc folder.

<h3><b>Plugin commands</b></h3>

The following commands are available for now in the plugin:

<ul>
<li><b>/apertium_apy <em>position<em> <em>address</em> <em>port</em></b> Adds a new APY <em>address</em> to the APY list. <em>port</em> number is optional. </em>position</em> (optional) tells the plugin which position the new APY should take in the list (last if it exceeds the APY list length. If no arguments are given, it just shows the current address list.

For example, if the APY list look like [address1, address2, address3] and we issue the command '/apertium_apy 1 http://localhost 2737', it will insert the address http://localhost:2737 to the APY list in the position number 1, pushing back ay other APY, which results in the following APY list: [address1, http://localhost:2737, address2, address3]. This means that the new address wil take priority over address2 and address3 when issuing a command that makes a request to an APY, but it will always be asked after address1.

The default list only address is http://localhost:2737. The address http://apy.projectjj.com can be added to the list. This address, however, is not guaranteed to work 100% of the times, as it is still in test stage.</li>

<li><b>/apertium_check</b> Shows the current language pairs associated with the buddy whose conversation you issued the command on.</li>

<li><b>/apertium_pairs</b> Ask the apy which language pairs are available and shows them.</li>

<li><b>/apertium_bind <em>direction</em> <em>source</em> <em>target</em></b> Sets a language pair for the buddy whose conversation the command was issued on. <em>direction</em> must be either 'incoming' (for incoming messages) or 'outgoing' (for messages sent to that buddy). <em>source</em> and <em>target</em> are the source and target languages of the language pair to be set, respectively.</li>

<li><b>/apertium_unbind <em>direction</em></b> Delete language pair data for the buddy whose conversation the command was issued on. <em>direction</em> is an optional argument. If present, it must be either 'incoming' or 'outgoing', to delete the language pair bindings for incoming or outgoing messages, respectively. If omitted, all language pair bindings are deleted.</li>

<li><b><em>apertium_display _displayMode</em></b> Selects how the messages should be displayed. <em>displayMode</em> (optional) can be either 'both' (the translation and the original message are both displayed) or 'translation' (only the translated message is displayed). If no argument is passed, the current display mode is shown.</li>

<li><b>/apertium_errors <em>switch</em></b> Turns on/off the error notifications from the plugin. <em>switch</em> must be either 'on' (enable notifications) or 'off' (disable notifications).</li>
</ul>

*/
