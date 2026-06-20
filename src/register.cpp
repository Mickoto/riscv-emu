#include "register.h"
#include <sstream>

const char *names[32] = {
    "zero", "ra", "sp", "gp",
    "tp", "t0", "t1", "t2",
    "fp", "s1", "a0", "a1",
    "a2", "a3", "a4", "a5",
    "a6", "a7", "s1", "s3",
    "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6"
};

std::string getRegisterName(int reg) {
    if (reg < 0 || reg > 31) {
        // TODO: throw exception
    }
    std::stringstream ss;
    return names[reg];
    // ss << "x" << reg;
    // return ss.str();
}

void RegisterFile::setRegister(int reg, uint32_t word) {
    if (reg < 0 || reg > 31) {
        std::printf("invalid register %d\n", reg);
        // TODO: throw exception
    }
    if (reg != 0) {
        registers[reg] = word;
    }
}

uint32_t RegisterFile::readRegister(int reg) const {
    if (reg < 0 || reg > 31) {
        std::printf("invalid register %d\n", reg);
        // TODO: throw exception
    }
    return registers[reg];
}

void RegisterFile::printRegisters(std::ostream &os) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            int regi = i * 8 + j;
            os << getRegisterName(regi) << ": " << registers[regi] << "\t";
        }
        os << "\n";
    }
}

uint32_t RegisterFile::getPC() const {
    return pc;
}

void RegisterFile::setPC(uint32_t value) {
    pc = value;
}

void RegisterFile::jumpPC(uint32_t offset) {
    pc += offset;
}
