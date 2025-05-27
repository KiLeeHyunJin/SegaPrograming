#include "File.h"
#include "../include/GameLib/GameLib.h"
#include <fstream>

namespace GameLib
{
    File::File(const char* fileName) : mSize(0), mData(0) 
    {
        std::ifstream in(fileName, std::ifstream::binary);
        if (in) {
            in.seekg(0, std::ifstream::end);
            mSize = static_cast<int>(in.tellg());

            in.seekg(0, std::ifstream::beg);
            mData = new char[mSize];
            
            in.read(mData, mSize);
        }
        else {
            HALT("can't open file."); //ívñΩèùÇæÇ©ÇÁ?ÇﬂÇÈÅB
        }
    }

    unsigned File::getUnsigned(int position) const
    {
        const unsigned char* up = reinterpret_cast<const unsigned char*>(mData);
        unsigned    uiData  = up[position];
                    uiData |= up[position + 1] << 8;
                    uiData |= up[position + 2] << 16;
                    uiData |= up[position + 3] << 24;
        return uiData;
    }
}