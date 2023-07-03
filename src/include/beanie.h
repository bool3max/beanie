#include <bhashmap.h>

/* API: beanie_parse is an alias for beanie_parse_file */
#define beanie_parse(path) beanie_parse_file(path);

typedef BHashMap* BeanieMap;

BeanieMap
beanie_parse_buffer(const char *ini_data);

BeanieMap
beanie_parse_file(const char *path);

BeanieMap
beanie_parse_stream(FILE *stream); 