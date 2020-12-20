#pragma once

#include <iostream>
#include "database_mr.h"
#include <pmp/SurfaceMesh.h>

using namespace pmp;

class Preprocessing {
public:
	static bool normalize_shape(SurfaceMesh &mesh, bool print);
	static void flip_all_faces(const std::string& file_path, int vertex_count, bool print);

	static DatabaseShape extract_shape(SurfaceMesh mesh, bool print);
	static std::vector<DatabaseShape> extract_shapes(const std::vector<SurfaceMesh>& meshes, bool print);

	static DatabaseShape normalize_features_for_shape(const DatabaseShape& shape, bool print);
	static std::vector<DatabaseShape> normalize_features_for_shapes(const std::vector<DatabaseShape>& shapes, bool print);
};
