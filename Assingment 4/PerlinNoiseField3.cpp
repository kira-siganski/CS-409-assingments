//
//  PerlinNoiseField3.h
//
//  A module to calculate value noise and Perlin noise.
//

#include <cassert>
#include <cmath>
#include <climits>
#include <iostream>

#include "ObjLibrary/Vector3.h"

#include "PerlinNoiseField3.h"

using namespace std;
using namespace ObjLibrary;
namespace
{
	const unsigned int DEFAULT_SEED_X1 = 1273472206;
	const unsigned int DEFAULT_SEED_X2 = 4278162623;
	const unsigned int DEFAULT_SEED_Y1 = 1440014778;
	const unsigned int DEFAULT_SEED_Y2 =  524485263;
	const unsigned int DEFAULT_SEED_Z1 = 2813546167;
	const unsigned int DEFAULT_SEED_Z2 = 3305132234;
	const unsigned int DEFAULT_SEED_Q0 = 1498573726;
	const unsigned int DEFAULT_SEED_Q1 = 3476519523;
	const unsigned int DEFAULT_SEED_Q2 = 3905844518;
}



PerlinNoiseField3 :: PerlinNoiseField3 ()
		: m_grid_size(1.0f)
		, m_amplitude(1.0f)
		, m_seed_x1(DEFAULT_SEED_X1)
		, m_seed_x2(DEFAULT_SEED_X2)
		, m_seed_y1(DEFAULT_SEED_Y1)
		, m_seed_y2(DEFAULT_SEED_Y2)
		, m_seed_z1(DEFAULT_SEED_Z1)
		, m_seed_z2(DEFAULT_SEED_Z2)
		, m_seed_q0(DEFAULT_SEED_Q0)
		, m_seed_q1(DEFAULT_SEED_Q1)
		, m_seed_q2(DEFAULT_SEED_Q2)
{
	assert(invariant());
}

PerlinNoiseField3 :: PerlinNoiseField3 (float grid_size,
                                        float amplitude)
		: m_grid_size(grid_size)
		, m_amplitude(amplitude)
		, m_seed_x1(DEFAULT_SEED_X1)
		, m_seed_x2(DEFAULT_SEED_X2)
		, m_seed_y1(DEFAULT_SEED_Y1)
		, m_seed_y2(DEFAULT_SEED_Y2)
		, m_seed_z1(DEFAULT_SEED_Z1)
		, m_seed_z2(DEFAULT_SEED_Z2)
		, m_seed_q0(DEFAULT_SEED_Q0)
		, m_seed_q1(DEFAULT_SEED_Q1)
		, m_seed_q2(DEFAULT_SEED_Q2)
{
	assert(grid_size > 0.0f);

	assert(invariant());
}

PerlinNoiseField3 :: PerlinNoiseField3 (float grid_size,
                                       float amplitude,
                                       unsigned int seed_x1,
                                       unsigned int seed_x2,
                                       unsigned int seed_y1,
                                       unsigned int seed_y2,
                                       unsigned int seed_z1,
                                       unsigned int seed_z2,
                                       unsigned int seed_q0,
                                       unsigned int seed_q1,
                                       unsigned int seed_q2)
		: m_grid_size(grid_size)
		, m_amplitude(amplitude)
		, m_seed_x1(seed_x1)
		, m_seed_x2(seed_x2)
		, m_seed_y1(seed_y1)
		, m_seed_y2(seed_y2)
		, m_seed_z1(seed_z1)
		, m_seed_z2(seed_z2)
		, m_seed_q0(seed_q0)
		, m_seed_q1(seed_q1)
		, m_seed_q2(seed_q2)
{
	assert(grid_size > 0.0f);

	assert(invariant());
}



float PerlinNoiseField3 :: getGridSize () const
{
	return m_grid_size;
}

float PerlinNoiseField3 :: getAmplitude () const
{
	return m_amplitude;
}

float PerlinNoiseField3 :: valueNoise (float x, float y, float z) const
{
	int x0 = (int)(floor(x / m_grid_size));
	int y0 = (int)(floor(y / m_grid_size));
	int z0 = (int)(floor(z / m_grid_size));
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int z1 = z0 + 1;

	float x_frac = x / m_grid_size - x0;
	float y_frac = y / m_grid_size - y0;
	float z_frac = z / m_grid_size - z0;

	float x_fade = fade(x_frac);
	float y_fade = fade(y_frac);
	float z_fade = fade(z_frac);

	unsigned int value000 = pseudorandom(x0, y0, z0);
	unsigned int value001 = pseudorandom(x0, y0, z1);
	unsigned int value010 = pseudorandom(x0, y1, z0);
	unsigned int value011 = pseudorandom(x0, y1, z1);
	unsigned int value100 = pseudorandom(x1, y0, z0);
	unsigned int value101 = pseudorandom(x1, y0, z1);
	unsigned int value110 = pseudorandom(x1, y1, z0);
	unsigned int value111 = pseudorandom(x1, y1, z1);

	unsigned int value00 = interpolate(value000, value001, z_fade);
	unsigned int value01 = interpolate(value010, value011, z_fade);
	unsigned int value10 = interpolate(value100, value101, z_fade);
	unsigned int value11 = interpolate(value110, value111, z_fade);
	unsigned int value0  = interpolate(value00,  value01,  y_fade);
	unsigned int value1  = interpolate(value10,  value11,  y_fade);
	unsigned int value   = interpolate(value0,   value1,   x_fade);

	return unsignedIntToPM1(value) * m_amplitude;
}

float PerlinNoiseField3 :: perlinNoise (float x, float y, float z) const
{
	int x0 = (int)(floor(x / m_grid_size));
	int y0 = (int)(floor(y / m_grid_size));
	int z0 = (int)(floor(z / m_grid_size));
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int z1 = z0 + 1;

	float x_frac = x / m_grid_size - x0;
	float y_frac = y / m_grid_size - y0;
	float z_frac = z / m_grid_size - z0;

	float x_fade = fade(x_frac);
	float y_fade = fade(y_frac);
	float z_fade = fade(z_frac);

	Vector3 lattice000 = lattice(x0, y0, z0);
	Vector3 lattice001 = lattice(x0, y0, z1);
	Vector3 lattice010 = lattice(x0, y1, z0);
	Vector3 lattice011 = lattice(x0, y1, z1);
	Vector3 lattice100 = lattice(x1, y0, z0);
	Vector3 lattice101 = lattice(x1, y0, z1);
	Vector3 lattice110 = lattice(x1, y1, z0);
	Vector3 lattice111 = lattice(x1, y1, z1);

	Vector3 direction000(     - x_frac,      - y_frac,      - z_frac);
	Vector3 direction001(     - x_frac,      - y_frac, 1.0f - z_frac);
	Vector3 direction010(     - x_frac, 1.0f - y_frac,      - z_frac);
	Vector3 direction011(     - x_frac, 1.0f - y_frac, 1.0f - z_frac);
	Vector3 direction100(1.0f - x_frac,      - y_frac,      - z_frac);
	Vector3 direction101(1.0f - x_frac,      - y_frac, 1.0f - z_frac);
	Vector3 direction110(1.0f - x_frac, 1.0f - y_frac,      - z_frac);
	Vector3 direction111(1.0f - x_frac, 1.0f - y_frac, 1.0f - z_frac);

	float value000 = (float)(lattice000.dotProduct(direction000));
	float value001 = (float)(lattice001.dotProduct(direction001));
	float value010 = (float)(lattice010.dotProduct(direction010));
	float value011 = (float)(lattice011.dotProduct(direction011));
	float value100 = (float)(lattice100.dotProduct(direction100));
	float value101 = (float)(lattice101.dotProduct(direction101));
	float value110 = (float)(lattice110.dotProduct(direction110));
	float value111 = (float)(lattice111.dotProduct(direction111));

	float value00 = interpolate(value000, value001, z_fade);
	float value01 = interpolate(value010, value011, z_fade);
	float value10 = interpolate(value100, value101, z_fade);
	float value11 = interpolate(value110, value111, z_fade);
	float value0  = interpolate(value00,  value01,  y_fade);
	float value1  = interpolate(value10,  value11,  y_fade);
	float value   = interpolate(value0,   value1,   x_fade);

	return value * m_amplitude;
}

void PerlinNoiseField3 :: printPerlin (unsigned int print_rows,
                                       unsigned int print_columns,
                                       float interval) const
{
	float base_row    = print_rows    * interval * -0.5f;
	float base_column = print_columns * interval * -0.5f;

	cout << "Perlin noise: XY plane, z = 0" << endl;
	cout << endl;
	for(unsigned int y = 0; y < print_rows; y++)
	{
		for(unsigned int x = 0; x < print_columns; x++)
		{
			if(x == print_columns / 2 && y == print_rows / 2)
				cout << "+";
			else
				printValue(perlinNoise(base_column + x * interval, base_row + y * interval, 0));
		}
		cout << endl;
	}
	cout << endl;
	cout << endl;

	cout << "Perlin noise: XZ plane, y = 0" << endl;
	cout << endl;
	for(unsigned int z = 0; z < print_rows; z++)
	{
		for(unsigned int x = 0; x < print_columns; x++)
		{
			if(x == print_columns / 2 && z == print_rows / 2)
				cout << "+";
			else
				printValue(perlinNoise(base_column + x * interval, 0, base_row + z * interval));
		}
		cout << endl;
	}
	cout << endl;
	cout << endl;

	cout << "Perlin noise: YZ plane, x = 0" << endl;
	cout << endl;
	for(unsigned int z = 0; z < print_rows; z++)
	{
		for(unsigned int y = 0; y < print_columns; y++)
		{
			if(y == print_columns / 2 && z == print_rows / 2)
				cout << "+";
			else
				printValue(perlinNoise(0, base_column + y * interval, base_row + z * interval));
		}
		cout << endl;
	}
	cout << endl;
	cout << endl;
}

void PerlinNoiseField3 :: setGridSize (float grid_size)
{
	assert(grid_size > 0.0f);

	m_grid_size = grid_size;

	assert(invariant());
}

void PerlinNoiseField3 :: setAmplitude (float amplitude)
{
	m_amplitude = amplitude;

	assert(invariant());
}

void PerlinNoiseField3 :: setSeeds (unsigned int seed_x1,
                                    unsigned int seed_x2,
                                    unsigned int seed_y1,
                                    unsigned int seed_y2,
                                    unsigned int seed_z1,
                                    unsigned int seed_z2,
                                    unsigned int seed_q0,
                                    unsigned int seed_q1,
                                    unsigned int seed_q2)
{
	m_seed_x1 = seed_x1;
	m_seed_x2 = seed_x2;
	m_seed_y1 = seed_y1;
	m_seed_y2 = seed_y2;
	m_seed_z1 = seed_z1;
	m_seed_z2 = seed_z2;
	m_seed_q0 = seed_q0;
	m_seed_q1 = seed_q1;
	m_seed_q2 = seed_q2;

	assert(invariant());
}



unsigned int PerlinNoiseField3 :: pseudorandom (int x, int y, int z) const
{
	unsigned int n = (m_seed_x1 * x) +
	                 (m_seed_y1 * y) +
	                 (m_seed_z1 * z);
	unsigned int quad_term = m_seed_q2 * n * n +
	                         m_seed_q1 * n     +
	                         m_seed_q0;
	return quad_term +
	       (m_seed_x2 * x) +
	       (m_seed_y2 * y) +
	       (m_seed_z2 * z);
}

float PerlinNoiseField3 :: unsignedIntTo01 (unsigned int n) const
{
	return ((float)(n) / UINT_MAX);
}

float PerlinNoiseField3 :: unsignedIntToPM1 (unsigned int n) const
{
	return ((float)(n) / UINT_MAX) * 2.0f - 1.0f;
}

float PerlinNoiseField3 :: fade (float n) const
{
	//return n;
	//return (-2.0f * n + 3) * n * n;
	//return ((6 * n - 15) * n + 10) * n * n * n;
	return (1 - cos(n * 3.14159265f)) * 0.5f;
}

unsigned int PerlinNoiseField3 :: interpolate (unsigned int v0,
                                               unsigned int v1,
                                               float fraction) const
{
	return (unsigned int)(v0 * (1.0f - fraction)) +
	       (unsigned int)(v1 *         fraction );
}

float PerlinNoiseField3 :: interpolate (float v0,
                                        float v1,
                                        float fraction) const
{
	return (v0 * (1.0f - fraction)) +
	       (v1 *         fraction );
}

ObjLibrary::Vector3 PerlinNoiseField3 :: lattice (int x, int y, int z) const
{
	unsigned int value1 = pseudorandom(x, y, z);
	unsigned int value2 = pseudorandom(x + 1, y + 1, z + 1);  //  <|>
	return Vector3::getPseudorandomUnitVector(unsignedIntTo01(value1),
	                                          unsignedIntTo01(value2));
}

void PerlinNoiseField3 :: printValue (float value) const
{
	assert(value >= -1.0f);
	assert(value <   1.0f);

	static const unsigned int VALUE_COUNT = 11;
	static const char A_VALUES[VALUE_COUNT] =
	{
		'?', ' ', '.', '-', '=', 'o', 'O', 'H', 'M', '@', '?',
	//	<0>   1    2    3    4   <5>   6    7    8    9   <10>
	};

	float value01 = (value + 1.0f) * 0.5f;
	assert(value01 >= 0.0f);
	assert(value01 <  1.0f);

	int index = (int)(value01 * VALUE_COUNT);  // truncate
	assert(index >= 0);
	assert(index <  VALUE_COUNT);
	cout << A_VALUES[index];
}

bool PerlinNoiseField3 :: invariant () const
{
	if(m_grid_size <= 0.0) return false;
	return true;
}
