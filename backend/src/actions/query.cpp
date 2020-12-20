#include <random>
#include "query.h"
#include "../database_mr.h"
#include "../feature_matching.h"

int Query::run(const ActionArgs &action_args) {
	boost::filesystem::path if_abs_path = boost::filesystem::absolute(action_args.input_file);

	if (!boost::filesystem::exists(if_abs_path)) {
		std::cout << "Input file does not exist." << std::endl;
		return 1;
	}

	if (!boost::filesystem::is_regular_file(if_abs_path)) {
		std::cout << "Input has to be a file, not a directory." << std::endl;
		return 1;
	}

	std::string query_result;

	DatabaseShape input_shape = Database::get_shape_from_filename(Util::filename_of_abs_path(action_args.input_file));
	std::vector<DatabaseShape> similar_shapes = FeatureMatching::get_similar_shapes(input_shape, Database::shapes(), Database::metadata().feature_matching_method);

	for (int i = 0; i < similar_shapes.size(); i++) {
		query_result += boost::filesystem::absolute(Database::metadata().cache_dir + Util::separator() + similar_shapes[i].filename).string();

		if (i != similar_shapes.size() - 1)
			query_result += "\n";
	}


	bool query_result_found = !similar_shapes.empty();

	if (!query_result_found) {
		std::cout << "No similar mesh found in database." << std::endl;
		return 3;
	}

	std::cout << query_result;
	return 0;
}