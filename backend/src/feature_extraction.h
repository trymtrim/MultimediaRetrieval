#pragma once

#include <pmp/SurfaceMesh.h>
#include "database_mr.h"

using namespace pmp;

enum GlobalDescriptor
{
	SURFACE_AREA,
	COMPACTNESS,
	VOLUME,
	DIAMETER,
	ECCENTRICITY
};

enum PropertyDescriptor
{
	A3, // Angle between 3 random vertices
	D1, // Distance between barycenter and random vertex
	D2, // Distance between 2 random vertices
	D3, // Square root of area of triangle given by 3 random vertices
	D4  // Cube root of volume of tetrahedron formed by 4 random vertices
};

struct PropertyHistogramBar
{
	double range_min; // Bottom (range: min-max)
	double range_max; // Bottom (range: min-max)
	double average_of_range; // The average of min and max range
	int amount_of_items; // Side (Amount of items that fall into this range
	
	double normalized_range_min;
	double normalized_range_max;
	double normalized_average_of_range;
	double normalized_amount_of_items;
};

class FeatureExtraction
{
public:
	// This method needs to be run on each mesh in the database every time a new mesh (or several meshes) is added to the database
	static DatabaseShape get_normalized_shape_features(const DatabaseShape& shape, bool print);
	// This method needs to be run when adding a new mesh to the database - the data retrieved here is the data that goes into the database
	static DatabaseShape get_shape_features(SurfaceMesh &mesh, bool print);

private:
	static double get_global_descriptor(GlobalDescriptor global_descriptor, SurfaceMesh &mesh, bool print);
	static std::vector<PropertyHistogramBar> get_property_descriptor_histogram(PropertyDescriptor property_descriptor,
		int amount_of_bars, int amount_of_property_items, SurfaceMesh &mesh, bool print);
	static std::vector<double> get_property_descriptor(PropertyDescriptor property_descriptor, int amount, SurfaceMesh &mesh, bool print);
	static Eigen::Vector2d get_lowest_and_highest_number_from_list(const std::vector<double>& number_list);
	static std::vector<double> get_property_descriptor_items_in_range(const std::vector<double>& item_list, float range_min, float range_max, bool is_last_bar);
	static double get_average(GlobalDescriptor global_descriptor, const std::vector<DatabaseShape>& shapes);
	static double get_standard_deviation(GlobalDescriptor global_descriptor, const std::vector<DatabaseShape>& shapes);
};
