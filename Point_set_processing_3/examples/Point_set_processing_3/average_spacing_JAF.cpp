#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/IO/read_points.h>
#include <CGAL/compute_average_spacing.h>

#include <boost/tuple/tuple.hpp>
#include <fstream>
#include <vector>

#include <chrono>
using namespace std::chrono;

// Types
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;
typedef Kernel::FT FT;
typedef Kernel::Point_3 Point;

// Data type := index, followed by the point, followed by three integers that
// define the Red Green Blue color of the point.
typedef boost::tuple<int, Point> IndexedPoint;

// Concurrency
typedef CGAL::Parallel_if_available_tag Concurrency_tag;

int
main(int argc, char * argv[])
{
    const std::string fname
        = (argc > 1) ? argv[1] : CGAL::data_file_path("points_3/sphere_20k.xyz");

    // Reads a file in points.
    // As the point is the second element of the tuple (that is with index 1)
    // we use a property map that accesses the 1st element of the tuple.

    std::vector<IndexedPoint> points;
    if(!CGAL::IO::read_points(
           fname, std::back_inserter(points),
           CGAL::parameters::point_map(
               CGAL::Nth_of_tuple_property_map<1, IndexedPoint>())))
    {
        std::cerr << "Error: cannot read file " << fname << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Number of Input points: " << points.size() << "." << std::endl;

    auto start = high_resolution_clock::now();
    // Initialize index and RGB color fields in tuple.
    // As the index and RGB color are respectively the first and third-fifth elements
    // of the tuple we use a get function from the property map that accesses the 0
    // and 2-4th elements of the tuple.
    for(unsigned int i = 0; i < points.size(); i++)
    {
        points[i].get<0>() = i; // set index value of tuple to i
    }
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    std::cout << "Time taken by four loop with four assignments was: "
         << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    // Computes average spacing.
    const unsigned int nb_neighbors = 6; // 1 ring
    FT average_spacing = CGAL::compute_average_spacing<Concurrency_tag>(
        points, nb_neighbors,
        CGAL::parameters::point_map(
            CGAL::Nth_of_tuple_property_map<1, IndexedPoint>()));
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);
    std::cout << "Time taken by compute spacing was: "
         << duration.count() << " ms" << std::endl;

    std::cout << "Average spacing: " << average_spacing << " m"  << std::endl;

    return EXIT_SUCCESS;
}
