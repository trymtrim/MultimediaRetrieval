#pragma once

#include <pmp/SurfaceMesh.h>

using namespace pmp;

class Features
{
public:
	static double get_surface_area(SurfaceMesh &mesh, bool print);
	static double get_compactness(SurfaceMesh &mesh, bool print);
	static double get_volume(SurfaceMesh &mesh, bool print);
	static double get_diameter(SurfaceMesh &mesh, bool print);
	static double get_eccentricity(SurfaceMesh &mesh, bool print);

	static double get_a3(SurfaceMesh &mesh, bool print); // Angle between 3 random vertices
	static double get_d1(SurfaceMesh &mesh, bool print); // Distance between barycenter and random vertex
	static double get_d2(SurfaceMesh &mesh, bool print); // Distance between 2 random vertices
	static double get_d3(SurfaceMesh &mesh, bool print); // Square root of area of triangle given by 3 random vertices
	static double get_d4(SurfaceMesh &mesh, bool print); // cube root of volume of tetrahedron formed by 4 random vertices
};
