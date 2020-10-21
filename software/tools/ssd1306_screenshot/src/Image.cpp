#include "Image.h"
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////

struct PixelError
{
	float r = 0, g = 0, b = 0, a = 0;

	PixelError operator*(float scalar) const
	{
		return
		{
			r * scalar,
			g * scalar,
			b * scalar,
			a * scalar 
		};
	}

	operator bool() const
	{
		return (int)(r) || (int)(g) || (int)(b) || (int)(a);
	}
};

png::byte ClampChannel(float channel)
{
	return png::byte(channel < 0 ? 0 : (channel > 255 ? 255 : channel + 0.5f));
}

PixelError operator-(const Image::Pixel& a, const Image::Pixel& b)
{
	return
	{ 
		float(a.m_r - b.m_r),
		float(a.m_g - b.m_g),
		float(a.m_b - b.m_b),
		float(a.m_a - b.m_a) 
	};
}

Image::Pixel operator+(const Image::Pixel& pixel, const PixelError& error)
{
	return
	{
		ClampChannel(pixel.m_r + error.r),
		ClampChannel(pixel.m_g + error.g),
		ClampChannel(pixel.m_b + error.b),
		ClampChannel(pixel.m_a + error.a)
	};
}

png::byte QuantizeChannel(png::byte channel, int bits)
{
	if (bits < 8)
	{
		int maximum = (1 << bits) - 1;
		int current = int(0.5f + maximum * (float)channel / 255.f);
		return png::byte(0.5f + current * 255.f / (float)maximum);
	}
	return channel;
}

Image::Pixel QuantizePixel(const Image::Pixel& pixel, const Image::Format & format)
{
	return
	{
		QuantizeChannel(pixel.m_r, format.m_bitsR),
		QuantizeChannel(pixel.m_g, format.m_bitsG),
		QuantizeChannel(pixel.m_b, format.m_bitsB),
		QuantizeChannel(pixel.m_a, format.m_bitsA)
	};
}

////////////////////////////////////////////////////////////////////////////////

Image::Format::Dithering Image::DitheringNone
{
};
Image::Format::Dithering Image::DitheringFloydSteinberg
{
	+1, 0, 7.f / 16,
	+1, 1, 1.f / 16,
	 0, 1, 5.f / 16,
	-1, 1, 3.f / 16
};
Image::Format::Dithering Image::DitheringJarvisJudiceAndNinke
{
	+1, 0, 7.f / 48,
	+1, 1, 5.f / 48,
	 0, 1, 7.f / 48,
	-1, 1, 5.f / 48,
	+2, 0, 5.f / 48,
	+2, 1, 3.f / 48,
	+2, 2, 1.f / 48,
	+1, 2, 3.f / 48,
	 0, 2, 5.f / 48,
	-1, 2, 3.f / 48,
	-2, 2, 1.f / 48,
	-2, 1, 3.f / 48
};
Image::Format::Dithering Image::DitheringStucki
{
	+1, 0, 8.f / 42,
	+1, 1, 4.f / 42,
	 0, 1, 8.f / 42,
	-1, 1, 4.f / 42,
	+2, 0, 4.f / 42,
	+2, 1, 2.f / 42,
	+2, 2, 1.f / 42,
	+1, 2, 2.f / 42,
	 0, 2, 4.f / 42,
	-1, 2, 2.f / 42,
	-2, 2, 1.f / 42,
	-2, 1, 2.f / 42
};
Image::Format::Dithering Image::DitheringAtkinson
{
	+1, 0, 1.f / 8,
	+1, 1, 1.f / 8,
	 0, 1, 1.f / 8,
	-1, 1, 1.f / 8,
	+2, 0, 1.f / 8,
	 0, 2, 1.f / 8
};
Image::Format::Dithering Image::DitheringBurkes
{
	+1, 0, 8.f / 32,
	+1, 1, 4.f / 32,
	 0, 1, 8.f / 32,
	-1, 1, 4.f / 32,
	+2, 0, 4.f / 32,
	+2, 1, 2.f / 32,
	-2, 1, 2.f / 32
};
Image::Format::Dithering Image::DitheringSierra
{
	+1, 0, 5.f / 32,
	+1, 1, 4.f / 32,
	 0, 1, 5.f / 32,
	-1, 1, 4.f / 32,
	+2, 0, 3.f / 32,
	+2, 1, 2.f / 32,
	+1, 2, 2.f / 32,
	 0, 2, 3.f / 32,
	-1, 2, 2.f / 32,
	-2, 1, 2.f / 32
};
Image::Format::Dithering Image::DitheringTwoRowSierra
{
	+1, 0, 4.f / 16,
	+1, 1, 2.f / 16,
	 0, 1, 3.f / 16,
	-1, 1, 2.f / 16,
	+2, 0, 3.f / 16,
	+2, 1, 1.f / 16,
	-2, 1, 1.f / 16
};
Image::Format::Dithering Image::DitheringSierraLite
{
	+1, 0, 2.f / 4,
	 0, 1, 1.f / 4,
	-1, 1, 1.f / 4
};

Image::Format Image::Format1111(1, 1, 1, 1, DitheringFloydSteinberg, DitheringFloydSteinberg);
Image::Format Image::Format3320(3, 3, 2, 0, DitheringFloydSteinberg, DitheringFloydSteinberg);
Image::Format Image::Format4444(4, 4, 4, 4, DitheringFloydSteinberg, DitheringFloydSteinberg);
Image::Format Image::Format5551(5, 5, 5, 1, DitheringFloydSteinberg, DitheringFloydSteinberg);
Image::Format Image::Format5650(5, 6, 5, 0, DitheringFloydSteinberg, DitheringFloydSteinberg);
Image::Format Image::Format8880(8, 8, 8, 0, DitheringFloydSteinberg, DitheringFloydSteinberg);
Image::Format Image::Format8888(8, 8, 8, 8, DitheringFloydSteinberg, DitheringFloydSteinberg);

Image::Format::Format(int bitsR, int bitsG, int bitsB, int bitsA, Dithering colorDithering, Dithering alphaDithering)
	: m_bitsR(bitsR), m_bitsG(bitsG), m_bitsB(bitsB), m_bitsA(bitsA)
	, m_colorDithering(colorDithering), m_alphaDithering(alphaDithering)
{
}

Image::Format Image::Format::WithColorDithering(Dithering dithering)
{
	return Format(m_bitsR, m_bitsG, m_bitsB, m_bitsA, dithering, m_alphaDithering);
}

Image::Format Image::Format::WithAlphaDithering(Dithering dithering)
{
	return Format(m_bitsR, m_bitsG, m_bitsB, m_bitsA, m_colorDithering, dithering);
}

bool Image::Format::HasAlpha() const
{
	return m_bitsA > 0;
}

bool Image::Format::HasColorDithering() const
{
	return !m_colorDithering.empty() && (m_bitsR + m_bitsG + m_bitsB) < 24;
}

bool Image::Format::HasAlphaDithering() const
{
	return !m_alphaDithering.empty() && HasAlpha() && m_bitsA < 8;;
}

Image::Pixel::Pixel()
	: Pixel(0, 0, 0, 0)
{
}

Image::Pixel::Pixel(const png::rgb_pixel & pixel)
	: Pixel(pixel.red, pixel.green, pixel.blue)
{
}

Image::Pixel::Pixel(const png::rgba_pixel & pixel)
	: Pixel(pixel.red, pixel.green, pixel.blue, pixel.alpha)
{
}

Image::Pixel::Pixel(Channel r, Channel g, Channel b)
	: Pixel(r, g, b, 0xFF)
{
}

Image::Pixel::Pixel(Channel r, Channel g, Channel b, Channel a)
	: m_r(r), m_g(g), m_b(b), m_a(a)
{
}

Image::Pixel::operator png::rgb_pixel() const
{
	return png::rgb_pixel(m_r, m_g, m_b);
}

Image::Pixel::operator png::rgba_pixel() const
{
	return png::rgba_pixel(m_r, m_g, m_b, m_a);
}

size_t Image::Pixel::hash() const
{
	return m_a << 24 | m_b << 16 | m_g << 8 | m_r;
}

bool Image::Pixel::operator<(const Pixel& other) const
{
	return hash() < other.hash();
}

////////////////////////////////////////////////////////////////////////////////

Image::Image(const std::string& filename, const Format& format)
	: m_imageRGB(nullptr)
	, m_imageRGBA(nullptr)
{
	if (format.HasAlpha())
	{
		m_imageRGBA = new png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>>(filename);
	}
	else
	{
		m_imageRGB = new png::image<png::rgb_pixel, png::solid_pixel_buffer<png::rgb_pixel>>(filename);
	}
	Quantize(format);
}

Image::Image(size_t w, size_t h, bool hasAlpha)
	: m_imageRGB(nullptr)
	, m_imageRGBA(nullptr)
{
	if (hasAlpha)
	{
		m_imageRGBA = new png::image<png::rgba_pixel, png::solid_pixel_buffer<png::rgba_pixel>>(w, h);
	}
	else
	{
		m_imageRGB = new png::image<png::rgb_pixel, png::solid_pixel_buffer<png::rgb_pixel>>(w, h);
	}
}

Image::~Image()
{
	delete m_imageRGB;
	delete m_imageRGBA;
}

size_t Image::GetW() const
{
	if (m_imageRGB)
	{
		return m_imageRGB->get_width();
	}
	else if (m_imageRGBA)
	{
		return m_imageRGBA->get_width();
	}
	return 0;
}

size_t Image::GetH() const
{
	if (m_imageRGB)
	{
		return m_imageRGB->get_height();
	}
	else if (m_imageRGBA)
	{
		return m_imageRGBA->get_height();
	}
	return 0;
}

png::byte* Image::GetBytes()
{
	if (m_imageRGB)
	{
		return const_cast<png::byte*>(m_imageRGB->get_pixbuf().get_bytes().data());
	}
	else if (m_imageRGBA)
	{
		return const_cast<png::byte*>(m_imageRGBA->get_pixbuf().get_bytes().data());
	}
	return nullptr;
}

size_t Image::GetChannels() const
{
	return GetStride() / GetW();
}

size_t Image::GetStride() const
{
	if (m_imageRGB)
	{
		return m_imageRGB->get_pixbuf().get_bytes().size() / m_imageRGB->get_height();
	}
	else if (m_imageRGBA)
	{
		return m_imageRGBA->get_pixbuf().get_bytes().size() / m_imageRGBA->get_height();
	}
	return 0;
}

Image::Pixel Image::GetPixel(int x, int y) const
{
	if (m_imageRGB)
	{
		return m_imageRGB->get_pixel(x, y);
	}
	else if (m_imageRGBA)
	{
		return m_imageRGBA->get_pixel(x, y);
	}
	return Pixel();
}

void Image::SetPixel(int x, int y, const Pixel & pixel)
{
	if (m_imageRGB)
	{
		m_imageRGB->set_pixel(x, y, pixel);
	}
	else if (m_imageRGBA)
	{
		m_imageRGBA->set_pixel(x, y, pixel);
	}
}

void Image::CopyPixels(const Image& image, int sx, int sy, int dx, int dy, size_t w, size_t h)
{
	for (int y = 0; y < int(h); ++y)
	{
		for (int x = 0; x < int(w); ++x)
		{
			SetPixel(dx + x, dy + y, image.GetPixel(sx + x, sy + y));
		}
	}
}

void Image::Quantize(const Format& format)
{
	Quantize(format, 0, 0, GetW(), GetH());
}

void Image::Quantize(const Format& format, int x, int y, size_t w, size_t h)
{
	const int x0 = x, y0 = y, x1 = x + int(w) - 1, y1 = y + int(h) - 1;

	const bool colorDithering = format.HasColorDithering();
	const bool alphaDithering = format.HasAlphaDithering();

	auto DistributeError = [&](int x, int y, const PixelError& error, const float ditherRule[], int dx)
	{
		int xx = x + int(ditherRule[0]) * dx;
		int yy = y + int(ditherRule[1]);

		if (yy >= y0 && yy <= y1 && xx >= x0 && xx <= x1)
		{
			Pixel oldPixel = GetPixel(xx, yy);
			Pixel newPixel = oldPixel + (error * ditherRule[2]);
			SetPixel(xx, yy, newPixel);
		}
	};

	for (int y = y0, x = x0, dx = 1; y <= y1; ++y, dx = -dx, x += dx)
	{
		while (x >= x0 && x <= x1)
		{
			Pixel oldPixel = GetPixel(x, y);
			Pixel newPixel = QuantizePixel(oldPixel, format);
			SetPixel(x, y, newPixel);

			PixelError error = oldPixel - newPixel;
			PixelError colorError{ error.r, error.g, error.b, 0 };
			PixelError alphaError{ 0, 0, 0, error.a };

			if (colorDithering && colorError)
			{
				for (size_t i = 0, c = format.m_colorDithering.size(); i < c; i += 3)
				{
					DistributeError(x, y, colorError, &format.m_colorDithering[i], dx);
				}
			}

			if (alphaDithering && alphaError)
			{
				for (size_t i = 0, c = format.m_alphaDithering.size(); i < c; i += 3)
				{
					DistributeError(x, y, alphaError, &format.m_alphaDithering[i], dx);
				}
			}

			x += dx;
		}
	}
}

void Image::Save(const std::string & filename) const
{
	if (IndexedImagePtr indexedImagePtr = GetIndexedImage())
	{
		indexedImagePtr->write(filename);
	}
	else if (m_imageRGB)
	{
		m_imageRGB->write(filename);
	}
	else if (m_imageRGBA)
	{
		m_imageRGBA->write(filename);
	}
}

void Image::Save(std::ostream & stream) const
{
	if (IndexedImagePtr indexedImagePtr = GetIndexedImage())
	{
		indexedImagePtr->write_stream(stream);
	}
	else if (m_imageRGB)
	{
		m_imageRGB->write_stream(stream);
	}
	else if (m_imageRGBA)
	{
		m_imageRGBA->write_stream(stream);
	}
}

Image::IndexedImagePtr Image::GetIndexedImage() const
{
	size_t w = GetW(), h = GetH();
	IndexedImagePtr imagePtr = std::make_shared<IndexedImage>(w, h);

	png::palette palColor;
	png::tRNS palAlpha;

	std::unordered_map<size_t, size_t> map;
	for (size_t y = 0; y < h; ++y)
	{
		for (size_t x = 0; x < w; ++x)
		{
			size_t newColorIndex = palColor.size();
			if (newColorIndex > 255) return nullptr;
			
			Pixel pixel = GetPixel(x, y);
			auto result = map.emplace(pixel.hash(), newColorIndex);

			if (result.second)
			{
				palColor.push_back(png::color(pixel.m_r, pixel.m_g, pixel.m_b));
				palAlpha.push_back(png::byte(pixel.m_a));
			}

			size_t colorIndex = (*result.first).second;
			imagePtr->set_pixel(x, y, png::byte(colorIndex));
		}
	}

	imagePtr->set_palette(palColor);
	imagePtr->set_tRNS(palAlpha);
	return imagePtr;
}
