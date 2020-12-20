#include "preprocessing.h"
#include <utility>
#include "normalization.h"
#include "remeshing.h"
#include "config.h"
#include "feature_extraction.h"

bool Preprocessing::normalize_shape(SurfaceMesh &mesh, bool print) {
	// This is to to make sure we are only working with triangles
	mesh.triangulate();

	// Do normalization
	Remeshing::remesh_to_vertex_count(mesh, REMESHING_TARGET_VERTEX_COUNT, print);
	Normalization::recenter_barycenter_to_origin(mesh, print);
	Normalization::align(mesh, print);
	bool flip_faces = Normalization::flip_if_necessary(mesh, print);
	Normalization::scale_uniformly_to_bounds(mesh, print);

	return flip_faces;
}

void Preprocessing::flip_all_faces(const std::string &file_path, int vertex_count, bool print) {
	std::ifstream file(file_path);
	std::vector<std::string> lines;
	std::string str;
	int i = 0;

	while (std::getline(file, str)) {
		if (i <= vertex_count + 1) {
			lines.push_back(str);
			i++;
			continue;
		}

		std::vector<std::string> split_list;

		std::stringstream ss(str);
		std::string word;

		while (std::getline(ss, word, ' '))
			split_list.push_back(word);

		lines.push_back(split_list[0] + " " + split_list[3] + " " + split_list[2] + " " + split_list[1]);

		i++;
	}

	file.close();

	std::ofstream out_file;
	out_file.open(file_path);

	for (auto &line : lines)
		out_file << line << std::endl;

	out_file.close();
}

DatabaseShape Preprocessing::extract_shape(SurfaceMesh mesh, bool print) {
	return FeatureExtraction::get_shape_features(mesh, false);
}

DatabaseShape Preprocessing::normalize_features_for_shape(const DatabaseShape &shape, bool print) {
	return FeatureExtraction::get_normalized_shape_features(shape, print);
}

std::vector<DatabaseShape>
Preprocessing::normalize_features_for_shapes(const std::vector<DatabaseShape> &shapes, bool print) {
	std::vector<DatabaseShape> result_shapes;

	result_shapes.reserve(shapes.size());
	for (const auto &shape : shapes)
		result_shapes.push_back(normalize_features_for_shape(shape, print));

	return result_shapes;
}
