# Discord Rich Presence Winamp plugin

Do you use Winamp to play media, and Discord for social purposes? If you have ever wanted to integrate Winamp into Discord, this simple, easy-to-use plugin will allow you to do it.

Features:

- Plugin has a settings menu for ease of use
- Changing settings doesn't require re-starting Winamp or the plugin
- Option to not display currently-playing title
- Option to show elapsed time

Tested on

- Winamp version 5.623 (x86)
- Winamp version 5.8 Build 3660 (x86)

This plugin is for Windows 7/8/10+ x86-compatible environments.

Example:

![Winamp is shown running, playing a song, side-by-side with a Discord Rich Presence status indicator showing that song's name and artist](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Example.png "Winamp is shown running, playing a song, side-by-side with a Discord Rich Presence status indicator showing that song's name and artist")

# Frequently Asked Questions (F.A.Q.'s)

**Q: Winamp, really? In {current year}?**

A: Yep

**Q: How does it work?**

A: This is implemented as a Winamp plug-in which calls the Discord RPC API. The rich presence updates are sent to a Discord application which is associated with your Discord account.

**Q: How do I set it up?**

A: Here's step-by-step instructions:

1. Log in to the Discord web app.

Visit [discordapp.com/developers/applications/](http://www.discordapp.com/developers/applications/).

You'll see something like:

![A view of the Applications tab of the Discord Developer Portal, with a button that says Create an application](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup00.png "A view of the Applications tab of the Discord Developer Portal, with a button that says Create an application")

Click "**Create an application**". You can give it a name like "Winamp".

![An image of a dialog box titled Create an application. The user is typing in the text, Winamp. There are buttons labeled Create and Cancel](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup01.png "An image of a dialog box titled Create an application. The user is typing in the text, Winamp. There are buttons labeled Create and Cancel")

Afterward, you'll be taken to a screen to configure the application. You can set an icon, if you want. I got one from [here](https://commons.wikimedia.org/wiki/File:Winamp-logo.png).

If you want to include a logo in the Rich Presence, upload an asset for the application to "Rich Presence --> Art Assets" and name it "winamp-logo". It will show up to the left of the rich presence text. For an example of where the logo will show up and what that looks like, see "largeImageKey" described on [this page](https://discordapp.com/developers/docs/rich-presence/how-to).

![A view of the Applications tab of the Discord Developer Portal; viewing the General Information tab for the Winamp application. The screeenshot shows the Winamp Application's name as Winamp, and a CLIENT ID. An example ID of 112233445566778899 is shown.](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup02.png "A view of the Applications tab of the Discord Developer Portal; viewing the General Information tab for the Winamp application. The screeenshot shows the Winamp Application's name as Winamp, and a CLIENT ID. An example ID of 112233445566778899 is shown.")

The only thing to do here is to take note of the **Client ID**, also called an **Application ID**, since you'll need it later.

2. Close Winamp.

3. Run the installer from the [release](https://github.com/Aholicknight/wdrp/releases/tag/exeinstaller) tab. This will automatically place the plugin DLL in the Winamp folder for you + if Microsoft C++ Redistributable for Visual Studio 2015 is not installed, it will ask if you would like to install it. (Microsoft C++ Redistributable for Visual Studio 2015 is required for this plugin to work.)

4. Now open Winamp (or restart it if it was already open), and go to **Preferences**.

![Winamp is shown running, where the user has clicked the menu button, moused-over the Options menu, and is about to click Preferences. Preferences can also be accessed with Ctrl+P.](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup05.png "Winamp is shown running, where the user has clicked the menu button, moused-over the Options menu, and is about to click Preferences. Preferences can also be accessed with Ctrl+P.")

Under the Plug-ins tab, under General Purpose, the "Discord Rich Presence" item should appear. If it does, the plug-in was successfully installed!

![The Winamp Preferences window is shown. There are some Preferences categories, such as Media Library, Skins and Plug-ins. Under the Plug-ins category, the user has clicked the General Purpose sub-category. There's a listing of General Purpose plugins, and Discord Rich Presence, labeled gen_DiscordRichPresence.dll, is at the top, selected. There are buttons labeled Configure selected plug-in and Uninstall selected plug-in. There's an arrow pointing to the Configure selected plug-in button.](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup06.PNG "The Winamp Preferences window is shown. There are some Preferences categories, such as Media Library, Skins and Plug-ins. Under the Plug-ins category, the user has clicked the General Purpose sub-category. There's a listing of General Purpose plugins, and Discord Rich Presence, labeled gen_DiscordRichPresence.dll, is at the top, selected. There are buttons labeled Configure selected plug-in and Uninstall selected plug-in. There's an arrow pointing to the Configure selected plug-in button.")

Click the button **Configure selected plug-in.**

![There is a model dialog box titled Discord Rich Presence. In it, there is a checkbox labeled Display currently-playing title in Discord status. The checkbox is checked. Also, there is a text box labeled Discord Application ID. In the text box, the user has filled in the text 112233445566778899. There are buttons OK and Cancel.](https://raw.githubusercontent.com/clandrew/wdrp/master/Images/Setup07.PNG "There is a model dialog box titled Discord Rich Presence. In it, there is a checkbox labeled Display currently-playing title in Discord status. The checkbox is checked. Also, there is a text box labeled Discord Application ID. In the text box, the user has filled in the text 112233445566778899. There are buttons OK and Cancel.")

On this menu, paste the Discord **Application ID** from before. Also, you can check or un-check the box depending on whether you are comfortable showing the currently-playing media on Discord.

Alternatively, you can close Winamp and edit the settings.ini file, e.g., in

```
C:\Program Files (x86)\Winamp\Plugins\DiscordRichPresence\settings.ini
```

and verify it contains
ApplicationID:xxx (your Application/Client ID and not all zeores)
You may have to do this if Winamp plugins don't have privilege to change files in their folder.

Click OK, or save settings.ini, and you're done!

**Q: If I want to use the plugin, do I need to build from source?**

A: Nope- you can go to the [Releases](https://github.com/clandrew/wdrp/releases) page and download the binaries in a zip file.

**Q: Do I need to give the application my Discord credentials?**

A: No, the plugin doesn't ask for your credentials.

**Q: How come the status says "Playing a game" instead of "Listening to" like with Spotify?**

A: The Discord RPC library doesn't (at the time of writing this) currently expose a way for 3rd party plugins to change this UI element. The status for Spotify worked because Discord gave Spotify special treatment. If the RPC library adds flexibility of changing this, I'd be interested in adopting it.

Update (6/28/2019): Discord is distributing a new SDK called GameSDK to eventually replace the RPC SDK this uses. The [docs](https://discordapp.com/developers/docs/game-sdk/activities) say the new SDK supports the ability to customize this. But when I tried it, it didn't work. I had a weird back-and-forth with Discord folks about it. They told me basically it's there but not hooked up.

**Q: Sometimes, I'm seeing a small delay before my Discord Rich Presence status is updated. What gives?**

A: Discord thresholds the update frequency of the status text on its side. The frequency is every 15 seconds. [See this page for more information](https://discordapp.com/developers/docs/rich-presence/how-to#updating-presence).

**Q: Is there a way to get it to show album art?**

A: Currently, proabably not. At some point it might be doable.

One half of it would work fine- I can query Winamp for the art. The other missing half is to send it up to Discord. Discord doesn't, at the time of this writing, allow any "real" sort of programmatic upload of images- all images need to be uploaded through the Developer Portal web site. This is discussed a bit in their [FAQ](https://discordapp.com/developers/docs/rich-presence/faq) and [this Reddit thread](https://www.reddit.com/r/discordapp/comments/7fu2v5/i_made_spotify_rich_presence_i_think_its_nice/). If I could send images up with the RPC data, that would be perfect, but I'd be okay with anything really. 

**Q: Will other people be able to hear the music I'm listening to?**

A: No, Discord Rich Presence doesn't allow you to transmit music. You can look at other rich presences and see they don't play music either. For example, Spotify- if any Discord Rich Presence were to have it, it'd be that one (they have a special relationship with Discord and get access to special functionalities ordinary plug-ins don't have), and they don't have it. Their rich presence has a link on it you can click that opens Spotify in your browser, presumably at the same stream. The rich presence itself doesn't play music or video. Discord Rich Presence API doesn't even allow plug-ins to transmit images, much less audio and video.

If you want to stream audio to a particular server, Discord has a more heavyweight feature GoLive, which streams audio+video.

**Q: I can't seem to get the plug-in to start.**

A: One thing to check, is if you close Winamp and go to your Winamp installation folder, i.e.,

```
C:\Program Files (x86)\Winamp\Plugins\DiscordRichPresence
```

Verify that settings.ini contains
ApplicationID:xxx (your Application/Client ID and not all zeores)

you may need to edit it manually to set it. This is necessary if Winamp was run as a limited-privilege user without the permission to change the settings file in the plugin folder.

**Q: I'm seeing problems when I re-name the Discord application then try to use it with Rich Presence.**

A: I've seen this problem too. It appears to be a bug in Discord. The way I've found to work around it is to delete and re-create a new application with the right name.

**Q: If I want to build the plugin, what type of environment do I use?**

A: The plugin is built using Visual Studio 2017 on a Windows 10 environment. It uses the v141 toolset and SDK version 10.0.17134.0. Other environments might work too.

VS Code is supported as well. See [CONTRIBUTING.md](CONTRIBUTING.md) for setup assistance including required libraries and paths.

**Q: I have a question or other comment.**

A: Setting expectations: I've started focusing my attention on other projects. So I don't have capacity to do this maintenance on this project any more. If you reach out or open issues you might have a better result investigating them yourself.
