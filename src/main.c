#include <stdio.h>

#include "args.h"
#include "steganography.h"

int main(int argc, char **argv) {
    struct args args = args_parse(argc, argv);
    switch (args.action) {
        case EMBED: steg_embed(args); break;
        case EXTRACT: steg_extract(args); break;
        default: printf("Error: No operation specified\n"); break;
    }
}
