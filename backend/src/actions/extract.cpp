#include "../database_mr.h"
#include "extract.h"
#include "../preprocessing.h"

int Extract::run(const ActionArgs &action_args) {
	boost::filesystem::path if_abs_path = boost::filesystem::absolute(Database::metadata().cache_dir);

	if (!boost::filesystem::exists(if_abs_path)) {
		std::cout << "Input file does not exist." << std::endl;
		return 1;
	}

	if (action_args.debug)
		std::cout << "Started feature extraction " << std::endl;

	std::vector<DatabaseShape> shapes;

	for (const auto &file_path : Util::files_to_vector(if_abs_path, "off")) {
		const std::string filename = Util::filename_of_abs_path(file_path);

		if (action_args.debug)
			std::cout << "Extracting features for " << filename << std::endl;

		SurfaceMesh mesh;
		mesh.read(file_path.string());

		DatabaseShape shape = Preprocessing::extract_shape(mesh, action_args.debug);
		shape.filename = filename;
		shapes.push_back(shape);
	}

	Database::add_shapes(shapes);

	std::vector<DatabaseShape> database_shapes = Database::shapes();
	Database::add_shapes(Preprocessing::normalize_features_for_shapes(database_shapes, action_args.debug));

	return 0;
}