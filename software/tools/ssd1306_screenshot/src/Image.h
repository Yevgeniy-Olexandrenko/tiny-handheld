#pragma once

#include "png.hpp"

class Image
{
public:
	struct Format
	{
		using Dithering = std::vector<float>;

		const int  m_bitsR;
		const int  m_bitsG;
		const int  m_bitsB;
		const int  m_bitsA;
		const Dithering m_colorDithering;
		const Dithering m_alphaDithering;

		Format(int bitsR, int bitsG, int bitsB, int bitsA, Dithering colorDithering, Dithering alphaDithering);

		Format WithColorDithering(Dithering dithering);
		Format WithAlphaDithering(Dithering dithering);

		bool HasAlpha() const;
		bool HasColorDithering() const;
		bool HasAlphaDithering() const;
	};

	static Format::Dithering DitheringNone;
	static Format::Dithering DitheringFloydSteinberg;
	static Format::Dithering DitheringJarvisJudiceAndNinke;
	static Format::Dithering DitheringStucki;
	static Format::Dithering DitheringAtkinson;
	static Format::Dithering DitheringBurkes;
	static Format::Dithering DitheringSierra;
	static Format::Dithering DitheringTwoRowSierra;
	static Format::Dithering DitheringSierraLite;

	static Format Format1111;
	static Format Format3320;
	static Format Format4444;
	static Format Format5551;
	static Format Format5650;
	static Format Format8880;
	static Format Format8888;

	struct Pixel
	{
		using Channel = png::byte;

		Channel m_r, m_g, m_b, m_a;

		Pixel();
		Pixel(const png::rgb_pixel & pixel);
		Pixel(const png::rgba_pixel & pixel);
		Pixel(Channel r, Channel g, Channel b);
		Pixel(Channel r, Channel g, Channel b, Channel a);

		operator png::rgb_pixel() const;
		operator png::rgba_pixel() const;

		size_t hash() const;
		bool operator<(const Pixel& other) const;
		
	};

public:
	Image(const std::string& filename, const Format & format);
	Image(size_t w, size_t h, bool hasAlpha);
	~Image();

	size_t GetW() const;
	size_t GetH() const;

	png::byte * GetBytes();
	size_t GetChannels() const;
	size_t GetStride() const;

	Pixel GetPixel(int x, int y) const;
	void  SetPixel(int x, int y, const Pixel & pixel);
	void  CopyPixels(const Image& image, int sx, int sy, int dx, int dy, size_t w, size_t h);

	void Quantize(const Format& format);
	void Quantize(const Format& format, int x, int y, size_t w, size_t h);

	void Save(const std::string & filename) const;
	void Save(std::ostream & stream) const;

private:
	using IndexedImage = png::image<png::index_pixel>;
	using IndexedImagePtr = std::shared_ptr<IndexedImage>;
	IndexedImagePtr GetIndexedImage() const;

private:
	png::image<png::rgb_pixel, png::solid_pixel_buffer<png::rgb_pixel>> * m_imageRGB;
	png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>> * m_imageRGBA;
};
