#include "../preprocessing.h"
#include "store.h"

int Store::run(const ActionArgs &action_args) {
	boost::filesystem::path if_abs_path = boost::filesystem::absolute(action_args.input_file);

	if (!boost::filesystem::exists(if_abs_path)) {
		std::cout << "Input file does not exist." << std::endl;
		return 1;
	}

	if (!boost::filesystem::is_regular_file(if_abs_path)) {
		std::cout << "Input has to be a regular file." << std::endl;
		return 1;
	}

	if (action_args.debug)
		std::cout << "Started storing " << std::endl;

	SurfaceMesh mesh;
	mesh.read(if_abs_path.string());

	bool flip_faces = Preprocessing::normalize_shape(mesh, action_args.debug);

	boost::filesystem::path of_abs_path = boost::filesystem::absolute(Database::metadata().cache_dir).string()
	                                      + Util::separator()
	                                      + Util::filename_of_abs_path(if_abs_path);

	mesh.write(of_abs_path.string());

	if (flip_faces)
		Preprocessing::flip_all_faces(of_abs_path.string(), mesh.n_vertices(), action_args.debug);

	DatabaseShape shape = Preprocessing::extract_shape(mesh, action_args.debug);
	shape.filename = Util::filename_of_abs_path(of_abs_path);
	Database::add_shape(shape);

	const std::vector<DatabaseShape> shapes = Database::shapes();
	Database::add_shapes(Preprocessing::normalize_features_for_shapes(shapes, action_args.debug));

	return 0;
}