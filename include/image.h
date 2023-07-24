//
//  image.h
//  wee
//
//  Created by George Watson on 24/07/2023.
//

#ifndef image_h
#define image_h
#include "bool.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <setjmp.h>
#include <errno.h>

int RGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
int RGB(unsigned char r, unsigned char g, unsigned char b);
int RGBA1(unsigned char c, unsigned char a);
int RGB1(unsigned char c);
unsigned char Rgba(int c);
unsigned char rGba(int c);
unsigned char rgBa(int c);
unsigned char rgbA(int c);
int rGBA(int c, unsigned char r);
int RgBA(int c, unsigned char g);
int RGbA(int c, unsigned char b);
int RGBa(int c, unsigned char a);

typedef struct {
    int *buf, w, h;
} Image;

bool InitImage(Image *img, unsigned int w, unsigned int h);
void DestroyImage(Image *img);

void FillImage(Image *img, int col);
void FloodImage(Image *img, int x, int y, int col);
void ClearImage(Image *img);
void BSet(Image *img, int x, int y, int col);
void PSet(Image *img, int x, int y, int col);
int PGet(Image *img, int x, int y);
bool PasteImage(Image *dst, Image *src, int x, int y);
bool PasteImageClip(Image *dst, Image *src, int x, int y, int rx, int ry, int rw, int rh);
bool CopyImage(Image *a, Image *b);
void PassthruImage(Image *img, int(*fn)(int x, int y, int col));
bool ScaleImage(Image *a, int nw, int nh, Image *img);
bool RotateImage(Image *a, float angle, Image *b);
void DrawLine(Image *img, int x0, int y0, int x1, int y1, int col);
void DrawCircle(Image *img, int xc, int yc, int r, int col, bool fill);
void DrawRect(Image *img, int x, int y, int w, int h, int col, bool fill);
void DrawTri(Image *img, int x0, int y0, int x1, int y1, int x2, int y2, int col, bool fill);

bool LoadImage(Image *out, const char *path);
bool LoadImageMemory(Image *out, const void *data, size_t length);
bool SaveImage(Image *img, const char *path);

#if defined(__cplusplus)
}
#endif
#endif /* image_h */
