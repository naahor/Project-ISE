#include "Encoder.h"
#include "BarcodeImageGenerator.h"
#include <iostream>
#include <stdexcept>
using namespace std;

int main()
{
    BarcodeImageGenerator generator(3, 150);
    char again;

    do
    {
        string number;
        cout << "Enter a 6-digit number: ";
        cin >> number;

        try
        {
            string bits = Encoder::encode(number);
            QString fileName = QString::fromStdString("barcode_" + number + ".png");

            if (generator.saveAsPng(bits, fileName))
            {
                cout << "Barcode saved as " << fileName.toStdString() << endl;
            }
            else
            {
                cout << "Could not save the image." << endl;
            }
        }
        catch (invalid_argument& error)
        {
            cout << "Error: " << error.what() << endl;
        }

        cout << "Generate another barcode? (y/n): ";
        cin >> again;
    } while (again == 'y' || again == 'Y');

    return 0;
}
