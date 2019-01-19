# Discord Rich Presence Winamp plugin
Do you use Winamp to play media, and Discord for social purposes? If you have ever wanted to integrate Winamp into Discord, this simple, easy-to-use plugin will allow you to do it.

Features:
- Plugin has a settings menu for ease of use
- Changing settings doesn't require re-starting Winamp or the plugin
- Option to not display currently-playing title

Tested on 
- Winamp version 5.623 (x86)
- Winamp version 5.8 Build 3660 (x86)

This plugin is for Windows 7/Windows 10+ x86-compatible environments.

Example:

![Example of what plugin's effects are in Discord](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Example.png)

# Frequently Asked Questions (F.A.Q.'s)
**Q: Winamp, really? In {current year}?**

A: Yeah. Some people like it!

**Q: How do I set it up?**

A: The short version is you set up a Discord application attached to your Discord account through their web site. Then in Discord, designate Winamp as a game. Install the Winamp plugin. Then, in the plugin menu, set the Discord "application ID" to the application you created.

Here's step-by-step instructions, with more details:

1. Log in to the Discord web app.

Visit [discordapp.com/developers/applications/](discordapp.com/developers/applications/).

You'll see something like:

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup00.png)

Click "**Create an application**". You can give it a name like "Winamp".

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup01.png)

Afterward, you'll be taken to a screen to configure the application. You can set an icon, if you want. I got one from [here](https://commons.wikimedia.org/wiki/File:Winamp-logo.png).

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup02.png)

The only thing to do here is to take note of the **Client ID**, also called an **Application ID**, since you'll need it later.

2. In Discord settings, go to **Game Activity** and set Winamp as a game if you have not already. An easy way to do that is to run Winamp, and then click the **Add it!** button.

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup03.png)

3. To install the Winamp plugin, unzip the release folder and copy the results to the Plugins folder of your Winamp installation as shown.

Most often, the Plugins folder is located at **C:\Program Files (x86)\Winamp\Plugins**.

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup04.PNG)

When you're done, there should be a file **gen_DiscordRichPresence.dll** in the Plugins folder, plus a folder inside that called DiscordRichPresence.

4. Now open Winamp (or restart it if it was already open), and go to **Preferences**.

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup05.png)

Under the Plug-ins tab, under General Purpose, the "Discord Rich Presence" item should appear. If it does, the plug-in was successfully installed!

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup06.PNG)

Click the button **Configure selected plug-in.**

![Setup instructional image](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup07.PNG)

On this menu, paste the Discord **Application ID** from before. Also, you can check or un-check the box depending on whether you are comfortable showing the currently-playing media on Discord.

Click OK, and you're done!

**Q: If I want to use the plugin, do I need to build from source?**

A: Nope- you can go to the [Releases](https://github.com/clandrew/wdrp/releases) page and download the binaries in a zip file.

**Q: Do I need to give the application my Discord credentials?**

A: No, the plugin doesn't ask for your credentials.

**Q: How come the status says "Playing a game" instead of "Listening to" like with Spotify?**

A: The Discord RPC library doesn't currently expose a way of changing this UI element. The status for Spotify works because Discord gives Spotify special treatment. If it comes to be that the RPC library adds flexibility of changing this, I'd be interested in adopting it.

**Q: Sometimes, I'm seeing a small delay before my Discord Rich Presence status is updated. What gives?**

A: Discord thresholds the update frequency of the status text on its side. The frequency is every 15 seconds. [See this page for more information](https://discordapp.com/developers/docs/rich-presence/how-to#updating-presence).

**Q: If I want to build the plugin, what type of environment do I use?**

A: The plugin is built using Visual Studio 2017 on a Windows 10 environment. It uses the v141 toolset and SDK version 10.0.17134.0.

**Q: I have a question or other comment.**

A: Please contact cmlandrews at gmail dot com. While I can't concretely promise to fulfil specific feature requests since I am busy with a day job, I am appreciative of your time taken to comment and might get to it, depending on what it is. Thank you for your feedback!
