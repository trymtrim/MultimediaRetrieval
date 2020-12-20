#ifndef BACKEND_DATABASE_H
#define BACKEND_DATABASE_H

#include <boost/filesystem.hpp>
#include <iostream>
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/Transaction.h>
#include <vector>
#include "util.h"

using namespace std;

struct DatabaseMetadata {
	int backend_version_major;
	int backend_version_minor;
	int database_version_major;
	int database_version_minor;

	Util::FeatureMatchingMethod feature_matching_method;

	std::string originals_dir;
	std::string cache_dir;

	int maximum_returned_matches;
	double maximum_feature_matching_distance;

	double weight_surface_area;
	double weight_compactness;
	double weight_volume;
	double weight_diameter;
	double weight_eccentricity;
	double weight_A3;
	double weight_D1;
	double weight_D2;
	double weight_D3;
	double weight_D4;
};

struct DatabaseShape {
	int index;
	std::string filename;
	double surface_area;
	double compactness;
	double volume;
	double diameter;
	double eccentricity;
	std::vector<double> a3;
	std::vector<double> d1;
	std::vector<double> d2;
	std::vector<double> d3;
	std::vector<double> d4;
	double surface_area_normalized;
	double compactness_normalized;
	double volume_normalized;
	double diameter_normalized;
	double eccentricity_normalized;
};

class Database {
public:
	static int create(const boost::filesystem::path &database_path);

	static int open(const boost::filesystem::path &database_path);

	static int add_shape(const DatabaseShape &shape);

	static bool add_shapes(const std::vector<DatabaseShape> &shapes);

	static DatabaseMetadata metadata();

	static vector<DatabaseShape> shapes();

	static DatabaseShape get_shape_from_filename(const std::string &filename);

	static int close();

private:
	static SQLite::Database db;

	static bool create_metadata_table_if_needed();

	static bool create_shapes_table_if_needed();

	static void update_metadata(const DatabaseMetadata &metadata);
};

#endif //BACKEND_DATABASE_H
