#include <boost/lexical_cast.hpp>
#include "util.h"


bool Util::replace(std::string &str, const std::string &from, const std::string &to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

double Util::distance_between_two_points(Eigen::Vector3d a, Eigen::Vector3d b) {
	double x = (b.x() - a.x()) * (b.x() - a.x());
	double y = (b.y() - a.y()) * (b.y() - a.y());
	double z = (b.z() - a.z()) * (b.z() - a.z());

	return std::sqrtf(x + y + z);
}

double Util::dot(Eigen::VectorXd a, Eigen::VectorXd b) {
	return a.x() * b.x() + a.y() * b.y() + a.z() * b.z();
}

Eigen::Vector3d Util::cross(Eigen::Vector3d a, Eigen::Vector3d b) {
	float x = a.y() * b.z() - a.z() * b.y();
	float y = a.z() * b.x() - a.x() * b.z();
	float z = a.x() * b.y() - a.y() * b.x();

	return Eigen::Vector3d{x, y, z};
}

double
Util::map(double in_value, double min_in_range, double max_in_range, double min_out_range, double max_out_range) {
	double x = (in_value - min_in_range) / (max_in_range - min_in_range);
	return min_out_range + (max_out_range - min_out_range) * x;
}

double Util::euclidean_distance(double a, double b) {
	return sqrt((a - b) * (a - b));
}

double Util::average(const std::vector<double> &numbers) {
	double average = 0.0;

	for (double number : numbers)
		average += number;

	average /= numbers.size();

	return average;
}

double Util::standard_deviation(const std::vector<double> &numbers) {
	double mean = average(numbers);

	double standard_deviation = 0.0;

	for (double number : numbers)
		standard_deviation += (number - mean) * (number - mean);

	standard_deviation /= numbers.size();
	standard_deviation = sqrt(standard_deviation);

	return standard_deviation;
}

std::string Util::serialize(const std::vector<double> &vec) {
	std::string result;

	for (int i = 0; i < vec.size(); i++) {
		const double v = vec.at(i);
		result += std::to_string(v);

		if (i != vec.size() - 1) {
			result += ",";
		}
	}

	return result;
}

std::vector<double> Util::deserialize(const std::string &str) {
	std::vector<double> result;

	std::vector<std::string> strings;
	boost::split(strings, str, boost::is_any_of(","));

	result.reserve(strings.size());
	for (auto &string : strings) {
		result.push_back(atof(string.c_str()));
	}

	return result;
}

std::string Util::filename_of_abs_path(const boost::filesystem::path &abs_path) {
	int index_of_last_separator;

	char preferred_separator = boost::filesystem::path::preferred_separator;
	std::string preferred_separator_string;
	preferred_separator_string = preferred_separator;

	if (boost::algorithm::contains(abs_path.string(), preferred_separator_string))
		index_of_last_separator = abs_path.string().find_last_of(boost::filesystem::path::preferred_separator);
	else
		index_of_last_separator = abs_path.string().find_last_of(boost::filesystem::path::separator);

	return abs_path.string().substr(index_of_last_separator + 1);
}

char Util::separator() {
	return boost::filesystem::path::preferred_separator;
}

std::vector<int> Util::random_numbers(int amount_of_numbers, int maximum_range) {
	std::vector<int> random_numbers;

	for (int i = 0; i < amount_of_numbers; i++) {
		int random_number = std::rand() % maximum_range;

		while (std::find(random_numbers.begin(), random_numbers.end(), random_number) != random_numbers.end())
			random_number = std::rand() % maximum_range;

		random_numbers.push_back(random_number);
	}

	return random_numbers;
}

std::vector<boost::filesystem::path>
Util::files_to_vector(const boost::filesystem::path &if_abs_path, const std::string &extension) {
	std::vector<boost::filesystem::path> file_paths = std::vector<boost::filesystem::path>();

	const std::string extension_incl_dot = "." + extension;

	if (boost::filesystem::is_regular_file(if_abs_path)) {
		if (boost::algorithm::contains(if_abs_path.string(), extension_incl_dot)) {
			file_paths.push_back(if_abs_path);
		}
	} else {
		boost::filesystem::directory_iterator iter(if_abs_path);
		boost::filesystem::directory_iterator end;

		while (iter != end) {
			boost::filesystem::path file_path = iter->path();

			if (boost::algorithm::contains(file_path.string(), extension_incl_dot)) {
				file_paths.push_back(file_path);
			}

			boost::system::error_code ec;
			iter.increment(ec);
		}
	}

	return file_paths;
}
