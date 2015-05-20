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

// "using namespace" in a header file is conventionally frowned upon, but I'm
// including it here so that you may use things like size_t without having to
// type std::size_t every time.
using namespace std;

template <size_t N, typename ElemType>
class KDNode{
public:
    Point<N> position;
    ElemType element;
    KDNode *left;
    KDNode *right;
    KDNode(Point<N> pos, ElemType elem = ElemType(), KDNode *le = NULL, KDNode *rt = NULL) : position(pos),
            element(elem), left(le), right(rt) {};
};


template <size_t N, typename ElemType>
class KDTree {
public:


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
    KDNode<N, ElemType> *node = new KDNode<N, ElemType>(other->position, other->element);
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
    this->operator =(rhs);
}

template <size_t N, typename ElemType>
KDTree<N, ElemType>& KDTree<N, ElemType>::operator=(const KDTree& rhs) {
    sz = rhs.sz;
    Destroy(root);

    root = Clone(rhs.root);
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
        cur->left = new KDNode<N, ElemType>(pt,value);
    } else {
        sz++;
        cur->right = new KDNode<N, ElemType>(pt, value);
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
        cur->left = new KDNode<N, ElemType>(pt);
        sz++;
        cur = cur->left;
    } else {
        cur->right = new KDNode<N, ElemType>(pt);
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

    const KDNode<N, ElemType> *cur = search(pt);

    if (cur == NULL) throw out_of_range("No Point in KDTREE");
    else return cur->element;
}



#endif // KDTREE_INCLUDED
