#include "remeshing.h"
#include <pmp/algorithms/SurfaceRemeshing.h>

//void remesh(SurfaceMesh &mesh, float target_edge_length)
//{
//	float min_edge_length = target_edge_length * 0.95f;
//	float max_edge_length = target_edge_length * 1.05f;
//	float approx_error = target_edge_length * 0.5f;
//
//	std::cout << "Target edge length: " << target_edge_length << std::endl;
//
//	std::cout << "min: " << min_edge_length << std::endl;
//	std::cout << "max: " << max_edge_length << std::endl;
//	std::cout << "err: " << approx_error << std::endl;
//
//	SurfaceRemeshing(mesh).adaptive_remeshing(
//		min_edge_length,
//		max_edge_length,
//		approx_error);
//
//
//	float multiplier = current_vertex_count > target_vertex_count ? 2.0f : 0.5f; //(float)current_vertex_count / (float)target_vertex_count;
//	remesh(mesh, min_edge_length * multiplier, max_edge_length * multiplier);
//
//
//	float test = (float)current_vertex_count / (float)target_vertex_count;
//
//	float sum_of_all_edge_lengths = 0.0f;
//	for (auto edge : mesh.edges())
//		sum_of_all_edge_lengths += mesh.edge_length(edge);
//	float current_avg_edge_length = sum_of_all_edge_lengths / mesh.n_edges();
//
//	std::cout << "Current average edge length: " << current_avg_edge_length << std::endl;
//
//	float multiplier = test; // current_vertex_count > target_vertex_count ? 1.05f : 0.95f;
//	std::cout << "Multiplier: " << multiplier << std::endl;
//	remesh(mesh, current_avg_edge_length * multiplier);
//
//
//	float multiplier = current_vertex_count > target_vertex_count ? 0.01f : -0.01f; //(float)current_vertex_count / (float)target_vertex_count;
//	min_edge_length += multiplier;
//	max_edge_length += multiplier;
//}

void Remeshing::remesh_to_vertex_count(SurfaceMesh &mesh, int target_vertex_count, bool print) {
    SurfaceMesh test_mesh = mesh;

    int current_vertex_count = test_mesh.n_vertices();

    if (print) {
		std::cout << "Remeshing started." << std::endl;
        std::cout << "Current vertex count: " << current_vertex_count << std::endl;
        std::cout << "Target vertex count: " << target_vertex_count << std::endl;
    }

    float max_deviation = 250.0f;

    float min_edge_length = get_edge_length(test_mesh, SHORTEST_EDGE);
    float max_edge_length = get_edge_length(test_mesh, LONGEST_EDGE);
	
    remesh(test_mesh, min_edge_length, max_edge_length);

    current_vertex_count = test_mesh.n_vertices();

    if (print) {
        std::cout << "Current vertex count: " << current_vertex_count << std::endl;
        std::cout << "Target vertex count: " << target_vertex_count << std::endl;
    }

    min_edge_length = get_edge_length(test_mesh, SHORTEST_EDGE);
    max_edge_length = get_edge_length(test_mesh, LONGEST_EDGE);

    int max_iterations = 50;
    int iterations_completed = 0;

    while (std::abs(current_vertex_count - target_vertex_count) > max_deviation) {
        test_mesh = mesh;

        float vertex_count_difference = std::abs(current_vertex_count - target_vertex_count);

        if (vertex_count_difference > 10000.0f)
            vertex_count_difference = 10000.0f;

        float mapped_difference = (vertex_count_difference - 0.0f) / (10000.0f - 0.0f) * (0.01f - 0.0f) + 0.0f;
        mapped_difference = current_vertex_count > target_vertex_count ? mapped_difference : -mapped_difference;

        min_edge_length += mapped_difference;
        max_edge_length += mapped_difference;

        remesh(test_mesh, min_edge_length, max_edge_length);

        current_vertex_count = test_mesh.n_vertices();

        if (print) {
            std::cout << "Current vertex count: " << current_vertex_count << std::endl;
            std::cout << "Target vertex count: " << target_vertex_count << std::endl;
        }

        iterations_completed++;

        // Failsafe
        if (iterations_completed == max_iterations)
            break;
    }

    std::cout << "Applying remeshing..." << std::endl;

    remesh(mesh, min_edge_length, max_edge_length);

    std::cout << "Remeshing completed." << std::endl;
}

void Remeshing::remesh(SurfaceMesh &mesh, float min_edge_length, float max_edge_length) {
    float approx_error = 0.0005f * mesh.bounds().size();

    std::cout << "min: " << min_edge_length << std::endl;
    std::cout << "max: " << max_edge_length << std::endl;
    std::cout << "err: " << approx_error << std::endl;

    SurfaceRemeshing(mesh).adaptive_remeshing(
            min_edge_length,
            max_edge_length,
            approx_error);
}

float Remeshing::get_edge_length(SurfaceMesh &mesh, EdgeLengthOption edge_length_option) {
    float min_edge_length = 100000.0f;
    float max_edge_length = 0.0f;

    for (auto edge : mesh.edges()) {
        float edge_length = mesh.edge_length(edge);

        if (edge_length < min_edge_length)
            min_edge_length = edge_length;
        else if (edge_length > max_edge_length)
            max_edge_length = edge_length;
    }

    return edge_length_option == SHORTEST_EDGE ? min_edge_length : max_edge_length;
}
