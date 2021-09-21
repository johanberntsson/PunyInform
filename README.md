# PunyInform

PunyInform is a library written in Inform 6 which allows people to
create text adventure games / interactive fiction using the Z-machine
virtual machine, to be played on 8-bit computers as well as newer
platforms. It provides a parser, implementations of common verbs, as
well as a framework for writing text adventures.

PunyInform is based on the Inform 6 library, developed by Graham Nelson.
We are trying to make it easy for authors used to writing in Inform 6 to
start using PunyInform. There is a manual describing the differences.

Games using PunyInform can be compiled to z3, z5 and z8 format. Compared
to the Inform 6 library, this means we lack support for Glulx, but we
have support for z3 (which the Inform 6 library lacks).

To compile games using PunyInform, we recommend the official Inform 6
compiler maintained by David Kinder, at
https://github.com/DavidKinder/Inform6.  Binaries can be found at the
[IF-Archive](https://www.ifarchive.org/indexes/if-archive/infocom/compilers/inform6/executables/).
PunyInform requires Inform v6.35.

You can also use [borogove.app](https://borogove.app/) to write PunyInform
code and play the resulting game in your browser. Just select PunyInform from
the dropdown menu under Inform 6 and press the arrow next to it. It's meant
for desktop use only though, so don't expect to do development work on your
phone.

![A minimal game in PunyInform](https://github.com/johanberntsson/PunyInform/blob/master/documentation/screenshots/simplegame.png?raw=true)

## Motivation

The main goal of PunyInform is to allow for games which are fast and
have a small memory footprint. This should make the games run well on
older architectures, such as the 8-bit computers of the 1980s. Our main
target is to make it suitable for games on the Commodore 64 using
[Ozmoo](https://github.com/johanberntsson/ozmoo/).


## Status

PunyInform is fully working and has been used for multiple games. We
carefully add and change features and fix bugs as they are found. Please
check releasenotes.txt for more details.

If you are interested in our progress, please star or follow the project
here on Github. Announcements are made on https://twitter.com/FRamsberg
and on https://intfiction.org/c/authoring/inform-6/66

You can read the manual on our
[wiki](https://github.com/johanberntsson/PunyInform/wiki/Manual). It is
also included as a PDF when you download PunyInform.

There is also a Game Author's Guide (under Documentation) with various
tips for authors writing a game using PunyInform. It's not intended to
teach the entire library, but more to give some tips you may find useful
when you have a working knowledge of the library and you've started
developing a game which you think you will release.

You can see what tasks we're currently working on, tasks we've done and
tasks coming up on our Trello board:
https://trello.com/b/mfGVsB4H/punyinform

## Learning PunyInform

There are two manuals you need to know about: DM4 and the PunyInform
manual. Read on to learn how to use them and where to find them.
Alternatively, you can start learning PunyInform by following the
tutorials under
[Articles](https://github.com/johanberntsson/PunyInform#articles).

* **Inform Designer's manual, 4th Edition, AKA DM4:
  [Online version](https://www.inform-fiction.org/manual/html/index.html)
  or [PDF version](https://www.inform-fiction.org/manual/DM4.pdf)**

* **PunyInform manual:
  [Online version](https://github.com/johanberntsson/PunyInform/wiki/manual)
  or [PDF version](https://github.com/johanberntsson/PunyInform/tree/master/documentation)**

Writing a game with PunyInform is very similar to writing a game with
the Inform 6 standard library - almost everything works the same. If you
want to learn PunyInform by reading manuals, you first need to read the
official documentation for the Inform 6 standard library, called The
Inform Designer's Manual, 4th Edition, or DM4 for short (see links
above). You can assume that everything in it also goes for PunyInform
(but read the short list stating the biggest differences just below the
next paragraph!).

As you start trying out examples, or start writing your own game, and
there's something which you don't get to work, you should check the
PunyInform manual, which describes everything that is different from the
standard library (see links above).

The biggest differences between the standard library and PunyInform are:

* The files you need to include, and what goes where in your source
  code. Start your project by copying `minimal.inf` in the root
  directory and you're good to go.

* Parts of the PunyInform library are optional. I.e. if you want to use
  the directions northwest, southwest, northeast and southeast, you need
  to define the constant `OPTIONAL_FULL_DIRECTIONS`. If you want just
  about all the verbs and actions available in the standard library to
  work in PunyInform, you need to define `OPTIONAL_EXTENDED_VERBSET`.
  See [Customizing the
  library](https://github.com/johanberntsson/PunyInform/wiki/Manual#customizing-the-library)
  in the PunyInform manual.

* Directions are handled quite differently. See
  [Direction handling](https://github.com/johanberntsson/PunyInform/wiki/Manual#direction-handling)
  in the PunyInform manual.

* Changing library messages (like the message "Taken." that is printed
  when the player takes something) is done differently. See
  [Library messages and customization](https://github.com/johanberntsson/PunyInform/wiki/Manual#library-messages-and-customization)
  in the PunyInform manual.

* PunyInform doesn't support identical objects. You can have very
  similar objects like a small green book and a small red book, but the
  player must be able to say which one (s)he means.

* PunyInform doesn't have the versatile routine `WriteListFrom`, for
  printing what's in a location or in/on an object. Instead, it has a
  simpler routine called `PrintContents`. See
  [Printing the contents of an object](https://github.com/johanberntsson/PunyInform/wiki/Manual#printing-the-contents-of-an-object)
  in the PunyInform manual.

## Tools

* To work with source code you may want a text editor which has a syntax
  colouring mode for Inform 6. Some popular choices include Visual Studio
  Code and Atom, both available for Windows, Mac and Linux.

* To quickly build a game for play on Commodore 64, 128, Plus/4 or MEGA65, you
  can use [Ozmoo Online](http://microheaven.com/ozmooonline/).

* To build a game for 25 different platforms at once, you can use the
  [Puddle BuildTools](https://github.com/ByteProject/Puddle-BuildTools). This
  tool set can be installed on Linux. Windows users can create a virtual machine
  running Linux.

## Games Using PunyInform

These games are known to use PunyInform:

* [Tristam Island](https://hlabrande.itch.io/tristam-island), by Hugo Labrande.

* [Calypso](http://microheaven.com/ozmoo/games/calypso/calypso.z3)
  ([src](https://github.com/dave-f/calypso)), by Dave Footitt

* [Behind Closed Doors 9B](http://www.zenobi.co.uk/2020/07/punyinfom-enters-fray.html),
  by John Wilson.

* [Alien Research Centre 3](https://ifdb.tads.org/viewgame?id=cf9x7zhfnohedim),
  by John Wilson.

* [Return to the Castle](https://ifdb.tads.org/viewgame?id=5bo8vhrguyr1d11s),
  by John Wilson.

* [The Fishing-Trip - Brown Trout and Goblins](https://ifdb.tads.org/viewgame?id=fqhoqlh01ex5wwvo),
  by John Wilson.

* [Ghost Mountain](https://compdelta.itch.io/ghost-mountain), by Michael Cox.

* [Hibernated 1, Director's Cut](https://8bitgames.itch.io/hibernated1), by Stefan Vogt.

* [Craverly Heights](https://github.com/johanberntsson/Craverly-Heights_PunyInform), ported by Johan Berntsson from the original game written by Ryan Veeder.

There were also nine new PunyInform games submitted to [PunyJam #1](https://ifdb.org/viewcomp?id=z913f8kco23t5dz4):

* [Captain Cutter's Treasure](https://warrigal.itch.io/captain-cutters-treasure), by Garry Francis.

* [Djinn on the Rocks](https://captainedgecase.itch.io/djinn-on-the-rocks), by Joshua Wilson.

* [Pub Hubbub](https://christopherdrum.itch.io/pub-hubbub), by Christopher Drum.

* [Death Number 4](https://themeteorstorm.itch.io/deathnumber4), by Dave Footitt.

* [Pub Adventure](https://electrotrains.itch.io/pub-adventure), by Tom and Robin Edwards.

* [Closet of Mystery](https://compdelta.itch.io/closet-of-mystery), by Michael Cox.

* [Arthur's day out](https://waulok.itch.io/arthurs-day-out), by Jason Oakley.

* [Buccaneer's Cache](https://frodewin.itch.io/buccaneers-cache), by Wilfried Elmenreich.

* [The Job](https://fredrikr.itch.io/the-job), by Fredrik Ramsberg.

All of these games can be downloaded from the links above. Many of them
are also available at [Ozmoo Online](http://microheaven.com/ozmooonline/)
where you can create disk images for Commodore C64, C128 or Plus/4.

## Articles

Fredrik Ramsberg has written a few articles describing PunyInform for
beginners:

* [PunyInform, a new library for writing text adventures for old computers](https://vintageisthenewold.com/punyinform-a-new-library-for-writing-text-adventures-for-old-computers/)
  gives a general overview and introduction.

* [PunyInform: Installing the tools and compiling a game](https://vintageisthenewold.com/punyinform-installing-the-tools-and-compiling-a-game/)
  goes more into detail on how to setup an environment with PunyInform
  and the inform compiler on Windows, and getting started on a simple
  game.

* [PunyInform Coding 1: The Basics](https://vintageisthenewold.com/punyinform-coding-1-the-basics/)
  covers the fundamentals of developing a game using PunyInform.

* [PunyInform Coding 2: Objects and Actions](https://vintageisthenewold.com/punyinform-coding-2-objects-and-actions/)
  covers more of verbs, actions and adding code to objects in PunyInform.

* [PunyInform Coding 3: Daemons and Describing Objects](https://vintageisthenewold.com/punyinform-coding-3-daemons-and-describing-objects/)
  covers daemons, some techniques for affecting how objects are described,
  fake actions and performing actions in code.

* [A tutorial index](https://docs.google.com/spreadsheets/d/1DSAu3bTkmWByLOGGGeb9qj9TwXoUEvAJCihF_8hYzPE/edit?usp=sharing)
saying in which coding tutorial different concepts are first explained,
and which concepts we intend to cover but haven't yet.

Fredrik has also written
[a comparison of PunyInform and the standard Inform 6 library](https://intfiction.org/t/punyinform-vs-inform-6-a-comparison/46625).

## Community

If you want to ask questions about PunyInform, these are good places to go:

* **[intfiction.org forum, Inform 6 section](https://intfiction.org/c/authoring/inform-6/66).
  Please tag your posts with "punyinform".**

* **[PunyInform Discord server](https://discord.com/invite/y9anzKJTBa).
  Please note that you need to click checkmark in the Rules channel and
  wait a few minutes before you can post.**

There are also categories/channels in the above forums where you can
discuss plot, story, puzzles etc, and posts requests for beta-testers
for your game.

## Credits

PunyInform was conceived and designed by Johan Berntsson and Fredrik
Ramsberg. Coding by Johan Berntsson, Fredrik Ramsberg, Pablo Martinez
and Tomas Öberg. Includes code from the Inform 6 standard library, by
Graham Nelson. Thanks to Stefan Vogt, Jason Compton, John Wilson, Hugo
Labrande, Richard Fairweather, Adam Sommerfield, auraes and Hannesss for
issue reporting, advice, testing, code contributions and promotion.
Thanks to David Kinder and Andrew Plotkin for helping out with compiler
issues and sharing their deep knowledge of the compiler. Huge thanks to
Graham Nelson for creating the Inform 6 compiler and library in the
first place.
