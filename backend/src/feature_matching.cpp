#include "feature_matching.h"

#include <utility>
#include "../thirdparty/Wasserstein/wasserstein.h"
#include "config.h"

std::vector<DatabaseShape>
FeatureMatching::get_similar_shapes(const DatabaseShape &input_shape,
                                    const std::vector<DatabaseShape> &database_shapes,
                                    Util::FeatureMatchingMethod search_type) {
	std::vector<DatabaseShape> similar_shapes;

	switch (search_type) {
		case Util::FeatureMatchingMethod::STD:
			similar_shapes = get_similar_shapes_standard(input_shape, database_shapes);
			break;
		case Util::FeatureMatchingMethod::KNN:
		case Util::FeatureMatchingMethod::RNN:
			similar_shapes = get_similar_shapes_ann(input_shape, database_shapes, search_type);
			break;
	}

	if (!similar_shapes.empty()) {
		if (similar_shapes[0].filename == input_shape.filename)
			similar_shapes.erase(similar_shapes.begin(), similar_shapes.begin() + 1);
	}

	return similar_shapes;
}

std::vector<DatabaseShape>
FeatureMatching::get_similar_shapes_standard(DatabaseShape input_shape,
                                             const std::vector<DatabaseShape> &database_shapes) {
	const DatabaseMetadata db_metadata = Database::metadata();

	std::vector<DatabaseShape> similar_shapes;
	std::vector<double> sorted_distance_list;
	std::map<double, DatabaseShape> shape_distance_map;

	for (auto shape_to_check_towards : database_shapes) {
		if (Util::filename_of_abs_path(input_shape.filename) ==
		    Util::filename_of_abs_path(shape_to_check_towards.filename))
			continue;

		double distance = std::sqrt(FeatureMatching::get_feature_distance(input_shape, shape_to_check_towards));

		bool is_similar = distance < db_metadata.maximum_feature_matching_distance;

		if (is_similar) {
			similar_shapes.push_back(shape_to_check_towards);
			sorted_distance_list.push_back(distance);
			shape_distance_map.emplace(distance, shape_to_check_towards);
		}
	}

	// Sort them in order from low to high
	std::sort(sorted_distance_list.begin(), sorted_distance_list.end());

	std::vector<DatabaseShape> sorted_shape_list;

	sorted_shape_list.reserve(sorted_distance_list.size());
	for (double distance : sorted_distance_list) {
		sorted_shape_list.push_back(shape_distance_map[distance]);
	}

	std::vector<DatabaseShape> returned_sorted_distance_list;
	int maximum_returned_matches =
			db_metadata.maximum_returned_matches >= sorted_distance_list.size() ? sorted_distance_list.size()
			                                                                    : db_metadata.maximum_returned_matches;

	returned_sorted_distance_list.reserve(maximum_returned_matches);
	for (int i = 0; i < maximum_returned_matches; i++)
		returned_sorted_distance_list.push_back(sorted_shape_list[i]);

	return returned_sorted_distance_list;
}

double FeatureMatching::get_feature_distance(DatabaseShape shape_a, DatabaseShape shape_b) {
	double a = Util::euclidean_distance(shape_a.surface_area_normalized, shape_b.surface_area_normalized);
	double b = Util::euclidean_distance(shape_a.compactness_normalized, shape_b.compactness_normalized);
	double c = Util::euclidean_distance(shape_a.volume_normalized, shape_b.volume_normalized);
	double d = Util::euclidean_distance(shape_a.diameter_normalized, shape_b.diameter_normalized);
	double e = Util::euclidean_distance(shape_a.eccentricity_normalized, shape_b.eccentricity_normalized);

	std::vector<double> weight{0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1}; // 0.1 * 10 = 1

	double f = wasserstein(shape_a.a3, weight, shape_b.a3, weight);
	double g = wasserstein(shape_a.d1, weight, shape_b.d1, weight);
	double h = wasserstein(shape_a.d2, weight, shape_b.d2, weight);
	double i = wasserstein(shape_a.d3, weight, shape_b.d3, weight);
	double j = wasserstein(shape_a.d4, weight, shape_b.d4, weight);

	// Note: all weights should add up to one.
	// We do not test for this at the moment, so we trust our end-users to do this.
	// This is never going to cause problems or weird results, since all our end-users read manuals.
	const DatabaseMetadata db_metadata = Database::metadata();
	double weight_surface_area = db_metadata.weight_surface_area;
	double weight_compactness = db_metadata.weight_compactness;
	double weight_volume = db_metadata.weight_volume;
	double weight_diameter = db_metadata.weight_diameter;
	double weight_eccentricity = db_metadata.weight_eccentricity;
	double weight_A3 = db_metadata.weight_A3;
	double weight_D1 = db_metadata.weight_D1;
	double weight_D2 = db_metadata.weight_D2;
	double weight_D3 = db_metadata.weight_D3;
	double weight_D4 = db_metadata.weight_D4;

	return (a * weight_surface_area + b * weight_compactness + c * weight_volume + d * weight_diameter +
	        e * weight_eccentricity + f * weight_A3 + g * weight_D1 +
	        h * weight_D2 + i * weight_D3 + j * weight_D4);
}

std::vector<DatabaseShape>
FeatureMatching::get_similar_shapes_ann(const DatabaseShape &input_shape,
                                        std::vector<DatabaseShape> database_shapes,
                                        Util::FeatureMatchingMethod search_type) {
	std::vector<DatabaseShape> similar_shapes;

	std::string query_feature_vector = get_feature_vector_as_string(input_shape);
	std::string database_feature_vectors;

	for (int i = 0; i < database_shapes.size(); i++) {
		DatabaseShape shape = database_shapes[i];

		if (i == database_shapes.size() - 1)
			database_feature_vectors += get_feature_vector_as_string(shape);
		else
			database_feature_vectors += get_feature_vector_as_string(shape) + "\n";
	}

	std::vector<int> similar_shapes_indices = get_similar_shapes_indices(query_feature_vector, database_feature_vectors,
	                                                                     database_shapes.size(), search_type);

	similar_shapes.reserve(similar_shapes_indices.size());
	for (int shape_index : similar_shapes_indices)
		similar_shapes.push_back(database_shapes[shape_index]);

	return similar_shapes;
}

std::string FeatureMatching::get_feature_vector_as_string(const DatabaseShape &shape) {
	std::string feature_vector_string = std::to_string(shape.surface_area_normalized)
	                                    + " " + std::to_string(shape.compactness_normalized)
	                                    + " " + std::to_string(shape.volume_normalized)
	                                    + " " + std::to_string(shape.diameter_normalized)
	                                    + " " + std::to_string(shape.eccentricity_normalized);

	for (double i : shape.a3)
		feature_vector_string += " " + std::to_string(i);
	for (double i : shape.d1)
		feature_vector_string += " " + std::to_string(i);
	for (double i : shape.d2)
		feature_vector_string += " " + std::to_string(i);
	for (double i : shape.d3)
		feature_vector_string += " " + std::to_string(i);
	for (double i : shape.d4)
		feature_vector_string += " " + std::to_string(i);

	return feature_vector_string;
}

std::vector<int>
FeatureMatching::get_similar_shapes_indices(const std::string &query_feature_vector,
                                            const std::string &database_feature_vectors,
                                            int database_shape_count,
                                            Util::FeatureMatchingMethod search_type) {
	const DatabaseMetadata db_metadata = Database::metadata();

	std::vector<int> similar_shapes_indices;

	int k = db_metadata.maximum_returned_matches; // number of nearest neighbors
	int dim = HISTOGRAM_BAR_COUNT * 5 + 5; // dimension
	double eps = 0; // error bound

	int nPts = 0; // actual number of data points
	ANNpointArray dataPts = annAllocPts(database_shape_count, dim); // allocate data points
	ANNpoint queryPt = annAllocPt(dim); // allocate query point
	auto nnIdx = new ANNidx[k]; // allocate near neigh indices
	auto dists = new ANNdist[k]; // allocate near neighbor dists

	std::istringstream query_stream(query_feature_vector);
	std::istream *queryIn = &query_stream; // input for query points

	std::istringstream data_stream(database_feature_vectors);
	std::istream *dataIn = &data_stream; // input for data points

	while (read_point(*dataIn, dataPts[nPts], dim))
		nPts++;

	auto *kdTree = new ANNkd_tree( // build search structure
			dataPts, // the data points
			nPts, // number of points
			dim); // dimension of space

	while (read_point(*queryIn, queryPt, dim)) // read query points
	{
		if (search_type == Util::FeatureMatchingMethod::KNN) {
			kdTree->annkSearch( // search
					queryPt, // query point
					k, // number of near neighbors
					nnIdx, // nearest neighbors (returned)
					dists, // distance (returned)
					eps); // error bound

			// Debug
			//std::cout << "\tNN:\tIndex\tDistance\n";

			for (int i = 0; i < k; i++) {
				dists[i] = sqrt(dists[i]); // Un-square distance

				similar_shapes_indices.push_back(nnIdx[i]);

				// Debug
				//std::cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << std::endl;
			}
		} else if (search_type == Util::FeatureMatchingMethod::RNN) {
			ANNdist square_radius =
					db_metadata.maximum_feature_matching_distance * db_metadata.maximum_feature_matching_distance;

			kdTree->annkFRSearch( // search
					queryPt, // query point
					square_radius, // square distance
					k, // number of near neighbors
					nnIdx, // nearest neighbors (returned)
					dists, // distance (returned)
					eps); // error bound

			// Debug
			//std::cout << "\tNN:\tIndex\tDistance\n";

			for (int i = 0; i < k; i++) {
				dists[i] = sqrt(dists[i]); // unsquare distance

				if (nnIdx[i] >= 0)
					similar_shapes_indices.push_back(nnIdx[i]);

				// Debug
				//if (nnIdx[i] >= 0)
				//	std::cout << "\t" << i << "\t" << nnIdx[i] << "\t" << dists[i] << std::endl;
			}
		}
	}

	delete[] nnIdx;
	delete[] dists;
	delete kdTree;

	annClose();

	return similar_shapes_indices;
}

bool FeatureMatching::read_point(std::istream &in, ANNpoint p, int dim) {
	for (int i = 0; i < dim; i++) {
		if (!(in >> p[i]))
			return false;
	}

	return true;
}
