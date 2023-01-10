#include "netflow.hpp"

#include <iostream>
int main()
{
    std::vector<Edge> maxFlow = solveNetworkFlow({
            {3, 5, 5},
            {3, 0, 10},
            {0, 5, 5},
            {5, 4, 10},
            {0, 4, 3},
            {0, 1, 1},
            {4, 1, 20},
            {4, 2, 5},
            {1, 2, 7}
        }, 6
    );

    for (const Edge& edge : maxFlow)
        std::cout << edge.from << " -> " << edge.to
            << " (" << edge.weight << ")\n";
}

/* Run code using following commands */
// g++ -std=c++14 -Wall -Werror demo_netflow.cpp netflow.cpp -o demo_netflow
// ./demo_netflow
