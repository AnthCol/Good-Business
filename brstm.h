#pragma once

#include "common.h"


// See spec: https://wiibrew.org/wiki/BRSTM_file

struct Header {
    unsigned char rstm [4] = {0}; 
    unsigned char byteOrderMark [2] = {0}; 
    unsigned char majorVersion = 0; 
    unsigned char minorVersion = 0; 
    unsigned int fileSize = 0; 
    unsigned char headerSize [2] = {0}; 
    unsigned char chunkCount [2] = {0}; 
    unsigned int offsetToHEAD = 0; 
    unsigned int sizeOfHEAD = 0; 
    unsigned int offsetToADPC = 0; 
    unsigned int sizeOfADPC = 0; 
    unsigned int offsetToDATA = 0; 
    unsigned int sizeOfDATA = 0; 
}; 

struct HEADChunk1 {
    unsigned char codec = 0; 
    unsigned char loopFlag = 0; 
    unsigned char channelCount = 0; 
    unsigned char sampleRate [2] = {0}; 
    unsigned int loopStart = 0; 
    unsigned int totalSampleCount = 0; 
    unsigned int absoluteOffsetADPCM = 0; 
    unsigned int totalBlockCount = 0; 
    unsigned int blockSize = 0; 
    unsigned int samplesPerBlock = 0; 
    unsigned int finalBlockSizeNoPadding = 0; 
    unsigned int finalBlockSampleCount = 0; 
    unsigned int finalBlockSizeWithPadding = 0; 
    unsigned int samplesPerEntryInADPCTable = 0; 
    unsigned int bytesPerEntryInADPCTable = 0; 
}; 


struct Track {
    unsigned char channelCount = 0; 
    unsigned char leftChannelID = 0; 
    unsigned char rightChannelID = 0; 
}; 

// Only for clarity
struct TrackType0 : Track {

}; 

struct TrackType1 : Track {
    unsigned char trackVolume = 0; 
    unsigned char trackPanning = 0; 
}; 

struct OffsetTableHEADChunk2 {
    unsigned char trackDescriptionType = 0; 
    unsigned int offsetToTrackDescription = 0; 
    Track track; 
}; 

namespace std {
    template <>
    struct hash<OffsetTableHEADChunk2> {
        size_t operator()(const OffsetTableHEADChunk2& key) const {
            unsigned int sum = key.trackDescriptionType; 
            sum += key.offsetToTrackDescription; 
            sum += key.track.channelCount; 
            sum += key.track.leftChannelID; 
            sum += key.track.rightChannelID; 
            return std::hash<unsigned int>()(sum); 
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
    unsigned char trackCount = 0; 
    unsigned char trackDescriptionType = 0; 
    std::unordered_map<OffsetTableHEADChunk2, Track> offsetTablesToTracks; 
}; 


struct ChannelInfo {
    unsigned int marker = 0; 
    unsigned int offsetToADPCMCoefficients = 0; 
    unsigned char ADPCMCoefficients [0x20] = {0}; 
    unsigned char gain [2] = {0}; 
    unsigned char initialPredictor [2] = {0};
    unsigned char historySample1 [2] = {0}; 
    unsigned char historySample2 [2] = {0}; 
    unsigned char loopPredictor [2] = {0}; 
    unsigned char loopHistory1 [2] = {0}; 
    unsigned char loopHistory2 [2] = {0}; 
}; 

struct OffsetTableHEADChunk3 {
    unsigned int marker = 0; 
    unsigned int offsetToChannelInfo = 0; 
}; 


namespace std {
    template <>
    struct hash<OffsetTableHEADChunk3> {
        size_t operator()(const OffsetTableHEADChunk3& key) const {
            const unsigned int sum = key.marker + key.offsetToChannelInfo; 
            return std::hash<unsigned int>()(sum); 
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
    unsigned char channelCount = 0; 
    std::unordered_map<OffsetTableHEADChunk3, ChannelInfo> offsetTableToChannel; 
}; 


struct HEAD {
    unsigned char head [4] = {0};
    unsigned int lengthOfSection = 0; 
    unsigned char marker1 [4] = {0}; 
    unsigned int offsetToHEAD1 = 0; 
    unsigned char marker2 [4] = {0}; 
    unsigned int offsetToHEAD2 = 0; 
    unsigned char marker3 [4] = {0}; 
    unsigned int offsetToHEAD3 = 0; 

    HEADChunk1 chunk1; 
    HEADChunk2 chunk2; 
    HEADChunk3 chunk3; 
}; 

struct ChannelHistorySample {
    unsigned char sample1 [2] = {0}; 
    unsigned char sample2 [2] = {0}; 
}; 

struct ADPCEntry {
    std::vector<ChannelHistorySample> samples; 
}; 


struct ADPC {
    unsigned char adpc [4] = {0}; 
    unsigned int lengthOfSection = 0; 
    std::vector<ADPCEntry> entries; 
}; 


struct Block {
    std::vector<unsigned char> data; 
}; 

struct DATA {
    unsigned char data [4] = {0}; 
    unsigned int lengthOfSection = 0; 
    std::unordered_map<unsigned int, std::vector<Block>> channelToBlocks; 
}; 


class BRSTM {
private:
    Header header; 
    HEAD head; 
    ADPC adpc; 
    DATA data; 

public:
    void parse(const std::vector<char>&); 

}; 


