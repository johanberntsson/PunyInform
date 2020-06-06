# PunyInform

PunyInform is a library written in Inform which allows people to create text adventures / interactive fiction using the Z-machine virtual machine. It provides a parser, implementations of common verbs, as well as a framework for writing text adventures.

PunyInform is based on the Inform 6 library, developed by Graham Nelson. We are trying to make it easy for authors used to writing in Inform 6 to start using PunyInform. There will be a manual describing the differences. 

We aim to make it possible to compile a game to z3, z5 and z8. Compared to the Inform 6 library, this means we lack support for Glulx, but we have support for z3 (which the Inform 6 library lacks).

To compile games using PunyInform, we recommend the official Inform 6 compiler maintained by David Kinder, at https://github.com/DavidKinder/Inform6 . We are using version 6.34 for all development work. Newer versions are highly likely to work fine as well.

## Motivation

The main goal of PunyInform is to allow for games which are fast and have a small memory footprint. This should make the games run well on older architectures, such as the 8-bit computers of the 1980s. Our main target is to make it suitable for games on the Commodore 64 using [Ozmoo](https://github.com/johanberntsson/ozmoo/).


## Status

We're close to a beta release. Pretty much everything works.

If you are interested in our progress, please star or follow the project here on Github.

You can see the manual on our [wiki](https://github.com/johanberntsson/PunyInform/wiki/Manual).


You can see what tasks we're currently working on, tasks we've done and tasks coming up on our Trello board: https://trello.com/b/mfGVsB4H/punyinform
