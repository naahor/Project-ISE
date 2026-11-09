#include "BarcodeDecoder.h"

const QVector<int> BarcodeDecoder::STOP_PATTERN = { 2, 3, 3, 1, 1, 1, 2 };

BarcodeDecoder::BarcodeDecoder() {
    initializePatterns();
}

void BarcodeDecoder::initializePatterns() {
    // Subset of standard Code 128 width patterns (B1, S1, B2, S2, B3, S3)
    // Common digits (00-99 mapping), Start C (105), and internal controls
    m_symbolTable = {
        {0,  {2, 1, 2, 2, 2, 2}}, {1,  {2, 2, 2, 1, 2, 2}}, {2,  {2, 2, 2, 2, 2, 1}},
        {3,  {1, 2, 1, 2, 2, 3}}, {4,  {1, 2, 1, 3, 2, 2}}, {5,  {1, 3, 1, 2, 2, 2}},
        {6,  {1, 2, 2, 2, 1, 3}}, {7,  {1, 2, 2, 3, 1, 2}}, {8,  {1, 3, 2, 2, 1, 2}},
        {9,  {2, 2, 1, 2, 1, 3}}, {10, {2, 2, 1, 3, 1, 2}}, {11, {2, 3, 1, 2, 1, 2}},
        {12, {1, 1, 2, 2, 3, 2}}, {13, {1, 2, 2, 1, 3, 2}}, {14, {1, 2, 2, 2, 3, 1}},
        {15, {1, 1, 3, 2, 2, 2}}, {16, {1, 2, 3, 1, 2, 2}}, {17, {1, 2, 3, 2, 2, 1}},
        {18, {2, 2, 3, 2, 1, 1}}, {19, {2, 2, 1, 1, 3, 2}}, {20, {2, 2, 1, 2, 3, 1}},
        {21, {2, 1, 3, 2, 1, 2}}, {22, {2, 2, 3, 1, 1, 2}}, {23, {3, 1, 2, 1, 3, 1}},
        {24, {3, 1, 1, 2, 2, 2}}, {25, {3, 2, 1, 1, 2, 2}}, {26, {3, 2, 1, 2, 2, 1}},
        {27, {3, 1, 2, 2, 1, 2}}, {28, {3, 2, 2, 1, 1, 2}}, {29, {3, 2, 2, 2, 1, 1}},
        {30, {2, 1, 2, 1, 2, 3}}, {31, {2, 1, 2, 3, 2, 1}}, {32, {2, 3, 2, 1, 2, 1}},
        {33, {1, 1, 1, 3, 2, 3}}, {34, {1, 3, 1, 1, 2, 3}}, {35, {1, 3, 1, 3, 2, 1}},
        {36, {1, 1, 2, 3, 1, 3}}, {37, {1, 3, 2, 1, 1, 3}}, {38, {1, 3, 2, 3, 1, 1}},
        {39, {2, 1, 1, 3, 1, 3}}, {40, {2, 3, 1, 1, 1, 3}}, {41, {2, 3, 1, 3, 1, 1}},
        {42, {1, 1, 2, 1, 3, 3}}, {43, {1, 1, 2, 3, 3, 1}}, {44, {1, 3, 2, 1, 3, 1}},
        {45, {1, 1, 3, 1, 2, 3}}, {46, {1, 1, 3, 3, 2, 1}}, {47, {1, 3, 3, 1, 2, 1}},
        {48, {3, 1, 3, 1, 2, 1}}, {49, {2, 1, 1, 3, 3, 1}}, {50, {2, 3, 1, 1, 3, 1}},
        {51, {2, 1, 3, 1, 1, 3}}, {52, {2, 1, 3, 3, 1, 1}}, {53, {2, 1, 3, 1, 3, 1}},
        {54, {3, 1, 1, 1, 2, 3}}, {55, {3, 1, 1, 3, 2, 1}}, {56, {3, 3, 1, 1, 2, 1}},
        {57, {3, 1, 2, 1, 1, 3}}, {58, {3, 1, 2, 3, 1, 1}}, {59, {3, 3, 2, 1, 1, 1}},
        {60, {3, 1, 4, 1, 1, 1}}, {61, {2, 2, 1, 4, 1, 1}}, {62, {4, 3, 1, 1, 1, 1}},
        {63, {1, 1, 1, 2, 2, 4}}, {64, {1, 1, 1, 4, 2, 2}}, {65, {1, 2, 1, 1, 2, 4}},
        {66, {1, 2, 1, 4, 2, 1}}, {67, {1, 4, 1, 1, 2, 2}}, {68, {1, 4, 1, 2, 2, 1}},
        {69, {1, 1, 2, 2, 1, 4}}, {70, {1, 1, 2, 4, 1, 2}}, {71, {1, 2, 2, 1, 1, 4}},
        {72, {1, 2, 2, 4, 1, 1}}, {73, {1, 4, 2, 1, 1, 2}}, {74, {1, 4, 2, 2, 1, 1}},
        {75, {2, 4, 1, 2, 1, 1}}, {76, {2, 2, 1, 1, 1, 4}}, {77, {4, 1, 3, 1, 1, 1}},
        {78, {2, 4, 1, 1, 1, 2}}, {79, {1, 3, 4, 1, 1, 1}}, {80, {1, 1, 1, 2, 4, 2}},
        {81, {1, 2, 1, 1, 4, 2}}, {82, {1, 2, 1, 2, 4, 1}}, {83, {1, 1, 4, 2, 1, 2}},
        {84, {1, 2, 4, 1, 1, 2}}, {85, {1, 2, 4, 2, 1, 1}}, {86, {4, 1, 1, 2, 1, 2}},
        {87, {4, 2, 1, 1, 1, 2}}, {88, {4, 2, 1, 2, 1, 1}}, {89, {2, 1, 2, 1, 4, 1}},
        {90, {2, 1, 4, 1, 2, 1}}, {91, {4, 1, 2, 1, 2, 1}}, {92, {1, 1, 1, 1, 4, 3}},
        {93, {1, 1, 1, 3, 4, 1}}, {94, {1, 3, 1, 1, 4, 1}}, {95, {1, 1, 4, 1, 1, 3}},
        {96, {1, 1, 4, 3, 1, 1}}, {97, {4, 1, 1, 1, 1, 3}}, {98, {4, 1, 1, 3, 1, 1}},
        {99, {1, 1, 3, 1, 4, 1}}, {105, {2, 1, 1, 4, 1, 2}} // Start C
    };
}

int BarcodeDecoder::matchSymbol(const QVector<int>& runGroup) {
    for (const auto& sym : m_symbolTable) {
        if (sym.widthPattern == runGroup) {
            return sym.id;
        }
    }
    return -1;
}

bool BarcodeDecoder::decode(const QVector<int>& normalizedRuns, QString& outDecodedValue, QString& outErrorMessage) {
    // For a 6-digit Code 128 Code C barcode:
    // Start C (6 runs) + 3 Data Pairs (18 runs) + Checksum (6 runs) + Stop (7 runs) = 37 total runs.
    if (normalizedRuns.size() < 37) {
        outErrorMessage = QString("Invalid run length count: Expected 37, got %1.").arg(normalizedRuns.size());
        return false;
    }

    // 1. Verify Start C (Symbol ID 105)
    QVector<int> startSymbol = normalizedRuns.mid(0, 6);
    int startId = matchSymbol(startSymbol);
    if (startId != 105) {
        outErrorMessage = "Invalid Start Symbol. Expected Start C pattern.";
        return false;
    }

    // 2. Verify Stop Symbol (7 width elements)
    QVector<int> stopSymbol = normalizedRuns.mid(30, 7);
    if (stopSymbol != STOP_PATTERN) {
        outErrorMessage = "Invalid Stop Symbol pattern.";
        return false;
    }

    // 3. Decode Data Symbols (3 pairs -> 6 digits total)
    QVector<int> dataSymbolIds;
    for (int i = 6; i < 24; i += 6) {
        QVector<int> symbolRuns = normalizedRuns.mid(i, 6);
        int symId = matchSymbol(symbolRuns);
        if (symId < 0 || symId > 99) {
            outErrorMessage = QString("Invalid data symbol pattern at index offset %1.").arg(i);
            return false;
        }
        dataSymbolIds.append(symId);
    }

    // 4. Decode Checksum Symbol
    QVector<int> checksumRuns = normalizedRuns.mid(24, 6);
    int scannedChecksumId = matchSymbol(checksumRuns);
    if (scannedChecksumId == -1) {
        outErrorMessage = "Invalid Checksum symbol pattern.";
        return false;
    }

    // 5. Validate Modulo 103 Checksum
    int checksumSum = startId;
    for (int i = 0; i < dataSymbolIds.size(); ++i) {
        checksumSum += (i + 1) * dataSymbolIds[i];
    }
    int calculatedChecksum = checksumSum % 103;

    if (calculatedChecksum != scannedChecksumId) {
        outErrorMessage = QString("Checksum mismatch: Calculated %1, Read %2.")
            .arg(calculatedChecksum)
            .arg(scannedChecksumId);
        return false;
    }

    // 6. Reconstruct original 6-digit string
    QString result = "";
    for (int id : dataSymbolIds) {
        result += QString("%1").arg(id, 2, 10, QChar('0'));
    }

    outDecodedValue = result;
    return true;
}