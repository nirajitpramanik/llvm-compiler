/* isa_encoder.cpp - Implementation of ISA encoding */
#include "isa_encoder.h"
#include <sstream>

std::string encodeMatrixMultiplication() {
    std::ostringstream isa;
    isa << "LOAD R1, A\n";
    isa << "LOAD R2, B\n";
    isa << "MUL R3, R1, R2\n";
    isa << "STORE R3, C\n";
    return isa.str();
}
