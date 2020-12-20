#ifndef PMP_TEST_NORMALIZATION_H
#define PMP_TEST_NORMALIZATION_H

#include <pmp/SurfaceMesh.h>

using namespace pmp;

class Normalization {
public:
	static Point calculate_barycenter_of(SurfaceMesh &mesh);

	static void recenter_barycenter_to_origin(SurfaceMesh &mesh, bool print);

	static Point get_bounds(SurfaceMesh &mesh, bool print);

	static void scale_mesh(SurfaceMesh &mesh, Point scalar);

	static void scale_uniformly_to_bounds(SurfaceMesh &mesh, bool print);

	static Eigen::Matrix3d covariance(const Eigen::MatrixXd &points_matrix);

	static void align(SurfaceMesh &mesh, bool print);

	static Eigen::EigenSolver<Eigen::MatrixXd> get_eigen_matrix(SurfaceMesh &mesh);

	static bool flip_if_necessary(SurfaceMesh &mesh, bool print);
};

#endif //PMP_TEST_NORMALIZATION_H
