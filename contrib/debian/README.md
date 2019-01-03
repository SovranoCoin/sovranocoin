
Debian
====================
This directory contains files used to package sovranocoind/sovranocoin-qt
for Debian-based Linux systems. If you compile sovranocoind/sovranocoin-qt yourself, there are some useful files here.

## sovranocoin: URI support ##


sovranocoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install sovranocoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your sovranocoinqt binary to `/usr/bin`
and the `../../share/pixmaps/sovranocoin128.png` to `/usr/share/pixmaps`

sovranocoin-qt.protocol (KDE)

