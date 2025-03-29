
#include <stdio.h>
#include "../include/common.h"
#include "../src/crypto_math.h"
#include "../src/ecc.h"

// Simple implementation of ECDH key exchange
int main() {
    printf("Elliptic Curve Diffie-Hellman (ECDH) Key Exchange Example\n");
    printf("=======================================================\n\n");
    
    // Initialize generator point
    ECPoint G = {0};
    memcpy(G.x, generator_x, WORDS * sizeof(u29));
    memcpy(G.y, generator_y, WORDS * sizeof(u29));
    G.is_infinity = 0;
    
    // Alice's private key
    u29 alice_private[WORDS] = {0};
    alice_private[0] = 123456789; // Example private key
    
    // Bob's private key
    u29 bob_private[WORDS] = {0};
    bob_private[0] = 987654321; // Example private key
    
    // Alice computes her public key: A = alice_private * G
    ECPoint alice_public = {0};
    ecc_scalar_mult_l2r(alice_private, &G, &alice_public, mod, mu);
    
    printf("Alice's public key:\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (alice_public.x[i] != 0) {
            printf("%lu ", (unsigned long)alice_public.x[i]);
        }
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (alice_public.y[i] != 0) {
            printf("%lu ", (unsigned long)alice_public.y[i]);
        }
    }
    printf("\n\n");
    
    // Bob computes his public key: B = bob_private * G
    ECPoint bob_public = {0};
    ecc_scalar_mult_l2r(bob_private, &G, &bob_public, mod, mu);
    
    printf("Bob's public key:\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (bob_public.x[i] != 0) {
            printf("%lu ", (unsigned long)bob_public.x[i]);
        }
    }
    printf("\ny: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (bob_public.y[i] != 0) {
            printf("%lu ", (unsigned long)bob_public.y[i]);
        }
    }
    printf("\n\n");
    
    // Alice computes the shared secret: S = alice_private * B
    ECPoint alice_secret = {0};
    ecc_scalar_mult_l2r(alice_private, &bob_public, &alice_secret, mod, mu);
    
    // Bob computes the shared secret: S = bob_private * A
    ECPoint bob_secret = {0};
    ecc_scalar_mult_l2r(bob_private, &alice_public, &bob_secret, mod, mu);
    
    // Verify both computed the same shared secret
    int same_secret = 1;
    for (int i = 0; i < WORDS; i++) {
        if (alice_secret.x[i] != bob_secret.x[i] || alice_secret.y[i] != bob_secret.y[i]) {
            same_secret = 0;
            break;
        }
    }
    
    printf("Shared secret (Alice):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (alice_secret.x[i] != 0) {
            printf("%lu ", (unsigned long)alice_secret.x[i]);
        }
    }
    printf("\n");
    
    printf("Shared secret (Bob):\n");
    printf("x: ");
    for (int i = WORDS - 1; i >= 0; i--) {
        if (bob_secret.x[i] != 0) {
            printf("%lu ", (unsigned long)bob_secret.x[i]);
        }
    }
    printf("\n\n");
    
    printf("ECDH key exchange %s!\n", same_secret ? "successful" : "failed");
    
    return 0;
}
