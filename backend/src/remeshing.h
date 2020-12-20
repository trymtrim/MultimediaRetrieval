#pragma once

#include <pmp/SurfaceMesh.h>

using namespace pmp;

class Remeshing {
public:
    static void remesh_to_vertex_count(SurfaceMesh &mesh, int target_vertex_count, bool print);


private:
    enum EdgeLengthOption {
        SHORTEST_EDGE,
        LONGEST_EDGE
    };

    static void remesh(SurfaceMesh &mesh, float min_edge_length, float max_edge_length);

    static float get_edge_length(SurfaceMesh &mesh, EdgeLengthOption edge_length_option);
};

