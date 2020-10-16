# Windows NT bootfont.bin Editor

This is a small app which I've written around 2010, which allows you to edit font used for hibernation resume, F8 boot menu, as well as first text-based install stage in Windows 2000 as well as Windows XP, perhaps in earlier versions too.

I've started this project with realization that somehow those early text mode screens use localized fonts instead of default codepage.
I've looked around the system and found the bootfont.bin file on the main directory of my system partition, started hacking around it in hex editor, and some days later the editor was complete.

## Features

- Save/load bootfont.bin files.
- Supports wide character format for Asia-translated versions of Windows.

## Modified bootfonts

I've used this app to modify my startup font to resemble Topaz font as seen on Amiga computers.
You can grab two different versions:

- [topaz.bin](new_bootfonts/topaz.bin)
- [topaz_plus_stretched.bin](new_bootfonts/topaz_plus_stretched.bin)

Just replace them as bootfont.bin and it should work!

## File format

Funny that after all those years, there are no similar apps and the documentation is even more scarce right now. Unfortunately, the only thing which I've written and resembles some form of documentation is [bootfonthdr.c](doc/bootfonthdr.c) file, but more should be decipherable from the app itself.

The most reverse-engineering work was done by doing a mix of:

- looking at bytes in hex editor until they're scared enough and start to talk,
- changing some bytes in file and analyzing result,
- comparing between different versions of files (different languages of Windows).

Some points of reference which I've used across developing:

- [Windows locale codes](https://www.science.co.il/language/Locale-codes.php?s=hexadecimal)
- [GBK Code tables](http://www.khngai.com/chinese/charmap/tblgbk.php?page=2)
- According to my bookmarks there was some bootfont documentation on [chiplink.spaces.live.com](http://chiplink.spaces.live.com/blog/cns!D22964D1AB87651!176.entry) but unfortunately no copy of such page exists anymore.

## Code status & future

I'm releasing this code as part of my disk cleanup.
It's written purely in win32api using very awful codestyle which I used back in 2010.
Originally, I've used code::blocks but since that IDE is very much dead, I've written brand new CMakeLists to make building future-proof.
Builds with MinGW GCC 7.3 just fine!

```shell
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

There were some other versions of this app, but unfortunately none of them were preserved. Instead, there are [some screenshots](doc/old_versions/README.md) from initial development process.

I don't plan to maintain this program any further - just sharing this little app for obscure aspect of the dead OS.
Still, I welcome you to fork my sources and add some features!
