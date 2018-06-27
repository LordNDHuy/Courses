/*
 * =========================================================================================
 * Name        : dsaLib.h
 * Author      : Duc Dung Nguyen
 * Email       : nddung@hcmut.edu.vn
 * Copyright   : Faculty of Computer Science and Engineering - Bach Khoa University
 * Description : Library for Assignment 2 - Data structures and Algorithms - Spring 2018
 * =========================================================================================
 */

#ifndef A02_DSALIB_H
#define A02_DSALIB_H

#define AVL_USE_HEIGHT

#include <string>
#include <math.h>
#include <vector>
#include <functional>
#include <string.h>

using namespace std;

struct VRecord;
struct IDInfo{
    char*         ID;

    // (N/2) is the number of pass-by time
    unsigned int  N;

    bool          lastState;

    IDInfo(char* input, unsigned int n, bool state): ID(input), N(n), lastState(state){}
};

class DSAException {
    int     _error;
    string  _text;
public:

    DSAException() : _error(0), _text("Success") {}
    DSAException(int err) : _error(err), _text("Unknown Error") {}
    DSAException(int err, const char* text) : _error(err), _text(text) {}

    int getError() { return _error; }
    string& getErrorText() { return _text; }
};

template <class T>
class List {
public:
    virtual void    clean() = 0;
    virtual void    clone(void*) = 0;
    virtual bool    isEmpty() = 0;
    virtual size_t  getSize() = 0;
    virtual T&      at(int i) = 0;
    virtual T&      operator[](int i) = 0;

    virtual bool    find(T& a, int& idx) = 0;
    virtual T*      find(T& a) = 0;
    virtual T*      find(T& a, std::function<bool (T&, T&)> eqCmp) = 0;
    virtual int     insert(int i, T& a) = 0;
    virtual int     insert(int i, T&& a) = 0;
    virtual int     remove(int i) = 0;
    virtual int     remove(T& a, std::function<bool (T&, T&)> eqCmp) = 0;

    virtual int     push_back(T& a) = 0;
    virtual int     push_back(T&& a) = 0;
    virtual int     insertHead(T& a) = 0;
    virtual int     insertHead(T&& a) = 0;
    virtual int     removeHead() = 0;
    virtual int     removeLast() = 0;

    virtual void    traverse(std::function<void (T&)> op) = 0;
    virtual void    traverse(std::function<void (T&, void*)> op, void* pParam) = 0;

    virtual void    reverse() = 0;
};

template <class T>
struct L1Item {
    T data;
    L1Item<T> *pNext;

    L1Item(T &a) : data(a), pNext(NULL) {}
    L1Item(T &a, void* next) : data(a), pNext(next) {}
    L1Item(T &&a) : data(std::move(a)), pNext(NULL) {}
    L1Item(T &&a, void* next) : data(std::move(a)), pNext(next) {}
};

template <class T>
class L1List : public List<T> {
    
public:
L1Item<T>   *_pHead;
    size_t      _size;
    
    L1List() : _pHead(NULL), _size(0) {}
    ~L1List() { clean();}

    void    clean(){
        if (_pHead == NULL) return;

        L1Item<T> *pIterator = _pHead;
        while (pIterator != NULL){
            _pHead = _pHead->pNext;
            delete pIterator;
            pIterator = _pHead;
        }

        _size = 0;
    }
    void    clone(void*){}
    bool    isEmpty() { return _pHead == NULL; }
    size_t  getSize() { return _size; }

    T&      at(int i){
        L1Item<T>   *pTmp = this->_pHead;
        for (int count = 0; count < i; count++)
        	pTmp = pTmp->pNext;

        return pTmp->data;
    }
    T&      operator[](int i){
        return at(i);
    }


    bool    find(T& a, int& idx){
        if (isEmpty()) return false;

        L1Item<T>   *pIterator = _pHead;
        for (idx = 0; !(pIterator->data == a) && pIterator != NULL; pIterator = pIterator->pNext, idx++);

        if (pIterator != NULL) return true;
        else return false;
    }
    T*      find(T& a){ return 0;}
    T*      find(T& a,std::function<bool (T&, T&)> eqCmp){return 0;}//bool (*eqCmp)(T&, T&));



    int     insert(int i, T& a){
        if (isEmpty() || i <= 0){
            insertHead(a);
            return 0;
        }

        // Tommy: will conduct insert in the middle of list and exceed size of list 
        L1Item<T>   *pPrev = _pHead, *pAfter = NULL;

        for (int count = 0; count < i-1 && pPrev->pNext != NULL; pPrev = pPrev->pNext, count++);
        pAfter = pPrev->pNext;
        pPrev->pNext = new L1Item<T>(a);
        pPrev->pNext->pNext = pAfter;
        
        return 0;
    }
    int     insert(int i, T&& a){ return 0;}


    int     remove(int i){
        if (i >= (int)_size || i < 0 || isEmpty())
    		return -1;

    	L1Item<T>   *pIterator = _pHead, *pTmp;
        for (int count = 0; count < i-1; count++, pIterator = pIterator->pNext);
        pTmp = pIterator->pNext;
        pIterator->pNext = pTmp->pNext;
        delete pTmp;
        return 0;
    }
    int     remove(T& a, std::function<bool (T&, T&)> eqCmp){ return 0;}


    int     push_back(T&& a);
    int     push_back(T& a);
    int     insertHead(T& a);  
    int     insertHead(T&& a);
    int     removeHead();
    int     removeLast();

    void    reverse(){
        if (isEmpty()) return;

        L1Item<T>   *pIterator_rvsList = NULL, *pTmp;
        while (_pHead != NULL){
            pTmp = _pHead;
            _pHead = _pHead->pNext;
            pTmp->pNext = pIterator_rvsList;
            pIterator_rvsList = pTmp; 
        }

        _pHead = pIterator_rvsList;
    }

    void    traverse(std::function<void (T&)> op) {
        L1Item<T>   *p = _pHead;
        while (p) {
            op(p->data);
            p = p->pNext;
        }
    }
    //void    traverse(void (*op)(T&, void*), void* pParam) {
    void    traverse(std::function<void (T&, void*)> op, void* pParam) {
        L1Item<T>   *p = _pHead;
        while (p) {
            op(p->data, pParam);
            p = p->pNext;
        }
    }
    void    traverse(void (*op)(T&)) {
        L1Item<T>   *p = _pHead;
        while (p) {
            op(p->data);
            p = p->pNext;
        }
    }

    bool    traverse(bool (*op)(T, void* input), void* input) {
        L1Item<T>   *p = _pHead;
        while (p) {
            if (op(p->data, input) == true) return true;
            p = p->pNext;
        }

        return false;
    }

};

template <class T>
int L1List<T>::push_back(T &&a) {
    if (_pHead == NULL) {
        _pHead = new L1Item<T>(std::move(a));
    }
    else {
        L1Item<T>   *p = _pHead;
        while (p->pNext) p = p->pNext;
        p->pNext = new L1Item<T>(std::move(a));
    }

    _size++;
    return 0;
}

/// Insert item to the end of the list
/// Return 0 if success
template <class T>
int L1List<T>::push_back(T &a) {
    if (_pHead == NULL) {
        _pHead = new L1Item<T>(a);
    }
    else {
        L1Item<T>   *p = _pHead;
        while (p->pNext) p = p->pNext;
        p->pNext = new L1Item<T>(a);
    }

    _size++;
    return 0;
}


/// Insert item to the front of the list
/// Return 0 if success
template <class T>
int L1List<T>::insertHead(T &a) {
    L1Item<T>   *p = new L1Item<T>(a);
    p->pNext = _pHead;
    _pHead = p;
    _size++;
    return 0;
}

template <class T>
int L1List<T>::insertHead(T&& a) {
    L1Item<T>   *p = new L1Item<T>(std::move(a));
    p->pNext = _pHead;
    _pHead = p;
    _size++;
    return 0;
}


/// Remove the first item of the list
/// Return 0 if success
template <class T>
int L1List<T>::removeHead() {
    if(_pHead) {
        L1Item<T>* p = _pHead;
        _pHead = p->pNext;
        delete p;
        _size--;
        return 0;
    }
    return -1;
}

/// Remove the last item of the list
/// Return 0 if success
template <class T>
int L1List<T>::removeLast() {
    if(_pHead) {
        if(_pHead->pNext) {
            L1Item<T>* prev = _pHead;
            L1Item<T>* pcur = prev->pNext;
            while(pcur->pNext) {
                prev = pcur;
                pcur = pcur->pNext;
            }
            delete pcur;
            prev->pNext = NULL;
        }
        else {
            delete _pHead;
            _pHead = NULL;
        }
        _size--;
        return 0;
    }
    return -1;
}


/************************************************************************
 * This section is for AVL tree
 ************************************************************************/
template <class T>
struct AVLNode {
    T              _data;                       // to serve the goal that each AVLNode will only deal with 1 field of VRecord
    VRecord*     _pointRecord;                  //     _data            will store that field
                                                //     _pointRecord     will store the record containing the field stored by _data

    AVLNode<T>   *_pLeft, *_pRight;
#ifdef AVL_USE_HEIGHT
    int         _height;
    AVLNode(T &a) : _data(a),  _pointRecord(nullptr), _pLeft(NULL), _pRight(NULL), _height(0) {}
#else
    int         _bFactor;
    AVLNode(T &a) : _data(a), _pLeft(NULL), _pRight(NULL), _bFactor(0) {}
#endif

    void    setpointRecord(VRecord* input){ _pointRecord = input;}
};



template <class T>
class AVLTree{
    AVLNode<T> *_pRoot;
public:
    AVLTree() : _pRoot(NULL) {}
    ~AVLTree() { destroy(_pRoot); }


    bool    find(T& key, T& ret) { return find(_pRoot, key, ret); }
    void    insert(T key, VRecord* node) { insert(_pRoot, key, node);}
    void    remove(T& key) { remove(_pRoot, key); }

    void    traverseNLR(void (*op)(VRecord*)) { traverseNLR(_pRoot, op); }
    void    traverseLNR(void (*op)(VRecord*)) { traverseLNR(_pRoot, op); }
    void    traverseLRN(void (*op)(VRecord*)) { traverseLRN(_pRoot, op); }

    void    traverseLNR(void (*op)(IDInfo*))  { traverseLNR(_pRoot, op); }

    // same function with the above find() but for this one, it will find all eligitble nodes
    // instead of returning after finding out a node
    void    findDuplication(T data, L1List<VRecord*>* &resultList) {findDuplication(_pRoot, data, resultList);}
    void    findDuplication(T data, void (*op)(VRecord*)){
        findDuplication(_pRoot, data, op);
    }

    

protected:
    void    destroy(AVLNode<T>* &pR);
    bool    find(AVLNode<T> *pR, T& key, T& ret);
    void    insert(AVLNode<T>* &pR, T a, VRecord* node);

    void    remove(AVLNode<T>* &pR, T& a);
    void    deletebyCopy();


    void    rotLeft(AVLNode<T>* &pR);
    void    rotRight(AVLNode<T>* &pR);
    void    rotLL(AVLNode<T>* &pR);
    void    rotRR(AVLNode<T>* &pR);
    void    rotLR(AVLNode<T>* &pR);
    void    rotRL(AVLNode<T>* &pR);
    
    // each time in traversal loop, it will deal directly with _pointRecord
    void    traverseNLR(AVLNode<T> *pR, void (*op)(VRecord*));
    void    traverseLNR(AVLNode<T> *pR, void (*op)(VRecord*));
    void    traverseLRN(AVLNode<T> *pR, void (*op)(VRecord*));

    // deal with IDInfo
    void    traverseLNR(AVLNode<T> *pR, void (*op)(IDInfo*));

private:
    bool    less(char* a, char* b){
        return strcmp(a, b) < 0;
    }
    bool    equal(char* a, char* b){
        return strcmp(a, b) == 0;
    }

    bool    less(double a, double b){
        return a < b;
    }
    bool    equal(double a, double b){
        return a == b;
    }

    bool    less(IDInfo* a, IDInfo* b){
        return strcmp(a->ID, b->ID) < 0;
    }
    bool    equal(IDInfo* a, IDInfo* b){
        return strcmp(a->ID, b->ID) == 0;
    }


    int     max(int a, int b){
        return (a < b ? b : a);
    }
    int     getHeight(AVLNode<T>* &p){
        if (p == nullptr)
            return -1;
        else 
            return p->_height;
    }    
    int     diffHeight(AVLNode<T>* &p){
        return (getHeight(p->_pLeft) - getHeight(p->_pRight));
    }
    void    updateHeight(AVLNode<T>* &p){
        p->_height = max(getHeight(p->_pLeft), getHeight(p->_pRight)) + 1;
    }

    void    balanceLeft(AVLNode<T>* &pR){
        updateHeight(pR);

        if (diffHeight(pR) >= 2 && pR->_pLeft != nullptr){              // subleft tree is taller
            if (diffHeight(pR->_pLeft) > 0)     // left-left case
                rotLL(pR);
            else
                rotLR(pR);                      // left-right case
        updateHeight(pR->_pLeft);           // pR and its left child must be update
        updateHeight(pR->_pRight);
        updateHeight(pR);                   // in case unexpected changes
        }
    }

    void    balanceRight(AVLNode<T>* &pR){
        updateHeight(pR);
        
        if (diffHeight(pR) <= -2 && pR->_pRight != nullptr){               // subright tree is taller
            if (diffHeight(pR->_pRight) < 0)    // right-right case
                rotRR(pR);
            else
                rotRL(pR);                      // right-left case        
            
        updateHeight(pR->_pLeft);           // pR and its left child must be update
        updateHeight(pR->_pRight);
        updateHeight(pR);                   // in case unexpected changes
        }
    }

    void    findDuplication(AVLNode<T> *pR, T data, L1List<VRecord*>* &resultList){
        if (pR == nullptr)
            return;

        if (less(data, pR->_data))
            findDuplication(pR->_pLeft, data, resultList);
        else if (!equal(data, pR->_data))
            findDuplication(pR->_pRight, data, resultList);
        else {
            resultList->insertHead(pR->_pointRecord);
            findDuplication(pR->_pLeft, data, resultList);
            findDuplication(pR->_pRight, data, resultList);
        }
    }

    void    findDuplication(AVLNode<T> *pR, T data, void (*op)(VRecord*)){
        if (pR == nullptr)
            return;

        if (less(data, pR->_data))
            findDuplication(pR->_pLeft, data, op);
        else if (!equal(data, pR->_data))
            findDuplication(pR->_pRight, data, op);
        else {
            op(pR->_pointRecord);
            findDuplication(pR->_pRight, data, op);
        }
    }
};

// ******************************* define methods ********************************

// ***************************************************
// *                general operations               *
// ***************************************************
template <class T>
void AVLTree<T>::destroy(AVLNode<T>* &pR){
    if (pR == nullptr)
        return;
    destroy(pR->_pLeft);
    destroy(pR->_pRight);
    delete pR;
}

template <class T>
bool AVLTree<T>::find(AVLNode<T> *pR, T& key, T& ret){
    if (pR == nullptr){
        ret = nullptr;
        return false;
    }

    if (less(pR->_data, key))
        return find(pR->_pRight, key, ret);
    else if (less(key, pR->_data))
        return find(pR->_pLeft, key, ret);
    else {
        ret = (pR->_data);
        return true;
    }
}

template <class T>
void AVLTree<T>::insert(AVLNode<T>* &pR, T a, VRecord* node){
    if (pR == nullptr){
        pR = new AVLNode<T>(a);
        pR->_pLeft = pR->_pRight = nullptr;
        pR->_pointRecord = node;
        return;
    }

    if (less(a, pR->_data)){
        insert(pR->_pLeft, a, node);
        balanceLeft(pR);
    }
    else{
        insert(pR->_pRight, a, node);
        balanceRight(pR);
    }
}

template <class T>
void AVLTree<T>::remove(AVLNode<T>* &pR, T& a){
    // remove() method has 2 functions: 
        //     + find the node pDel contaning data matching removing one
        //     + find the rightmost pRightMost of the left subtree of pDel
        // There are 4 possibles cases for pDel:
        //   - has only 1 pLeft (pRight) child : point pR to that child and remove the current one
        //   - has no child                    : remove pR and set pR to nullptr
        //   - has 2 children                  : find pRightMost
        // There are 2 possible cases for pRightMost:
        //   - has only 1 left child : point pR to that child and delete data of pR
        //   - has no child          : remove pR's data and set pR to nullptr

    if (pR == nullptr)
        return;
    
    if (less(a, pR->_data)){                             // wanted node on the left    
        remove(pR->_pLeft, a);
        balanceLeft(pR);
        if (pR->_pRight)
            balanceRight(pR);
    }
    else if (less(pR->_data, a)){                         // wanted node on the right
        remove(pR->_pRight, a);
        balanceRight(pR);
        if (pR->_pRight)
            balanceLeft(pR);
    }
    else {                                          // reach the wanted node
        if (pR->_pLeft && pR->_pRight){             // 4th case of pDel
            AVLNode<T>* pRightMost = pR->_pLeft;
            while (pRightMost->_pRight != nullptr)  // find pRightMost
                pRightMost = pRightMost->_pRight;

            pR->_data = pRightMost->_data;          // copy data of pRightMost to pR

            remove(pR->_pLeft, pR->_data);
            balanceLeft(pR);
            if (pR->_pRight)
                balanceRight(pR);
        }
        else if (!pR->_pLeft && !pR->_pRight){      // 3th case of pDel ; 2nd case of pRightMost
            delete pR;
            pR = nullptr;
            return;
        }
        else {                                      // 1st, 2nc cases of pDel ; 1st case of pRightMost
            AVLNode<T>* &tmp = pR;
            pR = pR->_pLeft ? pR->_pLeft : pR->_pRight;
            delete tmp;
        }
    }
}

// *******************************************************************************
// *                             rotation and balance                            *
// - rotLeft, rotRight are fundamental operations                                *
// - rotLL, rotRR, rotLR, rotLR are operations for specific cases                *
//     2 last letters represent cases, for example rotRL used in left-right case *
//     and rotRL wild rotate right child first, and rotate left parents          *
// *******************************************************************************

template <class T>
void AVLTree<T>::rotLeft(AVLNode<T>* &pR){
    AVLNode<T>  *tmp = pR;
    pR = pR->_pRight;
    tmp->_pRight = pR->_pLeft;
    pR->_pLeft = tmp;
}

template <class T>
void AVLTree<T>::rotRight(AVLNode<T>* &pR){
    AVLNode<T>  *tmp = pR;
    pR = pR->_pLeft;
    tmp->_pLeft = pR->_pRight;
    pR->_pRight = tmp;
}


template <class T>
void AVLTree<T>::rotLL(AVLNode<T>* &pR){
    rotRight(pR);
}

template <class T>
void AVLTree<T>::rotRR(AVLNode<T>* &pR){
    rotLeft(pR);
}

template <class T>
void AVLTree<T>::rotLR(AVLNode<T>* &pR){
    rotLeft(pR->_pLeft);
    rotRight(pR);
}

template <class T>
void AVLTree<T>::rotRL(AVLNode<T>* &pR){
    rotRight(pR->_pRight);
    rotLeft(pR);
}

// *****************************************************************************
// *                                 traverse                                  *
// *****************************************************************************
template <class T>
void AVLTree<T>::traverseNLR(AVLNode<T> *pR, void (*op)(VRecord*)){
    if (pR == nullptr)
        return;
    op(pR->_pointRecord);
    traverseNLR(pR->_pLeft, op);
    traverseNLR(pR->_pRight, op);
}

template <class T>
void AVLTree<T>::traverseLNR(AVLNode<T> *pR, void (*op)(VRecord*)){
    if (pR == nullptr)
        return;    
    traverseLNR(pR->_pLeft, op);
    op(pR->_pointRecord);
    traverseLNR(pR->_pRight, op);
}

template <class T>
void AVLTree<T>::traverseLRN(AVLNode<T> *pR, void (*op)(VRecord*)){
    if (pR == nullptr)
        return;
    traverseLRN(pR->_pLeft, op);
    traverseLRN(pR->_pRight, op);
    op(pR->_pointRecord);
}

template <class T>
void AVLTree<T>::traverseLNR(AVLNode<T> *pR, void (*op)(IDInfo*)){
    if (pR == nullptr)
        return;
    op(pR->_data);
    traverseLNR(pR->_pLeft, op);
    traverseLNR(pR->_pRight, op);
}


#endif //A02_DSALIB_H