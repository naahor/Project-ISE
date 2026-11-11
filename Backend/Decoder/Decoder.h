#pragma once
#ifndef DECODER_H
#define DECODER_H

#include <string>
#include <vector>
#include "ImageProcessor.h"

using namespace std;
class Decoder {
private:
    static const string startPattern;
    static const string stopPattern;
    static const string patterns[103];
    static const int startValue;
    static const int checksumDivisor;

    // Helper to find which Code 128 pattern index matches an 11-bit binary chunk
    static int findPatternIndex(const string& patternChunk);

public:
    // Converts a full 68-bit binary string back into the original 6-digit string
    static string decodeBinary(const string& binaryBits);

    // Converts color runs and normalized module counts directly into the decoded 6-digit string
    static string decodeRuns(const vector<ColorRun>& runs, const vector<int>& moduleCounts);
};

#endif // DECODER_H