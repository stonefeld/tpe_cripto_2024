#include <stdio.h>

#include "args.h"

int main(int argc, char **argv) {
    struct args args;

    args = parse_args(argc, argv);

    if (args.embed_flag) {
        printf("Embedding\n");
        printf("Input file: %s\n", args.input_file);
        printf("Bitmap file: %s\n", args.bitmap_file);
        printf("Output file: %s\n", args.output_file);
        printf("Steg algo: %d\n", args.steg_algo);
        printf("Encryption algo: %d\n", args.encryption_algo);
        printf("Mode: %d\n", args.mode);
    } else if (args.extract_flag) {
        printf("Extracting\n");
        printf("Input file: %s\n", args.input_file);
        printf("Bitmap file: %s\n", args.bitmap_file);
        printf("Output file: %s\n", args.output_file);
        printf("Steg algo: %d\n", args.steg_algo);
        printf("Encryption algo: %d\n", args.encryption_algo);
        printf("Mode: %d\n", args.mode);
    } else {
        printf("No operation specified\n");
    }

    return 0;
}
