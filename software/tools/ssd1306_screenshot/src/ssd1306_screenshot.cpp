#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include "Image.h"

enum class State
{
    SearchForDataBegin,
    ReadDataBytes,
    SearchForLineEnd
};

unsigned char getNibble(char ch)
{
    if (ch >= '0' && ch <= '9') return (ch - '0');
    if (ch >= 'A' && ch <= 'F') return (ch - 'A') + 10;
    return 0;
}

void fillBackground(Image& image, const Image::Pixel& color)
{
    for (int y = 0; y < image.GetH(); ++y)
    {
        for (int x = 0; x < image.GetW(); ++x)
        {
            image.SetPixel(x, y, color);
        }
    }
}

void placeByteOnImage(Image& image, unsigned char byte, int byteNumber)
{
    int c = (byteNumber & 0x007F);
    int p = (byteNumber & 0x0380) >> 7;

    const Image::Pixel pixelOn (0x00, 0xFF, 0xFF);
    const Image::Pixel pixelOff(0x00, 0x00, 0x00);

    for (int b = 0; b < 8; ++b)
    {
        image.SetPixel(16 + c, 16 + (p << 3 | b), (byte & 0x01) != 0 ? pixelOn : pixelOff);
        byte >>= 1;
    }
}

int main()
{
    std::ifstream fileStream("screenshot.txt");
    std::string fileContent((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());

    Image result(160, 96, false);
    fillBackground(result, Image::Pixel(0x40, 0x40, 0x40));

    int byteNumber = 0;
    State state = State::SearchForDataBegin;
    for (int i = 0; i < fileContent.size(); ++i)
    {
        char ch = fileContent[i];
        switch (state)
        {
        case State::SearchForDataBegin:
            if (ch == ':') { i++; state = State::ReadDataBytes; }
            break;

        case State::ReadDataBytes:
            for (int b = 0; b < 16; ++b)
            {
                unsigned char byte = 0x00;
                byte |= getNibble(fileContent[i++]) << 4;
                byte |= getNibble(fileContent[i++]);
                placeByteOnImage(result, byte, byteNumber);
                byteNumber++;
                i++;
            }
            i--;
            state = State::SearchForLineEnd;
            break;

        case State::SearchForLineEnd:
            if (ch == '\n') { state = State::SearchForDataBegin; }
            break;
        }
    }

    if (byteNumber == 128 * 64 / 8)
    {
        result.Save("screenshot.png");
    }   
}
