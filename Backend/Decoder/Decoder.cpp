#include "Decoder.h"
#include <stdexcept>
#include <iostream>

using namespace std;

// Code 128 Constants matching the encoder specifications
const string Decoder::startPattern = "11010011100";
const string Decoder::stopPattern = "1100011101011";
const int Decoder::startValue = 105;
const int Decoder::checksumDivisor = 103;

// Full Code 128 Pattern Table (0 to 102)[cite: 4]
const string Decoder::patterns[103] = {
    "11011001100", "11001101100", "11001100110", "10010011000", "10010001100",
    "10001001100", "10011001000", "10011000100", "10001100100", "11001001000",
    "11001000100", "11000100100", "10110011100", "10011011100", "10011001110",
    "10111001100", "10011101100", "10011100110", "11001110010", "11001011100",
    "11001001110", "11011100100", "11001110100", "11101101110", "11101001100",
    "11100101100", "11100100110", "11101100100", "11100110100", "11100110010",
    "11011011000", "11011000110", "11000110110", "10100011000", "10001011000",
    "10001000110", "10110001000", "10001101000", "10001100010", "11010001000",
    "11000101000", "11000100010", "10110111000", "10110001110", "10001101110",
    "10111011000", "10111000110", "10001110110", "11101110110", "11010001110",
    "11000101110", "11011101000", "11011100010", "11011101110", "11101011000",
    "11101000110", "11100010110", "11101101000", "11101100010", "11100011010",
    "11101111010", "11001000010", "11110001010", "10100110000", "10100001100",
    "10010110000", "10010000110", "10000101100", "10000100110", "10110010000",
    "10110000100", "10011010000", "10011000010", "10000110100", "10000110010",
    "11000010010", "11001010000", "11110111010", "11000010100", "10001111010",
    "10100111100", "10010111100", "10010011110", "10111100100", "10011110100",
    "10011110010", "11110100100", "11110010100", "11110010010", "11011011110",
    "11011110110", "11110110110", "10101111000", "10100011110", "10001011110",
    "10111101000", "10111100010", "11110101000", "11110100010", "10111011110",
    "10111101110", "11101011110", "11110101110"
};

int Decoder::findPatternIndex(const string& patternChunk)
{
    for (int i = 0; i < 103; i++) {
        if (patterns[i] == patternChunk) {
            return i; // Returns the decoded index value (0-102)
        }
    }
    return -1; // Not found / invalid pattern
}

string Decoder::decodeBinary(const string& binaryBits)
{
    // 1. Verify minimum length for Code-C 6-digit barcode (Start + 3 data chars + 1 checksum + Stop = 68 bits)
    if (binaryBits.length() < 68) {
        throw invalid_argument("Binary string is too short to be a valid Code 128 barcode.");
    }

    // 2. Verify Start Pattern
    string startCheck = binaryBits.substr(0, 11);
    if (startCheck != startPattern) {
        throw invalid_argument("Invalid Code 128 Start pattern.");
    }

    // 3. Extract 11-bit chunks for data pairs (3 pairs = 6 digits) and 1 checksum chunk
    // Each character is exactly 11 bits long.
    // Index mapping: 
    // Chars 0: start (11 bits)
    // Char 1: bits 11 to 21 (Digits 1 & 2)
    // Char 2: bits 22 to 32 (Digits 3 & 4)
    // Char 3: bits 33 to 43 (Digits 5 & 6)
    // Char 4: bits 44 to 54 (Checksum character)

    int index1 = findPatternIndex(binaryBits.substr(11, 11));
    int index2 = findPatternIndex(binaryBits.substr(22, 11));
    int index3 = findPatternIndex(binaryBits.substr(33, 11));
    int checksumIndex = findPatternIndex(binaryBits.substr(44, 11));

    if (index1 < 0 || index2 < 0 || index3 < 0 || checksumIndex < 0) {
        throw runtime_error("Failed to match barcode patterns against Code 128 dictionary.");
    }

    // 4. Verify Checksum Logic
    // Formula from encoder: sum = startValue + (val1 * 1) + (val2 * 2) + (val3 * 3); checksum = sum % 103[cite: 4]
    int calculatedChecksum = (startValue + (index1 * 1) + (index2 * 2) + (index3 * 3)) % checksumDivisor;
    if (calculatedChecksum != checksumIndex) {
        throw runtime_error("Barcode checksum validation failed.");
    }

    // 5. Convert indices back to 2-digit string blocks
    auto formatPair = [](int val) {
        string s = to_string(val);
        if (val < 10) s = "0" + s; // Ensure leading zero if needed (e.g. 5 -> "05")
        return s;
        };

    string decodedDigits = formatPair(index1) + formatPair(index2) + formatPair(index3);
    return decodedDigits;
}

string Decoder::decodeRuns(const vector<ColorRun>& runs, const vector<int>& moduleCounts)
{
    // Reconstruct the binary bit stream from normalized module counts and color runs
    string reconstructedBinary = "";

    for (size_t i = 0; i < runs.size(); i++) {
        char bitChar = runs[i].isBlack ? '1' : '0';
        int modules = moduleCounts[i];

        // Append the bit character according to its module width
        reconstructedBinary.append(modules, bitChar);
    }

    // Pass the reconstructed binary string into the binary decoder
    return decodeBinary(reconstructedBinary);
}