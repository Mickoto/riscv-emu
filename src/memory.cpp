#include "memory.h"
#include <cstdint>
#include <cstdio>

FlatMemoryModel::FlatMemoryModel(size_t size) : size(size) {
    mem = new uint8_t[size];
}

FlatMemoryModel::~FlatMemoryModel() {
    delete[] mem;
}

void FlatMemoryModel::writeWord(std::size_t address, uint32_t word) {
    if (address >= size) {
        // TODO: throw exception
        return;
    }
    *reinterpret_cast<uint32_t *>(mem + address) = word;
}

void FlatMemoryModel::writeHalfword(std::size_t address, uint16_t hw) {
    if (address >= size) {
        // TODO: throw exception
        return;
    }
    *reinterpret_cast<uint16_t *>(mem + address) = hw;
}

void FlatMemoryModel::writeByte(std::size_t address, uint8_t byte) {
    if (address >= size) {
        // TODO: throw exception
        return;
    }
    *reinterpret_cast<uint8_t *>(mem + address) = byte;
}

uint32_t FlatMemoryModel::readWord(std::size_t address) const {
    if (address >= size) {
        // TODO: throw exception
        return 0;
    }
    return *reinterpret_cast<uint32_t *>(mem + address);;
}

uint16_t FlatMemoryModel::readHalfword(std::size_t address) const {
    if (address >= size) {
        // TODO: throw exception
        return 0;
    }
    return *reinterpret_cast<uint16_t *>(mem + address);
}

uint8_t FlatMemoryModel::readByte(std::size_t address) const {
    if (address >= size) {
        // TODO: throw exception
        return 0;
    }
    return *reinterpret_cast<uint16_t *>(mem + address);
}

uint8_t *FlatMemoryModel::raw() const {
    return mem;
}

size_t FlatMemoryModel::getSize() const {
    return size;
}

MemoryMappedIOModel::MemoryMappedIOModel(MemoryModel *parent, std::ostream &out, std::istream &in) : parent(parent), out(out), in(in) {}

void MemoryMappedIOModel::writeWord(std::size_t address, uint32_t word) {
    if (address / 4 == 0xb800 / 4) { // stdout
        out << (char)(word % 256);
    }
    else if (address / 4 == 0xb804 / 4) { // stdin
        // nop
    }
    else if (address / 4 == 0xb808 / 4) { // exit
        exit(word);
    }
    else {
        parent->writeWord(address, word);
    }
}

void MemoryMappedIOModel::writeHalfword(std::size_t address, uint16_t hw) {
    if (address / 4 == 0xb800 / 4) { // stdout
        if (address % 4)
            out << (char)(hw % 256);
    }
    else if (address / 4 == 0xb804 / 4) { // stdin
        // nop
    }
    else if (address / 4 == 0xb808 / 4) { // exit
        exit(hw);
    }
    else {
        parent->writeHalfword(address, hw);
    }
}

void MemoryMappedIOModel::writeByte(std::size_t address, uint8_t byte) {
    if (address == 0xb800) { // stdout
        out << (char)byte;
    }
    else if (address / 4 == 0xb804 / 4) { // stdin
        // nop
    }
    else if (address / 4 == 0xb808 / 4) { // exit
        exit(byte);
    }
    else {
        parent->writeByte(address, byte);
    }
}

uint32_t MemoryMappedIOModel::readWord(std::size_t address) const {
    if (address / 4 == 0xb800 / 4) { // stdout
        return 0;
    }
    else if (address / 4 == 0xb804 / 4) { // stdin
        char input;
        in >> input;
        return input;
    }
    else if (address / 4 == 0xb808 / 4) { // exit
        return 0;
    }
    else {
        return parent->readWord(address);
    }
}

uint16_t MemoryMappedIOModel::readHalfword(std::size_t address) const {
    if (address / 4 == 0xb800 / 4) { // stdout
        return 0;
    }
    else if (address / 4 == 0xb804 / 4) { // stdin
        char input;
        in >> input;
        return input;
    }
    else if (address / 4 == 0xb808 / 4) { // exit
        return 0;
    }
    else {
        return parent->readHalfword(address);
    }
}

uint8_t MemoryMappedIOModel::readByte(std::size_t address) const {
    if (address / 4 == 0xb800 / 4) { // stdout
        return 0;
    }
    else if (address / 4 == 0xb804 / 4) { // stdin
        char input;
        in >> input;
        return input;
    }
    else if (address / 4 == 0xb808 / 4) { // exit
        return 0;
    }
    else {
        return parent->readByte(address);
    }

}

MemoryMappedIOModel::~MemoryMappedIOModel() {};
