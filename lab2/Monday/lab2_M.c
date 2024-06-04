#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char **argv)
{
    int aflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    int index;
    int c;

    char *gvalue = NULL;
    char *evalue = NULL;

    opterr = 0;

    while ((c = getopt (argc, argv, "abc:ge:")) != -1)
    switch (c) {
        case 'a':
            aflag = 1;
            break;
        case 'b':
            bflag = 1;
            break;
        case 'c':
            cvalue = optarg;
            break;
        case '?':
            if (optopt == 'c')
                fprintf (stderr, "Option -%c requires an argument.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Unknown option `-%c'.\n", optopt);
            else
                fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
            return 1;
        case 'g':
            if( cvalue == NULL ) {
                fprintf(stderr, "Option -g requires an argument.\n");
                return 1;
            }
            int cvalue_int = atoi(cvalue);
            printf("g enabled: %d\n",aflag | bflag | cvalue_int);
            break;
        case 'e':
            evalue = optarg;
            break;
        default:
            abort ();
    }

    int evalue_int = atoi(evalue);
    evalue_int += aflag + bflag;
    if(cvalue != NULL)
        evalue_int += atoi(cvalue);

    printf ("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);
    printf ("evalue is %d\n", evalue_int);

    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);
    return 0;
}