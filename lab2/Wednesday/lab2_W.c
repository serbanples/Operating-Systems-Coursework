#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

float AVG(int n) {
    int aux, prime = 2, number_of_div = 0, number_of_primes = 0;
    float avg = 0;
    while (n > 0) {
        aux = prime;
        number_of_div = 0;
        for (int f = 2; f <= aux / 2; f++) {
            if (aux % f == 0)
                number_of_div++;
        }
        if (number_of_div == 0) {
            avg += prime;
            number_of_primes++;
            n--;
        }
        prime++;
    }
    return avg / number_of_primes;
}

int main(int argc, char **argv) {
    int aflag = 0;
    int bflag = 0;
    char *cvalue = NULL;
    char *dvalue = NULL;
    char *evalue = NULL;
    int index;
    int c;

    opterr = 0;

    while ((c = getopt(argc, argv, "abc:de:")) != -1) {
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
            case 'd':
                dvalue = (char*) malloc(20 * sizeof(char));
                sprintf(dvalue, "%f", AVG(100));
                break;
            case 'e':
                evalue = optarg;
                break;
            case '?':
                if (optopt == 'c')
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort();
        }
    }

    if (evalue == NULL) {
        fprintf(stderr, "Option -e requires an argument.\n");
        return 1;
    }

    float cvalue_AVG = AVG(atoi(cvalue));
    float evalue_AVG = AVG(atoi(evalue));

    printf("aflag = %d, bflag = %d, cvalue = %s\n", aflag, bflag, cvalue);
    printf("avg of the first 100 prime numbers = %s\n", dvalue);
    printf("evalue = %s\n", evalue);
    printf("AVG(%f, %f) = %f\n", cvalue_AVG, evalue_AVG, (cvalue_AVG + evalue_AVG) / 2);

    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);

    return 0;
}
