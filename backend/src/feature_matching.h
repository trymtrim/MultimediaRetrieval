#pragma once

#include <ANN/ANN.h>
#include "database_mr.h"
#include "util.h"

class FeatureMatching {
public:
	static std::vector<DatabaseShape>
	get_similar_shapes(const DatabaseShape& input_shape, const std::vector<DatabaseShape>& database_shapes,
	                   Util::FeatureMatchingMethod search_type);

private:
	static std::vector<DatabaseShape>
	get_similar_shapes_standard(DatabaseShape input_shape, const std::vector<DatabaseShape>& database_shapes);

	static double get_feature_distance(DatabaseShape shape_a, DatabaseShape shape_b);

	static std::vector<DatabaseShape>
	get_similar_shapes_ann(const DatabaseShape& input_shape, std::vector<DatabaseShape> database_shapes,
	                       Util::FeatureMatchingMethod search_type);

	static std::string get_feature_vector_as_string(const DatabaseShape& shape);

	static std::vector<int>
	get_similar_shapes_indices(const std::string& query_feature_vector, const std::string& database_feature_vectors,
	                           int database_shape_count, Util::FeatureMatchingMethod search_type);

	static bool read_point(std::istream &in, ANNpoint p, int dim);
};
