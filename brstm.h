#pragma once

#include "common.h"


// See spec: https://wiibrew.org/wiki/BRSTM_file

struct Header {
    uint8_t rstm [4] = {0}; 
    uint8_t byteOrderMark [2] = {0}; 
    uint8_t majorVersion = 0; 
    uint8_t minorVersion = 0; 
    uint32_t fileSize = 0; 
    uint16_t headerSize = 0; 
    uint16_t chunkCount = 0; 
    uint32_t offsetToHEAD = 0; 
    uint32_t sizeOfHEAD = 0; 
    uint32_t offsetToADPC = 0; 
    uint32_t sizeOfADPC = 0; 
    uint32_t offsetToDATA = 0; 
    uint32_t sizeOfDATA = 0; 
}; 

struct HEADChunk1 {
    uint8_t codec = 0; 
    uint8_t loopFlag = 0; 
    uint8_t channelCount = 0; 
    uint16_t sampleRate [2] = {0}; 
    uint32_t loopStart = 0; 
    uint32_t totalSampleCount = 0; 
    uint32_t absoluteOffsetADPCM = 0; 
    uint32_t totalBlockCount = 0; 
    uint32_t blockSize = 0; 
    uint32_t samplesPerBlock = 0; 
    uint32_t finalBlockSizeNoPadding = 0; 
    uint32_t finalBlockSampleCount = 0; 
    uint32_t finalBlockSizeWithPadding = 0; 
    uint32_t samplesPerEntryInADPCTable = 0; 
    uint32_t bytesPerEntryInADPCTable = 0; 
}; 


struct Track {
    uint8_t channelCount = 0; 
    uint8_t leftChannelID = 0; 
    uint8_t rightChannelID = 0; 
}; 

// Only for clarity
struct TrackType0 : Track {

}; 

struct TrackType1 : Track {
    uint8_t trackVolume = 0; 
    uint8_t trackPanning = 0; 
}; 

struct OffsetTableHEADChunk2 {
    uint8_t trackDescriptionType = 0; 
    uint32_t offsetToTrackDescription = 0; 
    Track track; 
}; 

namespace std {
    template <>
    struct hash<OffsetTableHEADChunk2> {
        size_t operator()(const OffsetTableHEADChunk2& key) const {
            uint32_t sum = key.trackDescriptionType; 
            sum += key.offsetToTrackDescription; 
            sum += key.track.channelCount; 
            sum += key.track.leftChannelID; 
            sum += key.track.rightChannelID; 
            return std::hash<uint32_t>()(sum); 
        }
    }; 
}

inline bool operator==(const OffsetTableHEADChunk2& lhs, const OffsetTableHEADChunk2& rhs) {
    const bool comparison = (
        lhs.offsetToTrackDescription == rhs.offsetToTrackDescription
        && lhs.track.channelCount == rhs.track.channelCount 
        && lhs.track.leftChannelID == rhs.track.leftChannelID 
        && lhs.track.rightChannelID == rhs.track.rightChannelID
    ); 
    return comparison; 
}


struct HEADChunk2 {
    uint8_t trackCount = 0; 
    uint8_t trackDescriptionType = 0; 
    std::unordered_map<OffsetTableHEADChunk2, Track> offsetTablesToTracks; 
}; 


struct ChannelInfo {
    uint32_t marker = 0; 
    uint32_t offsetToADPCMCoefficients = 0; 
    uint8_t ADPCMCoefficients [0x20] = {0}; 
    uint8_t gain [2] = {0}; 
    uint8_t initialPredictor [2] = {0};
    uint8_t historySample1 [2] = {0}; 
    uint8_t historySample2 [2] = {0}; 
    uint8_t loopPredictor [2] = {0}; 
    uint8_t loopHistory1 [2] = {0}; 
    uint8_t loopHistory2 [2] = {0}; 
}; 

struct OffsetTableHEADChunk3 {
    uint32_t marker = 0; 
    uint32_t offsetToChannelInfo = 0; 
}; 


namespace std {
    template <>
    struct hash<OffsetTableHEADChunk3> {
        size_t operator()(const OffsetTableHEADChunk3& key) const {
            const uint32_t sum = key.marker + key.offsetToChannelInfo; 
            return std::hash<uint32_t>()(sum); 
        }
    }; 
}


inline bool operator==(const OffsetTableHEADChunk3& lhs, const OffsetTableHEADChunk3& rhs) {
    const bool comparison = (
        lhs.marker == rhs.marker 
        && lhs.offsetToChannelInfo == rhs.offsetToChannelInfo
    ); 
    return comparison; 
}


struct HEADChunk3 {
    uint8_t channelCount = 0; 
    std::unordered_map<OffsetTableHEADChunk3, ChannelInfo> offsetTableToChannel; 
}; 


struct HEAD {
    uint8_t head [4] = {0};
    uint32_t lengthOfSection = 0; 
    uint8_t marker1 [4] = {0}; 
    uint32_t offsetToHEAD1 = 0; 
    uint8_t marker2 [4] = {0}; 
    uint32_t offsetToHEAD2 = 0; 
    uint8_t marker3 [4] = {0}; 
    uint32_t offsetToHEAD3 = 0; 

    HEADChunk1 chunk1; 
    HEADChunk2 chunk2; 
    HEADChunk3 chunk3; 
}; 

struct ChannelHistorySample {
    uint8_t sample1 [2] = {0}; 
    uint8_t sample2 [2] = {0}; 
}; 

struct ADPCEntry {
    std::vector<ChannelHistorySample> samples; 
}; 


struct ADPC {
    uint8_t adpc [4] = {0}; 
    uint32_t lengthOfSection = 0; 
    std::vector<ADPCEntry> entries; 
}; 


struct Block {
    std::vector<uint8_t> data; 
}; 

struct DATA {
    uint8_t data [4] = {0}; 
    uint32_t lengthOfSection = 0; 
    std::unordered_map<uint32_t, std::vector<Block>> channelToBlocks; 
}; 


class BRSTM {
private:
    Header header; 
    HEAD head; 
    ADPC adpc; 
    DATA data; 

    void copyBytes(uint8_t*, const std::vector<char>&, int, uint32_t&); 
    void copyBytes(uint16_t&, const std::vector<char>&, uint32_t&); 
    void copyBytes(uint32_t&, const std::vector<char>&, uint32_t&); 

    void movePointer(int, uint32_t&); 

public:
    void parse(const std::vector<char>&); 

}; 


