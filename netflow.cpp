#include "netflow.hpp"

#include <iostream>
#include <vector>
#include <queue>
#include <climits>

#define TOO_FEW_VERTICES "Too few vertices."
#define TOO_FEW_EDGES "Too few edges."
#define EDGE_WEIGHT_ZERO "Detected edge weight of 0."
#define EDGE_BAD_ENDPOINT "Edge interacts with nonexistent vertex."
#define SELF_LOOP "At least one self-loop."
#define MULTI_EDGES "Detected multi-edges."
#define NOT_ONE_SRC "Zero or more than one source."
#define NOT_ONE_SINK "Zero or more than one sink."

/*
* - The flow network does not contain only one source.
*      - Runtime error message: "Zero or more than one source."
* - The flow network does not contain only one sink.
*      - Runtime error message: "Zero or more than one sink."
*/

bool bfs(std::vector<std::vector<unsigned> >& residual_graph,
    std::vector<int>& parent, unsigned source, unsigned sink, unsigned numVertices)
{
    bool visited[numVertices];
    for (unsigned i = 0; i < numVertices; ++i) {
        visited[i] = false;
    }

    std::queue<unsigned> que;
    que.push(source);
    visited[source] = true;
    parent[source] = -1;

    while (!que.empty()) {
        unsigned vertex = que.front();
        que.pop();

        for (unsigned i = 0; i < numVertices; ++i) {
            if (visited[i] == false && residual_graph[vertex][i] > 0) {
                que.push(i);
                parent[i] = (int)vertex;
                visited[i] = true;
            }
        }
    }
    return (visited[sink] == true);
}

void ford_fulkerson(std::vector<std::vector<unsigned> >& capacity_graph,
    std::vector<std::vector<unsigned> >& residual_graph,
    unsigned source, unsigned sink, unsigned numVertices)
{
    std::vector<int> parent(numVertices, -1);

    unsigned i = 0;
    unsigned j = 0;

    while (bfs(residual_graph, parent, source, sink, numVertices)) {
        unsigned flow = INT_MAX;
        for (i = sink; i != source; i = parent[i]) {
            j = parent[i];
            flow = std::min(flow, residual_graph[j][i]);
        }

        for (i = sink; i != source; i = parent[i]) {
            j = parent[i];
            residual_graph[j][i] -= flow;
            residual_graph[i][j] += flow;
        }
    }
}

std::vector<Edge> solveNetworkFlow(
    const std::vector<Edge>& flowNetwork,
    unsigned numVertices)
{
    if (numVertices < 2) {
        throw std::runtime_error(TOO_FEW_VERTICES);
    }

    // Use Adjacency Matrix for Capacities, Flow, and Residual
    std::vector<std::vector<bool> > graph2D_edge_exists(numVertices, std::vector<bool> (numVertices, false));
    std::vector<std::vector<unsigned> > graph2D_capacity(numVertices, std::vector<unsigned> (numVertices, 0));
    std::vector<std::vector<unsigned> > graph2D_residual(numVertices, std::vector<unsigned> (numVertices, 0));
    unsigned numEdges = 0;

    for (const Edge& edge : flowNetwork) {
        graph2D_capacity[edge.from][edge.to] = edge.weight;
        graph2D_residual[edge.from][edge.to] = edge.weight;

        if (edge.weight == 0) {
            throw std::runtime_error(EDGE_WEIGHT_ZERO);
        }
        if (edge.from >= numVertices || edge.to >= numVertices) {
            throw std::runtime_error(EDGE_BAD_ENDPOINT);
        }
        if (edge.from == edge.to) {
            throw std::runtime_error(SELF_LOOP);
        }
        if (graph2D_edge_exists[edge.from][edge.to]) {
            throw std::runtime_error(MULTI_EDGES);
        } else {
            graph2D_edge_exists[edge.from][edge.to] = true;
        }
        numEdges++;
    }

    if (numEdges == 0) {
        throw std::runtime_error(TOO_FEW_EDGES);
    }

    unsigned numSource = 0;
    unsigned nodeSource;

    // Find the Source
    for (unsigned i = 0; i < numVertices; ++i) {
        for (unsigned j = 0; j < numVertices; ++j) {
            if (graph2D_capacity[j][i] != 0) {
                break;
            } else {
                if (j == numVertices - 1 && graph2D_capacity[numVertices - 1][j] == 0){
                    numSource++;
                    nodeSource = i;
                }
            }
        }
    }

    if (numSource != 1) { // numSink must be 1.
        throw std::runtime_error(NOT_ONE_SRC);
    }

    unsigned numSink = 0;
    unsigned nodeSink;

    // Find the Sink
    for (unsigned i = 0; i < numVertices; ++i) {
        for (unsigned j = 0; j < numVertices; ++j) {
            if (graph2D_capacity[i][j] != 0) {
                break;
            } else {
                if (j == numVertices - 1 && graph2D_capacity[i][numVertices - 1] == 0){
                    numSink++;
                    nodeSink = i;
                }
            }
        }
    }

    if (numSink != 1) { // numSink must be 1.
        throw std::runtime_error(NOT_ONE_SINK);
    }

    ford_fulkerson(graph2D_capacity, graph2D_residual, nodeSource, nodeSink, numVertices);

    // struct Edge edge[];
    std::vector<Edge> edge;
    Edge e;
    for (unsigned i = 0; i < numVertices; ++i) {
        for (unsigned j = 0; j < numVertices; ++j) {
            if (graph2D_edge_exists[i][j] == true) {
                e.from = i;
                e.to = j;
                e.weight = graph2D_capacity[i][j] - graph2D_residual[i][j];
                edge.push_back(e);
            }
        }
    }
    return edge;
}

void assignCourses(
    std::vector<Instructor>& instructors,
    const std::vector<std::string>& courses)
{
    unsigned num_instructors = instructors.size();
    unsigned num_courses = courses.size();
    unsigned num_vertices = num_instructors + num_courses + 2; // +2 because we need source and sink

    // Make pairs of index and name of the vertex.
    std::pair<unsigned, std::string> pair_idx_name[num_vertices];
    for (unsigned i = 0; i < num_instructors; ++i) {
        pair_idx_name[i].first = i;
        pair_idx_name[i].second = instructors[i].lastName;
    }
    pair_idx_name[num_instructors].first = num_instructors;
    pair_idx_name[num_instructors].second = "Source";
    pair_idx_name[num_instructors + 1].first = num_instructors + 1;
    pair_idx_name[num_instructors + 1].second = "Sink";
    for (unsigned i = num_instructors + 2, j = 0; i < num_vertices; ++i, ++j) {
        pair_idx_name[i].first = i;
        pair_idx_name[i].second = courses[j];
    }

    // Create a 2D array.
    std::vector<std::vector<bool> > graph2D_edge_exists(num_vertices, std::vector<bool> (num_vertices, false));
    std::vector<std::vector<unsigned> > graph2D_capacity(num_vertices, std::vector<unsigned> (num_vertices, 0));
    std::vector<std::vector<unsigned> > graph2D_residual(num_vertices, std::vector<unsigned> (num_vertices, 0));

    for (unsigned i = 0; i < num_instructors; ++i) {
        for (unsigned j = 0; j < instructors[i].preferences.size(); ++j) {
            for (unsigned k = num_instructors + 2; k < num_vertices; ++k) {
                if (instructors[i].preferences[j] == pair_idx_name[k].second) {
                    graph2D_capacity[i][pair_idx_name[k].first] = 1;
                    graph2D_residual[i][pair_idx_name[k].first] = 1;
                }
            }
        }
    }
    for (unsigned i = 0; i < num_instructors; ++i) {
        graph2D_capacity[num_instructors][i] = instructors[i].maxCourses;
        graph2D_residual[num_instructors][i] = instructors[i].maxCourses;
    }
    for (unsigned i = num_instructors + 2; i < num_vertices; ++i) {
        graph2D_capacity[i][num_instructors + 1] = 1;
        graph2D_residual[i][num_instructors + 1] = 1;
    }

    // Call ford_fulkerson function
    ford_fulkerson(graph2D_capacity, graph2D_residual, num_instructors, num_instructors+1, num_vertices);

    // Add assigned courses to each instructors.
    for (unsigned i = num_instructors + 2; i < num_vertices; ++i) {
        for (unsigned j = 0; j < num_instructors; ++j) {
            if (graph2D_residual[i][j] == 1) {
                instructors[j].assignedCourses.push_back(pair_idx_name[i].second);
            }
        }
    }
}
