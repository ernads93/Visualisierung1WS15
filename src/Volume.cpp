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

	std::vector<float> out;
	out.resize(m_Width * m_Height);

	//sample rate
	float sample_step_size = 32.f;

	vec3 start, end, intersection_1, intersection_2;

	for (int i = 0; i < m_Width; i++){
		for (int j = 0; j < m_Height; j++){
			
			// start of ray
			start.x =			(m_Plane.x.x * j) + (m_Plane.y.x * i);
			start.y = m_Width + (m_Plane.x.y * j) + (m_Plane.y.y * i);
			start.z = m_Depth + (m_Plane.x.z * j) + (m_Plane.y.z * i);

			//end of ray
			end.x = start.x;
			end.y = start.y;
			end.z = start.z + m_Depth;

			//finds an intersection
			bool isIntersect = isIntersection(start, end, m_Plane.v, intersection_1, intersection_2);

			float maxIntensity = 0.f;

			if (isIntersect) {
				glm::vec3 front = glm::vec3(intersection_1.x, intersection_1.y, intersection_1.z);
				glm::vec3 back = glm::vec3(intersection_2.x, intersection_2.y, intersection_2.z);

				glm::vec3 direction = (back - front);
				direction = glm::vec3(direction.x*sample_step_size / direction.z, direction.y*sample_step_size / direction.z, sample_step_size);

				float value;

				//Maximum-Intensity-Projektion
				while (front.z < back.z){
					//get voxel value and compare
					value = m_Voxels[round(front.x) + m_Width*(round(front.y) + m_Depth*round(front.z))].getValue();
					if (value > maxIntensity) {
						maxIntensity = value;
					}
					//next
					front = front + direction;
				}
			}
			out[j*m_Width + i] = maxIntensity;
		}
	}
	return out;
}

bool Volume::isIntersection(vec3 point_1, vec3 point_2, vec3 v, vec3& intersection_1, vec3& intersection_2) {
	//TODO: implement

	//return false for those rays not in Bounding Box
	if (point_1.x < 0 && point_2.x < 0) {
		return false;
	}
	if (point_1.x > m_Width && point_2.x > m_Width) {
		return false;
	}
	if (point_1.y < 0 && point_2.y < 0)  {
		return false;
	}
	if (point_1.y > m_Height && point_2.y > m_Height) {
		return false;
	}
	if (point_1.z < 0 && point_2.z < 0) {
		return false;
	}
	if (point_1.z > m_Depth && point_2.z > m_Depth) {
		return false;
	}

	bool firstIntersectFound = false;
	Vector3 temp;

	// check if parralel
	//for X
	if (v.x == 0){
		if (v.y != 0 && v.z != 0){
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, 0)) {
				return true;
			}
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, m_Height)) {
				return true;
			}
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, 0)) {
				return true;
			}
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, m_Depth)) {
				return true;
			}
			return false;
		}
		else{
			if (v.y == 0){
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, 0)) {
					return true;
				}
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, m_Depth)) {
					return true;
				}
				return false;
			}
			if (v.z == 0){
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, 0)) {
					return true;
				}
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, m_Height)) {
					return true;
				}
				return false;
			}
		}
	}

	//for Y
	if (v.y == 0){
		if (v.x != 0 && v.z != 0){
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, 0)) {
				return true;
			}
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, m_Width)) {
				return true;
			}
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, 0)) {
				return true;
			}
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, m_Depth)) {
				return true;
			}
			return false;
		}
		else{
			if (v.x == 0){
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, 0)) {
					return true;
				}
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, m_Depth)) {
					return true;
				}
				return false;
			}
			if (v.z == 0){
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, 0)) {
					return true;
				}
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, m_Width)) {
					return true;
				}
				return false;
			}
		}
	}

	//for Z
	if (v.z == 0){
		if (v.x != 0 && v.y != 0){
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, 0)) return true;
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, m_Width)) return true;
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, 0)) return true;
			if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, m_Height)) return true;
			return false;
		}
		else{
			if (v.y == 0){
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, 0)) return true;
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, m_Width)) return true;
				return false;
			}
			if (v.x == 0){
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, 0)) return true;
				if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, m_Height)) return true;
				return false;
			}
		}
	}

	//not parallel
	if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, 0)) {
		return true;
	}
	if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, X, m_Width)) {
		return true;
	}
	if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, 0)) {
		return true;
	}
	if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Y, m_Height)) {
		return true;
	}
	if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, 0)) {
		return true;
	}
	if (searchIntersection(point_1, v, firstIntersectFound, intersection_1, intersection_2, Z, m_Depth)) {
		return true;
	}
	return false;
}

bool Volume::searchIntersection(vec3 point, vec3 v, bool& firstIntersectFound, vec3& intersection_1, vec3& intersection_2, Axis axis, float fixPoint) {

	vec3 temp;

	if (axis == X){
		float s = (fixPoint - point.x) / v.x;
		temp.x = fixPoint;
		temp.y = point.y + s * v.y;
		temp.z = point.z + s * v.z;
	}

	if (axis == Y){
		float s = (fixPoint - point.y) / v.y;
		temp.x = point.x + s * v.x;
		temp.y = fixPoint;
		temp.z = point.z + s * v.z;
	}

	if (axis == Z){
		float s = (fixPoint - point.z) / v.z;
		temp.x = point.x + s * v.x;
		temp.y = point.y + s * v.y;
		temp.z = fixPoint;
	}

	if (isInsideBB(temp)){
		if (!firstIntersectFound){
			intersection_1 = temp;
			firstIntersectFound = true;
			return false;
		}
		else{
			if (!(intersection_1.x == temp.x && intersection_1.y == temp.y && intersection_1.z == temp.z)){
				intersection_2 = temp;
				return true;
			}
		}
	}
	return false;
}

bool Volume::isInsideBB(vec3 point){
	if (point.x < 0 || point.x > m_Width || point.y < 0 || point.y > m_Height || point.z < 0 || point.z > m_Depth) {
		return false;
	}
	return true;
}

void Volume::initializePlane() {

	m_Plane.pivot.x = m_Width / 2;
	m_Plane.pivot.y = m_Height / 2;
	m_Plane.pivot.z = m_Depth / 2;

	m_Plane.p1.x = m_Plane.pivot.x - m_Width / 2;
	m_Plane.p1.y = m_Plane.pivot.y - m_Height / 2;
	m_Plane.p1.z = -1000;

	m_Plane.p2.x = m_Plane.pivot.x + m_Width / 2;
	m_Plane.p2.y = m_Plane.pivot.y - m_Height / 2;
	m_Plane.p2.z = -1000;

	m_Plane.p3.x = m_Plane.pivot.x + m_Width / 2;
	m_Plane.p3.y = m_Plane.pivot.y + m_Height / 2;
	m_Plane.p3.z = -1000;

	m_Plane.p4.x = m_Plane.pivot.x - m_Width / 2;
	m_Plane.p4.y = m_Plane.pivot.y + m_Height / 2;
	m_Plane.p4.z = -1000;

	m_Plane.middle.x = m_Plane.pivot.x;
	m_Plane.middle.y = m_Plane.pivot.y;
	m_Plane.middle.z = -1000;

	m_Plane.v.x = 0;
	m_Plane.v.y = 0;
	m_Plane.v.z = -m_Plane.middle.z + std::max(m_Width, std::max(m_Height, m_Depth));

	std::cout << "v: " << m_Plane.v.z << std::endl;

	m_Plane.x.x = m_Plane.p3.x - m_Plane.p4.x;
	m_Plane.x.y = m_Plane.p3.y - m_Plane.p4.y;
	m_Plane.x.z = m_Plane.p3.z - m_Plane.p4.z;

	m_Plane.y.x = m_Plane.p1.x - m_Plane.p4.x;
	m_Plane.y.y = m_Plane.p1.y - m_Plane.p4.y;
	m_Plane.y.z = m_Plane.p1.z - m_Plane.p4.z;

	//normalize
	m_Plane.x.x = m_Plane.x.x / m_Width;
	m_Plane.x.y = m_Plane.x.y / m_Width;
	m_Plane.x.z = m_Plane.x.z / m_Width;

	m_Plane.y.x = m_Plane.y.x / m_Height;
	m_Plane.y.y = m_Plane.y.y / m_Height;
	m_Plane.y.z = m_Plane.y.z / m_Height;

}