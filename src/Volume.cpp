#include "Volume.h"
#include <glm.hpp>
#include <gtx/string_cast.hpp>
#include <gtc/matrix_transform.hpp>
#include <math.h>

//-------------------------------------------------------------------------------------------------
// Voxel
//-------------------------------------------------------------------------------------------------

Voxel::Voxel()
{
	setValue(0.0f);
}

Voxel::Voxel(const Voxel &other)
{
	setValue(other.getValue());
}

Voxel::Voxel(const float value)
{
	setValue(value);
}

Voxel::~Voxel()
{
}

void Voxel::setValue(const float value)
{
	m_Value = value;
}

const float Voxel::getValue() const
{
	return m_Value;
};

const bool Voxel::operator==(const Voxel &other) const
{
	return (getValue() == other.getValue());
};

const bool Voxel::operator!=(const Voxel &other) const
{
	return !(*this == other);
};

const bool Voxel::operator>(const Voxel &other) const
{
	return getValue() > other.getValue();
};

const bool Voxel::operator>=(const Voxel &other) const
{
	return getValue() >= other.getValue();
};

const bool Voxel::operator<(const Voxel &other) const
{
	return getValue() < other.getValue();
};

const bool Voxel::operator<=(const Voxel &other) const
{
	return getValue() <= other.getValue();
};

const Voxel& Voxel::operator+=(const Voxel &other)
{
	m_Value += other.m_Value;
	return *this;
};

const Voxel& Voxel::operator-=(const Voxel &other)
{
	m_Value -= other.m_Value;
	return *this;
};

const Voxel& Voxel::operator*=(const float &value)
{
	m_Value *= value;
	return *this;
};

const Voxel& Voxel::operator/=(const float &value)
{
	m_Value /= value;
	return *this;
};

const Voxel Voxel::operator+(const Voxel &other) const
{
	Voxel voxNew = *this;
	voxNew += other;
	return voxNew;
};

const Voxel Voxel::operator-(const Voxel &other) const
{
	Voxel voxNew = *this;
	voxNew -= other;
	return voxNew;
};

const Voxel Voxel::operator*(const float &value) const
{
	Voxel voxNew = *this;
	voxNew *= value;
	return voxNew;
};

const Voxel Voxel::operator/(const float &value) const
{
	Voxel voxNew = *this;
	voxNew /= value;
	return voxNew;
};


//-------------------------------------------------------------------------------------------------
// Volume
//-------------------------------------------------------------------------------------------------

Volume::Volume()
	: m_Width(1), m_Height(1), m_Depth(1), m_Size(0), m_Voxels(1)
{
}

Volume::~Volume()
{
}

const Voxel& Volume::voxel(const int x, const int y, const int z) const
{
	return m_Voxels[x + y*m_Width + z*m_Width*m_Height];
}

const Voxel& Volume::voxel(const int i) const
{
	return m_Voxels[i];
}

const Voxel* Volume::voxels() const
{
	return &(m_Voxels.front());
};

const int Volume::width() const
{
	return m_Width;
};

const int Volume::height() const
{
	return m_Height;
};

const int Volume::depth() const
{
	return m_Depth;
};

const int Volume::size() const
{
	return m_Size;
};


//-------------------------------------------------------------------------------------------------
// Volume File Loader
//-------------------------------------------------------------------------------------------------

bool Volume::loadFromFile(QString filename, QProgressBar* progressBar)
{
	// load file
	FILE *fp = NULL;
	fopen_s(&fp, filename.toStdString().c_str(), "rb");
	if (!fp)
	{
		std::cerr << "+ Error loading file: " << filename.toStdString() << std::endl;
		return false;
	}

	// progress bar

	progressBar->setRange(0, m_Size + 10);
	progressBar->setValue(0);


	// read header and set volume dimensions

	unsigned short uWidth, uHeight, uDepth;
	fread(&uWidth, sizeof(unsigned short), 1, fp);
	fread(&uHeight, sizeof(unsigned short), 1, fp);
	fread(&uDepth, sizeof(unsigned short), 1, fp);
	
	m_Width = int(uWidth);
	m_Height = int(uHeight);
	m_Depth = int(uDepth);

	// check dataset dimensions
	if (
			m_Width <= 0 || m_Width > 1000 ||
			m_Height <= 0 || m_Height > 1000 ||
			m_Depth <= 0 || m_Depth > 1000)
	{
		std::cerr << "+ Error loading file: " << filename.toStdString() << std::endl;
		std::cerr << "Unvalid dimensions - probably loaded .dat flow file instead of .gri file?" << std::endl;
		return false;
	}

	// compute dimensions
	int slice = m_Width * m_Height;
	m_Size = slice * m_Depth;
	int test = INT_MAX;
	m_Voxels.resize(m_Size);

	// set sample steps
	m_samples = round(m_Depth / 5);
	//set alpha opacitie
	m_transparency = 0.2f;


	// read volume data

	// read into vector before writing data into volume to speed up process
	std::vector<unsigned short> vecData;
	vecData.resize(m_Size);
	fread((void*)&(vecData.front()), sizeof(unsigned short), m_Size, fp);
	fclose(fp);

	progressBar->setValue(10);

	// store volume data

	for (int i = 0; i < m_Size; i++)
	{
		// data is converted to FLOAT values in an interval of [0.0 .. 1.0];
		// uses 4095.0f to normalize the data, because only 12bit are used for the
		// data values, and then 4095.0f is the maximum possible value
		const float value = std::fmax(0.0f, std::fmin(1.0f, (float(vecData[i]) / 4095.0f)));
		m_Voxels[i] = Voxel(value);
		
		progressBar->setValue(10 + i);
	}

	progressBar->setValue(0);

	std::cout << "Loaded VOLUME with dimensions " << m_Width << " x " << m_Height << " x " << m_Depth << std::endl;

	return true;
}


std::vector<float> Volume::rayCasting()
{
	m_factor = 1;

	std::vector<float> out;
	out.resize(m_Width * m_Height);

	for (int x = 0; x < m_Width; x++)
	{
		for (int y = 0; y < m_Height; y++)
		{
			// intensity
			Voxel& value = Voxel();

			// Compositions Faktor
			float alpha = 0.0;

			for (int z = 0; z < m_Depth; z += m_samples)
			{
				// Maximum-Intensity-Projektion
				if (mip)
				{
					if (this->voxel(x, y, z).operator>(value))
					{
						value = this->voxel(x, y, z);
					}
				}

				// First-Hit Renderingtechnik
				if (firstHit)
				{
					if (this->voxel(x, y, z).getValue() > 0.f)
					{
						value = this->voxel(x, y, z);
						break;
					}
				}

				// Average  rendering
				if (average)
				{
					value = value.operator+(this->voxel(x, y, z));
				}

				// Alpha-Compositing
				else
				{
					alpha += this->voxel(x, y, z).getValue() * ((1.0 - z / m_Depth) * m_transparency);
					
					if (alpha > 1.0) {
						alpha = 1.0;
						break;
					}
				}
			}

			if (average)		  out[y*m_Width + x] = (value.operator/=(m_Depth / m_samples)).getValue();
			if (alphaCompositing) out[y*m_Width + x] = alpha;
			else				  out[y*m_Width + x] = value.getValue();
		}
	}
	return out;
}

std::vector<float> Volume::rayCasting2()
{
	float pixel_width = m_Width * m_factor;
	float pixel_height = m_Height * m_factor;

	std::vector<float> out;
	out.resize(pixel_width * pixel_height);

	for (int x = 0; x < pixel_width; x++)
	{
		for (int y = 0; y < pixel_height; y++)
		{
			// intensity
			Voxel& value = Voxel();

			// Compositions Faktor
			float alpha = 0.0;

			// current voxel
			Voxel& voxel = Voxel();

			// position in volume
			float p_x = (float)x / (float)m_factor;
			float p_y = (float)y / (float)m_factor;


			for (int z = 0; z < m_Depth; z += m_samples)
			{
				// interpolated voxel
				if (((float)x / (float)m_factor) != (int)((float)x / (float)m_factor))
				{
					voxel = getInterpolatedVoxel(p_x, p_y, z);
				}
				else
				{
					voxel = this->voxel((int)p_x, (int)p_y, z);
				}

				// Maximum-Intensity-Projektion
				if (mip)
				{
					if (voxel.operator>(value))
					{
						value = voxel;
					}
				}

				// First-Hit Renderingtechnik
				if (firstHit)
				{
					if (voxel.getValue() > 0.f)
					{
						value = voxel;
						break;
					}
				}

				// Average  rendering
				if (average)
				{
					value = value.operator+(voxel);
				}

				// Alpha-Compositing
				else
				{
					alpha += voxel.getValue() * ((1.0 - z / m_Depth) * m_transparency);

					if (alpha > 1.0) {
						alpha = 1.0;
						break;
					}
				}
			}

			if (average)		  out[y * pixel_width + x] = (value.operator/=(m_Depth / m_samples)).getValue();
			if (alphaCompositing) out[y * pixel_width + x] = alpha;
			else				  out[y * pixel_width + x] = value.getValue();
		}
	}
	return out;
}
Voxel Volume::getInterpolatedVoxel(float x, float y, int z)
{
	int x0 = floor(x);
	int x1 = ceil(x);
	int y0 = floor(y);
	int y1 = ceil(y);

	if (x1 == m_Width) x1 = m_Width - 1;
	if (y1 == m_Height) y1 = m_Height - 1;

	float v1 = this->voxel(x0, y0, z).getValue();
	float v2 = this->voxel(x1, y0, z).getValue();
	float v3 = this->voxel(x1, y1, z).getValue();
	float v4 = this->voxel(x0, y1, z).getValue();

	return Voxel((v1 + v2 + v3  + v4) / 4);
}

void Volume::setSampleDistance(int distance)
{
	m_samples = distance;
}

void Volume::setScaleFactor(int factor)
{
	m_factor = factor;
}

void Volume::setTransparency(float alpha)
{
	m_transparency = alpha;
}

void Volume::setMip()
{
	mip = true;
	firstHit = false;
	alphaCompositing = false;
	average = false;
}

void Volume::setFirstHit()
{
	firstHit = true;
	mip = false;
	alphaCompositing = false;
	average = false;
}

void Volume::setAlphaCompositing()
{
	alphaCompositing = true;
	mip = false;
	firstHit = false;
	average = false;
}

void Volume::setAverage()
{
	average = true;
	alphaCompositing = false;
	mip = false;
	firstHit = false;
}

int Volume::getSampleDistance()
{
	return m_samples;
}

int Volume::getScaleFactor()
{
	return m_factor;
}