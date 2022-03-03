//
//  PerlinNoiseField3.h
//
//  A module to calculate 3D value noise and Perlin noise.
//

#pragma once

#include "ObjLibrary/Vector3.h"



//
//  PerlinNoiseField3.h
//
//  A class to calculate 3D value noise and Perlin noise.
//
//  Class Invariant:
//    <1> m_grid_size > 0.0
//
class PerlinNoiseField3
{
public:
	PerlinNoiseField3 ();
	PerlinNoiseField3 (float grid_size,
	                  float amplitude);
	PerlinNoiseField3 (float grid_size,
	                  float amplitude,
	                  unsigned int seed_x1,
	                  unsigned int seed_x2,
	                  unsigned int seed_y1,
	                  unsigned int seed_y2,
	                  unsigned int seed_z1,
	                  unsigned int seed_z2,
	                  unsigned int seed_q0,
	                  unsigned int seed_q1,
	                  unsigned int seed_q2);
	PerlinNoiseField3 (const PerlinNoiseField3& to_copy) = default;
	~PerlinNoiseField3 () = default;
	PerlinNoiseField3& operator= (const PerlinNoiseField3& to_copy) = default;

	float getGridSize () const;
	float getAmplitude () const;
	float valueNoise (float x, float y, float z) const;
	float perlinNoise (float x, float y, float z) const;

	void printPerlin (unsigned int print_rows,
	                  unsigned int print_columns,
	                  float interval) const;

	void setGridSize (float grid_size);
	void setAmplitude (float amplitude);
	void setSeeds (unsigned int seed_x1,
	               unsigned int seed_x2,
	               unsigned int seed_y1,
	               unsigned int seed_y2,
	               unsigned int seed_z1,
	               unsigned int seed_z2,
	               unsigned int seed_q0,
	               unsigned int seed_q1,
	               unsigned int seed_q2);


private:
	unsigned int pseudorandom (int x, int y, int z) const;
	float unsignedIntTo01 (unsigned int n) const;
	float unsignedIntToPM1 (unsigned int n) const;
	float fade (float n) const;
	unsigned int interpolate (unsigned int v0,
	                          unsigned int v1,
	                          float fraction) const;
	float interpolate (float v0,
	                   float v1,
	                   float fraction) const;
	ObjLibrary::Vector3 lattice (int x, int y, int z) const;
	void printValue (float value) const;
	bool invariant () const;

private:
	float m_grid_size;
	float m_amplitude;
	unsigned int m_seed_x1;
	unsigned int m_seed_x2;
	unsigned int m_seed_y1;
	unsigned int m_seed_y2;
	unsigned int m_seed_z1;
	unsigned int m_seed_z2;
	unsigned int m_seed_q0;
	unsigned int m_seed_q1;
	unsigned int m_seed_q2;
};
