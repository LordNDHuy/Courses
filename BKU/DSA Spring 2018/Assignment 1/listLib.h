/*
 * =========================================================================================
 * Name        : listLib.h
 * Author      : Duc Dung Nguyen
 * Email       : nddung@hcmut.edu.vn
 * Copyright   : Faculty of Computer Science and Engineering - Bach Khoa University
 * Description : library for Assignment 1 - Data structures and Algorithms - Spring 2018
 * =========================================================================================
 */

#ifndef A01_LISTLIB_H
#define A01_LISTLIB_H

#include <string>
using namespace std;

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
struct L1Item {
    T data;
    L1Item<T> *pNext;
    L1Item() : pNext(NULL) {}
    L1Item(L1Item<T>* p) : pNext(p) {}
    L1Item(T &a) : data(a), pNext(NULL) {}
    L1Item(T &&a) : data(std::move(a)), pNext(NULL) {}
};

template <class T>
class L1List {
    L1Item<T>   *_pHead;// The head pointer of linked list
    size_t      _size;// number of elements in this list
public:
    L1List() : _pHead(NULL), _size(0) {}
    L1List(T input) : _pHead(new L1Item<T>(input)), _size(0) {}
    ~L1List()                         { clean();}

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
    bool    isEmpty() {
        return _pHead == NULL;
    }
    size_t  getSize() {
        return _size;
    }

    T&      at(int i){
        if (!(isEmpty() || i > (int)_size || i < 0)){


        	L1Item<T>   *pTmp = this->_pHead;
        	for (int count = 0; count < i; count++)
        		pTmp = pTmp->pNext;

        	return pTmp->data;
        }
    }
    L1Item<T>*      getHead(){
        return this->_pHead;
    }
    T&      operator[](int i){
        return at(i);
    }

    bool    find(T& a, int& idx){
        if (isEmpty()) return false;

        L1Item<T>   *pIterator = _pHead;
        for (idx = 0; pIterator->data != a && pIterator != NULL; pIterator = pIterator->pNext, idx++);

        if (pIterator != NULL) return true;
        else return false;
    }
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

    int     push_back(T& a);
    int     insertHead(T& a);
    int     insertHead() {
        _pHead = new L1Item<T>(_pHead);
        _size++;
        return 0;
    }

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

    //  3 versions of traverse() 
    //     2 of them loop to the end of recList
    //     1 of them halt by the time finding out
    void        traverse(void (*op)(L1Item<T>*&)) {
        L1Item<T>   *p = _pHead;
        while (p) {
            op(p);
            p = p->pNext;
        }
    }

    L1Item<T>*  traverse1(bool (*op)(T&, void*), void* pParam) {
        L1Item<T>   *p = _pHead;
        while (p) {
            if (op(p->data, pParam)) return p;
            p = p->pNext;
        }
        return NULL;
    }

    void        traverse(void (*op)(T&, void*, size_t&), void* param) {
        L1Item<T>   *p = _pHead;
        size_t      position = 0;
        while (p) {           
            op(p->data, param, position);
            p = p->pNext;
            position++;
        }
    }


    
};

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



#endif //A01_LISTLIB_H
