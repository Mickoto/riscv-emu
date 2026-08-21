#include "instruction.h"
#include <cstdint>
#include <functional>

std::ostream &operator<<(std::ostream &os, Instruction *i) {
    i->print(os);
    return os;
}

struct ArithmeticOp {
    std::string name;
    std::function<int(int, int)> op;
};

std::unordered_map<uint8_t, ArithmeticOp> rops {
    {0, {"add", [](int a, int b) {return a + b;}}},
    {64, {"sub", [](int a, int b) {return a - b;}}},
    {1, {"sll", [](int a, int b) {return a << b;}}},
    {2, {"slt", [](int a, int b) {return a < b;}}},
    {3, {"sltu", [](int a, int b) {return (unsigned int)a < (unsigned int)b;}}},
    {4, {"xor", [](int a, int b) {return a ^ b;}}},
    {5, {"srl", [](int a, int b) {return (unsigned int)a >> b;}}},
    {69, {"sra", [](int a, int b) {return a >> b;}}},
    {6, {"or", [](int a, int b) {return a | b;}}},
    {7, {"and", [](int a, int b) {return a & b;}}},
};

ArithmeticOp iops[8] = {
    {"addi", [](int a, int b) {return a + b;}},
    {"slli", [](int a, int b) {return a << (b >> 7);}},
    {"slti", [](int a, int b) {return a < b;}},
    {"sltiu", [](int a, int b) {return (unsigned int)a < (unsigned int)b;}},
    {"xori", [](int a, int b) {return a ^ b;}},
    {"srli", [](int a, int b) {return (b & 0b10) ? a >> (b >> 7) : (unsigned int)a >> (b >> 7);}},
    {"ori", [](int a, int b) {return a | b;}},
    {"and", [](int a, int b) {return a & b;}},
};

InstructionArithmetic::InstructionArithmetic(InstructionWord i) {
    REncoding &re = i.re;
    rs1 = re.rs1;
    rs2 = re.rs2;
    rd = re.rd;
    type = re.funct7 << 1 | re.funct3;
    if (!rops.count(type)) {
        throw "bad funct7 field in arithmetic operation";
    }
}

void InstructionArithmetic::execute(PState &state) const {
    state.registers.setRegister(rd, rops[type].op(state.registers.readRegister(rs1), state.registers.readRegister(rs2)));
}

void InstructionArithmetic::print(std::ostream& out) const noexcept {
    out << rops[type].name << " " << getRegisterName(rd) << ", " << getRegisterName(rs1) << ", " << getRegisterName(rs2);
}

InstructionArithmeticImmediate::InstructionArithmeticImmediate(InstructionWord i) {
    IEncoding &ie = i.ie;
    rd = ie.rd;
    rs1 = ie.rs1;
    imm = getIImm(ie);
    type = ie.funct3;
}

void InstructionArithmeticImmediate::execute(PState &state) const {
    state.registers.setRegister(rd, iops[type].op(state.registers.readRegister(rs1), imm));
}

void InstructionArithmeticImmediate::print(std::ostream& out) const noexcept {
    if (type == 5 && imm & 0b10) {
        out << "srli";
    }
    else {
        out << iops[type].name;
    }
    out << " " << getRegisterName(rd) << ", " << getRegisterName(rs1) << ", " << imm;
}

InstructionLoad::InstructionLoad(InstructionWord i) {
    IEncoding &ie = i.ie;
    flags = ie.funct3;
    if (flags & 0b010 && (flags & 0b001 || flags & 0b100)) {
        throw "bad funct3 field in load instruction";
    }
    rd = ie.rd;
    rs1 = ie.rs1;
    imm = getIImm(ie);
}

void InstructionLoad::execute(PState &state) const {
    uint32_t loaded = 0;
    int sz;
    if (flags & 0b010) {
        loaded = state.memory->readWord(state.registers.readRegister(rs1) + imm);
    }
    else if (flags & 0b001) {
        loaded = state.memory->readHalfword(state.registers.readRegister(rs1) + imm);
        sz = 16;
    }
    else {
        loaded = state.memory->readByte(state.registers.readRegister(rs1) + imm);
        sz = 8;
    }
    if ((flags & 0b110) == 0) {
        loaded = signExtend(loaded, sz);
    }
    state.registers.setRegister(rd, loaded);
}

void InstructionLoad::print(std::ostream& out) const noexcept {
    if (flags & 0b010) {
        out << "lw";
    }
    else if (flags & 0b001) {
        out << "lh";
    }
    else {
        out << "lb";
    }
    if (flags & 0b100) {
        out << "u";
    }
    out << " " << getRegisterName(rd) << ", " << imm << "(" << getRegisterName(rs1) << ")";
}

InstructionStore::InstructionStore(InstructionWord i) {
    SEncoding &se = i.se;
    if (se.funct3 > 2) {
        throw "bad funct3 field in store instruction";
    }
    flags = se.funct3;
    rs2 = se.rs2;
    rs1 = se.rs1;
    imm = getSImm(se);
}

void InstructionStore::execute(PState &state) const {
    switch (flags) {
        case 0:
            state.memory->writeByte(state.registers.readRegister(rs1) + imm, state.registers.readRegister(rs2));
            break;
        case 1:
            state.memory->writeHalfword(state.registers.readRegister(rs1) + imm, state.registers.readRegister(rs2));
            break;
        case 2:
            state.memory->writeWord(state.registers.readRegister(rs1) + imm, state.registers.readRegister(rs2));
            break;
    }
}

void InstructionStore::print(std::ostream& out) const noexcept {
    switch (flags) {
        case 0:
            out << "sb ";
            break;
        case 1:
            out << "sh ";
            break;
        case 2:
            out << "sw ";
            break;
    }
    out << getRegisterName(rs2) << ", " << imm << "(" << getRegisterName(rs1) << ")";
}

InstructionJumpAndLink::InstructionJumpAndLink(InstructionWord i) {
    JEncoding &je = i.je;
    rd = je.rd;
    imm = getJImm(je);
}

void InstructionJumpAndLink::execute(PState &state) const {
    // TODO: unscuff
    uint32_t return_address = state.registers.getPC() + 4;
    state.registers.jumpPC(imm - 4);
    state.registers.setRegister(rd, return_address);
}

void InstructionJumpAndLink::print(std::ostream& out) const noexcept {
    out << "jal " << getRegisterName(rd) << ", " << imm;
}

InstructionJumpAndLinkRegister::InstructionJumpAndLinkRegister(InstructionWord i) {
    IEncoding &ie = i.ie;
    rd = ie.rd;
    rs1 = ie.rs1;
    imm = getIImm(ie);
}

void InstructionJumpAndLinkRegister::execute(PState &state) const {
    // TODO: unscuff
    uint32_t return_address = state.registers.getPC() + 4;
    state.registers.setPC(state.registers.readRegister(rs1) + imm - 4);
    state.registers.setRegister(rd, return_address);
}

void InstructionJumpAndLinkRegister::print(std::ostream& out) const noexcept {
    out << "jalr " << getRegisterName(rd) << ", " << imm << "(" << getRegisterName(rs1)<< ")";
}

InstructionBranch::InstructionBranch(InstructionWord i) {
    BEncoding &be = i.be;
    flags = be.funct3;
    if (flags & 0b010 && ~flags & 0b100) {
        throw "bad funct3 field in branch instruction";
    }
    rs1 = be.rs1;
    rs2 = be.rs2;
    imm = getBImm(be);
}

void InstructionBranch::execute(PState &state) const {
    // TODO: unscuff
    bool result = false;
    if (flags & 0b100) {
        if (flags & 0b010) {
            result = state.registers.readRegister(rs1) < state.registers.readRegister(rs2);
        }
        else {
            result = (int)state.registers.readRegister(rs1) < (int)state.registers.readRegister(rs2);
        }
    }
    else {
        result = state.registers.readRegister(rs1) == state.registers.readRegister(rs2);
    }
    if (flags & 0b001) {
        result = !result;
    }
    if (result) {
        state.registers.jumpPC(imm - 4);
    }
}

void InstructionBranch::print(std::ostream& out) const noexcept {
    switch (flags) {
        case 0:
            out << "beq ";
            break;
        case 1:
            out << "bne ";
            break;
        case 4:
            out << "blt ";
            break;
        case 5:
            out << "bge ";
            break;
        case 6:
            out << "bltu ";
            break;
        case 7:
            out << "bgeu ";
            break;
    }
    out << getRegisterName(rs1) << ", " << getRegisterName(rs2) << ", " << imm;
}

InstructionLoadUpperImmediate::InstructionLoadUpperImmediate(InstructionWord i) {
    UEncoding &ue = i.ue;
    rd = ue.rd;
    imm = getUImm(ue);
}

void InstructionLoadUpperImmediate::execute(PState &state) const {
    state.registers.setRegister(rd, imm);
}

void InstructionLoadUpperImmediate::print(std::ostream& out) const noexcept {
    out << "lui " << getRegisterName(rd) << ", " << (imm >> 12);
}

InstructionAddUpperImmediatePC::InstructionAddUpperImmediatePC(InstructionWord i) {
    UEncoding &ue = i.ue;
    rd = ue.rd;
    imm = getUImm(ue);
}

void InstructionAddUpperImmediatePC::execute(PState &state) const {
    state.registers.setRegister(rd, state.registers.getPC() + imm);
}

void InstructionAddUpperImmediatePC::print(std::ostream& out) const noexcept {
    out << "auipc " << getRegisterName(rd) << ", " << (imm >> 12);
}
