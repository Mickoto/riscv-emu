#include <cstdint>

// Standard encoding formats for RV32I

struct __attribute__((packed)) REncoding {
    uint32_t opcode  : 7;
    uint32_t rd      : 5;
    uint32_t funct3  : 3;
    uint32_t rs1     : 5;
    uint32_t rs2     : 5;
    uint32_t funct7  : 7;
};

struct __attribute__((packed)) IEncoding {
    uint32_t opcode  : 7;
    uint32_t rd      : 5;
    uint32_t funct3  : 3;
    uint32_t rs1     : 5;
    uint32_t imm    : 12;
};

struct __attribute__((packed)) SEncoding {
    uint32_t opcode  : 7;
    uint32_t imm_lo  : 5;
    uint32_t funct3  : 3;
    uint32_t rs1     : 5;
    uint32_t rs2     : 5;
    uint32_t imm_hi  : 7;
};

struct __attribute__((packed)) BEncoding {
    uint32_t opcode  : 7;
    uint32_t imm11   : 1;
    uint32_t imm_lo  : 4;
    uint32_t funct3  : 3;
    uint32_t rs1     : 5;
    uint32_t rs2     : 5;
    uint32_t imm_hi  : 6;
    uint32_t imm12   : 1;
};

struct __attribute__((packed)) UEncoding {
    uint32_t opcode  : 7;
    uint32_t rd      : 5;
    uint32_t imm    : 20;
};

struct __attribute__((packed)) JEncoding {
    uint32_t opcode  : 7;
    uint32_t rd      : 5;
    uint32_t imm_hi : 8;
    uint32_t imm11  : 1;
    uint32_t imm_lo : 10;
    uint32_t imm20  : 1;
};

union InstructionWord {
    struct __attribute__((packed)) {
        uint8_t opcode  : 7;
        uint32_t suffix : 25;
    };
    REncoding re;
    IEncoding ie;
    SEncoding se;
    BEncoding be;
    UEncoding ue;
    JEncoding je;
};


// Decoding helpers

inline int signExtend(uint32_t value, int bits) {
    int m = 1 << (bits - 1);
    return (value ^ m) - m;
}

inline int getIImm(IEncoding ie) {
    return signExtend(ie.imm, 12);
}

inline int getSImm(SEncoding se) {
    return signExtend(se.imm_hi << 5 | se.imm_lo, 12);
}

inline int getBImm(BEncoding be) {
    return signExtend(be.imm12 << 12 | be.imm11 << 11 | be.imm_hi << 5 | be.imm_lo << 1, 13);
}

inline int getUImm(UEncoding ue) {
    return ue.imm << 12;
}

inline int getJImm(JEncoding je) {
    return signExtend(je.imm20 << 20 | je.imm_hi << 12 | je.imm11 << 11 | je.imm_lo << 1, 21);
}
