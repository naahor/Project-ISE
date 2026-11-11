#ifndef ENCODER_H
#define ENCODER_H

#include <string>
using namespace std;
class Encoder{
private:
    static int length;
    static int pairValue(const string& digits, int index);

public:
    static void validate(const string& digits);
    static int calculateChecksum(const string& digits);
    static string encode(const string& digits);
};

#endif