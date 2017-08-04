# Stil Yet Another Sokoban

Download Page: http://grayskygames.com/sokoban.html

Official source code repo: https://github.com/davidjoffe/sokoban

License: Your choice of GPL 2, GPL 2 or higher, or BSD-style license (http://www.opensource.org/licenses/bsd-license.php)

# About

Still Yet Another Sokoban is a clone of the popular puzzle game Sokoban. It runs on Windows, Linux, Mac OS X and more.

Sokoban was invented in Japan and the original program was written by Hiroyuki Imabayashi. It is copyright (C) 1982 Thinking Rabbit Inc. Japan.

"Sokoban" means "warehouse keeper" in Japanese; the goal of the game is to push all the crates into the designated crate positions without getting stuck. Crates can only be pushed forward.

The levels provided are not my own, they are assembled from various sources. The default 90 levels are public domain.

You can add more sets of levels in the data/sokoban/levels folder. The file must have the .txt extension. See existing sets of levels for examples.

Please do not contact me to assist with solving individual levels.

Created by: David Joffe http://djoffe.com/

![Sokoban Screenshot](http://grayskygames.com/sokoban/gallery/SYA%20Sokoban.png)

# Developer Info / Build Info

Dependencies:  LibSDL1.2 http://libsdl.org/

## Windows Build Instructions

* Visual Studio 2010 project is included.
* **NB** You must set your Visual Studio 'Working Directory' to .. in order to run this (under Project / Properties / Configure Properties / Debugging) or it won't find the data files and just immediately exit on run
* If you get an error about not finding SDL DLL(s) when running, set 'Project / Properties / Configure Properties / Environment' to: PATH=c:\your\path\to\DLLs;%PATH%

## Linux Build Instructions

Type 'make'

Run with ./syasokoban if it built correctly

Installing dependencies on Debian etc.:

apt-get install libsdl1.2-dev

## Mac OS X Build Instructions

Same as Linux - type 'make'. Run with ./syasokoban if it built correctly.

# History / Changes

13 Nov 2016

* Windows compile fix
* Fix possible crash on press Esc in About window
* Create new github source code repo for the project: https://github.com/davidjoffe/sokoban

2.0.1(d)        4 Aug 2017
                Nearly identical to v2.0.1(c), plus Windows compile fix

2.0.1(c)        8 Oct 2016
                Identical release to v2.0.1(b), plus some Linux compile fixes

2.0.1(b)        3 Oct 2016
                Identical release to v2.0.1, except executables have been signed

2.0.1           29 July 2007
                [URL changed to grayskygames.com]

2.0             18 June 2006
                Add support for multiple sets of levels (finally!)
                Add "Are you sure" messagebox when closing
                Fixed 'uses lots of CPU' bug

1.02            21 January 2004
                Updated release (some bugfixes)
                6 March 2005
                Mac OS X release

1.01            11 May 2003
                Source code release.

1.0             20 May 2002
                First release.
