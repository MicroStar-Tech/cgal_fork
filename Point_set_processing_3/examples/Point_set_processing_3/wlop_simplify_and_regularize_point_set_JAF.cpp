#include <CGAL/Simple_cartesian.h>

#include <CGAL/IO/read_points.h>
#include <CGAL/IO/write_points.h>
#include <CGAL/wlop_simplify_and_regularize_point_set.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std::chrono;

// types
typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;

// Concurrency
typedef CGAL::Parallel_if_available_tag Concurrency_tag;

int
main(int argc, char ** argv)
{
    const std::string input_filename
        = (argc > 1) ? argv[1] : CGAL::data_file_path("points_3/sphere_20k.xyz");
    const char * output_filename = (argc > 2) ? argv[2] : "data/sphere_20k_WLOPED.xyz";

    // Reads a .xyz point set file in points[]
    std::vector<Point> points;

    if(!CGAL::IO::read_points(input_filename, std::back_inserter(points)))
    {
        std::cerr << "Error: cannot read file " << input_filename << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Number of Input points: "
         << points.size() << "." << std::endl;

    std::vector<Point> output;

    // parameters
    const double retain_percentage = 2; // percentage of points to retain.
    const double neighbor_radius = 0.25; // neighbors size.

    auto start = high_resolution_clock::now();
    CGAL::wlop_simplify_and_regularize_point_set<Concurrency_tag>(
        points, std::back_inserter(output),
        CGAL::parameters::select_percentage(retain_percentage).neighbor_radius(neighbor_radius));
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    std::cout << "Time taken by WLOP was: "
         << duration.count() << " seconds" << std::endl;
    std::cout << "Number of WLOP points: "
         << output.size() << "." << std::endl;

    if(!CGAL::IO::write_points(output_filename, output, CGAL::parameters::stream_precision(17)))
    {
        std::cout << "Failed writing to " << output_filename << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Succeeded writing to " << output_filename << std::endl;

    return EXIT_SUCCESS;
}
