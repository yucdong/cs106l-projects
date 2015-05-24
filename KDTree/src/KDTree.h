/**
 * File: KDTree.h
 * Author: (your name here)
 * ------------------------
 * An interface representing a kd-tree in some number of dimensions. The tree
 * can be constructed from a set of data and then queried for membership and
 * nearest neighbors.
 */

#ifndef KDTREE_INCLUDED
#define KDTREE_INCLUDED

#include "Point.h"
#include "BoundedPQueue.h"
#include <stdexcept>
#include <cmath>
#include <stack>
#include <map>
#include <algorithm>
#include <vector>

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.
using namespace std;

template <size_t N, typename ElemType>
class KDTree;

template <size_t N, typename ElemType>
class KDNode{
private:
    Point<N> position;
    ElemType element;
    size_t split;   // the dimension to compare
    KDNode *left;
    KDNode *right;
    KDNode(Point<N> pos, ElemType elem = ElemType(), size_t sp = 0, KDNode *le = NULL, KDNode *rt = NULL) : position(pos),
            element(elem), split(sp), left(le), right(rt) {};

    friend class KDTree<N, ElemType>;
};


template <size_t N, typename ElemType>
class KDTree {
public:

    // Define ElemType to be value_type
    // for simplification
    typedef pair<Point<N>, ElemType> KDPair;

    // Constructor: KDTree();
    // Usage: KDTree<3, int> myTree;
    // ----------------------------------------------------
    // Constructs an empty KDTree.
    KDTree();
    
    // Destructor: ~KDTree()
    // Usage: (implicit)
    // ----------------------------------------------------
    // Cleans up all resources used by the KDTree.
    ~KDTree();
    
    // KDTree(const KDTree& rhs);
    // KDTree& operator=(const KDTree& rhs);
    // Usage: KDTree<3, int> one = two;
    // Usage: one = two;
    // -----------------------------------------------------
    // Deep-copies the contents of another KDTree into this one.
    KDTree(const KDTree& rhs);
    KDTree& operator=(const KDTree& rhs);

    // Build KDTree from a bunch of data
    // Need to sort and split to make the tree balanced
    template <typename InputIterator>
    KDTree(InputIterator first, InputIterator last);
    
    // size_t dimension() const;
    // Usage: size_t dim = kd.dimension();
    // ----------------------------------------------------
    // Returns the dimension of the points stored in this KDTree.
    size_t dimension() const;
    
    // size_t size() const;
    // bool empty() const;
    // Usage: if (kd.empty())
    // ----------------------------------------------------
    // Returns the number of elements in the kd-tree and whether the tree is
    // empty.
    size_t size() const;
    bool empty() const;
    
    // bool contains(const Point<N>& pt) const;
    // Usage: if (kd.contains(pt))
    // ----------------------------------------------------
    // Returns whether the specified point is contained in the KDTree.
    bool contains(const Point<N>& pt) const;
    
    // void insert(const Point<N>& pt, const ElemType& value);
    // Usage: kd.insert(v, "This value is associated with v.");
    // ----------------------------------------------------
    // Inserts the point pt into the KDTree, associating it with the specified
    // value. If the element already existed in the tree, the new value will
    // overwrite the existing one.
    void insert(const Point<N>& pt, const ElemType& value);
    
    // ElemType& operator[](const Point<N>& pt);
    // Usage: kd[v] = "Some Value";
    // ----------------------------------------------------
    // Returns a reference to the value associated with point pt in the KDTree.
    // If the point does not exist, then it is added to the KDTree using the
    // default value of ElemType as its key.
    ElemType& operator[](const Point<N>& pt);
    
    // ElemType& at(const Point<N>& pt);
    // const ElemType& at(const Point<N>& pt) const;
    // Usage: cout << kd.at(v) << endl;
    // ----------------------------------------------------
    // Returns a reference to the key associated with the point pt. If the point
    // is not in the tree, this function throws an out_of_range exception.
    ElemType& at(const Point<N>& pt);
    const ElemType& at(const Point<N>& pt) const;
    
    // ElemType kNNValue(const Point<N>& key, size_t k) const
    // Usage: cout << kd.kNNValue(v, 3) << endl;
    // ----------------------------------------------------
    // Given a point v and an integer k, finds the k points in the KDTree
    // nearest to v and returns the most common value associated with those
    // points. In the event of a tie, one of the most frequent value will be
    // chosen.
    ElemType kNNValue(const Point<N>& key, size_t k) const;

private:
    KDNode<N, ElemType>* modify_search(const Point<N>& pt, int &direction);
    KDNode<N, ElemType>* search(const Point<N>& pt) const;
    void Destroy(KDNode<N, ElemType>* node);
    KDNode<N, ElemType>* Clone(KDNode<N, ElemType>* other);

    template <typename InputIterator>
    KDNode<N, ElemType>* createKDTree(InputIterator first, InputIterator last, int level);

    const static int LEFT;
    const static int RIGHT;
    const static int NODIR;
    // Dimension of the KD-tree
    size_t dim;   // The Dimension of this KD-Tree

    size_t sz;        // Current number of elements stored in this KDTree

    KDNode<N, ElemType> *root;       // Current root node of this KD-Tree


};

template <size_t N, typename ElemType>
const int KDTree<N, ElemType>::LEFT = 0;

template <size_t N, typename ElemType>
const int KDTree<N, ElemType>::RIGHT = 1;

template <size_t N, typename ElemType>
const int KDTree<N, ElemType>::NODIR = 2;




/** KDTree class implementation details */
template <size_t N, typename ElemType>
KDNode<N, ElemType> * KDTree<N, ElemType>::modify_search(const Point<N>& pt, int& direction) {
    KDNode<N, ElemType> *cur = root;
    direction = NODIR;
    int rd = 0; // Current Dimension to compare
    while (cur != NULL && cur->position != pt) {
        if (pt[rd % N] < cur->position[rd % N]) {
            if (cur->left == NULL) {
                direction = LEFT;
                break;
            }
            cur = cur->left;
        } else {
            if (cur->right == NULL) {
                direction = RIGHT;
                break;
            }
            cur = cur->right;
        }
        rd++;
    }

    return cur;
}

template <size_t N, typename ElemType>
KDNode<N, ElemType> *KDTree<N, ElemType>::search(const Point<N>& pt) const {

    // We can't modify the object's internal data
    // When the method is const-qualified
    // And const objects can only access const member functions
    KDNode<N, ElemType> *cur = root;
    int rd = 0; // Current Dimension to compare
    while (cur != NULL && cur->position != pt) {
        if (pt[rd % N] < cur->position[rd % N]) {
            cur = cur->left;
        } else {
            cur = cur->right;
        }
        rd++;
    }
    return cur;
}

template <size_t N, typename ElemType>
void KDTree<N,ElemType>::Destroy(KDNode<N, ElemType> *node) {
    if (node == NULL) return;
    Destroy(node->left);
    Destroy(node->right);
    delete node;
}

template <size_t N, typename ElemType>
KDNode<N, ElemType>* KDTree<N, ElemType>::Clone(KDNode<N, ElemType>* other) {
    if (other == NULL) return NULL;

    KDNode<N, ElemType> *node = NULL;

    node = new KDNode<N, ElemType>(other->position, other->element, other->split);

    node->left = Clone(other->left);
    node->right = Clone(other->right);
    return node;
}



template <size_t N, typename ElemType>
KDTree<N, ElemType>::~KDTree() {
    Destroy(root);
    sz = 0;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree(const KDTree& rhs) {
    // Destroy previous KD-Tree first
    root = NULL;
    this->operator =(rhs);
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>& KDTree<N, ElemType>::operator=(const KDTree& rhs) {


    KDNode<N, ElemType> *copy = NULL;
    try {
        copy = Clone(rhs.root);
        Destroy(root);
        sz = rhs.sz;
        dim = rhs.dim;
        root = copy;
    } catch (...) {
        Destroy(copy);
    }

    return *this;
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>::KDTree() {
    root = NULL;
    dim = N;
    sz = 0;
}


template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::dimension() const {
    return dim;
}

template <size_t N, typename ElemType>
void KDTree<N, ElemType>::insert(const Point<N>& pt, const ElemType& value) {
    int direction;
    KDNode<N, ElemType> *cur = modify_search(pt, direction);

    if (cur != NULL && direction == NODIR) {
        // Override the element
        cur->element = value;
    } else if (cur == NULL) {
        // NULL TREE
        sz++;
        root = new KDNode<N, ElemType>(pt, value);
    } else if (cur != NULL && direction == LEFT) {
        sz++;
        cur->left = new KDNode<N, ElemType>(pt,value, (cur->split + 1) % dim);
    } else {
        sz++;
        cur->right = new KDNode<N, ElemType>(pt, value, (cur->split + 1) % dim);
    }
}

template <size_t N, typename ElemType>
size_t KDTree<N, ElemType>::size() const {
    return sz;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::empty() const {
    return sz == 0;
}

template <size_t N, typename ElemType>
bool KDTree<N, ElemType>::contains(const Point<N>& pt) const {
    KDNode<N, ElemType> *cur = search(pt);

    if (cur == NULL) return false;
    return true;
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::operator[](const Point<N>& pt) {
    int direction;
    KDNode<N, ElemType> *cur = modify_search(pt, direction);

    // If the position is not in the KD-Tree
    // Insert one with default parameter and return it
    if (cur == NULL) {
        sz++;
        cur = root = new KDNode<N, ElemType>(pt);
    } else if (cur != NULL && direction == NODIR) {
        // Find
    } else if (cur != NULL && direction == LEFT) {
        cur->left = new KDNode<N, ElemType>(pt, ElemType(), (cur->split + 1) % dim);
        sz++;
        cur = cur->left;
    } else {
        cur->right = new KDNode<N, ElemType>(pt, ElemType(), (cur->split + 1) % dim);
        cur = cur->right;
        sz++;
    }

    return cur->element;
}

template <size_t N, typename ElemType>
ElemType& KDTree<N, ElemType>::at(const Point<N>& pt) {
    int direction;
    KDNode<N, ElemType> *cur = modify_search(pt, direction);

    if (cur == NULL || direction != NODIR) throw out_of_range("No Point in KDTREE");
    else return cur->element;
}

template <size_t N, typename ElemType>
const ElemType& KDTree<N, ElemType>::at(const Point<N>& pt) const {
    return const_cast<KDTree<N, ElemType>* >(this)->at(pt);
}

template <size_t N, typename ElemType>
template <typename InputIterator>
KDNode<N, ElemType>* KDTree<N, ElemType>::createKDTree(
                                  InputIterator first, InputIterator last, int split) {


    if (first > last) return NULL;
    if (first == last) {
        sz++;
        return new KDNode<N, ElemType>(first->first, last->second, split);
    }

    // Sort this vector according to current split
    // Use the split elem of Point<N> to sort this part of data.
    // Use lambda Expression from C++11
    sort(first, last, [=] (const KDPair &pva, const KDPair &pvb) {return pva.first[split] < pvb.first[split];});

    // Recursively Construct subtrees(sort by certain dimension)
    InputIterator mid = first + (last - first) / 2;

    // Left Subtree : <
    // Right Subtree : >=
    // Need to find the first elem with split dim > previous one
    while (true) {
        InputIterator left = mid - 1;
        if (left < first || mid->first[split] != left->first[split])
            break;
        else
            mid = left;
    }

    KDNode<N, ElemType>* cur = new KDNode<N, ElemType>(mid->first, mid->second, split);
    sz++;
    cur->left = createKDTree(first, mid, (split + 1) % dim);
    cur->right = createKDTree(mid + 1, last, (split + 1) % dim);
    return cur;
}


template <size_t N, typename ElemType>
template <typename InputIterator>
KDTree<N, ElemType>::KDTree(InputIterator first, InputIterator last) {

    root = createKDTree(first, last, 0);
}



template <size_t N, typename ElemType>
ElemType KDTree<N, ElemType>::kNNValue(const Point<N>& key, size_t k) const {

    // Recording of the search path
    stack<KDNode<N, ElemType>* > search_path;

    double min_dist =  LONG_MAX;
    Point<N> nearest;

    // The Bounded Priority Queue
    BoundedPQueue<ElemType> bqueue(k);
    KDNode<N, ElemType> *curr = root;
    while (curr != NULL) {
        search_path.push(curr); // push current path node to stack

        // Distance is the priority for this bqueue
        double dist = Distance(curr->position, key);
        bqueue.enqueue(curr->element, dist);

        if (key[curr->split] <= curr->position[curr->split]) {
            curr = curr->left;
        } else {
            curr = curr->right;
        }
    }

    // BackTracking
    KDNode<N, ElemType> *path_end = search_path.top();
    nearest = path_end->position;
    min_dist = Distance(key, nearest);

    while (!search_path.empty()) {
        curr = search_path.top();
        KDNode<N, ElemType> *pkdnode = NULL;
        search_path.pop();

        // Calculate Distance
        // if Distance is smaller, update the value
        if (Distance(curr->position, key) < min_dist) {
            nearest = curr->position;
            min_dist = Distance(nearest, key);
        }

        // Calculate aligned Distance
        // If the intersection happens, we need to dig down to branches
        if (bqueue.maxSize() != bqueue.size() ||
                fabs(key[curr->split] - curr->position[curr->split]) < bqueue.worst()) {
            if (key[curr->split] <= curr->position[curr->split]) {
                pkdnode = curr->right;
            } else {
                pkdnode = curr->left;
            }

            // A recursion to find the leaf node
            while (pkdnode != NULL) {
                search_path.push(pkdnode); // push current path node to stack

                // Distance is the priority for this bqueue
                double dist = Distance(pkdnode->position, key);
                bqueue.enqueue(pkdnode->element, dist);

                if (key[pkdnode->split] <= pkdnode->position[pkdnode->split]) {
                    pkdnode = pkdnode->left;
                } else {
                    pkdnode = pkdnode->right;
                }
            }
        }


    }

    // Return the frequent value
    map<ElemType, int> elemCount;
    ElemType ret;
    int maxcount = -1;
    while (!bqueue.empty()) {
        ElemType cur = bqueue.dequeueMin();
        elemCount[cur] += 1;
        if (elemCount[cur] > maxcount) {
            maxcount = elemCount[cur];
            ret = cur;
        }

    }

    return ret;

}



#endif // KDTREE_INCLUDED
