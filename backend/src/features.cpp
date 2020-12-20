#include "features.h"
#include "util.h"
#include "normalization.h"

double Features::get_surface_area(SurfaceMesh &mesh, bool print)
{
	// Shape surface area

	auto points = mesh.get_vertex_property<Point>("v:point");

	double surface_area = 0.0;

	for (auto face : mesh.faces())
	{
		Eigen::Vector3d a;
		Eigen::Vector3d b;
		Eigen::Vector3d c;

		int i = 0;

		for (pmp::Vertex vertex : SurfaceMesh::VertexAroundFaceCirculator(&mesh, face))
		{
			switch (i)
			{
			case 0:
				a = points[vertex];
				break;
			case 1:
				b = points[vertex];
				break;
			case 2:
				c = points[vertex];
				break;
			}

			i++;
		}

		double ab = Util::distance_between_two_points(a, b);
		double ac = Util::distance_between_two_points(a, c);
		double bc = Util::distance_between_two_points(b, c);

		double p = (ab + bc + ac) / 2.0;
		double area = std::sqrt(p * (p - ab) * (p - bc) * (p - ac));

		surface_area += area;
	}

	if (print)
		std::cout << "Surface area: " << surface_area << std::endl;

	return surface_area;
}

double Features::get_compactness(SurfaceMesh &mesh, bool print)
{
	// Compactness (with respect to a sphere)
	
	double compactness = std::pow(get_surface_area(mesh, false), 1.5) / get_volume(mesh, false);

	if (print)
		std::cout << "Compactness: " << compactness << std::endl;

	return compactness;
}

double Features::get_volume(SurfaceMesh &mesh, bool print)
{
	// Axis-aligned bounding-box volume
	auto points = mesh.get_vertex_property<Point>("v:point");

	Eigen::Vector3d barycenter = Eigen::Vector3d{ 0.0, 0.0, 0.0 }; // Assuming barycenter is (0,0,0)

	double sum_of_tris = 0.0f;

	for (auto face : mesh.faces())
	{
		Eigen::Vector3d v1;
		Eigen::Vector3d v2;
		Eigen::Vector3d v3;

		int i = 0;

		for (pmp::Vertex vertex : SurfaceMesh::VertexAroundFaceCirculator(&mesh, face))
		{
			switch (i)
			{
			case 0:
				v1 = points[vertex];
				break;
			case 1:
				v2 = points[vertex];
				break;
			case 2:
				v3 = points[vertex];
				break;
			}

			i++;
		}

		v1 -= barycenter;
		v2 -= barycenter;
		v3 -= barycenter;

		sum_of_tris += Util::dot(Util::cross(v1, v2), v3);
	}

	double volume = (1.0 / 6.0) * std::abs(sum_of_tris);

	if (print)
		std::cout << "Volume: " << volume << std::endl;

	return volume;
}

double Features::get_diameter(SurfaceMesh &mesh, bool print)
{
	// Shape diameter
	// Doing this using brute-force - this could be improved

	double diameter = 0.0f;

	auto points = mesh.get_vertex_property<Point>("v:point");
	int vertex_count = mesh.n_vertices();

	for (int i = 0; i < vertex_count; i++)
	{
		for (int j = 0; j < vertex_count; j++)
		{
			double distance = Util::distance_between_two_points(points.data()[i], points.data()[j]);

			if (distance > diameter)
				diameter = distance;
		}
	}

	if (print)
		std::cout << "Diameter: " << diameter << std::endl;

	return diameter;
}

double Features::get_eccentricity(SurfaceMesh &mesh, bool print)
{
	// Eccentricity (ratio of largest to smallest eigenvalues of covariance matrix)

	Eigen::Vector3cd eigen_values = Normalization::get_eigen_matrix(mesh).eigenvalues();

	double largest_eigen_value = 0;
	double smallest_eigen_value = 1;

	for (int i = 0; i < 3; i++) {
		double this_eigen_value = eigen_values[i].real();
		if (this_eigen_value > largest_eigen_value)
			largest_eigen_value = this_eigen_value;
	}

	for (int i = 0; i < 3; i++) {
		double this_eigen_value = eigen_values[i].real();
		if (this_eigen_value < smallest_eigen_value)
			smallest_eigen_value = this_eigen_value;
	}

	double eccentricity = largest_eigen_value / smallest_eigen_value;

	if (print)
		std::cout << "Eccentricity (ratio of largest to smallest eigenvalues): " << eccentricity << " : 1" << std::endl;

	return eccentricity;
}

double Features::get_a3(SurfaceMesh &mesh, bool print)
{
	// A3: angle between 3 random vertices
	// TODO: Most of this code has straight up been copy pasted, needs to be tested
	// TODO: There is a possibility that we get the same random vertex more than once, that should preferably not happen

	auto points = mesh.get_vertex_property<Point>("v:point");
	int vertex_count = mesh.n_vertices();

	std::vector<int> random_numbers = Util::random_numbers(3, vertex_count);

	int random_vertex_index_a = random_numbers[0];
	int random_vertex_index_b = random_numbers[1];
	int random_vertex_index_c = random_numbers[2];

	Eigen::Vector3d a = points.data()[random_vertex_index_a];
	Eigen::Vector3d b = points.data()[random_vertex_index_b];
	Eigen::Vector3d c = points.data()[random_vertex_index_c];

	double x1 = a.x(), y1 = a.y(), z1 = a.z();
	double x2 = b.x(), y2 = b.y(), z2 = b.z();
	double x3 = c.x(), y3 = c.y(), z3 = c.z();

	// Find direction ratio of line AB 
	double ABx = x1 - x2;
	double ABy = y1 - y2;
	double ABz = z1 - z2;

	// Find direction ratio of line BC 
	double BCx = x3 - x2;
	double BCy = y3 - y2;
	double BCz = z3 - z2;

	// Find the dotProduct of lines AB & BC 
	double dotProduct = ABx * BCx + ABy * BCy + ABz * BCz;

	// Find magnitude of line AB and BC 
	double magnitudeAB = ABx * ABx + ABy * ABy + ABz * ABz;
	double magnitudeBC = BCx * BCx + BCy * BCy + BCz * BCz;

	// Find the cosine of the angle formed by line AB and BC 
	double angle = dotProduct;
	angle /= sqrt(magnitudeAB * magnitudeBC);

	double pi = 3.14159265358979323846f;

	// Find angle in radian 
	angle = std::abs((angle * 180) / pi);

	if (print)
		std::cout << "Angle between vertices [" << random_vertex_index_a << "]" <<
		"[" << random_vertex_index_b << "]" << "[" << random_vertex_index_c << "]: " << angle << std::endl;

	return angle;
}

double Features::get_d1(SurfaceMesh &mesh, bool print)
{
	// D1: distance between barycenter and random vertex
	// Here we assume the barycenter is (0,0,0)

	auto points = mesh.get_vertex_property<Point>("v:point");

	std::vector<int> random_numbers = Util::random_numbers(1, mesh.n_vertices());

	int random_vertex_index = random_numbers[0];

	Eigen::Vector3d a = Eigen::Vector3d{ 0.0, 0.0, 0.0 };
	Eigen::Vector3d b = points.data()[random_vertex_index];

	double distance = Util::distance_between_two_points(a, b);

	if (print)
		std::cout << "Distance between barycenter and vertex [" << random_vertex_index << "]: " << distance << std::endl;

	return distance;
}

double Features::get_d2(SurfaceMesh &mesh, bool print)
{
	// D2: distance between 2 random vertices
	// TODO: There is a possibility that we get the same random vertex more than once, that should preferably not happen

	auto points = mesh.get_vertex_property<Point>("v:point");
	int vertex_count = mesh.n_vertices();

	std::vector<int> random_numbers = Util::random_numbers(2, vertex_count);

	int random_vertex_index_a = random_numbers[0];
	int random_vertex_index_b = random_numbers[1];

	Eigen::Vector3d a = points.data()[random_vertex_index_a];
	Eigen::Vector3d b = points.data()[random_vertex_index_b];

	double distance = Util::distance_between_two_points(a, b);

	if (print)
		std::cout << "Distance between vertices [" << random_vertex_index_a << "]" << "[" << random_vertex_index_b << "]: " << distance << std::endl;

	return distance;
}

double Features::get_d3(SurfaceMesh &mesh, bool print)
{
	// D3: square root of area of triangle given by 3 random vertices
	// TODO: We might be doing one sqrt too much (?) - not sure if it even matters though
	// TODO: There is a possibility that we get the same random vertex more than once, that should preferably not happen

	auto points = mesh.get_vertex_property<Point>("v:point");
	int vertex_count = mesh.n_vertices();

	std::vector<int> random_numbers = Util::random_numbers(3, vertex_count);

	int random_vertex_index_a = random_numbers[0];
	int random_vertex_index_b = random_numbers[1];
	int random_vertex_index_c = random_numbers[2];

	Eigen::Vector3d a = points.data()[random_vertex_index_a];
	Eigen::Vector3d b = points.data()[random_vertex_index_b];
	Eigen::Vector3d c = points.data()[random_vertex_index_c];

	double ab = Util::distance_between_two_points(a, b);
	double ac = Util::distance_between_two_points(a, c);
	double bc = Util::distance_between_two_points(b, c);

	double p = (ab + bc + ac) / 2.0;
	double area = std::sqrt(p * (p - ab) * (p - bc) * (p - ac));

	double sqrt_of_area = std::sqrt(area);

	if (print)
		std::cout << "Square root of area of vertices [" << random_vertex_index_a << "]" <<
		"[" << random_vertex_index_b << "]" << "[" << random_vertex_index_c << "]: " << sqrt_of_area << std::endl;

	return sqrt_of_area;
}

double Features::get_d4(SurfaceMesh &mesh, bool print)
{
	// D4: cube root of volume of tetrahedron formed by 4 random vertices
	// TODO: There is a possibility that we get the same random vertex more than once, that should preferably not happen

	auto points = mesh.get_vertex_property<Point>("v:point");
	int vertex_count = mesh.n_vertices();

	std::vector<int> random_numbers = Util::random_numbers(4, vertex_count);

	int random_vertex_index_a = random_numbers[0];
	int random_vertex_index_b = random_numbers[1];
	int random_vertex_index_c = random_numbers[2];
	int random_vertex_index_d = random_numbers[3];

	Eigen::Vector3d a = points.data()[random_vertex_index_a];
	Eigen::Vector3d b = points.data()[random_vertex_index_b];
	Eigen::Vector3d c = points.data()[random_vertex_index_c];
	Eigen::Vector3d d = points.data()[random_vertex_index_d];

	double u = Util::distance_between_two_points(b, c);
	double v = Util::distance_between_two_points(a, c);
	double w = Util::distance_between_two_points(c, d);
	double U = Util::distance_between_two_points(a, d);
	double V = Util::distance_between_two_points(b, d);
	double W = Util::distance_between_two_points(a, b);

	double uPow = std::pow(u, 2);
	double vPow = std::pow(v, 2);
	double wPow = std::pow(w, 2);
	double UPow = std::pow(U, 2);
	double VPow = std::pow(V, 2);
	double WPow = std::pow(W, 2);

	double temp_volume = 4 * (uPow * vPow * wPow)
		- uPow * std::pow((vPow + wPow - UPow), 2)
		- vPow * std::pow((wPow + uPow - VPow), 2)
		- wPow * std::pow((uPow + vPow - WPow), 2)
		+ (vPow + wPow - UPow) * (wPow + uPow - VPow)
		* (uPow + vPow - WPow);

	double volume = sqrt(temp_volume);

	double cbrt_of_volume = std::cbrt(volume);

	if (print)
		std::cout << "Cube root of volume of vertices [" << random_vertex_index_a << "]" << "["
		<< random_vertex_index_b << "]" << "[" << random_vertex_index_c << "]" << "[" << random_vertex_index_d << "]: "
		<< cbrt_of_volume << std::endl;

	return cbrt_of_volume;
}
