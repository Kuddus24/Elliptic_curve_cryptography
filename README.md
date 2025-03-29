
# Elliptic Curve Cryptography Implementation

This repository contains a custom implementation of Elliptic Curve Cryptography (ECC) in C, focused on educational purposes and understanding the underlying mathematics and algorithms.

## Features

- Large integer arithmetic using 29-bit word representation
- Barrett reduction for efficient modular arithmetic
- Montgomery ladder exponentiation for secure calculations
- Left-to-right and right-to-left binary exponentiation algorithms
- Elliptic curve point operations (addition, doubling)
- Scalar multiplication with multiple algorithms
- Hex string to big integer conversion utilities

## Implementation Details

This implementation uses:
- 29-bit words within 64-bit integers for efficient arithmetic
- A 256-bit prime modulus
- Barrett reduction for optimized modular operations
- Secure point operations for ECC

## Directory Structure

```
elliptic-curve-cryptography/
├── src/                 # Source code for core functionality
│   ├── crypto_math.h    # Modular arithmetic operations
│   ├── crypto_math.c
│   ├── ecc.h            # Elliptic curve operations
│   ├── ecc.c
│   ├── hex_converter.c  # Utility for hex to bignum conversion
│   └── main.c           # Example usage
├── include/             # Header files
│   └── common.h         # Common definitions
├── examples/            # Example applications
│   ├── key_exchange.c   # ECDH key exchange example
│   └── point_operations.c  # Point operations demo
├── tests/               # Testing code
│   └── test_main.c      # Basic tests
├── docs/                # Documentation
│   └── ECC_Implementation_Analysis.pdf
├── Makefile
├── LICENSE
└── README.md
```

## Building and Running

### Prerequisites

- GCC compiler
- Make utility

### Building the Project

```bash
make all
```

This will compile the main program, examples, and tests.

### Running Examples

```bash
# Run the ECDH key exchange example
./bin/key_exchange

# Run point operations example
./bin/point_operations

# Run the hex converter utility
./bin/hex_converter
```

### Running Tests

```bash
./bin/test_main
```

## Mathematical Background

This implementation is based on elliptic curves defined over a finite field Fp, where p is a large prime number. The curve is defined by the equation:

y² = x³ + ax + b (mod p)

The implementation uses:
- A 256-bit prime modulus p
- Curve parameters a = 0 and b = 7 (similar to secp256k1)
- A generator point G that generates a large subgroup

## Security Considerations

This implementation is primarily for educational purposes. While care has been taken to implement secure algorithms like the Montgomery ladder for exponentiation, it has not been audited for production use.

## Detailed Implementation

### Large Integer Representation

Numbers in this implementation are represented using a 29-bit word system stored in 64-bit integers. This approach was chosen to:

- Maximize the usage of 64-bit registers
- Prevent overflows during intermediate calculations
- Optimize carry handling in arithmetic operations

A 256-bit integer requires 9 words (⌈256/29⌉), though we allocate 10 words to handle overflows during operations.

### Modular Arithmetic Operations

The implementation includes several core modular arithmetic functions:

- **Addition**: Handles carry propagation between words
- **Subtraction**: Uses two's complement approach with modular correction
- **Multiplication**: Classical schoolbook multiplication algorithm
- **Modular Reduction**: Barrett reduction for efficient modulo operations

### Elliptic Curve Operations

The implementation supports various elliptic curve operations:

- **Point Addition**: Adding two distinct points on the curve
- **Point Doubling**: Adding a point to itself
- **Scalar Multiplication**: Computing k·P using binary methods
  - Left-to-right binary method
  - Right-to-left binary method
  - Montgomery ladder (for side-channel resistance)

## License

[MIT License](LICENSE)

## References

- Hankerson, D., Menezes, A., & Vanstone, S. Guide to Elliptic Curve Cryptography, Springer.
- Safieh, M., Furch, A., & De Santis, F. "An Efficient Barrett Reduction Algorithm for Gaussian Integer Moduli".

## Author

SK GOLAM KUDDUS
