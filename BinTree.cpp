//
// Created by Jeza on 2017/8/31.
//
#include "BinTree.h
#include <algorithm>
#include <stack>
#include "Common.h"
using namespace std;
//BinNode
template <typename T> int BinNode<T>::size() //统计当前节点后代总数，即以其为根的子树规模
{
    int s = 1; //计入本身
    if ( lc ) s += lc->size(); //递归计入左子树规模
    if ( rc ) s += rc->size(); //递归计入右子树规模
    return s;
}
template <typename T>
BinNodePosi(T) BinNode<T>::insertAsLC ( T const& e )
{ return lc = new BinNode ( e, this ); } //将e作为当前节点的左孩子插入二叉树

template <typename T>
BinNodePosi(T) BinNode<T>::insertAsRC ( T const& e )
{ return rc = new BinNode ( e, this ); } //将e作为当前节点的右孩子插入二叉树

template <typename T>
BinNodePosi(T) BinNode<T>::succ()  //定位节点v的直接后继
{
    BinNodePosi(T) s = this; //记录后继的临时变量
    if ( rc ) { //若有右孩子，则直接后继必在右子树中，具体地就是
        s = rc; //右子树中
        while ( HasLChild ( *s ) ) s = s->lc; //最靠左（最小）的节点
    } else { //否则，直接后继应是“将当前节点包含于其左子树中的最低祖先”，具体地就是
        while ( IsRChild ( *s ) ) s = s->parent; //逆向地沿右向分支，不断朝左上方移动
        s = s->parent; //最后再朝右上方移动一步，即抵达直接后继（如果存在）
    }
    return s;
}

//BinTree
//高度更新
template <typename T>
int BinTree<T>::updateHeight ( BinNodePosi(T) x ) //更新节点x高度
{
    return x->height = 1 + max ( stature ( x->lc ), stature ( x->rc ) );
}

template <typename T>
void BinTree<T>::updateHeightAbove ( BinNodePosi(T) x ) //更新节点x及其祖先的高度
{
    while ( x )
    {
        updateHeight ( x ); x = x->parent; //从x出发，覆盖历代祖先。可优化
    }
}

//节点插入
template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRoot ( T const& e ) //将e当作根节点插入空的二叉树
{
    _size = 1;  //首先更新规模
    return _root = new BinNode<T>(e);
}
template <typename T>
BinNodePosi(T) BinTree<T>::insertAsLC ( BinNodePosi(T) x, T const& e ) //e插入为x的左孩子
{
    _size++;  //首先更新规模
    x->insertAsLC ( e ); //接入
    updateHeightAbove ( x ); //更新x及其祖先的高度
    return x->lc;  //返回左孩子
}

template <typename T>
BinNodePosi(T) BinTree<T>::insertAsRC ( BinNodePosi(T) x, T const& e )//e插入为x的右孩子
{
    _size++;
    x->insertAsRC ( e );
    updateHeightAbove ( x );
    return x->rc;
}

//子树接入
template <typename T>
BinNodePosi(T) BinTree<T>::attachAsLC ( BinNodePosi(T) x, BinTree<T>* &S )
{

    if ( x->lc = S->_root ) x->lc->parent = x; //接入，这个循环的作用是防止新来的孩子是空节点从而导致循环内的语句错误
    _size += S->_size; updateHeightAbove ( x ); //更新全树规模与x所有祖先的高度
    S->_root = NULL; S->_size = 0; release ( S ); S = NULL; return x; //释放原树，返回接入位置
}

template <typename T>
BinNodePosi(T) BinTree<T>::attachAsRC ( BinNodePosi(T) x, BinTree<T>* &S )
{
    if ( x->rc = S->_root ) x->rc->parent = x;
    _size += S->_size; updateHeightAbove ( x );
    S->_root = NULL; S->_size = 0; release ( S ); S = NULL; return x;
}

//子树分离
template <typename T> //二叉树子树分离算法：将子树x从当前树中摘除，将其封装为一棵独立子树返回
BinTree<T>* BinTree<T>::secede ( BinNodePosi(T) x ) { //assert: x为二叉树中的合法位置
    FromParentTo ( *x ) = NULL; //切断来自父节点的指针，也就是父亲不认这个儿子了
    updateHeightAbove ( x->parent ); //更新原树中所有祖先的高度，仍然可以用parent是因为儿子仍旧认这个爹
    BinTree<T>* S = new BinTree<T>; S->_root = x; x->parent = NULL; //新树以x为根
    S->_size = x->size(); _size -= S->_size; return S; //更新规模，返回分离出来的子树
}

//删除
template <typename T>
static int removeAt ( BinNodePosi(T) x )  //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
{
    if ( !x ) return 0; //递归基：空树
    int n = 1 + removeAt ( x->lc ) + removeAt ( x->rc ); //递归释放左、右子树
    release ( x->data ); release ( x ); return n; //释放被摘除节点，并返回删除节点总数
}

template <typename T>
int BinTree<T>::remove ( BinNodePosi(T) x ) //删除二叉树中位置x处的节点及其后代，返回被删除节点的数值
{
    FromParentTo ( *x ) = NULL; //切断来自父节点的指针
    updateHeightAbove ( x->parent ); //更新祖先高度
    int n = removeAt ( x ); _size -= n; return n; //删除子树x，更新规模，返回删除节点总数
}

//遍历
//递归遍历
template <typename T, typename VST> //元素类型、操作器
void travPre_R ( BinNodePosi(T) x, VST& visit ) //二叉树先序遍历算法
{
    if ( !x ) return;
    visit ( x->data );
    travPre_R ( x->lc, visit );
    travPre_R ( x->rc, visit );
}
template <typename T, typename VST> //元素类型、操作器
void travIn_R ( BinNodePosi(T) x, VST& visit ) //二叉树中序遍历算法
{
    if ( !x ) return;
    travIn_R ( x->lc, visit );
    visit ( x->data );
    travIn_R ( x->rc, visit );
}
template <typename T, typename VST> //元素类型、操作器
void travPost_R ( BinNodePosi(T) x, VST& visit ) //二叉树后序遍历算法
{
    if ( !x ) return;
    travPost_R ( x->lc, visit );
    travPost_R ( x->rc, visit );
    visit ( x->data );
}

//迭代遍历
//先序遍历#2
template <typename T, typename VST>
static void visitAlongLeftBranch ( BinNodePosi(T) x, VST& visit, stack<BinNodePosi(T)>& S )
{
    while ( x ) {
        visit ( x->data );
        S.push ( x->rc );
        x = x->lc;
    }
}
template <typename T, typename VST>
void travPre_I2 ( BinNodePosi(T) x, VST& visit ) {
    stack<BinNodePosi(T)> S;
    while ( true ) {
        visitAlongLeftBranch ( x, visit, S );
        if ( S.empty() ) break;
        x = S.top();S.pop();
    }
}
//中序遍历#1
template <typename T>
static void goAlongLeftBranch ( BinNodePosi(T) x, stack<BinNodePosi(T)>& S ) //从当前节点出发，沿左分支不断深入，直至没有左分支的节点
{
    while ( x ) { S.push ( x ); x = x->lc; } //当前节点入栈后随即向左侧分支深入，迭代直到无左孩子
}

template <typename T, typename VST> //元素类型、操作器
void travIn_I1 ( BinNodePosi(T) x, VST& visit )
{
    stack<BinNodePosi(T)> S; //辅助栈
    while ( true ) {
        goAlongLeftBranch ( x, S ); //从当前节点出发，逐批入栈
        if ( S.empty() ) break; //直至所有节点处理完毕
        x = S.top();S.pop(); visit ( x->data ); //弹出栈顶节点并访问之
        x = x->rc; //转向右子树
    }
}
//中序遍历#2
template <typename T, typename VST> //元素类型、操作器
void travIn_I2 ( BinNodePosi(T) x, VST& visit )
{
    stack<BinNodePosi(T)> S; //辅助栈
    while ( true )
        if ( x ) {
            S.push ( x ); //根节点进栈
            x = x->lc; //深入遍历左子树
        } else if ( !S.empty() ) {
            x = S.top();S.pop(); //尚未访问的最低祖先节点退栈
            visit ( x->data ); //访问该祖先节点
            x = x->rc; //遍历祖先的右子树
        } else
            break; //遍历完成
}
//中序遍历#3
template <typename T, typename VST> //元素类型、操作器
void travIn_I3 ( BinNodePosi(T) x, VST& visit )
{
    bool backtrack = false; //前一步是否刚从左子树回溯——省去栈，仅O(1)辅助空间
    while ( true )
        if ( !backtrack && HasLChild ( *x ) ) //若有左子树且不是刚刚回溯，则
            x = x->lc; //深入遍历左子树
        else { //否则——无左子树或刚刚回溯（相当于无左子树）
            visit ( x->data ); //访问该节点
            if ( HasRChild ( *x ) ) { //若其右子树非空，则
                x = x->rc; //深入右子树继续遍历
                backtrack = false; //并关闭回溯标志
            } else { //若右子树空，则
                if ( ! ( x = x->succ() ) ) break; //回溯到直接后继（含抵达末节点时的退出返回）
                backtrack = true; //并设置回溯标志
            }
        }
}

//后序遍历#1
template <typename T> //在以S栈顶节点为根的子树中，找到最高左侧可见叶节点
static void gotoHLVFL ( stack<BinNodePosi(T)>& S ) { //沿途所遇节点依次入栈
    while ( BinNodePosi(T) x = S.top() ) //自顶而下，反复检查当前节点（即栈顶）
        if ( HasLChild ( *x ) ) { //尽可能向左
            if ( HasRChild ( *x ) ) S.push ( x->rc ); //若有右孩子，优先入栈
            S.push ( x->lc ); //然后才转至左孩子
        } else //实不得已
            S.push ( x->rc ); //才向右
    S.pop(); //返回之前，弹出栈顶的空节点
}

template <typename T, typename VST>
void travPost_I ( BinNodePosi(T) x, VST& visit ) { //二叉树的后序遍历（迭代版）
    stack<BinNodePosi(T)> S; //辅助栈
    if ( x ) S.push ( x ); //根节点入栈
    while ( !S.empty() ) {
        if ( S.top() != x->parent ) //若栈顶非当前节点之父（则必为其右兄），此时需
            gotoHLVFL ( S ); //在以其右兄为根之子树中，找到HLVFL（相当于递归深入其中）
        x = S.top();S.pop(); visit ( x->data ); //弹出栈顶（即前一节点之后继），并访问之
    }
}