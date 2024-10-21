#include <stdio.h>

#include "args.h"
#include "steganography.h"

int main(int argc, char **argv) {
    struct args args = args_parse(argc, argv);

    if (args.action == EMBED) {
        steg_embed(args);
    } else if (args.action == EXTRACT) {
        steg_extract(args);
    } else {
        printf("No operation specified\n");
    }

    return 0;
}
