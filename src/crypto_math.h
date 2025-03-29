#ifndef CRYPTO_MATH_H
#define CRYPTO_MATH_H

#include "../include/common.h"

// Given prime p = 105470615072424007464777057006017113535036866827082468263120632948849084329973
extern u29 mod[WORDS];

// Barrett constant mu = 2^(29*18) = 2^522 / p
extern u29 mu[WORDS];

// Basic modular arithmetic operations
void normalize(u29* number, int length);
void rshift1(u29* num, int length);
void add_29bit(const u29* a, const u29* b, u29* result, int length);
void sub_29bit(const u29* a, const u29* b, u29* result, int length);
void mult_29bit(const u29* a, const u29* b, u29* result);
int geq(const u29* a, const u29* b);

// Advanced modular operations
void barrett_reduction(u29* a, u29* mod, u29* mu, u29* result);
void modular_multiply(u29* a, u29* b, u29* result, u29* mod, u29* mu);
void montgomery_ladder_exponentiation(u29* base, u29* exponent, u29* mod, u29* mu, u29* result);
void left_to_right_binary_exponentiation(u29* base, u29* exponent, u29* mod, u29* mu, u29* result);
void right_to_left_binary_exponentiation(u29* base, u29* exponent, u29* mod, u29* mu, u29* result);

// Utility function for debugging
void print_array_brr(const char* label, const u29* arr, int size);

#endif // CRYPTO_MATH_H
