/* isa_encoder.cpp - Implementation of ISA encoding */
#include "isa_encoder.h"
#include <sstream>

std::string encodeMatrixMultiplication(int M, int N, int P) {
    std::ostringstream isa;

    isa << "INIT C\n";  // Initialize matrix C to zero

    // Loop over rows of A
    isa << "LOOP R4, 0, " << M << "  ; Loop over rows of A\n"; 

    // Loop over columns of B
    isa << "  LOOP R5, 0, " << P << "  ; Loop over columns of B\n"; 
    
    // Initialize C[i][j] to 0
    isa << "    LOAD R6, 0\n";  // R6 will store C[i][j] (accumulator)
    
    // Loop over elements of the row/column
    isa << "    LOOP R7, 0, " << N << "  ; Loop over elements of A row & B column\n"; 
    isa << "      LOAD R1, A[R4][R7]\n";
    isa << "      LOAD R2, B[R7][R5]\n";
    isa << "      MUL R3, R1, R2\n";
    isa << "      ADD R6, R6, R3\n"; // Accumulate result in R6
    isa << "    END_LOOP\n"; 

    // Store the computed value in C[i][j]
    isa << "    STORE C[R4][R5], R6\n";

    isa << "  END_LOOP\n";  // End column loop
    isa << "END_LOOP\n";  // End row loop

    return isa.str();
}
