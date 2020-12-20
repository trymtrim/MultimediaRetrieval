#pragma once

#include <vector>
#include <string>

struct QualityValues {
	std::string class_name; // This can be a class name or 'Database'
	int shape_count{};

	double average_true_positives{};
	double average_false_positives{};
	double average_false_negatives{};

	double average_true_positive_rate{};
	double average_false_negative_rate{};
	double average_critical_success_rate{};
};

class Evaluation {
public:
	static std::vector<QualityValues> get_quality_values(std::string database_path);

private:
	static std::string get_class_name(std::string file_name);
};
