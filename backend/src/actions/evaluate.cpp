#include "evaluate.h"
#include "../evaluation.h"

int Evaluate::run(const ActionArgs &action_args) {
	std::string database_path = action_args.database;
	std::vector<QualityValues> qualities = Evaluation::get_quality_values(database_path);

	for (const QualityValues &quality : qualities) {
		std::cout << "---------- " << quality.class_name << " ----------" << std::endl;
		std::cout << "Shape count: " << quality.shape_count << std::endl;

		if (quality.class_name != "Database") {
			std::cout << "Average TP: " << quality.average_true_positives << std::endl;
			std::cout << "Average FP: " << quality.average_false_positives << std::endl;
			std::cout << "Average FN: " << quality.average_false_negatives << std::endl;
			std::cout << std::endl;
		}

		std::cout << "Average Hit Rate: " << quality.average_true_positive_rate * 100.0 << "%" << std::endl;
		std::cout << "Average Miss Rate: " << quality.average_false_negative_rate * 100.0 << "%" << std::endl;
		std::cout << "Average Critical Success Rate: " << quality.average_critical_success_rate * 100.0 << "%"
		          << std::endl;
		std::cout << std::endl;
	}

	return 0;
}
