#include "normalization.h"
#include "config.h"
#include <iostream>
#include <math.h>
#include "util.h"

Point Normalization::calculate_barycenter_of(SurfaceMesh &mesh) {
	const auto points = mesh.get_vertex_property<Point>("v:point");

	Point p(0, 0, 0);

	for (auto vit : mesh.vertices())
		p += points[vit];

	p /= mesh.n_vertices();

	return p;
}

void Normalization::recenter_barycenter_to_origin(SurfaceMesh &mesh, bool print) {
	// Get barycenter
	const auto barycenter = calculate_barycenter_of(mesh);
	if (print)
		std::cout << "barycenter: " << barycenter << std::endl;

	// Center the shape on the barycenter
	auto points = mesh.get_vertex_property<Point>("v:point");
	for (auto vit : mesh.vertices())
		points[vit] -= barycenter;
}

Point Normalization::get_bounds(SurfaceMesh &mesh, bool print) {
	const auto bounds_min = mesh.bounds().min();
	const auto bounds_max = mesh.bounds().max();

	if (print) {
		std::cout << "bounding box (min): " << bounds_min << std::endl;
		std::cout << "bounding box (max): " << bounds_max << std::endl;
	}

	return bounds_max - bounds_min;
}

void Normalization::scale_mesh(SurfaceMesh &mesh, Point scalar) {
	auto points = mesh.get_vertex_property<Point>("v:point");
	for (auto vit : mesh.vertices()) {
		for (int d = 0; d < 3; d++) {
			points[vit].data()[d] = points[vit].data()[d] * scalar.data()[d];
		}
	}
}

void Normalization::scale_uniformly_to_bounds(SurfaceMesh &mesh, bool print) {
	const auto bounds = get_bounds(mesh, print);

	auto max_bound = 0.0f;
	for (int d = 0; d < 3; d++) {
		const auto this_bound = bounds.data()[d];
		if (max_bound < this_bound)
			max_bound = this_bound;
	}

	const auto scalar_value = BOUNDING_BOX_EDGE_LENGTH / max_bound;
	const Point scalar(scalar_value, scalar_value, scalar_value);

	scale_mesh(mesh, scalar);
}

Eigen::Matrix3d Normalization::covariance(const Eigen::MatrixXd &points_matrix) {
	Eigen::MatrixXd centered = points_matrix.rowwise() - points_matrix.colwise().mean();
	return (centered.adjoint() * centered) / double(points_matrix.rows() - 1);
}

void Normalization::align(SurfaceMesh &mesh, bool print) {
	auto eigen_matrix = get_eigen_matrix(mesh);

	if (print) {
		std::cout << "eigenvalues:" << std::endl;
		std::cout << eigen_matrix.eigenvalues() << std::endl;
		std::cout << "eigenvectors:" << std::endl;
		std::cout << eigen_matrix.eigenvectors() << std::endl;
	}

	Eigen::Vector3cd eigen_values = eigen_matrix.eigenvalues();

	int major_eigen_value_index = -1;
	double major_eigen_value = 0;
	for (int i = 0; i < 3; i++) {
		double this_eigen_value = eigen_values[i].real();
		if (this_eigen_value > major_eigen_value) {
			major_eigen_value = this_eigen_value;
			major_eigen_value_index = i;
		}
	}

	int medium_eigen_value_index = -1;
	double medium_eigen_value = 0;
	for (int j = 0; j < 3; j++) {
		if (j == major_eigen_value_index)
			continue;

		double this_eigen_value = eigen_values[j].real();
		if (this_eigen_value > medium_eigen_value) {
			medium_eigen_value = this_eigen_value;
			medium_eigen_value_index = j;
		}
	}

	Eigen::Matrix3cd eigen_vectors = eigen_matrix.eigenvectors();

	Eigen::Vector3d major_eigen_vector = eigen_vectors.col(major_eigen_value_index).real();
	Eigen::Vector3d medium_eigen_vector = eigen_vectors.col(medium_eigen_value_index).real();

	auto points = mesh.get_vertex_property<Point>("v:point");

	for (auto vit : mesh.vertices()) {
		// Assuming barycenter is (0,0,0)
		Eigen::Vector3d vertex_vector{ points[vit].data()[0], points[vit].data()[1], points[vit].data()[2] };

		points[vit].data()[0] = Util::dot((vertex_vector - Eigen::Vector3d{ 0, 0, 0 }), major_eigen_vector);
		points[vit].data()[1] = Util::dot((vertex_vector - Eigen::Vector3d{ 0, 0, 0 }), medium_eigen_vector);
		points[vit].data()[2] = Util::dot((vertex_vector - Eigen::Vector3d{ 0, 0, 0 }),
			Util::cross(major_eigen_vector, medium_eigen_vector));
	}
}

Eigen::EigenSolver<Eigen::MatrixXd> Normalization::get_eigen_matrix(SurfaceMesh &mesh) {
	Eigen::MatrixXd matrix;
	matrix.conservativeResize(mesh.n_vertices(), 3);

	auto points = mesh.get_vertex_property<Point>("v:point");

	int i = 0;
	for (auto vit : mesh.vertices()) {
		Eigen::Vector3d temp_vector = Eigen::Vector3d{ points[vit].data()[0], points[vit].data()[1],
													  points[vit].data()[2] };
		matrix.row(i) = temp_vector;

		i++;
	}

	auto cov = covariance(matrix);
	Eigen::EigenSolver<Eigen::MatrixXd> eigen_matrix(cov);

	return eigen_matrix;
}

bool Normalization::flip_if_necessary(SurfaceMesh &mesh, bool print) {
	// Use the moment test to flip the shape along the 3 axes

	// Compute a value (-1/1) along each axis

	int fx = 0, fy = 0, fz = 0;

	// Go over all faces in the mesh
	for (auto face : mesh.faces()) {
		Eigen::Vector3f center_of_face;
		auto points = mesh.get_vertex_property<Point>("v:point");
		int i = 0;

		// Go over all vertices in the face
		for (auto vertex : mesh.vertices(face)) {
			center_of_face[i] += points[vertex].data()[i];
			i++;
		}

		// Center of face
		center_of_face /= 3;

		fx += center_of_face[0] > 0.0f ? 1 : -1;
		fy += center_of_face[1] > 0.0f ? 1 : -1;
		fz += center_of_face[2] > 0.0f ? 1 : -1;
	}

	fx = fx > 0 ? 1 : -1;
	fy = fy > 0 ? 1 : -1;
	fz = fz > 0 ? 1 : -1;

	if (print)
		std::cout << "Flipping: " << "x: " << fx << " y: " << fy << " z: " << fz << std::endl;

	Point scale_scalar;
	scale_scalar.data()[0] = (float)fx;
	scale_scalar.data()[1] = (float)fy;
	scale_scalar.data()[2] = (float)fz;

	scale_mesh(mesh, scale_scalar);

	int number_of_flipped = 0;
	for (int i = 0; i < 3; i++) {
		if (scale_scalar.data()[i] == -1) {
			number_of_flipped++;
		}
	}

	auto should_flip_all_normals = (number_of_flipped == 1) || (number_of_flipped == 3);

	return should_flip_all_normals;
}