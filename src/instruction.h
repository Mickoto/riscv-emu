#include "encoding.h"
#include "register.h"
#include "memory.h"
#include <cstdint>
#include <iostream>

struct PState {
    RegisterFile registers;
    MemoryModel *memory;
};

class Instruction {
public:
    virtual void print(std::ostream& out) const = 0;
    virtual void execute(PState &state) const= 0;
};

std::ostream &operator<<(std::ostream &os, Instruction *i);

// RV32I instruction set
class InstructionArithmetic : public Instruction {
public:
    InstructionArithmetic(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rs1, rs2, rd;
    uint8_t type;
};

class InstructionArithmeticImmediate : public Instruction {
public:
    InstructionArithmeticImmediate(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rd, rs1;
    int imm;
    uint8_t type;
};

class InstructionLoad : public Instruction {
public:
    InstructionLoad(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rd, rs1;
    int imm;
    uint8_t flags;
};

class InstructionStore : public Instruction {
public:
    InstructionStore(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rs2, rs1;
    int imm;
    uint8_t flags;
};

class InstructionJumpAndLink : public Instruction {
public:
    InstructionJumpAndLink(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rd;
    int imm;
};

class InstructionJumpAndLinkRegister : public Instruction {
public:
    InstructionJumpAndLinkRegister(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rd, rs1;
    int imm;
};

class InstructionBranch : public Instruction {
public:
    InstructionBranch(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rs1, rs2;
    int imm;
    uint8_t flags;
};

class InstructionLoadUpperImmediate : public Instruction {
public:
    InstructionLoadUpperImmediate(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rd;
    int imm;
};

class InstructionAddUpperImmediatePC : public Instruction {
public:
    InstructionAddUpperImmediatePC(InstructionWord i);

    virtual void execute(PState &state) const;
    virtual void print(std::ostream& out) const noexcept;
private:
    int rd;
    int imm;
};
