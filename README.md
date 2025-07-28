Shamir's Secret Sharing Solver in C++ for hashira online test
This project is a C++ implementation designed to solve a version of Shamir's Secret Sharing algorithm. Given a set of n shares, it reconstructs a secret that is the constant term (c) of a polynomial of degree k-1. The program is built to be robust against corrupt or invalid shares in the input data.

The Challenge
The core challenge involves two main difficulties:

Corrupt Shares: The input data contains incorrect shares. For instance, a share specified as base-8 might contain an invalid digit like '8'. The algorithm must be able to identify and discard these corrupt shares.

Large Numbers: The coefficients and share values are far too large to fit into standard 64-bit integer types like long long. The problem requires handling numbers up to 256 bits, which necessitates a custom solution for arbitrary-precision arithmetic.

The Solution: A Step-by-Step Approach
The program solves this problem by following a robust, multi-step process.

Step 1: Parsing and Validation
The program begins by reading the entire JSON input from standard input. It parses this content to extract the key parameters (n and k) and a list of all provided shares.

Crucially, as each share's value is converted from its specified base, it is validated. If a share contains an invalid digit (e.g., '8' in a base-8 number), a try...catch block handles the error. This allows the program to safely ignore the corrupt share and continue processing the valid ones.

Step 2: Arbitrary-Precision Arithmetic (BigNumber Class)
To handle numbers larger than what long long can store, the code uses a custom BigNumber class. This class represents numbers internally as a std::string, allowing for virtually unlimited size. It overloads standard arithmetic operators (+, -, *) to perform calculations on these string-based numbers, thus preventing any data overflow.

Step 3: Precise Division (Rational Class)
Lagrange interpolation, the mathematical method used to find the secret, requires precise fractional division. Standard integer division in C++ (/) truncates the result (e.g., 10 / 3 = 3), which is mathematically incorrect for this algorithm.

To solve this, the program uses a Rational class. This class represents any number as a fraction, using two BigNumber objects for the numerator and denominator. All intermediate calculations are performed using fraction arithmetic, which completely avoids precision errors. The final division only happens at the very end.

Step 4: Finding the Secret Through Voting
With the valid shares parsed and the right tools for the math, the final step is to find the secret:

Generate Combinations: The program generates every possible combination of k shares from the pool of valid shares.

Reconstruct a Secret: For each combination, it uses Lagrange interpolation to calculate a potential secret.

Vote: A map is used to count the occurrences of each calculated secret. Since the correct shares will always produce the same secret, the true secret will be the one that appears most frequently.

Output: The program identifies the secret with the most "votes" and prints it as the final output.

How to Compile and Run
This program is designed to be run from the command line.

1. Compilation
You need a C++ compiler like g++. Save the code as solver.cpp and compile it with the following command:

g++ solver.cpp -o solver -std=c++17

2. Execution
The program reads the JSON file from standard input. You need to redirect the content of your JSON file into the program using the < operator.

For example, if your test case is in testcase.json:

./solver < testcase.json

The program will then output the single, correct secret to the console.
