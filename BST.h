//
// Created by Jeza on 2017/8/31.
//

#ifndef TREE_BST_H
#define TREE_BST_H
#include "BinTree.h"

//用于简化AVL树算法描述的宏
#define Balanced(x) (stature((x).lc)==stature((x).rc))
#define BalFac(x) (stature((x).lc)-stature((x).rc))
#define AvlBalanced(x) ((-2<BalFac(x))&&(BalFac(x)<2))
#define tallerChild(x) \
(stature((x)->lc)>stature((x)->rc)?(x)->lc:(\
stature((x)->lc)<stature((x)->rc))?(x)->rc:(\
IsLChild(*(x))?(x)->lc:(x)->rc))

template <typename T> class BST : public BinTree<T> { //由BinTree派生BST模板类
protected:
    BinNodePosi(T) _hot; //“命中”节点的父亲
    BinNodePosi(T) connect34 ( //按照“3 + 4”结构，联接3个节点及四棵子树，用于AVL树等
            BinNodePosi(T), BinNodePosi(T), BinNodePosi(T),
            BinNodePosi(T), BinNodePosi(T), BinNodePosi(T), BinNodePosi(T) );
    BinNodePosi(T) rotateAt ( BinNodePosi(T) x ); //对x及其父亲、祖父做统一旋转调整

public: //基本接口：以virtual修饰，强制要求所有派生类（BST变种）根据各自的规则对其重写
    virtual BinNodePosi(T) & search ( const T& e ); //查找
    virtual BinNodePosi(T) insert ( const T& e ); //插入
    virtual bool remove ( const T& e ); //删除
};

template<typename T>
class AVL:public BST<T>
{
public:
    BinNodePosi(T) insert(const T& e); //重写插入
    virtual bool remove(const T& e); //删除
};

template<typename T>
class Splay:public BST<T>
{
protected:
    BinNodePosi(T) splay(BinNodePosi(T) v); //将节点v伸展至根
public:
    BinNodePosi(T) & search (const T& e); //查找，重写
    BinNodePosi(T) insert(const T& e); //插入，重写
    bool remove(const T& e); //删除，重写
};

#endif //TREE_BST_H
