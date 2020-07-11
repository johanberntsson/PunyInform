# PunyInform

PunyInform is a library written in Inform 6 which allows people to create text adventures / interactive fiction using the Z-machine virtual machine. It provides a parser, implementations of common verbs, as well as a framework for writing text adventures.

PunyInform is based on the Inform 6 library, developed by Graham Nelson. We are trying to make it easy for authors used to writing in Inform 6 to start using PunyInform. There is a manual describing the differences. 

Games using PunyInform can be compiled to z3, z5 and z8 format. Compared to the Inform 6 library, this means we lack support for Glulx, but we have support for z3 (which the Inform 6 library lacks).

To compile games using PunyInform, we recommend the official Inform 6 compiler maintained by David Kinder, at https://github.com/DavidKinder/Inform6 . Binaries can be found at [if-archive](http://www.ifarchive.org/indexes/if-archiveXinfocomXcompilersXinform6Xexecutables.html) . PunyInform requires Inform 6.34 or newer.

## Motivation

The main goal of PunyInform is to allow for games which are fast and have a small memory footprint. This should make the games run well on older architectures, such as the 8-bit computers of the 1980s. Our main target is to make it suitable for games on the Commodore 64 using [Ozmoo](https://github.com/johanberntsson/ozmoo/).


## Status

We have released version 1.0.  We are about to release version 1.1 which fixes a number of bugs from release 1.0. Please check releasenotes.txt for more details.

If you are interested in our progress, please star or follow the project here on Github.

You can see the manual on our [wiki](https://github.com/johanberntsson/PunyInform/wiki/Manual).

You can see what tasks we're currently working on, tasks we've done and tasks coming up on our Trello board: https://trello.com/b/mfGVsB4H/punyinform

## Credits

PunyInform was conceived and designed by Johan Berntsson and Fredrik Ramsberg. Coding by Johan Berntsson, Fredrik Ramsberg, Pablo Martinez and Tomas Öberg. Includes code from the Inform 6 standard library, by Graham Nelson. Thanks to Stefan Vogt, Jason Compton and John Wilson for advice, testing and promotion. Thanks to David Kinder and Andrew Plotkin for helping out with compiler issues and sharing their deep knowledge of the compiler. Huge thanks to Graham Nelson for creating the Inform 6 compiler and library in the first place.
 
