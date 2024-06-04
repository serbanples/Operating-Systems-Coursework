#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

int computation() {
    int i;
    float array[100];

    // Block all signals
    sigset_t mask, prev_mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);

    // Perform computation
    for (i = 0; i < 100; i++)
        array[i] = 1000 * sin(cos((float)i) + tan((float)(100 - i)));

    // Restore previous signal mask
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);

    exit(0);
}

int main() {
    computation(); // Execute computation function
    return 0;
}
