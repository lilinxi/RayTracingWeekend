set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/RestLife/bin)
add_executable(simple_monte_carlo test/simple_monte_carlo.cpp)
add_executable(one_dimen_monte_carlo test/one_dimen_monte_carlo.cpp)
add_executable(sphere_dirs_monte_carlo test/sphere_dirs_monte_carlo.cpp)
add_executable(test_importance_sample_1 test/test_importance_sample_1.cpp)
add_executable(test_random_directions_1 test/test_random_directions_1.cpp)
add_executable(test_random_directions_2 test/test_random_directions_2.cpp)
add_executable(test_onb test/test_onb.cpp)
add_executable(test_sample_light_1 test/test_sample_light_1.cpp)
add_executable(test_sample_light_2 test/test_sample_light_2.cpp)
add_executable(test_mixture_densities_1 test/test_mixture_densities_1.cpp)
add_executable(test_mixture_densities_2 test/test_mixture_densities_2.cpp)
add_executable(test_mixture_densities_3 test/test_mixture_densities_3.cpp)