#include "a7.h"

int main(int argc, char **argv)
{
    if (argc < 2)
        return EXIT_FAILURE;

    if (argv[1][0] != '-' || argv[1][1] == '\0')
        return EXIT_FAILURE;

    char command = argv[1][1];

    if (command == 'b' && argc == 4)
    {
        int check = build(argv[2], argv[3]);
        if (check != 1)
            fprintf(stdout, "exit failure\n");
        return EXIT_FAILURE;
        fprintf(stdout, "1\n");
        return EXIT_SUCCESS;
    }
    else if (command == 'e' && argc == 3)
    {
        int check = evaluate(argv[2]);
        if (check != 1)
            return EXIT_FAILURE;
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}
