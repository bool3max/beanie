# beanie

Beanie is a tiny INI configuration file parser written in C.
It uses [**`bhashmap`**](https://github.com/bool3max/bhashmap) as a dependency
to store key-value configuration pairs of the INI file.

Beanie was written as a quick exercise, and as such it is hevily opinionated, minimally configurable, and not very optimized. It parses exclusively whole buffers or files - 
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

## API

#### **`beanie_parse_buffer`**

```c
BeanieMap
beanie_parse_buffer(const char *ini_data);
```

Parse the entirety of the null-terminated string buffer containing INI file data and return
a data structure that can then be queried for individual key-value pairs.

Returns NULL on error on INI syntax error.

#### **`beanie_parse_file`**

```c
BeanieMap
beanie_parse_file(const char *path);
```
Open, read, and parse the file at path `path`. Returns NULL on error or INI syntax error.

#### **`beanie_get`**

```c
char *
beanie_get(const BeanieMap map, const char *section, const char *key); 
```

Get the associated value of a particular key in a particular section. If the key is in a global section, `section` should be either `NULL` or `"_"`.

Returns a pointer to the associated null-terminated string if it is found, or NULL if the section does not exist or the key is not found in the section.

#### **`beanie_cleanup`**

```c
void
beanie_cleanup(BeanieMap map);
```
Free the resources occupied by the map. The map should not be accessed afterwards.

## Examples

1. Parse the file `user_config.ini` and access the `username` key from the `proxy` section:

```c
BeanieMap map = beanie_parse("user_config.ini");
if (!map) {
    /* error parsing file */
}

char *username = beanie_get(map, "proxy", "username");
if (username == NULL) {
    /* no "proxy" section in file, or no "username" key in "proxy" section
}
```

2. Acess the `path` key from the global section:

```c
char *path = beanie_get(map, NULL, "path");
if (path == NULL) {
    /* no "path" key in the global section */
}
```

3. Free the resources occupied by the map: 

```c
beanie_cleanup(map);
```