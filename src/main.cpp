#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include "instruction.h"
#include "elf.h"

void usage() {
    // TODO:
}

std::unique_ptr<Instruction> decode(InstructionWord iw) {
    switch (iw.opcode) {
        case 51:
            return std::make_unique<InstructionArithmetic>(iw);
        case 19:
            return std::make_unique<InstructionArithmeticImmediate>(iw);
        case 3:
            return std::make_unique<InstructionLoad>(iw);
        case 35:
            return std::make_unique<InstructionStore>(iw);
        case 111:
            return std::make_unique<InstructionJumpAndLink>(iw);
        case 103:
            return std::make_unique<InstructionJumpAndLinkRegister>(iw);
        case 99:
            return std::make_unique<InstructionBranch>(iw);
        case 55:
            return std::make_unique<InstructionLoadUpperImmediate>(iw);
        case 23:
            return std::make_unique<InstructionLoadUpperImmediate>(iw);
    }

    return nullptr;
}

void load_elf32(std::ifstream &in, FlatMemoryModel *m, RegisterFile &r, ELFHeaderPref prefix) {
    ELFHeader32 header;
    header.prefix = prefix;
    in.read(reinterpret_cast<char *>(&header.type), sizeof(ELFHeader32) - sizeof(ELFHeaderPref));
    if (header.headerSize != sizeof(ELFHeader32)) {
        std::cerr << "Invalid elf header size!\n";
        exit(1);
    }

    SectionHeader32 *section_headers = new SectionHeader32[header.sectionHeadersNum];
    in.seekg(header.sectionHeadersOffset, std::ios::beg);
    in.read(reinterpret_cast<char *>(section_headers), header.sectionHeadersNum * sizeof(SectionHeader32));

    for (int i = 0; i < header.sectionHeadersNum; i++) {
        if (section_headers[i].flags & SHF_ALLOC) {
            in.seekg(section_headers[i].offset, std::ios::beg);
            in.read(reinterpret_cast<char *>(m->raw() + section_headers[i].address), section_headers[i].size);
        }
    }

    r.setPC(header.entryOffset);
}

void load_elf64(std::ifstream &in, FlatMemoryModel *m, RegisterFile &r, ELFHeaderPref prefix) {
    ELFHeader64 header;
    header.prefix = prefix;
    in.read(reinterpret_cast<char *>(&header.type), sizeof(ELFHeader64) - sizeof(ELFHeaderPref));
    if (header.headerSize != sizeof(ELFHeader64)) {
        std::cerr << "Invalid elf header size!\n";
        exit(1);
    }

    SectionHeader64 *section_headers = new SectionHeader64[header.sectionHeadersNum];
    in.seekg(header.sectionHeadersOffset, std::ios::beg);
    in.read(reinterpret_cast<char *>(section_headers), header.sectionHeadersNum * sizeof(SectionHeader64));

    for (int i = 0; i < header.sectionHeadersNum; i++) {
        in.seekg(section_headers[i].offset, std::ios::beg);
        if (section_headers[i].size > 0) {
            in.read(reinterpret_cast<char *>(m->raw() + section_headers[i].address), section_headers[i].size);
        }
    }

    r.setPC(header.entryOffset);
}

void load_file(std::ifstream &in, FlatMemoryModel *m, RegisterFile &r) {
    ELFHeaderPref elfPrefix;
    in.read(reinterpret_cast<char *>(&elfPrefix), sizeof(ELFHeaderPref));
    if (elfPrefix.magic == ELFMAGIC) {
        if (elfPrefix._class == CLASS32) {
            load_elf32(in, m, r, elfPrefix);
        }
        else if (elfPrefix._class == CLASS64) {
            load_elf64(in, m, r, elfPrefix);
        }
        else {
            std::cerr << "Invalid elf type!";
            exit(1);
        }
    }
    else {
        std::cerr << "Loading raw file!";
        in.seekg(0, std::ios::end);
        std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);

        if (!in.read((char *)m->raw(), size)) {
            std::printf("Error reading file!\n");
        }
    }
}

void main_loop(PState &state) {
    //while(1) {
    for (int j = 0; j < 1000; j++) {
        uint32_t pc = state.registers.getPC();
        // Fetch
        if (pc % 4) {
            std::printf("Misaligned program counter! (PC = %x)\n", pc);
            // TODO: trap?
            break;
        }
        // auto temp = state.memory->readWord(0x1018c);
        // std::cout << decode(*reinterpret_cast<InstructionWord *>(&temp)).get() << std::endl;
        uint32_t ir = state.memory->readWord(pc);
        if ((ir & 0x3) != 0x3) {
            std::printf("Illegal instruction encountered! (detected by fetch : IP = 0x%x, IW = 0x%x)\n", pc, ir);
            // TODO: trap on sufficiently small ISAs
            break;
        }

        // Decode
        InstructionWord iw;
        try {
            iw = *reinterpret_cast<InstructionWord *>(&ir);
        }
        catch (const char *error) {
            // TODO: trap
            std::cerr << "Decoding error: " << error << "\n";
            break;
        }
        std::unique_ptr<Instruction> i = decode(iw);
        if (!i) {
            std::printf("Illegal instruction encountered! (Opcode %x)\n", iw.opcode);
            // TODO: trap
            break;
        }

        // Execute
        // std::cout << i.get() << std::endl;
        i->execute(state);

        state.registers.jumpPC(4);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usage();
        exit(1);
    }

    FlatMemoryModel memory(1 << 20);
    MemoryMappedIOModel m(&memory, std::cout, std::cin);
    PState s;
    s.memory = &m;

    std::ifstream input(argv[1], std::ios::in | std::ios::binary);
    load_file(input, &memory, s.registers);

    main_loop(s);

    return 0;
}
