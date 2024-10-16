#include <stdio.h>

#include "args.h"
#include "steganography.h"

int main(int argc, char **argv) {
    struct args args = parse_args(argc, argv);

    if (args.action == EMBED) {
        embed(args);
    } else if (args.action == EXTRACT) {
        extract(args);
    } else {
        printf("No operation specified\n");
    }

    return 0;
}
