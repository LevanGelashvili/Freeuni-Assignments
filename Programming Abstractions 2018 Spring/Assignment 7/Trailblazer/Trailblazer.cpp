/******************************************************************************
 * File: Trailblazer.cpp
 *
 * Implementation of the graph algorithms that comprise the Trailblazer
 * assignment.

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 

 This program allows user to choose how to generate a random maze, with a standart method, or with
 union-find, an extension method.

 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

 */

#include "Trailblazer.h"
#include "TrailblazerGraphics.h"
#include "TrailblazerTypes.h"
#include "TrailblazerPQueue.h"
#include "random.h"
#include "UnionFind.h"
#include "simpio.h"
using namespace std;

/* Function: shortestPath
 * 
 * Finds the shortest path between the locations given by start and end in the
 * specified world.	 The cost of moving from one edge to the next is specified
 * by the given cost function. The resulting path is then returned as a
 * Vector<Loc> containing the locations to visit in the order in which they
 * would be visited. If no path is found, this function should report an
 * error.
 *
 * In Part Two of this assignment, you will need to add an additional parameter
 * to this function that represents the heuristic to use while performing the
 * search.  Make sure to update both this implementation prototype and the
 * function prototype in Trailblazer.h.
 */

Vector<Loc> shortestPath(Loc start, Loc end, Grid<double>& world, double costFn(Loc from, Loc to, Grid<double>& world),
						 double heuristic(Loc start, Loc end, Grid<double>& world)) {
	TrailblazerPQueue<Loc> queue;
	Map<Loc,Node> map; //This map keeps track of all locs and its nodes, containing parent and total distance

	map[start] = createNode(0,start);
	queue.enqueue(start,heuristic(start,end,world));
	colorCell(world,start,YELLOW);

	while(!queue.isEmpty()) { 
		Loc cur = queue.dequeueMin();
		colorCell(world,cur,GREEN);
		if(cur == end) return initResult(start,end,map); //If cur and end are same, we have reached the destination

		//This for loop iterates over all node's neighbours, excluding itself
		for (int i=-1; i<=1; i++) {
			for (int j=-1; j<=1; j++) {
				if(world.inBounds(cur.row + i,cur.col + j) && !(i==0 && j==0)) {
					Loc next;
					next.row = cur.row + i;
					next.col = cur.col + j;
					chooseNext(cur,next,world,queue,map,costFn(cur,next,world),heuristic(next,end,world));
				}
			}
		}
	}
}

//Returns a node with initialized fields
Node createNode(double dist, Loc parent) {
	Node node;
	node.dist = dist;
	node.parent = parent;
	return node;
}

//This function recieves a neighbouring node(next) and updates all fields, if necessary
void chooseNext(Loc prev, Loc next, Grid<double> &world, TrailblazerPQueue<Loc> &queue, Map<Loc,Node> &map, double weight, double heur) {
	double distance = weight + map[prev].dist;
	//If we haven't visited this node, we need to store its distance from start
	if(!map.containsKey(next)) {
		colorCell(world,next,YELLOW);
		map[next] = createNode(distance,prev);
		queue.enqueue(next,distance + heur);
	//If we have visited this node, but found a more effective way to get there, we need to update its info
	} else if(map.containsKey(next) && map[next].dist > distance) {
		map[next].dist = distance;
		map[next].parent = prev;
		queue.decreaseKey(next,distance + heur);
	}
}

/*This method exists for a single reason : insert function of a vector is O(n) for each element. Which takes a lot of time.
  Using stack for insertion is more effective here.
*/
Vector<Loc> initResult(Loc start, Loc end, Map<Loc,Node> &map) {
	Vector<Loc> result;
	Loc last = end;
	Stack<Loc> stack;
	stack.push(end);

	while(last != start) {
		stack.push(map[last].parent);
		last = map[last].parent;
	}
	while(!stack.isEmpty()) result += stack.pop();
	return result;
}

//Creates a maze using Krucksal algorithm, Arc is a structure containing an edge and a double, its weight
Set<Edge> createMaze(int numRows, int numCols) {

	//Asks user which method of generation to choose
	if(answerIsYes("Would you like maze to be generated with Union-find (extension, faster method) ? : ")) {
		return createMazeExtension(numRows,numCols,true);
	}

	Map<Loc, Set<Loc> > clusters; //Map which holds clustering information
	Map<Loc, Vector<Arc> > map = initMap(numRows,numCols,clusters,false);
	TrailblazerPQueue <Edge> queue;
	Set<Edge> result;

	foreach (Loc cur in map) {
		foreach (Arc a in map[cur]) {
			queue.enqueue(a.edge,a.weight); // enqueues every edge in a queue
		}
	}

	while(!queue.isEmpty()) {
		Edge e = queue.dequeueMin();
		if(clusters[e.start] != clusters[e.end]) { // if locs dont belong to the same cluster
			result += e;
			mergeClusters(clusters,e.start,e.end);
		}
	}
    return result;
}

/*Creates a map containing info, about every neighboring edge to a loc, while
	bool isUnion determines wheter user decided to use extension of this program (union-find algorithm)
*/
Map<Loc, Vector<Arc> > initMap(int numRows,int numCols, Map<Loc, Set<Loc> > &clusters, bool isUnion) {
	Map<Loc, Vector <Arc> > map;
	Loc cur;

	// Iterates over a graph, we don't need to create an actual grid, though
	for (int row=0; row<numRows; row++) {
		for (int col=0; col<numCols; col++) {
			cur.row = row;
			cur.col = col;
			
			//If user decided to use extension, we dont need the clusters map, so it remains empty, saving time and space.
			if(!isUnion) {
				Set<Loc> s;
				s += cur;
				clusters[cur] = s; // Adds itself to a cluster map
			}

			//Adds info of every neighboring edge
			addInfo(cur,map,row+1,col,numRows,numCols);
			addInfo(cur,map,row-1,col,numRows,numCols);
			addInfo(cur,map,row,col+1,numRows,numCols);
			addInfo(cur,map,row,col-1,numRows,numCols);
		}
	}
	return map;
}

// Generates a random edge
void addInfo(Loc cur, Map<Loc, Vector <Arc> > &map, int row, int col, int numRows, int numCols) {

	// If edge is out of bounds, it is not a neighbor
	if((row < 0 || row >= numRows) || (col < 0 || col >= numCols)) return;

	Loc next;
	next.row = row;
	next.col = col;

	Edge e;
	e.start = cur;
	e.end = next;

	Arc a;
	a.edge = e;
	a.weight = randomReal(0,1); // Random weight

	map[cur] += a;
}

// This method merges clusters, copies every single location from one Set<Loc> to another, to make them equal
void mergeClusters(Map<Loc, Set<Loc> > &clusters, Loc start, Loc end) {
	clusters[start] += clusters[end];
	clusters[end] += clusters[start];
	foreach (Loc l in clusters[end]) {
		clusters[l] += clusters[start];
	}
}

/* EXTENSION METHOD!!!
	Generates random maze using union-find algorithm (I implemented it myself, see UnionFind.h and UnionFind.cpp for more details).
	The whole generation process remains same, but we dont need the clusters map, instead we use unionMap to assign specific int to
	every location, basically, clusterisation.
*/
Set<Edge> createMazeExtension(int numRows, int numCols, bool isUnion) {
	Map<Loc, Set<Loc> > clusters; // We dont use this here, it remainst empty throughout the process
	Map<Loc, Vector<Arc> > map = initMap(numRows,numCols,clusters,isUnion);
	TrailblazerPQueue <Edge> queue;
	Set<Edge> result;
	Map<Loc,int> unionMap; // Clusterisation map, we use this here insteat of "clusters"
	int count = 0;

	foreach (Loc cur in map) {
		foreach (Arc a in map[cur]) {
			queue.enqueue(a.edge,a.weight);
			//If location is not present in a map, we assign unique int to it.
			if(!unionMap.containsKey(a.edge.start)) {
				unionMap[a.edge.start] = count;
				count++;
			}
			if(!unionMap.containsKey(a.edge.end)) {
				unionMap[a.edge.end] = count;
				count++;
			}
		}
	}

	UnionFind u(queue.size()); //Create UnionFind object.
	while(!queue.isEmpty()) {
		Edge e = queue.dequeueMin();
		//Acts same way as clusters map in original method, but way more efficiently
		if(!u.areMerged(unionMap[e.start],unionMap[e.end])) {
			result += e;
			u.merge(unionMap[e.start],unionMap[e.end]);
		}
	}
    return result;
}

//Asks user a question and returns true or false depending on input
bool answerIsYes(string question) {
	bool result;
	string line = getLine(question);
	while(true) {
		if(toLowerCase(line) == "yes") {
			result = true;
			break;
		}
		if(toLowerCase(line) == "no") {
			result = false;
			break;
		}
		line = getLine("Please enter yes or no : ");
	}
	return result;
}

