#pragma once
#define SimpleGraph simplegraph_temp //Hack to redefine SimpleGraph using QT libraries
// Bugfix: Disable C++11 feature macros on Mac/libstdc++ to fool Qt into not using C++11 headers
#ifdef __APPLE__
#undef __cplusplus
#define __cplusplus 200303L
#undef __GXX_EXPERIMENTAL_CXX0X__
#endif

/*************************************************************************
 * File: SimpleGraph.h
 *
 * A header file defining a set of functions which can be used to visualize a
 * simple graph.  The types declared here and the DrawGraph() function will
 * allow you to render a scaled version of a SimpleGraph you pass in.
 */

#include <vector>
#include <cstddef>
#include <QObject>
#include <QWidget>
#include <QTime>

/**
 * Type: Node
 * -----------------------------------------------------------------------
 * A type representing a node in a graph.  Each node stores only the x and
 * y coordinates of where the node is in the plane; all other information
 * about the node is stored implicitly by its position in the SimpleGraph
 * list of nodes.
 */
struct Node {
  double x, y;
};

/**
 * Type: Edge
 * -----------------------------------------------------------------------
 * A type representing an edge in the graph.  It stores its endpoints by
 * the indices in which they occur in the SimpleGraph's list of Nodes.
 */
struct Edge {
  std::size_t start, end;
};

/**
 * Type: SimpleGraph
 * -----------------------------------------------------------------------
 * A type representing a simple graph of nodes and edges.
 */

struct SimpleGraph {
    std::vector<Node> nodes;
    std::vector<Edge> edges;
};

/**
 * Function: DrawGraph(SimpleGraph& graph)
 * -----------------------------------------------------------------------
 * Draws the specified graph.
 */

void DrawGraph(SimpleGraph& userGraph);
void InitGraphVisualizer(SimpleGraph& userGraph);




/** Implementation Details Below
 * -----------------------------------------------------------------------
 * You do not need to read anything below this point and doing so may
 * be confusing.
 */

#undef SimpleGraph

/** Real SimpleGraph that uses Q_OBJECT Macro */
class SimpleGraph : public QObject {
  Q_OBJECT

  public:
    SimpleGraph(){}
    SimpleGraph(const SimpleGraph& other):QObject(), nodes(other.nodes), edges(other.edges){}
    std::vector<Node> nodes;
    std::vector<Edge> edges;
    void drawGraph(SimpleGraph & graph);

  signals:
    void graphUpdated(SimpleGraph graph);
};

void DrawGraph(SimpleGraph& userGraph);
void InitGraphVisualizer(SimpleGraph& userGraph);
void ReadGraph(const char *filename, SimpleGraph& userGraph);
void InitNodes(SimpleGraph& userGraph);
void UpdateGraph(SimpleGraph& userGraph);

Q_DECLARE_METATYPE(SimpleGraph) //allows use of Simplegraph in signals/slots

class MyWidget : public QWidget {
    Q_OBJECT
public:
    static MyWidget & getInstance();
    void initGraph(SimpleGraph & userGraph);

protected:
    void paintEvent(QPaintEvent *event);

private:
    SimpleGraph userGraph;
    QTime last_run;
    friend void SimpleGraph::drawGraph(SimpleGraph & graph);

};

/** Redefinition: main
 * -----------------------------------------------------------------------
 * Due to a quirk in the way that the QT graphics works, main
 * actually must be in the graphics module itself.  This macro redefines
 * main to some other harmless term so that in your implementation, you
 * can define main but have it really invoked by the graphics driver.
 * This is a fairly unpleasant hack, but it's necessary.
 */
#define main _userMain
