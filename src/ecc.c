
#include "ecc.h"

// Generator point coordinates
u29 generator_x[WORDS] = {511206874, 254855740, 285194860, 443465366, 268068044, 451848614, 325285346, 118243884, 13861873, 0};
u29 generator_y[WORDS] = {473912554, 460180803, 198869900, 64737929, 437536564, 372033891, 274885473, 29463607, 4333742, 0};

// Modular exponentiation for modular inverse using Fermat's Little Theorem
void mod_inverse(u29* x, u29* p, u29* mu, u29* result) {
    u29 exponent[WORDS] = {0};
    memcpy(exponent, p, WORDS * sizeof(u29));
    exponent[0] -= 2; // Compute p-2 for Fermat's Little Theorem

    // Call Montgomery ladder for modular exponentiation
    montgomery_ladder_exponentiation(x, exponent, p, mu, result);
}

// Elliptic curve point addition
void ecc_add(ECPoint* P, ECPoint* Q, ECPoint* result, u29* mod, u29* mu) {
    if (P->is_infinity) {
        memcpy(result, Q, sizeof(ECPoint));
        return;
    }
    if (Q->is_infinity) {
        memcpy(result, P, sizeof(ECPoint));
        return;
    }
    
    // Check if points are inverses of each other
    if (memcmp(P->x, Q->x, WORDS * sizeof(u29)) == 0 &&
        memcmp(P->y, Q->y, WORDS * sizeof(u29)) != 0) {
        result->is_infinity = 1;
        return;
    }

    u29 lambda[WORDS] = {0};
    u29 numerator[WORDS] = {0};
    u29 denominator[WORDS] = {0};
    u29 denominator_inv[WORDS] = {0};

    // lambda = (y2 - y1) / (x2 - x1)
    sub_29bit(Q->y, P->y, numerator, WORDS);
    sub_29bit(Q->x, P->x, denominator, WORDS);
    mod_inverse(denominator, mod, mu, denominator_inv);
    modular_multiply(numerator, denominator_inv, lambda, mod, mu);

    // x3 = lambda^2 - x1 - x2
    u29 lambda_squared[WORDS] = {0};
    modular_multiply(lambda, lambda, lambda_squared, mod, mu);

    sub_29bit(lambda_squared, P->x, result->x, WORDS);
    sub_29bit(result->x, Q->x, result->x, WORDS);
    normalize(result->x, WORDS);

    // y3 = lambda * (x1 - x3) - y1
    u29 temp[WORDS] = {0};
    sub_29bit(P->x, result->x, temp, WORDS);
    modular_multiply(lambda, temp, result->y, mod, mu);
    sub_29bit(result->y, P->y, result->y, WORDS);

    result->is_infinity = 0;
}

// Elliptic curve point doubling
void ecc_double(ECPoint* P, ECPoint* result, u29* mod, u29* mu) {
    if (P->is_infinity || P->y[0] == 0) {
        result->is_infinity = 1;
        return;
    }

    u29 lambda[WORDS] = {0};
    u29 numerator[WORDS] = {0};
    u29 denominator[WORDS] = {0};
    u29 temp1[WORDS] = {0};
    u29 temp2[WORDS] = {0};
    u29 denominator_inv[WORDS] = {0};

    // lambda = (3 * x1^2 + a) / (2 * y1)
    modular_multiply(P->x, P->x, temp1, mod, mu); // x1^2
    add_29bit(temp1, temp1, temp2, WORDS); // 2 * x1^2
    add_29bit(temp2, temp1, numerator, WORDS); // 3 * x1^2 (a = 0)
    // Note: For curves with a != 0, uncomment the line below
    // add_29bit(numerator, curve_param_a, numerator, WORDS);

    add_29bit(P->y, P->y, denominator, WORDS); // 2 * y1
    mod_inverse(denominator, mod, mu, denominator_inv);
    modular_multiply(numerator, denominator_inv, lambda, mod, mu);

    // x3 = lambda^2 - 2 * x1
    u29 lambda_squared[WORDS] = {0};
    modular_multiply(lambda, lambda, lambda_squared, mod, mu);

    sub_29bit(lambda_squared, P->x, result->x, WORDS);
    sub_29bit(result->x, P->x, result->x, WORDS);

    // y3 = lambda * (x1 - x3) - y1
    u29 temp[WORDS] = {0};
    sub_29bit(P->x, result->x, temp, WORDS);
    modular_multiply(lambda, temp, result->y, mod, mu);
    sub_29bit(result->y, P->y, result->y, WORDS);

    result->is_infinity = 0;
}

// Scalar multiplication (left-to-right binary method)
void ecc_scalar_mult_l2r(u29* scalar, ECPoint* P, ECPoint* result, u29* mod, u29* mu) {
    ECPoint Q = {0}; // Initialize Q as the point at infinity
    Q.is_infinity = 1;
    
    // Process scalar bits from MSB to LSB
    for (int i = WORDS * 29 - 1; i >= 0; i--) {
        int bit = (scalar[i / 29] >> (i % 29)) & 1;
        
        // Double Q
        ECPoint temp_double = {0};
        ecc_double(&Q, &temp_double, mod, mu);
        memcpy(&Q, &temp_double, sizeof(ECPoint));
        
        // If the current scalar bit is 1, add P to Q
        if (bit) {
            ECPoint temp_add = {0};
            ecc_add(&Q, P, &temp_add, mod, mu);
            memcpy(&Q, &temp_add, sizeof(ECPoint));
        }
    }

    // Copy the final result to output
    memcpy(result, &Q, sizeof(ECPoint));
}

// Scalar multiplication (right-to-left binary method)
void ecc_scalar_mult_r2l(u29* scalar, ECPoint* P, ECPoint* result, u29* mod, u29* mu) {
    ECPoint R0 = {0}; // R0 starts as point at infinity
    R0.is_infinity = 1;

    ECPoint R1 = {0}; // R1 starts as P
    memcpy(&R1, P, sizeof(ECPoint));

    // Process scalar bits from LSB to MSB
    for (int i = 0; i < WORDS * 29; i++) {
        if ((scalar[i / 29] >> (i % 29)) & 1) { // If the current bit is 1
            ECPoint temp = {0};
            ecc_add(&R0, &R1, &temp, mod, mu); // Add R1 to R0, store in temp
            memcpy(&R0, &temp, sizeof(ECPoint)); // Update R0 with temp
        }

        ECPoint temp = {0};
        ecc_double(&R1, &temp, mod, mu); // Double R1, store in temp
        memcpy(&R1, &temp, sizeof(ECPoint)); // Update R1 with temp
    }

    memcpy(result, &R0, sizeof(ECPoint)); // Copy final result
}
