#include <cstddef>
#include <cstdint>
#include <iostream>

class MemoryModel {
public:
    virtual void writeWord(std::size_t address, uint32_t word) = 0;
    virtual void writeHalfword(std::size_t address, uint16_t hw) = 0;
    virtual void writeByte(std::size_t address, uint8_t byte) = 0;
    virtual uint32_t readWord(std::size_t address) const = 0;
    virtual uint16_t readHalfword(std::size_t address) const = 0;
    virtual uint8_t readByte(std::size_t address) const = 0;
    virtual ~MemoryModel() {};
};

class FlatMemoryModel : public MemoryModel {
public:
    FlatMemoryModel(size_t size);
    virtual ~FlatMemoryModel();

    virtual void writeWord(std::size_t address, uint32_t word);
    virtual void writeHalfword(std::size_t address, uint16_t hw);
    virtual void writeByte(std::size_t address, uint8_t byte);

    virtual uint32_t readWord(std::size_t address) const;
    virtual uint16_t readHalfword(std::size_t address) const;
    virtual uint8_t readByte(std::size_t address) const;

    uint8_t *raw() const;
    size_t getSize() const;
private:
    uint8_t *mem;
    size_t size;
};

class MemoryMappedIOModel : public MemoryModel {
public:
    MemoryMappedIOModel(MemoryModel *parent, std::ostream &out, std::istream &in);
    virtual ~MemoryMappedIOModel();

    virtual void writeWord(std::size_t address, uint32_t word);
    virtual void writeHalfword(std::size_t address, uint16_t hw);
    virtual void writeByte(std::size_t address, uint8_t byte);

    virtual uint32_t readWord(std::size_t address) const;
    virtual uint16_t readHalfword(std::size_t address) const;
    virtual uint8_t readByte(std::size_t address) const;

private:
    MemoryModel *parent;
    std::ostream &out;
    std::istream &in;
};
