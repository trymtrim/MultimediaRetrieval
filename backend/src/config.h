#ifndef BACKEND_CONFIG_H
#define BACKEND_CONFIG_H

static const int BOUNDING_BOX_EDGE_LENGTH = 1;
static const int VERSION_MAJOR = 0;
static const int VERSION_MINOR = 8;

static const bool PRINT_DB_ERRORS = true;

static const int REMESHING_TARGET_VERTEX_COUNT = 10000;
static const int HISTOGRAM_BAR_COUNT = 10;
static const int ITEMS_IN_HISTOGRAM_COUNT = 1000000;

static const bool INCLUDE_FEATURE_SURFACE_AREA = true;
static const bool INCLUDE_FEATURE_COMPACTNESS = true;
static const bool INCLUDE_FEATURE_VOLUME = true;
static const bool INCLUDE_FEATURE_DIAMETER = true;
static const bool INCLUDE_FEATURE_ECCENTRICITY = true;
static const bool INCLUDE_FEATURE_A3 = true;
static const bool INCLUDE_FEATURE_D1 = true;
static const bool INCLUDE_FEATURE_D2 = true;
static const bool INCLUDE_FEATURE_D3 = true;
static const bool INCLUDE_FEATURE_D4 = true;

#endif //BACKEND_CONFIG_H