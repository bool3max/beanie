#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <bhashmap.h>

#include "beanie.h"
#include "utilities.h"

#define BEANIE_COMMENT_CHAR    ';'
#define BEANIE_ASSIGNMENT_CHAR '='

static void
value_cleanup_iterator(const void *key, size_t keylen, void *value) {
    free(value);
}

static void
section_cleanup_iterator(const void *key, size_t keylen, void *value) {
    BHashMap *child_section = value;
    bhm_iterate(child_section, value_cleanup_iterator);
    bhm_destroy(child_section);
}

/* Free all the resources occupied by the INI map data structure.
These include the memory for all of the hashmaps (the root one as well as all of its
children), as well as the memory for all of the keys and values.
*/
void
beanie_cleanup(BeanieMap map_root) {
    /* destroy all of the child maps */
    bhm_iterate(map_root, section_cleanup_iterator);
    bhm_destroy(map_root);
}

/* Get a key-value pair from a certain section of a BeanieMap.
To obtain a key-value pair from the global section, section can be either NULL
or an underscore string ("_"). Returns a pointer to a null-terminated associated value
of the key, or NULL if the key does not exist in the specified section.
*/
char *
beanie_get(const BeanieMap map, const char *section, const char *key) {
    BHashMap *s = bhm_get(
        map,
        section ? section : "_",
        section ? strlen(section) + 1 : sizeof("_")
    );

    /* section does not exist */
    if (!s) return NULL;

    return bhm_get(
        s,
        key,
        strlen(key) + 1
    );
}

/*
Parse the string buffer containing INI file data and return a structure
that can then be queried for individual key-value pairs.

ini_data should be a pointer to a null-terminated string containing the
INI file data.

In the case of an error or INI syntax error, NULL is returned.
*/
BeanieMap
beanie_parse_buffer(const char *ini_data) {
    /*
    The root map maps section names (strings) to other maps that 
    then map INI keys (strings) to INI values (strings). The "global" map 
    that stores INI key-value pairs not belonging to any section is indexed as "_" from the
    root map.
    */
    BHashMap *map_root = bhm_create(0, NULL);
    if (!map_root) {
        return NULL;
    }

    BHashMap *map_global = bhm_create(0, NULL);

    if (!map_global) {
        bhm_destroy(map_root);
        return NULL;
    }

    if (!bhm_set(map_root, "_", sizeof("_"), map_global)) {
        bhm_destroy(map_root);
        bhm_destroy(map_global);
        return NULL;
    }

    /* we store new key-value pairs here */
    BHashMap *current_section = map_global;

    const char *linectx = ini_data;
    while (*linectx) {
        // calculate length of the current line
        size_t ctx_length = 0;
        for (const char *temp = linectx; *temp != '\n' && *temp != '\0'; temp++, ctx_length++);

        // empty line - skip
        if (ctx_length == 0) {
            linectx++;
            continue;
        }

        //fprintf(stderr, "DEBUG: CTX[%lu]: %.*s\n------------\n", ctx_length, (int) ctx_length, linectx);
        
        // strip leading and trailing whitespace from current line
        char *current_line = strip_whitespace(linectx, ctx_length);
        size_t current_line_length = strlen(current_line);

        /* current line after stripping empty, go to next line */
        if (current_line_length == 0) {
            linectx += ctx_length;
            free(current_line);
            continue;
        }

        // fprintf(stderr, "DEBUG: LINE[%2lu]:{%s}\n", current_line_length, current_line);

        /* current line is an INI comment, skip */
        if (current_line[0] == BEANIE_COMMENT_CHAR) {
            linectx += ctx_length;
            free(current_line);
            continue;
        }

        /* start of a section definition */
        if (current_line[0] == '[') {
            /* find matching ] */
            bool found_matching = false;
            for (size_t i = 1; i < current_line_length; i++) {
                if (current_line[i] == ']') {
                    char *section_name = strip_whitespace(current_line + 1, i - 1);

                    /* insert the new section into the root map */
                    BHashMap *new_section = bhm_create(0, NULL);
                    if (!new_section) {
                        /* TODO: CLEANUP MAPS */
                        free(section_name);
                        free(current_line);
                        return NULL;
                    }

                    current_section = new_section;

                    if (!bhm_set(map_root, section_name, strlen(section_name) + 1, current_section)) {
                        /* TODO: CLEANUP MAPS */
                        free(section_name);
                        free(current_line);
                        return NULL;
                    }

                    free(section_name);

                    found_matching = true;
                    break;
                }
            }

            /* no matching ] on current line, parsing error, abort */
            if (!found_matching) {
                /* TODO: CLEANUP MAPS */
                
                /* only free section name buffer if it isn't static _ */
                free(current_line);
                return NULL;
            }
        } else {
            /* start of a key-value definition */
            
            // try to find = separator
            bool found_separator = false;
            for (size_t i = 0; i < current_line_length; i++) {
                if (current_line[i] == BEANIE_ASSIGNMENT_CHAR) {
                    char *key   = strip_whitespace(current_line, i),
                         *value = strip_whitespace(current_line + i + 1, current_line_length - i + 1);

                    if (!bhm_set(current_section, key, strlen(key) + 1, value)) {
                        /* TODO: CLEANUP MAPS */
                        free(key);
                        free(value);
                        free(current_line);
                        return NULL;
                    }

                    free(key);

                    found_separator = true;
                    break;
                }
            }

            // no separator on current line, error, abort
            if (!found_separator) {
                /* TODO: CLEANUP MAPS */
                free(current_line);
                return NULL;
            }
        }

        // jump to next line
        linectx += ctx_length;
        free(current_line);
    }

    return map_root;
}

/*
A wrapper around beanie_parse_buffer that accepts a file path to open, read, and parse.
*/
BeanieMap
beanie_parse_file(const char *path) {
    FILE *file = fopen(path, "r");

    if (!file) {
        return NULL;
    }

    /* seek -> tell trick to get size of file in bytes */
    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    void *buffer = malloc(filesize + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    if (fread(buffer, 1, filesize, file) != filesize) {
        fclose(file);
        free(buffer);
        return NULL;
    }

    /* null-terminate the buffer */
    ((char *) buffer)[filesize] = '\0';

    fclose(file);

    BeanieMap parse_result = beanie_parse_buffer(buffer);

    free(buffer);

    return parse_result;
}