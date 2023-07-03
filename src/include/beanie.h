#include <bhashmap.h>

/* API: beanie_parse is an alias for beanie_parse_file */
#define beanie_parse(path) beanie_parse_file(path);

typedef BHashMap* BeanieMap;

char *
beanie_get(const BeanieMap map, const char *section, const char *key); 

BeanieMap
beanie_parse_buffer(const char *ini_data);

BeanieMap
beanie_parse_file(const char *path);

BeanieMap
beanie_parse_stream(FILE *stream); 

void
beanie_cleanup(BeanieMap map);