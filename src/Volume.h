#pragma once

#include "Vector.h"

#include <vector>
#include <string>
#include <iostream>

#include <QProgressBar>


//-------------------------------------------------------------------------------------------------
// Voxel
//-------------------------------------------------------------------------------------------------

class Voxel
{
	public:

		Voxel();
		Voxel(const Voxel &other);
		Voxel(const float value);

		~Voxel();


		// VOXEL VALUE

		void					setValue(const float value);
		const float				getValue() const;


		// OPERATORS

		const bool				operator==(const Voxel &other) const;
		const bool				operator!=(const Voxel &other) const;
		const bool				operator>(const Voxel &other) const;
		const bool				operator>=(const Voxel &other) const;
		const bool				operator<(const Voxel &other) const;
		const bool				operator<=(const Voxel &other) const;

		const Voxel				operator+(const Voxel &other) const;
		const Voxel				operator-(const Voxel &other) const;
		const Voxel				operator*(const float &value) const;
		const Voxel				operator/(const float &value) const;
		
		const Voxel&			operator+=(const Voxel &other);
		const Voxel&			operator-=(const Voxel &other);
		const Voxel&			operator*=(const float &value);
		const Voxel&			operator/=(const float &value);


	private:

		float					m_Value;

};

/*class Plane
{
public:
	Vector3					p1;
	Vector3					p2;
	Vector3					p3;
	Vector3					p4;
	Vector3					middle;
	Vector3					pivot;
	Vector3					v;
	Vector3					x;
	Vector3					y;
};*/


//-------------------------------------------------------------------------------------------------
// Volume
//-------------------------------------------------------------------------------------------------

class Volume
{

	public:

		Volume();
		~Volume();


		// VOLUME DATA

		const Voxel&			voxel(const int i) const;
		const Voxel&			voxel(const int x, const int y, const int z) const;
		const Voxel*			voxels() const;

		const int				width() const;
		const int				height() const;
		const int				depth() const;

		const int				size() const;
		//enum					Axis { X, Y, Z };

		bool					loadFromFile(QString filename, QProgressBar* progressBar);
		std::vector<float>		getVolume();
		//bool					isIntersection(Vector3 point_1, Vector3 point_2, Vector3 v, Vector3& intersection_1, Vector3& intersection_2);
		//bool					searchIntersection(Vector3 point, Vector3 v, bool& firstIntersectFound, Vector3& intersec1, Vector3& intersec2, Axis axis, float fixPoint);
		//void					initializePlane();


	private:

		std::vector<Voxel>		m_Voxels;
		//Plane					m_p;
		int						m_Width;
		int						m_Height;
		int						m_Depth;

		int						m_Size;

};
