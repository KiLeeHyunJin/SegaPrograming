#pragma once

namespace GameLib
{


    class Image
    {
    public:
        Image(const char* filename);
        ~Image()                { delete[] mData; }
        int width()     const   { return mWidth; }
        int height()    const   { return mHeight; }
        void draw(
            int dstX,
            int dstY,
            int srcX,
            int srcY,
            int width,
            int height) const;
        void draw()     const   { draw(0, 0, 0, 0, mWidth, mHeight);   }

    private:

        int mWidth;
        int mHeight;
        unsigned* mData;
    };
}



