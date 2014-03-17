#include <iostream>
#include <fstream>
#include <cerrno>
#include <cstring>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <new>
#include <set>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>


static const int MAP_WATER = -1;
static const int MAP_START_GROUND = 0;

typedef boost::numeric::ublas::matrix<int> islands_union_array;


void union_cells(islands_union_array &islands_map, const size_t i1, const size_t j1,
                 const size_t i2, const size_t j2) {
    //piece of useful debug print
    /*std::cerr << "DEBUG: i1=" << i1 << ", j1=" << j1 << ", i2=" << i2 
              << ", j2=" << j2 << "\n";*/

    //we don't perform unions for water cells
    if (islands_map(i1, j1) == MAP_WATER) 
        return;
    if (islands_map(i2, j2) == MAP_WATER) 
        return;


    //we use far from optimal quick find method, which
    //could be significantlly optimized later e.g. by
    //using weighted quick union
    int ground_idx = islands_map(i1, j1);
    islands_map(i1, j1) = islands_map(i2, j2);
    for (size_t i = 0; i < islands_map.size1(); ++i)
        for (size_t j = 0; j < islands_map.size2(); ++j) {
            if (islands_map(i, j) == ground_idx) {
                islands_map(i, j) = islands_map(i2, j2);
            }
        }
}


void process_cell(islands_union_array &islands_map, const size_t i, const size_t j) {
    const size_t top_i = i - 1;
    const size_t top_j = j;
    if (i > 0) {
        union_cells(islands_map, i, j, top_i, top_j);
    }
    const size_t down_i = i + 1;
    const size_t down_j = j;
    if (down_i < islands_map.size1()) {
        union_cells(islands_map, i, j, down_i, down_j);
    }

    const size_t left_i = i;
    const size_t left_j = j - 1;
    if (j > 0) {
        union_cells(islands_map, i, j, left_i, left_j);
    }
    
    const size_t right_i = i;
    const size_t right_j = j + 1;
    if (right_j < islands_map.size2()) {
        union_cells(islands_map, i, j, right_i, right_j);
    }
}


int main(int argc, char **argv) {
    std::ifstream input;
    std::ofstream output;


    input.open(BINARY_DIR "/input.txt");
    output.open(BINARY_DIR "/output.txt");
    if (not input.is_open()) {
        std::cerr << "Unable to open the input file. Reason: " << 
                      strerror(errno) << "\n";
        return 1;
    }
    if (not output.is_open()) {
        std::cerr << "Unable to open the output file. Reason: " << 
                      strerror(errno) << "\n";
        return 1;
    }

    std::vector<std::string> islands_map;
    std::string line;

    while ( std::getline(input, line) ) {
        //piece of useful debug print
        //std::cerr << "DEBUG: line is " << line << "\n";
        islands_map.push_back(line);
    }
    input.close();

    const size_t column_count = islands_map[0].size();
    const size_t row_count = islands_map.size();
    //piece of useful debug print
    //std::cerr << "column_count is " << column_count << ", row_count is " 
    //          << row_count << "\n";

    islands_union_array union_array(row_count, column_count);

    int ground_counter = MAP_START_GROUND;

    for(size_t i = 0; i < islands_map.size(); ++i) {
        const std::string & map_row = islands_map[i];
        for(size_t j=0; j < map_row.size(); ++j) {
           const char c = map_row[j];
           if (c == 'o') {
                union_array(i, j) = ground_counter;
                ++ground_counter;
           } else if (c == '~') {
                union_array(i, j) = MAP_WATER;
           } else {
               std::cerr << "ERROR: invalid symbol `" << c << "`" 
                         << " in map. Row: " << i << ", col: " << j << "\n";
               return 1;
           }
        }
    }
    //overall complexity of this solution is O(N^2), not O(N^4). 
    //Please not that N=size1*size2, and if we have double loop(in double loop),
    //it doens't automatically means that complexity is O(N^4).
    //Algorithm performs linear scans every entry on map once and for each entry it
    //performs again linear scan for each entry(in the worst case).
    //The matrix could easily replaced by single array with complex addressing 
    //schema and we would have just one cycle + one internal cycle.
    //
    //I could rewrite the solution for using weighted quick-union with path compression
    //and would get O((amortized const)*N) complexity, but who will pay me for this work?:)
    //Implementing simple quick find it the simplest way to solve this problem, it 
    //takes just ten minutes. As an real engineerr and programmer I am lazy and dont' 
    //want to perform optimization before it's really needed.
    //
    //See Bob's Sedgewick explanation for additional information:
    //http://algs4.cs.princeton.edu/15uf/
    //
    //Y.a. good thing about this approach is it's dynamical nature. If we want to 
    //add data in real time on our map, complexity of solving this problem again will 
    //be just O(1).
    //
    //Y.a. approach is to use graphs, I see. 
    for (size_t i = 0; i < union_array.size1(); ++i)
        for (size_t j = 0; j < union_array.size2(); ++j) {
            process_cell(union_array, i, j);
        }
    //piece of useful debug print
    //std::cerr << "DEBUG: union_array is " << union_array << std::endl;


    std::set<int> islands_counter;
    for (size_t i = 0; i < union_array.size1(); ++i)
        for (size_t j = 0; j < union_array.size2(); ++j) {
            if (union_array(i, j) != MAP_WATER) {
                islands_counter.insert(union_array(i, j));
            }
        }

    output << islands_counter.size() << "\n";
    output.close();


    return 0;
}
