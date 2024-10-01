#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1310) /*Visual Studio: A few warning types are not desired here.*/
#pragma warning(disable : 4996)             /*VS does not like fopen, but fopen_s is not standard C so unusable here*/
#endif                                      /*_MSC_VER */

unsigned long byteswap_ulong(unsigned long i)
{
    unsigned int j;
    j = (i << 24);
    j += (i << 8) & 0x00FF0000;
    j += (i >> 8) & 0x0000FF00;
    j += (i >> 24);
    return j;
}

inline int Abs(int x)
{
    return (x ^ (x >> 31)) - (x >> 31);
}

unsigned short byteswap_ushort(unsigned short i)
{
    unsigned short j;
    j = (i << 8);
    j += (i >> 8);
    return j;
}

// Get Image size WITHOUT loading image into memory
// ref: http://stackoverflow.com/questions/15800704/python-get-image-size-without-loading-image-into-memory/

bool get_image_size_without_decode_image(const char *file_path, int *width, int *height, int *file_size)
{
    bool has_image_size = false;
    *height = -1;
    *width = -1;
    *file_size = -1;
    FILE *fp = fopen(file_path, "rb");
    if (fp == NULL)
        return has_image_size;
    struct stat st;
    char sigBuf[26];
    if (fstat(fileno(fp), &st) < 0)
    {
        fclose(fp);
        return has_image_size;
    }
    else
    {
        *file_size = st.st_size;
    }
    if (fread(&sigBuf, 26, 1, fp) < 1)
    {
        fclose(fp);
        return has_image_size;
    }
    char *png_signature = "\211PNG\r\n\032\n";
    char *ihdr_signature = "IHDR";
    char *gif87_signature = "GIF87a";
    char *gif89_signature = "GIF89a";
    char *jpeg_signature = "\377\330";
    char *bmp_signature = "BM";
    if ((*file_size >= 10) && (memcmp(sigBuf, gif87_signature, strlen(gif87_signature)) == 0 || memcmp(sigBuf, gif89_signature, strlen(gif89_signature)) == 0))
    {
        // image type: gif
        unsigned short *size_info = (unsigned short *)(sigBuf + 6);
        *width = size_info[0];
        *height = size_info[1];
        has_image_size = true;
    }
    else if ((*file_size >= 24) && (memcmp(sigBuf, png_signature, strlen(png_signature)) == 0 && memcmp(sigBuf + 12, ihdr_signature, strlen(ihdr_signature)) == 0))
    {
        // image type:   png
        unsigned long *size_info = (unsigned long *)(sigBuf + 16);
        *width = byteswap_ulong(size_info[0]);
        *height = byteswap_ulong(size_info[1]);
        has_image_size = true;
    }
    else if ((*file_size >= 16) && (memcmp(sigBuf, png_signature, strlen(png_signature)) == 0))
    {
        // image type: old png
        unsigned long *size_info = (unsigned long *)(sigBuf + 8);
        *width = byteswap_ulong(size_info[0]);
        *height = byteswap_ulong(size_info[1]);
        has_image_size = true;
    }
    else if ((*file_size >= 2) && (memcmp(sigBuf, jpeg_signature, strlen(jpeg_signature)) == 0))
    {
        // image type: jpeg
        fseek(fp, 0, SEEK_SET);
        char b = 0;
        fread(&sigBuf, 2, 1, fp);
        fread(&b, 1, 1, fp);
        int w = -1;
        int h = -1;
        while (b && ((unsigned char)b & 0xff) != 0xDA)
        {
            while (((unsigned char)b & 0xff) != 0xFF)
            {
                fread(&b, 1, 1, fp);
            }
            while (((unsigned char)b & 0xff) == 0xFF)
            {
                fread(&b, 1, 1, fp);
            }
            if (((unsigned char)b & 0xff) >= 0xC0 && ((unsigned char)b & 0xff) <= 0xC3)
            {
                fread(&sigBuf, 3, 1, fp);
                fread(&sigBuf, 4, 1, fp);
                unsigned short *size_info = (unsigned short *)(sigBuf);
                h = byteswap_ushort(size_info[0]);
                w = byteswap_ushort(size_info[1]);
            }
            else
            {
                unsigned short chunk_size = 0;
                fread(&chunk_size, 2, 1, fp);
                if (fseek(fp, byteswap_ushort(chunk_size) - 2, SEEK_CUR) != 0)
                    break;
            }
            fread(&b, 1, 1, fp);
        }
        if (w != -1 && h != -1)
        {
            *width = w;
            *height = h;
        }
        has_image_size = true;
    }
    else if ((*file_size >= 26) && (memcmp(sigBuf, bmp_signature, strlen(bmp_signature)) == 0))
    {
        // image type: bmp
        unsigned int header_size = (*(sigBuf + 14));
        if (header_size == 12)
        {
            unsigned short *size_info = (unsigned short *)(sigBuf + 18);
            *width = size_info[0];
            *height = size_info[1];
        }
        else if (header_size >= 40)
        {
            unsigned int *size_info = (unsigned int *)(sigBuf + 18);
            *width = size_info[0];
            *height = Abs((size_info[1]));
        }
        has_image_size = true;
    }
    else if (*file_size >= 2)
    {
        // image type: ico
        fseek(fp, 0, SEEK_SET);
        unsigned short format = -1;
        unsigned short reserved = -1;
        fread(&reserved, 2, 1, fp);
        fread(&format, 2, 1, fp);
        if (reserved == 0 && format == 1)
        {
            unsigned short num = -1;
            fread(&num, 2, 1, fp);
            if (num > 1)
            {
                printf("this is a muti-ico file.");
            }
            else
            {
                char w = 0, h = 0;
                fread(&w, 1, 1, fp);
                fread(&h, 1, 1, fp);
                *width = (int)((unsigned char)w & 0xff);
                *height = (int)((unsigned char)h & 0xff);
            }
        }
        has_image_size = true;
    }
    if (fp != NULL)
        fclose(fp);
    return has_image_size;
}
