# beanie

Beanie is a tiny INI configuration file parser written in C.
It uses [**`bhashmap`**](https://github.com/bool3max/bhashmap) as a dependency
to store key-value configuration pairs of the INI file.

Beanie was written as a quick exercise, and as such it is hevily opinionated and minimally configurable, and not very optimized. It parses exclusively whole buffers or files - 
you cannot request a specific portion nor section of the file to parse. If you are looking for a battle-tested, configurable C library for parsing
INI files, take a look at [**`benhoyt/inih`**](https://github.com/benhoyt/inih).

## INI File syntax and rules

As the INI file format is not standardized, Beanie conforms to an arbitrary, non-configurable set of syntax rules, described below.

* Lines are stripped of leading and trailing whitespace.
* Blank lines are ignored.
* Lines beginning with a semicolon (`;`) are considered as comments and are ignored.
    * Inline comments are not supported.
* Key-value pairs are separated by an equals sign (`=`).
    * If a line contains multiple equals signs, the first one is considered to be the separator.
    * Keys are stripped of trailing whitespace, and values are stripped of leading whitespace (in both of the lines `number = 123` and `number=123`, the key is `number` and the value is `123`).
* Sections are defined by a name enclosed in angle brackets (`[]`).
    * Section names are stripped of leading and trailing whitespace (`[  abc   ]` defines a section named `abc`).
    * Characters after a valid section definition are ignored (a line containing`[abc]one two ` defines a valid section named `abc`).
* Key-value pairs defined before any single section has been encountered in a file are considered to
be "global" - they do not belong to any section.
    * After the first section in a file is encountered, no more global key-value pairs can be defined.
* Value-less pairs are allowed (`key=`). In such cases, the value is an empty string.