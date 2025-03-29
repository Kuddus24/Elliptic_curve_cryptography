
#include <stdio.h>
#include <string.h>
#include "../include/common.h"
#include "../src/crypto_math.h"
#include "../src/ecc.h"

// Function to test basic modular arithmetic operations
void test_modular_arithmetic() {
    printf("Testing modular arithmetic operations...\n");
    
    // Test values
    u29 a[WORDS] = {0};
    u29 b[WORDS] = {0};
    u29 result[WORDS] = {0};
    
    // Set test values
    a[0] = 123456789;
    b[0] = 987654321;
    
    // Test addition
    add_29bit(a, b, result, WORDS);
    printf("Addition result: %lu\n", (unsigned long)result[0]);
    
    // Test subtraction
    sub_29bit(a, b, result, WORDS);
    printf("Subtraction result: %lu\n", (unsigned long)result[0]);
    
    // Test multiplication and modular reduction
    u29 prod[2 * WORDS] = {0};
    mult_29bit(a, b, prod);
    barrett_reduction(prod, mod, mu, result);
    printf("Multiplication and reduction result: %lu\n", (unsigned long)result[0]);
    
    printf("Modular arithmetic tests completed.\n\n");
}

// Function to test elliptic curve operations
void test_ecc_operations() {
    printf("Testing elliptic curve operations...\n");
    
    // Initialize a point on the curve
    ECPoint P = {0};
    memcpy(P.x, generator_x, WORDS * sizeof(u29));
    memcpy(P.y, generator_y, WORDS * sizeof(u29));
    P.is_infinity = 0;
    
    // Test point doubling
    ECPoint doubleP = {0};
    ecc_double(&P, &doubleP, mod, mu);
    printf("Point doubling test completed.\n");
    
    // Test point addition
    ECPoint sumP = {0};
    ecc_add(&P, &doubleP, &sumP, mod, mu);
    printf("Point addition test completed.\n");
    
    // Test scalar multiplication
    u29 scalar[WORDS] = {0};
    scalar[0] = 123;
    
    ECPoint multP_l2r = {0};
    ecc_scalar_mult_l2r(scalar, &P, &multP_l2r, mod, mu);
    printf("Left-to-right scalar multiplication test completed.\n");
    
    ECPoint multP_r2l = {0};
    ecc_scalar_mult_r2l(scalar, &P, &multP_r2l, mod, mu);
    printf("Right-to-left scalar multiplication test completed.\n");
    
    // Verify that both methods give the same result
    int equal = 1;
    for (int i = 0; i < WORDS; i++) {
        if (multP_l2r.x[i] != multP_r2l.x[i] || multP_l2r.y[i] != multP_r2l.y[i]) {
            equal = 0;
            break;
        }
    }
    
    printf("Scalar multiplication methods match: %s\n", equal ? "Yes (correct)" : "No (error)");
    printf("Elliptic curve operation tests completed.\n\n");
}

// Test hexadecimal conversion
void test_hex_conversion() {
    printf("Testing hexadecimal conversion...\n");
    
    // Example hex string (the prime modulus p)
    char hex[] = "e92e40ad6f281c8a082afdc49e1372659455bec8ceea043a614c835b7fe9eff5";
    u29 bignum[WORDS] = {0};
    
    parse_to_int(hex, bignum);
    
    printf("Converted big integer array:\n");
    for (int i = 0; i < WORDS; i++) {
        printf("bignum[%d] = %lu\n", i, (unsigned long)bignum[i]);
    }
    
    // Verify conversion by comparing with the known mod array
    int equal = 1;
    for (int i = 0; i < WORDS; i++) {
        if (bignum[i] != mod[i]) {
            equal = 0;
            break;
        }
    }
    
    printf("Hex conversion matches modulus: %s\n", equal ? "Yes (correct)" : "No (error)");
    printf("Hexadecimal conversion test completed.\n\n");
}

int main() {
    printf("Running ECC Implementation Tests\n");
    printf("================================\n\n");
    
    // Run tests
    test_modular_arithmetic();
    test_ecc_operations();
    test_hex_conversion();
    
    printf("All tests completed.\n");
    
    return 0;
}
