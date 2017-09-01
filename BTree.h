//
// Created by Jeza on 2017/9/1.
//

#ifndef TREE_BTREE_H
#define TREE_BTREE_H
#include<vector>
#include <iostream>
#include "Common.h"
using namespace std;
#define BTNodePosi(T) BTNode<T>*

template <typename T>
struct BTNode
{
    BTNodePosi(T) parent; //父节点
    vector<T> key; //关键码向量
    vector<BTNodePosi(T)> child; //孩子向量，比关键码向量长度大于一

    //构造函数
    BTNode() {parent=NULL;child.insert(child.begin(),NULL);}
    BTNode(T e,BTNodePosi(T) lc=NULL,BTNodePosi(T) rc=NULL)
    {
        parent=NULL;
        key.insert(key.begin(),e); //插入仅此一个关键码
        child.insert(child.begin(),lc); child.insert(child.begin()+1,rc); //插入两个孩子
        if(lc) lc->parent=this; if(rc) rc->parent=this;
    }
};

template <typename T>
class BTree
{
protected:
    int _size; //关键码总数，规模
    int _order; //B-树的阶次，一般不能修改
    BTNodePosi(T) _root; //根节点
    BTNodePosi(T) _hot; //search()最后访问的非空（除非树空）的节点位置
    void solveOverflow(BTNodePosi(T) e); //因插入而上溢之后的分裂处理
    void solveUnderflow(BTNodePosi(T) e); //因删除而下溢之后的分裂处理
public:
    BTree(int order=3):_order(order),_size(0) {_root=new BTNode<T>();} //构造函数：默认为最低的三阶
    ~BTree() {if(_root) release(_root);}
    int const order() {return _order;}
    int const size() {return _size;}
    BTNodePosi(T)& root() {return _root;}
    bool empty() const {return !_root;}

    BTNodePosi(T) search (const T& e); //查找
    bool insert(const T& e); //插入
    bool remove(const T& e); //删除
};
#endif //TREE_BTREE_H
