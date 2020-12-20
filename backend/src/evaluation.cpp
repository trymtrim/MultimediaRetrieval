#include "evaluation.h"
#include "database_mr.h"
#include "feature_matching.h"

std::vector<QualityValues> Evaluation::get_quality_values(std::string database_path) {
	std::map<std::string, QualityValues> mapped_quality_values;

	Database::open(database_path);
	std::vector<DatabaseShape> shapes = Database::shapes();

	int counter = 0;

	for (const DatabaseShape &shape : shapes) {
		std::cout << "Querying " << shape.filename << std::endl;

		std::string class_name = get_class_name(shape.filename);

		if (!mapped_quality_values.count(class_name)) {
			QualityValues quality_values_instance;
			quality_values_instance.class_name = class_name;
			quality_values_instance.shape_count = 0;
			quality_values_instance.average_true_positives = 0.0;
			quality_values_instance.average_false_positives = 0.0;
			quality_values_instance.average_false_negatives = 0.0;
			quality_values_instance.average_true_positive_rate = 0.0;
			quality_values_instance.average_false_negative_rate = 0.0;
			quality_values_instance.average_critical_success_rate = 0.0;

			mapped_quality_values.emplace(class_name, quality_values_instance);
		}

		QualityValues new_quality_values = mapped_quality_values[class_name];
		new_quality_values.shape_count++;

		std::vector<DatabaseShape> similar_shapes = FeatureMatching::get_similar_shapes(shape, shapes, Database::metadata().feature_matching_method);

		int true_positives = 0;
		int false_positives = 0;

		for (auto &similar_shape : similar_shapes) {
			std::string similar_shape_class_name = get_class_name(similar_shape.filename);

			if (class_name == similar_shape_class_name)
				true_positives++;
			else
				false_positives++;
		}

		new_quality_values.average_true_positives += true_positives;
		new_quality_values.average_false_positives += false_positives;

		mapped_quality_values[class_name] = new_quality_values;

		counter++;
		std::cout << counter << "/" << shapes.size() << std::endl;
	}

	std::vector<QualityValues> returned_quality_values;

	for (const auto &x : mapped_quality_values) {
		QualityValues quality_values = x.second;

		quality_values.average_false_negatives =
				(quality_values.shape_count * quality_values.shape_count - quality_values.average_true_positives) /
				quality_values.shape_count;

		quality_values.average_true_positives /= quality_values.shape_count;
		quality_values.average_false_positives /= quality_values.shape_count;

		quality_values.average_true_positive_rate = quality_values.average_true_positives /
		                                            (quality_values.average_true_positives +
		                                             quality_values.average_false_negatives);
		quality_values.average_false_negative_rate = quality_values.average_false_negatives /
		                                             (quality_values.average_false_negatives +
		                                              quality_values.average_true_positives);
		quality_values.average_critical_success_rate = quality_values.average_true_positives /
		                                               (quality_values.average_true_positives +
		                                                quality_values.average_false_negatives +
		                                                quality_values.average_false_positives);

		returned_quality_values.push_back(quality_values);
	}

	double total_shape_count = 0;
	double total_true_positive_rate = 0;
	double total_false_negative_rate = 0;
	double total_critical_success_rate = 0;

	for (const QualityValues &quality_values : returned_quality_values) {
		total_shape_count += quality_values.shape_count;
		total_true_positive_rate += quality_values.average_true_positive_rate;
		total_false_negative_rate += quality_values.average_false_negative_rate;
		total_critical_success_rate += quality_values.average_critical_success_rate;
	}

	QualityValues database_quality_values;
	database_quality_values.class_name = "Database";
	database_quality_values.shape_count = total_shape_count;
	database_quality_values.average_true_positive_rate = total_true_positive_rate / returned_quality_values.size();
	database_quality_values.average_false_negative_rate = total_false_negative_rate / returned_quality_values.size();
	database_quality_values.average_critical_success_rate =
			total_critical_success_rate / returned_quality_values.size();

	database_quality_values.average_true_positives = 0;
	database_quality_values.average_false_positives = 0;
	database_quality_values.average_false_negatives = 0;

	returned_quality_values.push_back(database_quality_values);

	Database::close();

	return returned_quality_values;
}

std::string Evaluation::get_class_name(std::string file_name) {
	int shape_index = std::stoi(file_name.erase(file_name.find_first_of('.'), 4));

	if (shape_index >= 1 && shape_index <= 20)
		return "Human";
	else if (shape_index >= 21 && shape_index <= 40)
		return "Cup";
	else if (shape_index >= 41 && shape_index <= 60)
		return "Glasses";
	else if (shape_index >= 61 && shape_index <= 80)
		return "Airplane";
	else if (shape_index >= 81 && shape_index <= 100)
		return "Ant";
	else if (shape_index >= 101 && shape_index <= 120)
		return "Chair";
	else if (shape_index >= 121 && shape_index <= 140)
		return "Octopus";
	else if (shape_index >= 141 && shape_index <= 160)
		return "Table";
	else if (shape_index >= 161 && shape_index <= 180)
		return "Teddy";
	else if (shape_index >= 181 && shape_index <= 200)
		return "Hand";
	else if (shape_index >= 201 && shape_index <= 220)
		return "Plier";
	else if (shape_index >= 221 && shape_index <= 240)
		return "Fish";
	else if (shape_index >= 241 && shape_index <= 260)
		return "Bird";
	else if (shape_index >= 281 && shape_index <= 300)
		return "Armadillo";
	else if (shape_index >= 301 && shape_index <= 320)
		return "Bust";
	else if (shape_index >= 321 && shape_index <= 340)
		return "Mech";
	else if (shape_index >= 341 && shape_index <= 360)
		return "Bearing";
	else if (shape_index >= 361 && shape_index <= 380)
		return "Vase";
	else if (shape_index >= 381 && shape_index <= 400)
		return "FourLeg";

	return file_name + " is not a valid mesh file";
}
