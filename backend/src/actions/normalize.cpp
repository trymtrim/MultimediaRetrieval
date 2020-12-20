#include <boost/algorithm/string.hpp>
#include "normalize.h"
#include "../preprocessing.h"

int Normalize::run(const ActionArgs &action_args) {
	boost::filesystem::path if_abs_path = boost::filesystem::absolute(Database::metadata().originals_dir);

	if (!boost::filesystem::exists(if_abs_path)) {
		std::cout << "Input file/directory does not exist." << std::endl;
		return 1;
	}

	boost::filesystem::path of_abs_path = boost::filesystem::absolute(Database::metadata().cache_dir);

	if (boost::filesystem::exists(of_abs_path) && !(action_args.append || action_args.overwrite)) {
		std::cout << "Output file/directory exists, but may not be overwritten or changed." << std::endl;
		std::cout << "Use --append to add new entries to the existing output file/directory." << std::endl;
		std::cout << "Use --overwrite to delete the current output file/directory, "
		             "and recreate it for the input file(s) provided." << std::endl;
		std::cout << "Or, choose a different output file/directory path." << std::endl;
		return 2;
	}

	if (boost::filesystem::is_directory(if_abs_path) && boost::filesystem::is_regular_file(of_abs_path)) {
		std::cout << "Input is a directory, but output is a file." << std::endl;
		std::cout << "Multiple input files can not be stored in a single output file." << std::endl;
		std::cout << "Either point input to a single file, or point output to a directory." << std::endl;
		return 2;
	}

	if (boost::filesystem::exists(of_abs_path) && action_args.overwrite) {
		boost::filesystem::remove_all(of_abs_path);
	}

	if (boost::filesystem::is_directory(if_abs_path) && !boost::filesystem::exists(of_abs_path)) {
		boost::filesystem::create_directory(of_abs_path);
	}

	if (action_args.debug)
		std::cout << "Started normalization " << std::endl;

	for (const auto &file_path : Util::files_to_vector(if_abs_path, "off")) {
		SurfaceMesh mesh;
		mesh.read(file_path.string());

		bool flip_faces = Preprocessing::normalize_shape(mesh, action_args.debug);

		boost::filesystem::path of_path_incl_name;
		if (boost::filesystem::is_regular_file(if_abs_path)) {
			of_path_incl_name = of_abs_path.string();
		} else {
			of_path_incl_name = of_abs_path.string()
			                    + Util::separator()
			                    + Util::filename_of_abs_path(file_path);
		}

		mesh.write(of_path_incl_name.string());

		if (flip_faces)
			Preprocessing::flip_all_faces(of_path_incl_name.string(), mesh.n_vertices(), action_args.debug);
	}

	return 0;
}
