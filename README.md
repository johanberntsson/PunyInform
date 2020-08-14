# PunyInform

PunyInform is a library written in Inform 6 which allows people to create text adventures / interactive fiction using the Z-machine virtual machine. It provides a parser, implementations of common verbs, as well as a framework for writing text adventures.

PunyInform is based on the Inform 6 library, developed by Graham Nelson. We are trying to make it easy for authors used to writing in Inform 6 to start using PunyInform. There is a manual describing the differences.

Games using PunyInform can be compiled to z3, z5 and z8 format. Compared to the Inform 6 library, this means we lack support for Glulx, but we have support for z3 (which the Inform 6 library lacks).

To compile games using PunyInform, we recommend the official Inform 6 compiler maintained by David Kinder, at https://github.com/DavidKinder/Inform6 . Binaries can be found at [if-archive](http://www.ifarchive.org/indexes/if-archiveXinfocomXcompilersXinform6Xexecutables.html) . PunyInform requires Inform v6.34 or newer.

## Motivation

The main goal of PunyInform is to allow for games which are fast and have a small memory footprint. This should make the games run well on older architectures, such as the 8-bit computers of the 1980s. Our main target is to make it suitable for games on the Commodore 64 using [Ozmoo](https://github.com/johanberntsson/ozmoo/).


## Status

We consider PunyInform ready for real-world use. Please check releasenotes.txt for more details on the latest version, what bugs have been fixed, what features have been added etc.

If you are interested in our progress, please star or follow the project here on Github. Announcements are made on https://twitter.com/FRamsberg and on https://intfiction.org/c/authoring/inform-6-and-7/19

You can see the manual on our [wiki](https://github.com/johanberntsson/PunyInform/wiki/Manual).

You can see what tasks we're currently working on, tasks we've done and tasks coming up on our Trello board: https://trello.com/b/mfGVsB4H/punyinform

## Publicity

Fredrik Ramsberg has written two articles describing PunyInform for beginners.

The first is [PunyInform, a new library for writing text adventures for old computers](https://vintageisthenewold.com/punyinform-a-new-library-for-writing-text-adventures-for-old-computers/), which gives a general overview and introduction.

The second is [PunyInform: Installing the tools and compiling a game](https://vintageisthenewold.com/punyinform-installing-the-tools-and-compiling-a-game/) which goes more into detail on how to setup an environment with PunyInform and the inform compiler on Windows, and getting started on a simple game.

## Credits

PunyInform was conceived and designed by Johan Berntsson and Fredrik Ramsberg. Coding by Johan Berntsson, Fredrik Ramsberg, Pablo Martinez and Tomas Öberg. Includes code from the Inform 6 standard library, by Graham Nelson. Thanks to Stefan Vogt, Jason Compton, John Wilson, Hugo Labrande and Richard Fairweather for advice, testing, code contributions and promotion. Thanks to David Kinder and Andrew Plotkin for helping out with compiler issues and sharing their deep knowledge of the compiler. Huge thanks to Graham Nelson for creating the Inform 6 compiler and library in the first place.
