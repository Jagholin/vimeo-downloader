## Motivation

There are many of free internet services that allow you to download a video from sites like YouTube, Vimeo, etc. However most of them have a major flaw, which is that they require you to know main page address for that video stream, they can't handle videos embedded on other sites. Another more risky option is to use a browser extension or an external software. There are some extensions that can download vimeo videos on the chrome store; but most of them either show ads, can't download embedded vids, or have trial periods after which they are deactivated until you pay for the service. Executables are a major security risk, and you should think twice before you download .exe from questionable source.

## Build instructions

vimeo-downloader uses Qt5 and C++ for its build, with no other extra dependencies. The build process is standard for a Qt5 project, .pro file is provided.

## Dependencies

vimeo-downloader uses ffmpeg to postprocess multimedia streams. Copy ffmpeg.exe to programs' working directory, or provide it with PATH.

## Usage

1. Install unpacked Chrome extension in /showvideoinfo directory.
2. The extension will detect embedded Vimeo videos on websites you visit. After opening one such website, click on extension button
3. The extension will show video and audio streams associated with the Vimeo video. Chose 1 video and 1 audio stream by clicking on them, then press Go to copy JSON with information about your selection to clipboard
4. In the vimeo-downloader application, paste the JSON in the text field on the top and press Go. 
5. Chose a location and file name to save your video. 
6. Wait until the work is done(this will take some time depending on length of the video and the chosen quality). The process happens in 3 phases, first video stream is downloaded, then the audio stream, then they are merged with ffmpeg.

## Current problems and limitations

vimeo-downloader was made in a couple of days to solve the task of downloading embedded Vimeo videos to harddrive. As such it is currently in the state of "just working fine", with no major issues that i know of. However there are some minor problems and inconviniences that might be amended in future revisions:

* The chrome extension that this program uses is not(and will not be) placed in Chrome Web store. Which means that Chrome browser will disable it if you try to install it from .crx archive. You have to enable dev mode in the "extensions" setting tab of Chrome, and then perform an install from unpacked directory.
* The extension is currently only showing the last detected video. That's a minor problem to fix, but it also has a very low priority in my schedule so it might take a while
* Vimeo servers far too often return 5** error codes instead of serving multimedia segments. vimeo-downloader, as it is at this moment, will abort download process on any sign of failure. The only option is then to remove temporary files and start over again. The fix for this and ability to resume download is coming shortly though.
* Using custom URI scheme handlers to pass information between the extension and the app is a potential feature that is currently under consideration.

## Credits

vimeo-downloader is based on python script with the same name by @eMBee, https://github.com/eMBee

vimeo-downloader chrome extension uses an icon designed by Freepik from Flaticon

## License

MIT license
