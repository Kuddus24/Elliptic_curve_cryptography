
#include <stdio.h>
#include "../include/common.h"
#include "crypto_math.h"
#include "ecc.h"

// Example usage of the elliptic curve implementation
int main() {
    printf("Elliptic Curve Cryptography Implementation\n");
    printf("===========================================\n\n");
    
    // Initialize a point on the curve
    ECPoint point = {0};
    memcpy(point.x, generator_x, WORDS * sizeof(u29));
    memcpy(point.y, generator_y, WORDS * sizeof(u29));
    point.is_infinity = 0;
    
    printf("Generator Point:\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        printf("%lu ", (unsigned long)point.x[i]);
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        printf("%lu ", (unsigned long)point.y[i]);
    }
    printf("\n\n");
    
    // Example scalar for multiplication
    u29 scalar[WORDS] = {0};
    scalar[0] = 123456789; // Example scalar value
    
    // Perform scalar multiplication using the Left-to-Right method
    ECPoint result_l2r = {0};
    ecc_scalar_mult_l2r(scalar, &point, &result_l2r, mod, mu);
    
    printf("Scalar Multiplication Result (Left-to-Right):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        printf("%lu ", (unsigned long)result_l2r.x[i]);
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        printf("%lu ", (unsigned long)result_l2r.y[i]);
    }
    printf("\n\n");
    
    // Perform scalar multiplication using the Right-to-Left method
    ECPoint result_r2l = {0};
    ecc_scalar_mult_r2l(scalar, &point, &result_r2l, mod, mu);
    
    printf("Scalar Multiplication Result (Right-to-Left):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        printf("%lu ", (unsigned long)result_r2l.x[i]);
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        printf("%lu ", (unsigned long)result_r2l.y[i]);
    }
    printf("\n");
    
    return 0;
}
