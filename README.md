# furDS
DS ROM export (and library?) for Furnace
---
The furDS player is currently a WIP but if you want to export a song, you can use my [Furnace fork](https://github.com/AnnoyedArt1256/furnace) to export an NDS Furnace song into my furDS library by going to `file -> export... -> furDS` and exporting the file as `furDS_directory/arm9/source/music.c`
For building the current furDS player make sure you have [devkitPro](https://devkitpro.org/wiki/Getting_Started/) and the NDS libraries installed, then just run
```
make
```
and hopefully you should see a file called `furDS-test.nds` on the furDS directory.
