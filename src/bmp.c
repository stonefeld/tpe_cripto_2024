#include "bmp.h"

#include <string.h>

struct bmp_header bmp_get_header(char *bitmap_data) {
    struct bmp_header header;
    memcpy(&header, bitmap_data, sizeof(struct bmp_header));
    return header;
}
