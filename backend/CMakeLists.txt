cmake_minimum_required(VERSION 3.17)
project(backend)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/ann/include)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp)

add_executable(backend
        thirdparty/ann/include/ANN/ANN.h
        thirdparty/ann/include/ANN/ANNperf.h
        thirdparty/ann/include/ANN/ANNx.h
        thirdparty/ann/src/ANN.cpp
        thirdparty/ann/src/bd_fix_rad_search.cpp
        thirdparty/ann/src/bd_pr_search.cpp
        thirdparty/ann/src/bd_search.cpp
        thirdparty/ann/src/bd_tree.cpp
        thirdparty/ann/src/bd_tree.h
        thirdparty/ann/src/brute.cpp
        thirdparty/ann/src/kd_dump.cpp
        thirdparty/ann/src/kd_fix_rad_search.cpp
        thirdparty/ann/src/kd_fix_rad_search.h
        thirdparty/ann/src/kd_pr_search.cpp
        thirdparty/ann/src/kd_pr_search.h
        thirdparty/ann/src/kd_search.cpp
        thirdparty/ann/src/kd_search.h
        thirdparty/ann/src/kd_split.cpp
        thirdparty/ann/src/kd_split.h
        thirdparty/ann/src/kd_tree.cpp
        thirdparty/ann/src/kd_tree.h
        thirdparty/ann/src/kd_util.cpp
        thirdparty/ann/src/kd_util.h
        thirdparty/ann/src/perf.cpp
        thirdparty/ann/src/pr_queue_k.h
        thirdparty/ann/src/pr_queue.h
        thirdparty/Wasserstein/wasserstein.h
        src/backend.cpp
        src/config.h
        src/normalization.cpp
        src/normalization.h
        src/preprocessing.cpp
        src/preprocessing.h
        src/remeshing.cpp
        src/remeshing.h
        src/util.cpp
        src/util.h
        src/action.h
        src/actions/version.cpp
        src/actions/version.h
        src/actions/authors.cpp
        src/actions/authors.h
        src/actions/normalize.cpp
        src/actions/normalize.h
        src/actions/extract.cpp
        src/actions/extract.h
        src/actions/store.cpp
        src/actions/store.h
        src/database_mr.cpp
        src/database_mr.h
        src/features.h
        src/features.cpp
        src/feature_extraction.h
        src/feature_extraction.cpp
        src/action_args.cpp
        src/action_args.h
        src/actions/query.cpp
        src/actions/query.h
        src/feature_matching.cpp
        src/feature_matching.h
        src/evaluation.cpp
        src/evaluation.h
        src/actions/evaluate.cpp
        src/actions/evaluate.h)

find_package(Boost REQUIRED ALL)
find_package(Eigen3 REQUIRED)
find_package(pmp REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(${Boost_INCLUDE_DIR})
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_LIST_DIR}/thirdparty/SQLiteCpp/include)

target_link_libraries(backend ${Boost_LIBRARIES})
target_link_libraries(backend Eigen3::Eigen)
target_link_libraries(backend pmp)
target_link_libraries(backend ${SQLite3_LIBRARIES})
target_link_libraries(backend SQLiteCpp sqlite3 pthread dl)