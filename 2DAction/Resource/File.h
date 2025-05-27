#pragma once

namespace GameLib
{
    class File
    {
    public :
        File(const char* fileName);
        ~File()                     { delete[] mData; mSize = 0; }
        int size() const            { return mSize;     }
        const char* data() const    { return mData;     }
        unsigned getUnsigned(int position) const;
    private:
        int mSize;
        char* mData;
    };

}