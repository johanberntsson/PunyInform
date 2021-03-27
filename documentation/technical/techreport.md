# Program and Data Structures

## Overview

The main parts of Ozmoo are as follows:

- Parser
- Grammar
- Messages
- Scope

# Parser

The entry point for the parser is `ParseAndPerformAction` which relies on a few global variables:

- buffer: the string to parse
- parse: the tokenised input string, where each token contains a pointer to the word string in `buffer` and a pointer to the dictionary word.

Both `buffer` and `parse` are normally set up by a call to `_ReadPlayerInput` which prints the prompt, waits for player input, and divides the input string into tokens.

## ParseAndPerformAction

## ParsePattern

## ParseToken

## GetNextNoun

## CheckNoun

## _FixIncompleteSentenceOrComplain
Called because sentence shorter than the pattern

Available data: wn, parse and p_pattern_token (last matched token)

Either guess missing parts in the pattern and return true,
or print a suitable error message and return false

## _GuessMissingNoun

## Utility Functions

### _AskWhichNoun

### AddMultipleNouns

### PronounNotice

### _PrintPartialMatch

### _PrintUnknownWord

# Grammar

The standard actions of PunyInform are defined in grammar.h. By default only the most essential actions are included, but different addional subsets can be enabled by defining the OPTIONAL_EXTENDED_VERBSET, OPTIONAL_EXTENDED_METAVERBS, OPTIONAL_PROVIDE_UNDO, and DEBUG constants.

For more detail and a list of standard actions defined for each subset, see the PunyInform manual.

# Messages

All texts and messages are located in messages.h, to make it easy to customise them. Customisation is described in the main PunyInform manual.

Puny internally accesses these messages through the `PrintMsg` function, which takes the identifier and optional arguments. For example, `PrintMsg(MSG_PARSER_NOT_MULTIPLE_VERB);` will print something like "You can't use multiple objects with thas verb.".

# Scope

Scope is a list of things you can interact with. Normally, PunyInform updates the scope when a turn starts, before the after routines are run, before the timers and daemons are run, and before each_turn is run. It is however possible to switch to manual scope updates by defining the constant `OPTIONAL_MANUAL_SCOPE`. With manual_scope enabled, scope is only updated at the start of each turn AND when the `scope_modified` variable is set to true.

The main routine is `_UpdateScope` which is called from `ParseAndPerformAction` and some other locations in the parser to update the scope when objects move or is modified by parsing the scope token. In addition, there are several utility functions that use the loop over or test if objects are in visible or touchable (that is, are in scope).

## _UpdateScope

## LoopOverScope

## ScopeWithin

## TestScope

## ObjectIsUntouchable

## ObjectIsInvisible


# Appendix: Infocom Dictionary and Grammar Formats

PunyInform uses the same dictionary and grammar formats that were created by Infocom, and which are also used by Inform 6. Infocom created two versions of the grammar tables, and we only use grammar version 2.

## Dictionary

Extra data in dictionary:

```
byte 0 & $1 : Verb flag
byte 0 & $2 : Meta flag
255 - (byte 1)
```
Verb number (255 - value is for "traditional Infocom reasons")

## Grammar 2

Grammar table is always located at the start of static memory (address pointed to by word at $0e in header). Word (Verb number) points to the start address for the grammar for a verb.

### Grammar for a verb

For a detailed description of grammar version 2, read the text starting with "GV2 is a much more suitable data structure" at https://www.inform-fiction.org/source/tm/TechMan.txt

```
Byte 0: Number of grammar lines for this verb + 1
Byte 1 ... Syntax line 0, 1, ...
```

Grammar line
```
0: Highbyte of action_value
1: Lowbyte of action_value
```

(action_value & $0400) ~= 0 means the action is reversed

action = action_value & $03ff

3 bytes per token:

-    byte 0 & $0f : token_type
-    byte 1 + 2 : Token data

If token_type = 15 (ENDIT_TOKEN), this is the end of line, and byte 1 and 2 are not supplied.

```
ILLEGAL_TT         = 0;    ! Types of grammar token: illegal
ELEMENTARY_TT      = 1;    !     (one of those below)
PREPOSITION_TT     = 2;    !     e.g. 'into'
ROUTINE_FILTER_TT  = 3;    !     e.g. noun=CagedCreature
ATTR_FILTER_TT     = 4;    !     e.g. edible
SCOPE_TT           = 5;    !     e.g. scope=Spells
GPR_TT             = 6;    !     a general parsing routine
ENDIT_TOKEN        = 15
```

## Grammar 1

Note that Grammar 1 is not used by PunyInform.

### Grammar for Quit
01 Number of grammar lines

```
00 params wanted
ff token 1
00 token 2
00 token 3
00 token 4
00 token 5
00 token 6
00 action number
```


```
02 Grammar line 1, highbyte of action routine
bd Grammar line 1, lowbyte of action routine
```


Tokens available in grammar version 1:
```
NOUN_TOKEN         = 0;    ! The elementary grammar tokens, and
HELD_TOKEN         = 1;    ! the numbers compiled by Inform to
MULTI_TOKEN        = 2;    ! encode them
MULTIHELD_TOKEN    = 3;
MULTIEXCEPT_TOKEN  = 4;
MULTIINSIDE_TOKEN  = 5;
CREATURE_TOKEN     = 6;
SPECIAL_TOKEN      = 7;
NUMBER_TOKEN       = 8;
ENDIT_TOKEN = $0f ! Legal, but doesn't seem to be used
```

Token $10-$2f are routine filters (ROUTINE_FILTER_TT).  e.g. noun=CagedCreature

Token $30-$4f are general parsing routines (GPR_TT).
Routine address is in #preactions_table-->(token-48)

Token $50-$7f are scope tokens (SCOPE_TT). e.g. scope=Spells
Routine address is in #preactions_table-->(token-80)


Token $80-$b3 are attribute filters (ATTR_FILTER_TT). e.g. edible

Token $b4-$ff are prepositions (PREPOSITION_TT). e.g. 'into'
Prepositions are located in the "adjectives table". The start address is in the constant #adjectives_table .
Each entry consists of two words. If the second word is the preposition-number (like $ff), the first
word is the address of the dictionary word. There is no length number or end marker. You should just expect
to find the entry somewhere in there.

