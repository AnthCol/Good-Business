#include "brstm.h"

inline void BRSTM::copyBytes(uint8_t* dst, 
    const std::vector<char>& src, 
    int amount, 
    uint32_t& ptr) 
{
    for (int i = 0; i < amount; i++) {
        dst[i] = src[ptr++]; 
    }    
}

inline void BRSTM::copyBytes(uint32_t& dst, 
    const std::vector<char>& src, 
    uint32_t& ptr) 
{
    // When we are copying into an uint32_t, the 
    // amout we are copying is always going to be four. 
    // Everything else is represented as a uint8_t array. 

    // NOTE Endian assumed for now

    dst = static_cast<uint32_t>(
        static_cast<uint32_t>(src[ptr])     << 24 |
        static_cast<uint32_t>(src[ptr + 1]) << 16 |
        static_cast<uint32_t>(src[ptr + 2]) << 8  |
        static_cast<uint32_t>(src[ptr + 3])
    ); 

    ptr += 4; 
}

inline void BRSTM::copyBytes(uint16_t& dst, 
    const std::vector<char>& src,
    uint32_t& ptr)
{
    dst = static_cast<uint16_t>(
        static_cast<uint16_t>(src[ptr]) << 8 |
        static_cast<uint16_t>(src[ptr + 1])
    ); 

    ptr += 2; 
}


inline void BRSTM::movePointer(int amount, uint32_t& ptr) 
{
    ptr += amount;
}

void BRSTM::parse(const std::vector<char>& fileContent) 
{ 
    // Literally just going through the steps of the spec
    // one by one and filling the data. 
    // Could use a rework later, but it really isn't that bad since 
    // the spec for this - if it were to change (of which the chances
    // are extremely low) would probably just be a new spec in it's own 
    // right considering how old this is. 

    uint32_t pointer = 0;     
    this->copyBytes(this->header.rstm, fileContent, 4, pointer); 
    this->copyBytes(this->header.byteOrderMark, fileContent, 2, pointer); 
    this->copyBytes(&this->header.majorVersion, fileContent, 1, pointer); 
    this->copyBytes(&this->header.minorVersion, fileContent, 1, pointer); 
    this->copyBytes(this->header.fileSize, fileContent, pointer); 


    std::cout << std::hex; 
    std::cout << (int)this->header.byteOrderMark[0] << (int)this->header.byteOrderMark[1] << "\n"; 
    std::cout << (int)this->header.majorVersion << "\n"; 
    std::cout << (int)this->header.minorVersion << "\n"; 
    std::cout << (int)this->header.fileSize << "\n"; 

}

