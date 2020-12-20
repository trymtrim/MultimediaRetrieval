#ifndef BACKEND_UTIL_H
#define BACKEND_UTIL_H

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <pmp/SurfaceMesh.h>

class Util {
public:
	enum FeatureMatchingMethod {
		STD,
		KNN,
		RNN,
	};

	static inline std::string ToString(FeatureMatchingMethod v) {
		switch (v) {
			case STD:
				return "STD";
			case KNN:
				return "KNN";
			case RNN:
				return "RNN";
			default:
				return "[Unknown FeatureMatchingMethod]";
		}
	}

	static inline FeatureMatchingMethod FromString(const std::string &v) {
		if (v == "STD") {
			return FeatureMatchingMethod::STD;
		} else if (v == "KNN") {
			return FeatureMatchingMethod::KNN;
		} else if (v == "RNN") {
			return FeatureMatchingMethod::RNN;
		}

		// TODO throw error?
		return FeatureMatchingMethod::STD;
	}

	static bool replace(std::string &str, const std::string &from, const std::string &to);

	static double distance_between_two_points(Eigen::Vector3d a, Eigen::Vector3d b);

	static double dot(Eigen::VectorXd a, Eigen::VectorXd b);

	static Eigen::Vector3d cross(Eigen::Vector3d a, Eigen::Vector3d b);

	static double
	map(double in_value, double min_in_range, double max_in_range, double min_out_range, double max_out_range);

	static double euclidean_distance(double a, double b);

	static double average(const std::vector<double>& numbers);

	static double standard_deviation(const std::vector<double>& numbers);

	static std::string serialize(const std::vector<double> &vec);

	static std::vector<double> deserialize(const std::string &str);

	static std::string filename_of_abs_path(const boost::filesystem::path &abs_path);

	static char separator();

	static std::vector<int> random_numbers(int amount_of_numbers, int maximum_range);

	static std::vector<boost::filesystem::path>
	files_to_vector(const boost::filesystem::path &if_abs_path, const std::string &extension);
};

#endif //BACKEND_UTIL_H
