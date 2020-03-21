/******************************************************************************
 * File: Trailblazer.h
 *
 * Exports functions that use Dijkstra's algorithm, A* search, and Kruskal's
 * algorithm as specified in the assignment handout.
 */

#ifndef Trailblazer_Included
#define Trailblazer_Included

#include "TrailblazerTypes.h"
#include "TrailblazerPQueue.h"
#include "set.h"
#include "grid.h"

/* Function: shortestPath
 * 
 * Finds the shortest path between the locations given by start and end in the
 * specified world.	 The cost of moving from one edge to the next is specified
 * by the given cost function.	The resulting path is then returned as a
 * Vector<Loc> containing the locations to visit in the order in which they
 * would be visited.	If no path is found, this function should report an
 * error.
 *
 * In Part Two of this assignment, you will need to add an additional parameter
 * to this function that represents the heuristic to use while performing the
 * search.  Make sure to update both this function prototype and the
 * implementation inside of Trailblazer.cpp.
 */
Vector<Loc>
shortestPath(Loc start,
             Loc end,
             Grid<double>& world,
             double costFn(Loc from, Loc to, Grid<double>& world),double heuristic(Loc start, Loc end, Grid<double>& world));

/* Function: createMaze
 * 
 * Creates a maze of the specified dimensions using a randomized version of
 * Kruskal's algorithm, then returns a set of all of the edges in the maze.
 *
 * As specified in the assignment handout, the edges you should return here
 * represent the connections between locations in the graph that are passable.
 * Our provided starter code will then use these edges to build up a Grid
 * representation of the maze.
 */
Set<Edge> createMaze(int numRows, int numCols);


//A helper struct that keeps track of node's total path weight and parent
struct Node {
	Loc parent;
	double dist;
};

Node createNode(double dist, Loc parent);
void chooseNext(Loc prev, Loc next, Grid<double> &world, TrailblazerPQueue <Loc> &queue, Map<Loc,Node> &map, double weight, double heur);
Vector<Loc> initResult(Loc start, Loc end, Map<Loc,Node> &map);

//A Helper struct in maze generation, keeping track of edge and its weight
struct Arc {
	Edge edge;
	double weight;
};

Map<Loc, Vector<Arc> > initMap(int numRows, int numCols, Map<Loc, Set<Loc> > &clusters, bool isUnion);
void addInfo(Loc cur, Map<Loc, Vector <Arc> > &map, int row, int col, int numRows, int numCols);
void mergeClusters(Map<Loc, Set<Loc> > &clusters, Loc start, Loc end);
Set<Edge> createMazeExtension(int numRows, int numCols, bool isUnion);
bool answerIsYes(string question);

#endif
