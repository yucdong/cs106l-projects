#include <iostream>
// Bugfix: Disable C++11 feature macros on Mac/libstdc++ to fool Qt into not using C++11 headers
#ifdef __APPLE__
#undef __cplusplus
#define __cplusplus 200303L
#undef __GXX_EXPERIMENTAL_CXX0X__
#endif

#include <QtGui>
#include <QWidget>
#include <QApplication>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QObject>
#include <QSemaphore>
#include <cmath>
#include <vector>

#include "SimpleGraph.h"
#undef main

using std::vector;

const double PI = 3.14159265358979323;
const double kRepel = 0.001;
const double kAttract = 0.001;


MyWidget& MyWidget::getInstance() {
    static MyWidget instance;
    return instance;
}

/* global semaphore to ensure only one update call at a time.
 * Prevents overload of update calls */
QSemaphore semaphore;

void InitGraphVisualizer(SimpleGraph & userGraph) {
    MyWidget& g = MyWidget::getInstance();
    QObject::connect(&userGraph, SIGNAL(graphUpdated(SimpleGraph)),
                     &g, SLOT(repaint()));
}

void MyWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    if (!userGraph.nodes.empty()) {
        SimpleGraph copy;
        copy.nodes = userGraph.nodes;
        copy.edges = userGraph.edges;        auto getX = [](const Node& a, const Node& b) {return a.x < b.x;};
        double maxX = std::max_element(copy.nodes.begin(), copy.nodes.end(), getX)->x;
        double minX = std::min_element(copy.nodes.begin(), copy.nodes.end(), getX)->x;

        auto getY = [](const Node& a, const Node& b) { return a.y < b.y;};
        double maxY = std::max_element(copy.nodes.begin(), copy.nodes.end(), getY)->y;
        double minY = std::min_element(copy.nodes.begin(), copy.nodes.end(), getY)->y;

        auto scale = [maxX, maxY, minX, minY](const Node& a) -> Node {
            return {(a.x - minX) * 590 / (-minX + maxX) + 5, (a.y - minY) * 590 / (-minY + maxY) + 5};
        };
        painter.setBrush(Qt::blue);
        std::transform(copy.nodes.begin(), copy.nodes.end(), copy.nodes.begin(), scale);
        for (Edge & e : copy.edges) {
            Node start = copy.nodes[e.start];
            Node end = copy.nodes[e.end];
            painter.drawLine(start.x, start.y, end.x, end.y);
        }
        for (Node & n : copy.nodes) {
            painter.drawEllipse(n.x - 5, n.y - 5, 10, 10);
        }
    }
    //last_run = QTime::currentTime();
    semaphore.release();
}

class WorkerThread : public QThread {
    void run() {
        int _userMain();
        _userMain();

    }
};

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    MyWidget & myWidget = MyWidget::getInstance();
    myWidget.resize(600, 600);
    myWidget.show();
    qRegisterMetaType<SimpleGraph>(); //allows use of SimpleGraph in signals/slots
    WorkerThread x;
    x.start();
    return app.exec();
}

void SimpleGraph::drawGraph(SimpleGraph &graph) {
    if (!semaphore.tryAcquire()) return;
    MyWidget& m = MyWidget::getInstance();
    m.userGraph.nodes = graph.nodes;
    m.userGraph.edges = graph.edges;
    emit graphUpdated(graph);
}

void DrawGraph(SimpleGraph& userGraph) {
    userGraph.drawGraph(userGraph);
}

void ReadGraph(const char *filename, SimpleGraph& userGraph) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(1);
    }

    int numNodes;
    in >> numNodes;

    userGraph.nodes.resize(numNodes);
    size_t edgeBegin, edgeEnd;
    while (in >> edgeBegin >> edgeEnd) {
        Edge e = {edgeBegin, edgeEnd};
        userGraph.edges.push_back(e);
    }

    in.close();
}

void InitNodes(SimpleGraph &userGraph) {
    int numNodes = userGraph.nodes.size();
    for (int k = 0; k < numNodes; ++k) {
            userGraph.nodes[k].x = cos(2*PI*k / numNodes);
            userGraph.nodes[k].y = sin(2*PI*k / numNodes);
    }
}

/**
 * This algorithm is based on the Fruchterman-Reingold algorithm
 * to compute forces between nodes
 * @brief UpdateGraph
 * @param userGraph
 */
void UpdateGraph(SimpleGraph &userGraph) {

    size_t nodeCount = userGraph.nodes.size();
    vector<double> deltaX(nodeCount, 0.0);
    vector<double> deltaY(nodeCount, 0.0);

    // Compute repulsive forces
    for (size_t pos = 0; pos < nodeCount - 1; pos++)
        for (size_t npos = pos + 1; npos < nodeCount; npos++) {
            double x0 = userGraph.nodes.at(pos).x;
            double y0 = userGraph.nodes.at(pos).y;
            double x1 = userGraph.nodes.at(npos).x;
            double y1 = userGraph.nodes.at(npos).y;

            // Repulsive force
            double fRepel = kRepel / (sqrt((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0)));
            double theta = atan2(y1 - y0, x1 - x0);
            deltaX[pos] -= fRepel * cos(theta);
            deltaY[pos] -= fRepel * sin(theta);
            deltaX[npos] += fRepel * cos(theta);
            deltaY[npos] += fRepel * sin(theta);
        }


    // Compute attractive forces
    for (Edge edge : userGraph.edges) {
        double x0 = userGraph.nodes.at(edge.start).x;
        double y0 = userGraph.nodes.at(edge.start).y;
        double x1 = userGraph.nodes.at(edge.end).x;
        double y1 = userGraph.nodes.at(edge.end).y;

        double fAttract = kAttract * ((y1 - y0) * (y1 - y0) + (x1 - x0) * (x1 - x0));

        double theta = atan2(y1 - y0, x1 - x0);
        deltaX[edge.start] += fAttract * cos(theta);
        deltaY[edge.start] += fAttract * sin(theta);
        deltaX[edge.end] -= fAttract * cos(theta);
        deltaY[edge.end] -= fAttract * sin(theta);
    }

    // Move the nodes accordingly
    for (size_t pos = 0; pos < nodeCount; pos++) {
        userGraph.nodes[pos].x += deltaX[pos];
        userGraph.nodes[pos].y += deltaY[pos];
    }
}
