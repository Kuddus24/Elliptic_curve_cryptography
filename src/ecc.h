#ifndef ECC_H
#define ECC_H

#include "crypto_math.h"

// Define the elliptic curve point structure
typedef struct {
    u29 x[WORDS];
    u29 y[WORDS];
    int is_infinity; // 1 if the point is at infinity, 0 otherwise
} ECPoint;

// Elliptic curve operations
void mod_inverse(u29* x, u29* p, u29* mu, u29* result);
void ecc_add(ECPoint* P, ECPoint* Q, ECPoint* result, u29* mod, u29* mu);
void ecc_double(ECPoint* P, ECPoint* result, u29* mod, u29* mu);
void ecc_scalar_mult_l2r(u29* scalar, ECPoint* P, ECPoint* result, u29* mod, u29* mu);
void ecc_scalar_mult_r2l(u29* scalar, ECPoint* P, ECPoint* result, u29* mod, u29* mu);

// Generator point coordinates
extern u29 generator_x[WORDS];
extern u29 generator_y[WORDS];

#endif // ECC_H
