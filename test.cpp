#include "Encoder.h"
#include <iostream>
#include <vector>
#include <stdexcept>
using namespace std;

int totalTests = 0;
int failedTests = 0;

const string startC = "11010011100";
const string stopC = "1100011101011";

void report(string name, bool passed)
{
    totalTests++;
    if (!passed){
        failedTests++;
    }

    if (passed){
        cout << "PASS " << name << endl;
    }
    else
    {
        cout << "FAIL " << name << endl;
    }
}

bool throwsInvalidArgument(string input)
{
    try{
        Encoder::encode(input);
    }
    catch (invalid_argument&)
    {
        return true;
    }
    return false;
}

bool onlyZerosAndOnes(string bits)
{
    for (int i = 0; i < (int)bits.length(); i++){
        if (bits[i] != '0' && bits[i] != '1'){
            return false;
        }
    }
    return true;
}

bool startsWith(string text, string prefix){
    return text.substr(0, prefix.length()) == prefix;
}

bool endsWith(string text, string suffix){
    if (text.length() < suffix.length()){
        return false;
    }
    return text.substr(text.length() - suffix.length()) == suffix;
}

void testValidInputs()
{
    vector<string> inputs = { "123456", "100000", "999999", "543210", "102030", "987654", "111111" };

    for (int i = 0; i < (int)inputs.size(); i++)
    {
        string bits = Encoder::encode(inputs[i]);
        report("length is 68 for " + inputs[i], bits.length() == 68);
        report("only 0 and 1 for " + inputs[i], onlyZerosAndOnes(bits));
        report("starts with Start C for " + inputs[i], startsWith(bits, startC));
        report("ends with Stop for " + inputs[i], endsWith(bits, stopC));
    }
}

void testChecksum(){
    
    report("checksum of 123456 is 44", Encoder::calculateChecksum("123456") == 44);
    report("checksum of 100000 is 12", Encoder::calculateChecksum("100000") == 12);
    report("checksum of 999999 is 81", Encoder::calculateChecksum("999999") == 81);
    report("checksum of 543210 is 47", Encoder::calculateChecksum("543210") == 47);
    report("checksum of 102030 is 39", Encoder::calculateChecksum("102030") == 39);
}

void testKnownBinaryPatterns()
{
    report("full pattern of 123456",
        Encoder::encode("123456") == "11010011100101100111001000101100011100010110100011011101100011101011");

    report("full pattern of 100000",
        Encoder::encode("100000") == "11010011100110010001001101100110011011001100101100111001100011101011");

    report("full pattern of 999999",
        Encoder::encode("999999") =="11010011100101110111101011101111010111011110100101111001100011101011");
}

void testInvalidInputs(){

    vector<string> invalidInputs = { "", "1", "12345", "1234567", "12345678901",
        "12a456", "abcdef", "123 56", " 23456", "12345 ",
        "-12345", "12.456", "012345", "000000" };

    for (int i = 0; i < invalidInputs.size(); i++){
        report("rejects \"" + invalidInputs[i] + "\"", throwsInvalidArgument(invalidInputs[i]));
    }
}

void testAllValidInputs()
{
    bool allGood = true;

    for (int number = 100000; number <= 999999; number++)
    {
        string bits = Encoder::encode(to_string(number));
        if (bits.length() != 68 || !startsWith(bits, startC) || !endsWith(bits, stopC))
        {
            allGood = false;
            break;
        }
    }

    report("all numbers 100000 to 999999 give a valid 68-module pattern", allGood);
}

int main()
{
    testValidInputs();
    testChecksum();
    testKnownBinaryPatterns();
    testInvalidInputs();
    testAllValidInputs();

    cout << endl << "Total: " << totalTests << "  Failed: " << failedTests << endl;

    if (failedTests == 0){
        return 0;
    }
    return 1;
}