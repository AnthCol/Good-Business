#include "brstm.h"


// DELUXE EDITION


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
        static_cast<uint8_t>(src[ptr])     << 24 |
        static_cast<uint8_t>(src[ptr + 1]) << 16 |
        static_cast<uint8_t>(src[ptr + 2]) << 8  |
        static_cast<uint8_t>(src[ptr + 3])
    ); 

    ptr += 4; 
}

inline void BRSTM::copyBytes(uint16_t& dst, 
    const std::vector<char>& src,
    uint32_t& ptr)
{
    dst = static_cast<uint16_t>(
        static_cast<uint8_t>(src[ptr]) << 8 |
        static_cast<uint8_t>(src[ptr + 1])
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
    // one by one and filling the data. Could definitely use a 
    // rework, but to be honest it really isn't that slow, and it's
    // for a standard that basically isn't used, and if it were to 
    // ever change would probably be something completely different, 
    // so for now, whatever. 
    
    uint32_t pointer = 0;     

    // HEADER
    this->copyBytes(this->header.rstm, fileContent, 4, pointer); 
    this->copyBytes(this->header.byteOrderMark, fileContent, 2, pointer); 
    this->copyBytes(&this->header.majorVersion, fileContent, 1, pointer); 
    this->copyBytes(&this->header.minorVersion, fileContent, 1, pointer); 
    this->copyBytes(this->header.fileSize, fileContent, pointer); 
    this->copyBytes(this->header.headerSize, fileContent, pointer); 
    this->copyBytes(this->header.chunkCount, fileContent, pointer); 
    this->copyBytes(this->header.offsetToHEAD, fileContent, pointer); 
    this->copyBytes(this->header.sizeOfHEAD, fileContent, pointer); 
    this->copyBytes(this->header.offsetToADPC, fileContent, pointer); 
    this->copyBytes(this->header.sizeOfADPC, fileContent, pointer); 
    this->copyBytes(this->header.offsetToDATA, fileContent, pointer); 
    this->copyBytes(this->header.sizeOfDATA, fileContent, pointer); 
    this->movePointer(24, pointer); 
    
    // HEAD    
    this->copyBytes(this->head.head, fileContent, 4, pointer);
    this->copyBytes(this->head.lengthOfSection, fileContent, pointer); 
    this->movePointer(4, pointer); // skip markers
    this->copyBytes(this->head.offsetToHEAD1, fileContent, pointer); 
    this->movePointer(4, pointer); 
    this->copyBytes(this->head.offsetToHEAD2, fileContent, pointer); 
    this->movePointer(4, pointer);
    this->copyBytes(this->head.offsetToHEAD3, fileContent, pointer); 

    // HEAD Chunk 1 
    this->copyBytes(&this->head.chunk1.codec, fileContent, 1, pointer); 
    this->copyBytes(&this->head.chunk1.loopFlag, fileContent, 1, pointer);
    this->copyBytes(&this->head.chunk1.channelCount, fileContent, 1, pointer); 
    this->movePointer(1, pointer); 
    this->copyBytes(this->head.chunk1.sampleRate, fileContent, pointer); 
    this->movePointer(2, pointer); 
    this->copyBytes(this->head.chunk1.loopStart, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.totalSampleCount, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.absoluteOffsetADPCM, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.totalBlockCount, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.blockSize, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.samplesPerBlock, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.finalBlockSizeNoPadding, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.finalBlockSampleCount, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.finalBlockSizeWithPadding, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.samplesPerEntryInADPCTable, fileContent, pointer); 
    this->copyBytes(this->head.chunk1.bytesPerEntryInADPCTable, fileContent, pointer); 

    // HEAD Chunk 2 
    this->copyBytes(&this->head.chunk2.trackCount, fileContent, 1, pointer); 
    this->copyBytes(&this->head.chunk2.trackDescriptionType, fileContent, 1, pointer); 
    this->movePointer(2, pointer); 

    for (int i = 0; i < this->head.chunk2.trackCount; i++) {
        OffsetTableHEADChunk2 offsetTable; 
        Track track; 

        this->movePointer(1, pointer); 
        this->copyBytes(&offsetTable.trackDescriptionType, fileContent, 1, pointer); 
        this->movePointer(2, pointer); 
        this->copyBytes(offsetTable.offsetToTrackDescription, fileContent, pointer);  

        this->head.chunk2.offsetTablesToTracks.emplace_back(offsetTable, track); 
    }

    for (auto& [offsetTable, track] : this->head.chunk2.offsetTablesToTracks) {
        if (offsetTable.trackDescriptionType == 1) {
            this->copyBytes(&track.trackVolume, fileContent, 1, pointer); 
            this->copyBytes(&track.trackPanning, fileContent, 1, pointer); 
            this->movePointer(2, pointer); 
            this->movePointer(4, pointer); 
        }
        
        this->copyBytes(&track.channelCount, fileContent, 1, pointer); 
        this->copyBytes(&track.leftChannelID, fileContent, 1, pointer); 
        this->copyBytes(&track.rightChannelID, fileContent, 1, pointer); 
        this->movePointer(1, pointer);         
    }

    // HEAD chunk 3
    this->copyBytes(&this->head.chunk3.channelCount, fileContent, 1, pointer); 
    this->movePointer(3, pointer); 

    for (int i = 0; i < this->head.chunk3.channelCount; i++) { 
        OffsetTableHEADChunk3 offsetTable; 
        ChannelInfo channel; 
        
        this->movePointer(4, pointer); 
        this->copyBytes(offsetTable.offsetToChannelInfo, fileContent, pointer); 

        this->head.chunk3.offsetTableToChannel.emplace_back(offsetTable, channel); 
    }

    for (auto& [offsetTable, channel] : this->head.chunk3.offsetTableToChannel) {
        this->movePointer(4, pointer); 
        this->copyBytes(channel.offsetToADPCMCoefficients, fileContent, pointer);   
        for (int i = 0; i < 16; i++) {
            this->copyBytes(channel.ADPCMCoefficients[i], fileContent, pointer); 
        }
        this->copyBytes(channel.gain, fileContent, 2, pointer); 
        this->copyBytes(channel.initialPredictor, fileContent, 2, pointer); 
        this->copyBytes(channel.historySample1, fileContent, 2, pointer); 
        this->copyBytes(channel.historySample2, fileContent, 2, pointer); 
        this->copyBytes(channel.loopPredictor, fileContent, 2, pointer); 
        this->copyBytes(channel.loopHistory1, fileContent, 2, pointer); 
        this->copyBytes(channel.loopHistory2, fileContent, 2, pointer); 
        this->movePointer(2, pointer); 
    }


    for (int i = 0; i < 16; i++) {
        std::cout << std::hex << this->head.chunk3.offsetTableToChannel[0].second.ADPCMCoefficients[i] << " " << "\n"; 
    }


    std::cout << (int)pointer << "\n"; 

    std::cout << std::hex << (int)this->head.chunk2.trackCount << "\n"; 
    std::cout << (int)this->head.chunk1.codec << "\n"; 


    // HEAD Chunk 3

}

