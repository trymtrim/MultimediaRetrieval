#include "feature_extraction.h"
#include "features.h"
#include "util.h"
#include "config.h"
#include "database_mr.h"

DatabaseShape FeatureExtraction::get_normalized_shape_features(const DatabaseShape &shape, bool print) {
	DatabaseShape normalized_shape;

	normalized_shape.index = shape.index;
	normalized_shape.filename = shape.filename;

	normalized_shape.a3 = shape.a3;
	normalized_shape.d1 = shape.d1;
	normalized_shape.d2 = shape.d2;
	normalized_shape.d3 = shape.d3;
	normalized_shape.d4 = shape.d4;

	normalized_shape.surface_area = shape.surface_area;
	normalized_shape.compactness = shape.compactness;
	normalized_shape.volume = shape.volume;
	normalized_shape.diameter = shape.diameter;
	normalized_shape.eccentricity = shape.eccentricity;

	std::vector<DatabaseShape> database_shapes = Database::shapes();

	normalized_shape.surface_area_normalized = (shape.surface_area - get_average(SURFACE_AREA, database_shapes)) /
	                                           get_standard_deviation(SURFACE_AREA, database_shapes);
	normalized_shape.compactness_normalized = (shape.compactness - get_average(COMPACTNESS, database_shapes)) /
	                                          get_standard_deviation(COMPACTNESS, database_shapes);
	normalized_shape.volume_normalized = (shape.volume - get_average(VOLUME, database_shapes)) /
	                                     get_standard_deviation(VOLUME, database_shapes);

	double diameter_minus_average = shape.diameter - get_average(DIAMETER, database_shapes);

	if (diameter_minus_average == 0.0)
		normalized_shape.diameter_normalized = 0.0;
	else
		normalized_shape.diameter_normalized = (shape.diameter - get_average(DIAMETER, database_shapes)) /
		                                       get_standard_deviation(DIAMETER, database_shapes);

	normalized_shape.eccentricity_normalized = (shape.eccentricity - get_average(ECCENTRICITY, database_shapes)) /
	                                           get_standard_deviation(ECCENTRICITY, database_shapes);

	if (print)
		std::cout << "Finished normalizing features for:\n" << shape.filename << std::endl;

	return normalized_shape;
}

DatabaseShape FeatureExtraction::get_shape_features(SurfaceMesh &mesh, bool print) {
	DatabaseShape shape;

	if (INCLUDE_FEATURE_SURFACE_AREA)
		shape.surface_area = get_global_descriptor(SURFACE_AREA, mesh, print);
	if (INCLUDE_FEATURE_COMPACTNESS)
		shape.compactness = get_global_descriptor(COMPACTNESS, mesh, print);
	if (INCLUDE_FEATURE_VOLUME)
		shape.volume = get_global_descriptor(VOLUME, mesh, print);
	if (INCLUDE_FEATURE_DIAMETER)
		shape.diameter = get_global_descriptor(DIAMETER, mesh, print);
	if (INCLUDE_FEATURE_ECCENTRICITY)
		shape.eccentricity = get_global_descriptor(ECCENTRICITY, mesh, print);

	if (INCLUDE_FEATURE_A3) {
		std::vector<PropertyHistogramBar> a3_histogram = get_property_descriptor_histogram(A3, HISTOGRAM_BAR_COUNT,
		                                                                                   ITEMS_IN_HISTOGRAM_COUNT,
		                                                                                   mesh, print);
		std::vector<double> a3_histogram_values;
		a3_histogram_values.reserve(a3_histogram.size());
		for (auto &i : a3_histogram)
			a3_histogram_values.push_back(i.normalized_amount_of_items);
		shape.a3 = a3_histogram_values;
	}

	if (INCLUDE_FEATURE_D1) {
		std::vector<PropertyHistogramBar> d1_histogram = get_property_descriptor_histogram(D1, HISTOGRAM_BAR_COUNT,
		                                                                                   ITEMS_IN_HISTOGRAM_COUNT,
		                                                                                   mesh, print);
		std::vector<double> d1_histogram_values;
		d1_histogram_values.reserve(d1_histogram.size());
		for (auto &i : d1_histogram)
			d1_histogram_values.push_back(i.normalized_amount_of_items);
		shape.d1 = d1_histogram_values;
	}

	if (INCLUDE_FEATURE_D2) {
		std::vector<PropertyHistogramBar> d2_histogram = get_property_descriptor_histogram(D2, HISTOGRAM_BAR_COUNT,
		                                                                                   ITEMS_IN_HISTOGRAM_COUNT,
		                                                                                   mesh, print);
		std::vector<double> d2_histogram_values;
		d2_histogram_values.reserve(d2_histogram.size());
		for (auto &i : d2_histogram)
			d2_histogram_values.push_back(i.normalized_amount_of_items);
		shape.d2 = d2_histogram_values;
	}

	if (INCLUDE_FEATURE_D3) {
		std::vector<PropertyHistogramBar> d3_histogram = get_property_descriptor_histogram(D3, HISTOGRAM_BAR_COUNT,
		                                                                                   ITEMS_IN_HISTOGRAM_COUNT,
		                                                                                   mesh, print);
		std::vector<double> d3_histogram_values;
		d3_histogram_values.reserve(d3_histogram.size());
		for (auto &i : d3_histogram)
			d3_histogram_values.push_back(i.normalized_amount_of_items);
		shape.d3 = d3_histogram_values;
	}

	if (INCLUDE_FEATURE_D4) {
		std::vector<PropertyHistogramBar> d4_histogram = get_property_descriptor_histogram(D4, HISTOGRAM_BAR_COUNT,
		                                                                                   ITEMS_IN_HISTOGRAM_COUNT,
		                                                                                   mesh, print);
		std::vector<double> d4_histogram_values;
		d4_histogram_values.reserve(d4_histogram.size());
		for (auto &i : d4_histogram)
			d4_histogram_values.push_back(i.normalized_amount_of_items);
		shape.d4 = d4_histogram_values;
	}

	return shape;
}

double FeatureExtraction::get_global_descriptor(GlobalDescriptor global_descriptor, SurfaceMesh &mesh, bool print) {
	switch (global_descriptor) {
		case SURFACE_AREA:
			return Features::get_surface_area(mesh, print);
		case COMPACTNESS:
			return Features::get_compactness(mesh, print);
		case VOLUME:
			return Features::get_volume(mesh, print);
		case DIAMETER:
			return Features::get_diameter(mesh, print);
		case ECCENTRICITY:
			return Features::get_eccentricity(mesh, print);
	}

	return 0;
}

std::vector<PropertyHistogramBar> FeatureExtraction::get_property_descriptor_histogram(
		PropertyDescriptor property_descriptor, int amount_of_bars, int amount_of_property_items, SurfaceMesh &mesh,
		bool print) {
	std::vector<double> property_descriptor_items = get_property_descriptor(property_descriptor,
	                                                                        amount_of_property_items, mesh, print);

	Eigen::Vector2d lowest_and_highest_number = get_lowest_and_highest_number_from_list(property_descriptor_items);
	double difference = std::abs(lowest_and_highest_number.y() - lowest_and_highest_number.x());

	double bar_range = difference / amount_of_bars;

	std::vector<PropertyHistogramBar> property_descriptor_histogram;

	for (int i = 0; i < amount_of_bars; i++) {
		double range_min = lowest_and_highest_number.x() + bar_range * i;
		double range_max = range_min + bar_range;

		bool is_last_bar = i == amount_of_bars - 1;

		std::vector<double> bar_items = get_property_descriptor_items_in_range(property_descriptor_items, range_min,
		                                                                       range_max, is_last_bar);

		PropertyHistogramBar bar{};
		bar.range_min = range_min;
		bar.range_max = range_max;
		bar.average_of_range = (range_min + range_max) / 2.0;
		bar.amount_of_items = bar_items.size();

		bar.normalized_range_min = Util::map(bar.range_min, lowest_and_highest_number.x(),
		                                     lowest_and_highest_number.y(), 0.0, 1.0);
		bar.normalized_range_max = Util::map(bar.range_max, lowest_and_highest_number.x(),
		                                     lowest_and_highest_number.y(), 0.0, 1.0);
		bar.normalized_average_of_range = Util::map(bar.average_of_range, lowest_and_highest_number.x(),
		                                            lowest_and_highest_number.y(), 0.0, 1.0);
		bar.normalized_amount_of_items = (double) bar.amount_of_items /
		                                 (double) property_descriptor_items.size();

		property_descriptor_histogram.push_back(bar);
	}

	if (print) {
		std::cout << std::endl;

		for (int i = 0; i < property_descriptor_histogram.size(); i++) {
			PropertyHistogramBar bar = property_descriptor_histogram[i];

			std::cout << "---------- BAR " << i + 1 << " ----------" << std::endl;
			std::cout << "Range: " << bar.range_min << " - " << bar.range_max << std::endl;
			std::cout << "Average: " << bar.average_of_range << std::endl;
			std::cout << "Amount: " << bar.amount_of_items << std::endl;
			std::cout << "--- Normalized values: ---\n" << bar.normalized_range_min << " - " << bar.normalized_range_max
			          <<
			          " (" << bar.normalized_average_of_range << "): " << bar.normalized_amount_of_items << " ("
			          << bar.normalized_amount_of_items * 100.0 << "%)" << std::endl;
		}
	}

	return property_descriptor_histogram;
}

std::vector<double>
FeatureExtraction::get_property_descriptor(PropertyDescriptor property_descriptor, int amount, SurfaceMesh &mesh,
                                           bool print) {
	std::vector<double> property_descriptor_array;

	switch (property_descriptor) {
		case A3:
			for (int i = 0; i < amount; i++)
				property_descriptor_array.push_back(Features::get_a3(mesh, print));
			break;
		case D1:
			for (int i = 0; i < amount; i++)
				property_descriptor_array.push_back(Features::get_d1(mesh, print));
			break;
		case D2:
			for (int i = 0; i < amount; i++)
				property_descriptor_array.push_back(Features::get_d2(mesh, print));
			break;
		case D3:
			for (int i = 0; i < amount; i++)
				property_descriptor_array.push_back(Features::get_d3(mesh, print));
			break;
		case D4:
			for (int i = 0; i < amount; i++)
				property_descriptor_array.push_back(Features::get_d4(mesh, print));
			break;
	}

	return property_descriptor_array;
}

Eigen::Vector2d FeatureExtraction::get_lowest_and_highest_number_from_list(const std::vector<double> &number_list) {
	double lowest_number = 999999.0;
	double highest_number = 0.0;

	for (double number : number_list) {
		if (number < lowest_number)
			lowest_number = number;

		if (number > highest_number)
			highest_number = number;
	}

	Eigen::Vector2d highest_and_lowest_number{lowest_number, highest_number};

	return highest_and_lowest_number;
}

std::vector<double>
FeatureExtraction::get_property_descriptor_items_in_range(const std::vector<double> &item_list,
                                                          float range_min,
                                                          float range_max,
                                                          bool is_last_bar) {
	std::vector<double> items_in_range;

	for (double item : item_list) {
		if (is_last_bar) {
			if (item >= range_min && item <= range_max)
				items_in_range.push_back(item);
		} else if (item >= range_min && item < range_max) {
			items_in_range.push_back(item);
		}
	}

	return items_in_range;
}

double FeatureExtraction::get_average(GlobalDescriptor global_descriptor, const std::vector<DatabaseShape> &shapes) {
	std::vector<double> numbers;

	switch (global_descriptor) {
		case SURFACE_AREA:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.surface_area);
			return Util::average(numbers);
		case COMPACTNESS:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.compactness);
			return Util::average(numbers);
		case VOLUME:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.volume);
			return Util::average(numbers);
		case DIAMETER:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.diameter);
			return Util::average(numbers);
		case ECCENTRICITY:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.eccentricity);
			return Util::average(numbers);
	}

	return 0.0;
}

double
FeatureExtraction::get_standard_deviation(GlobalDescriptor global_descriptor,
                                          const std::vector<DatabaseShape> &shapes) {
	std::vector<double> numbers;

	switch (global_descriptor) {
		case SURFACE_AREA:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.surface_area);
			return Util::standard_deviation(numbers);
		case COMPACTNESS:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.compactness);
			return Util::standard_deviation(numbers);
		case VOLUME:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.volume);
			return Util::standard_deviation(numbers);
		case DIAMETER:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.diameter);
			return Util::standard_deviation(numbers);
		case ECCENTRICITY:
			for (const DatabaseShape &shape : shapes)
				numbers.push_back(shape.eccentricity);
			return Util::standard_deviation(numbers);
	}

	return 0.0;
}
