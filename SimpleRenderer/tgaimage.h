#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>

#pragma pack(push,1)
struct TGA_Header {
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char  bitsperpixel;
	char  imagedescriptor;
};
#pragma pack(pop)

struct TGAColor {

	unsigned char val[4];
	unsigned char bytespp;

	TGAColor() : val (), bytespp(1)
	{
		for (int i = 0; i < 4; i++) val[i] = 0;
	}
	TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255)//Alpha channel is 255
	{
		val[0] = B;
		val[1] = G;
		val[2] = R;
		val[3] = A;
	}
	//SIngle value constructor
	TGAColor(unsigned char v) : val(), bytespp(1) {
		for (int i = 0; i < 4; i++) val[i] = 0;
		val[0] = v;
	}
	//copy constructor
	TGAColor(const TGAColor& c) {
		val[0] = c.val[0];
		val[1] = c.val[1];
		val[2] = c.val[2];
		val[3] = c.val[3];
		bytespp = c.bytespp;
	} 
	TGAColor(const unsigned char* p, unsigned char bpp) : val{ 0 }, bytespp{ bpp } {
		for (int i = 0; i < bpp; i++) {
			val[i] = p[i];
		}
		for (int i = bpp; i < 4; i++) {
			val[i] = 0;
		}
	}

	//copy assignment
	TGAColor& operator=(const TGAColor& c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val[0] = c.val[0];
			val[1] = c.val[1];
			val[2] = c.val[2];
			val[3] = c.val[3];
		}
		return *this;
	}
	TGAColor operator *(float intensity) const {
		TGAColor res = *this;
		intensity = (intensity > 1.f ? 1.f : (intensity < 0.f ? 0.f : intensity));
		for (int i = 0; i < 4; i++) res.val[i] = val[i] * intensity;
		return res;
	}

};

class TGAImage {

protected:
	unsigned char* data;
	int width;
	int height;
	//bytes per pixel
	int bytespp;

	bool load_rle_data(std::ifstream& in);
	bool unload_rle_data(std::ofstream& out);
public:
	enum Format {
		GRAYSCALE = 1, RGB = 3, RGBA = 4
	};

	TGAImage();
	TGAImage(int w, int h, int bpp);
	//copy contructor
	TGAImage(const TGAImage& img);
	bool read_tga_file(const char* filename);
	bool write_tga_file(const char* filename, bool rle = true);
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	TGAColor get(int x, int y);
	bool set(int x, int y, TGAColor c);
	~TGAImage();
	//assignment operator
	TGAImage& operator =(const TGAImage& img);
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char* buffer();
	void clear();

};


#endif