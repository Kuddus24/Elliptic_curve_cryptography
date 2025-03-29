#include "crypto_math.h"

// Given prime p = 105470615072424007464777057006017113535036866827082468263120632948849084329973
u29 mod[WORDS] = {535425013, 174332635, 444665496, 192778653, 388389189, 518147849, 304619691, 363717891, 15281728, 0};

// Barrett constant mu = 2^(29*18) = 2^522 / p
u29 mu[WORDS] = {450887704, 490307913, 387807083, 403879883, 291135210, 307268612, 110539282, 24605042, 70628772, 35};

// Normalize to handle carries in a 29-bit base system
void normalize(u29* number, int length) {
    uint64_t carry = 0;
    for (int i = 0; i < length; i++) {
        uint64_t value = (uint64_t)number[i] + carry;
        number[i] = value % BASE; // Retain only 29 bits
        carry = value / BASE;     // Propagate carry
    }
}

// Helper function to perform right shift by 1 bit
void rshift1(u29* num, int length) {
    for (int i = 0; i < length - 1; i++) {
        num[i] = (num[i] >> 1) | ((num[i + 1] & 1) << 28);
    }
    num[length - 1] >>= 1;
}

// Modular addition
void add_29bit(const u29* a, const u29* b, u29* result, int length) {
    uint64_t carry = 0;
    for (int i = 0; i < length; i++) {
        uint64_t sum = (uint64_t)a[i] + b[i] + carry;
        result[i] = sum % BASE;
        carry = sum / BASE;
    }
    normalize(result, length);
}

// Modular Subtraction
void sub_29bit(const u29* a, const u29* b, u29* result, int length) {
    uint64_t carry = 1;  // Start with carry = 1 for the 2's complement of b

    for (int i = 0; i < length; i++) {
        uint64_t diff = (uint64_t)a[i] + (~b[i] & BASE29_MASK) + carry;
        result[i] = diff % BASE;   // Keep only the lowest 29 bits
        carry = diff / BASE;       // Set carry for the next word
    }
    
    // Check if result is negative (borrow occurred)
    if (carry == 0) { // Borrow indicates a negative result
        // Add the modulus to ensure the result is positive
        add_29bit(result, mod, result, length);
    }
}

// Schoolbook multiplication
void mult_29bit(const u29* a, const u29* b, u29* result) {
    memset(result, 0, 2 * WORDS * sizeof(u29)); // Initialize result to zero
    for (int i = 0; i < WORDS; i++) {
        uint64_t carry = 0;
        for (int j = 0; j < WORDS; j++) {
            uint64_t product = (uint64_t)a[i] * b[j] + result[i + j] + carry;
            result[i + j] = product % BASE;
            carry = product / BASE;
        }
        result[i + WORDS] += carry;
    }
    normalize(result, 2 * WORDS);
}

// Function to check if a >= b for multi-word numbers
int geq(const u29* a, const u29* b) {
    for (int i = WORDS - 1; i >= 0; i--) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return 0;
    }
    return 1; // a == b
}

// Barrett reduction
void barrett_reduction(u29* a, u29* mod, u29* mu, u29* result) {
    u29 q1[WORDS] = {0};
    u29 q2[2 * WORDS] = {0};
    u29 q3[WORDS] = {0};
    u29 temp[2 * WORDS] = {0};
    u29 rem[WORDS + 1] = {0};

    // Step 1: Initialize q1 with a[8] to a[17]
    memcpy(q1, a + 8, (WORDS) * sizeof(u29));

    // Step 2: q2 = q1 * mu
    mult_29bit(q1, mu, q2);

    // Step 3: Populate q3 with q2[10] to q2[19]
    memcpy(q3, q2 + WORDS, WORDS * sizeof(u29));

    // Step 4: temp = q3 * mod
    mult_29bit(q3, mod, temp);

    // Step 5: rem = a - temp
    sub_29bit(a, temp, rem, WORDS + 1);

    // Step 6: Adjust remainder to ensure rem < mod
    while (geq(rem, mod)) {
        sub_29bit(rem, mod, rem, WORDS + 1);
    }

    // Store result in output parameter
    memcpy(result, rem, WORDS * sizeof(u29));
}

// Modular multiplication with Barrett reduction
void modular_multiply(u29* a, u29* b, u29* result, u29* mod, u29* mu) {
    u29 temp[2 * WORDS] = {0}; // Temporary variable to hold multiplication result
    mult_29bit(a, b, temp);    
    barrett_reduction(temp, mod, mu, result); 
}

// Montgomery Ladder for Exponentiation with Barrett Reduction
// 2 is a generator of Z*_p use use g=2 as base
void montgomery_ladder_exponentiation(u29* base, u29* exponent, u29* mod, u29* mu, u29* result) {
    u29 one[WORDS] = {0};
    one[0] = 1;  // Montgomery representation of 1

    u29 r0[WORDS];  // Initialize R0 = 1 (Montgomery form)
    u29 r1[WORDS];  // Initialize R1 = base (Montgomery form)
    memcpy(r0, one, sizeof(one));
    memcpy(r1, base, sizeof(u29) * WORDS);

    int total_bits = WORDS * 29;

    // Montgomery ladder loop
    for (int j = total_bits - 1; j >= 0; j--) {
        // Extract the j-th bit of the exponent
        u29 bit = (exponent[j / 29] >> (j % 29)) & 1;

        if (bit == 0) {
            modular_multiply(r1, r0, r1, mod, mu);  // R1 = R0 * R1
            modular_multiply(r0, r0, r0, mod, mu);  // R0 = (R0)^2
        } else {
            modular_multiply(r0, r1, r0, mod, mu);  // R0 = R0 * R1
            modular_multiply(r1, r1, r1, mod, mu);  // R1 = (R1)^2
        }
    }

    // The result is stored in R0
    memcpy(result, r0, sizeof(u29) * WORDS);
}

// Left-to-right modular exponentiation
void left_to_right_binary_exponentiation(u29* base, u29* exponent, u29* mod, u29* mu, u29* result) {
    u29 r0[WORDS];  // R0 = 1
    u29 r1[WORDS];  // R1 = base
    u29 one[WORDS] = {0};
    one[0] = 1;

    memcpy(r0, one, sizeof(one));         // Initialize R0 = 1
    memcpy(r1, base, sizeof(u29) * WORDS); // Initialize R1 = base

    int total_bits = WORDS * 29;

    for (int j = total_bits - 1; j >= 0; j--) {
        modular_multiply(r0, r0, r0, mod, mu); // R0 = (R0)^2

        // Check the j-th bit of the exponent
        u29 bit = (exponent[j / 29] >> (j % 29)) & 1;
        if (bit == 1) {
            modular_multiply(r0, r1, r0, mod, mu); // R0 = R0 * R1
        }
    }

    memcpy(result, r0, sizeof(u29) * WORDS); // Return R0
}

// Right-to-left modular exponentiation
void right_to_left_binary_exponentiation(u29* base, u29* exponent, u29* mod, u29* mu, u29* result) {
    u29 r0[WORDS];  // R0 = 1
    u29 r1[WORDS];  // R1 = base
    u29 one[WORDS] = {0};
    one[0] = 1;

    memcpy(r0, one, sizeof(one));         // Initialize R0 = 1
    memcpy(r1, base, sizeof(u29) * WORDS); // Initialize R1 = base

    int total_bits = WORDS * 29;

    for (int j = 0; j < total_bits; j++) {
        // Check the j-th bit of the exponent
        u29 bit = (exponent[j / 29] >> (j % 29)) & 1;

        if (bit == 1) {
            modular_multiply(r0, r1, r0, mod, mu); // R0 = R0 * R1
        }
        modular_multiply(r1, r1, r1, mod, mu); // R1 = (R1)^2
    }

    memcpy(result, r0, sizeof(u29) * WORDS); // Return R0
}

// Print function for debugging for barrett
void print_array_brr(const char* label, const u29* arr, int size) {
    printf("%s: ", label);
    for (int i = 0; i < size; i++) {
        printf("%lu ", (unsigned long)arr[i]);
    }
    printf("\n");
}
