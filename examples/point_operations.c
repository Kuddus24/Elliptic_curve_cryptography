#include <stdio.h>
#include "../include/common.h"
#include "../src/crypto_math.h"
#include "../src/ecc.h"

// Example demonstrating basic elliptic curve point operations
int main() {
    printf("Elliptic Curve Point Operations Example\n");
    printf("======================================\n\n");
    
    // Initialize a point P on the curve (using the generator point)
    ECPoint P = {0};
    memcpy(P.x, generator_x, WORDS * sizeof(u29));
    memcpy(P.y, generator_y, WORDS * sizeof(u29));
    P.is_infinity = 0;
    
    printf("Point P (Generator):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (P.x[i] != 0) {
            printf("%lu ", (unsigned long)P.x[i]);
        }
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (P.y[i] != 0) {
            printf("%lu ", (unsigned long)P.y[i]);
        }
    }
    printf("\n\n");
    
    // Compute 2P (point doubling)
    ECPoint doubleP = {0};
    ecc_double(&P, &doubleP, mod, mu);
    
    printf("Point 2P (Doubling result):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (doubleP.x[i] != 0) {
            printf("%lu ", (unsigned long)doubleP.x[i]);
        }
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (doubleP.y[i] != 0) {
            printf("%lu ", (unsigned long)doubleP.y[i]);
        }
    }
    printf("\n\n");
    
    // Compute 3P = P + 2P (point addition)
    ECPoint tripleP = {0};
    ecc_add(&P, &doubleP, &tripleP, mod, mu);
    
    printf("Point 3P (Addition result):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (tripleP.x[i] != 0) {
            printf("%lu ", (unsigned long)tripleP.x[i]);
        }
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (tripleP.y[i] != 0) {
            printf("%lu ", (unsigned long)tripleP.y[i]);
        }
    }
    printf("\n\n");
    
    // Scalar multiplication to compute 3P directly
    u29 scalar[WORDS] = {0};
    scalar[0] = 3; // Computing 3P
    
    ECPoint scalar3P = {0};
    ecc_scalar_mult_l2r(scalar, &P, &scalar3P, mod, mu);
    
    printf("Point 3P (Via scalar multiplication):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (scalar3P.x[i] != 0) {
            printf("%lu ", (unsigned long)scalar3P.x[i]);
        }
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (scalar3P.y[i] != 0) {
            printf("%lu ", (unsigned long)scalar3P.y[i]);
        }
    }
    printf("\n\n");
    
    // Verify that 3P = P + 2P
    int equal = 1;
    for (int i = 0; i < WORDS; i++) {
        if (tripleP.x[i] != scalar3P.x[i] || tripleP.y[i] != scalar3P.y[i]) {
            equal = 0;
            break;
        }
    }
    
    printf("Verification: 3P == P + 2P ? %s\n", equal ? "Yes (correct)" : "No (error)");
    
    return 0;
}
