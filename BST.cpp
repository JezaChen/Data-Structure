//
// Created by Jeza on 2017/8/31.
//
#include "BST.h"
#include "Common.h"
#include <algorithm>
using namespace std;
template <typename T>
static BinNodePosi(T) & searchIn ( BinNodePosi(T) & v, const T& e, BinNodePosi(T) & hot )//在以v为根的（AVL、SPLAY、rbTree等）BST子树中查找关键码e
{
    if ( !v || ( e == v->data ) ) return v; //递归基：在节点v（或假想的通配节点）处命中
    hot = v; //一般情况：先记下当前节点，然后再
    return searchIn ( ( ( e < v->data ) ? v->lc : v->rc ), e, hot ); //深入一层，递归查找
} //返回时，返回值指向命中节点（或假想的通配哨兵），hot指向其父亲（退化时为初始值NULL）

template <typename T>
BinNodePosi(T) & BST<T>::search ( const T& e ) //在BST中查找关键码e
{ return searchIn ( _root, e, _hot = NULL ); } //返回目标节点位置的引用，以便后续插入、删除操作

template <typename T>
BinNodePosi(T) BST<T>::insert ( const T& e ) //将关键码e插入BST树中
{
    BinNodePosi(T) & x = search ( e ); if ( x ) return x; //确认目标不存在（留意对_hot的设置）
    x = new BinNode<T> ( e, _hot ); //创建新节点x：以e为关键码，以_hot为父
    _size++; //更新全树规模
    updateHeightAbove ( x ); //更新x及其历代祖先的高度
    return x; //新插入的节点，必为叶子
} //无论e是否存在于原树中，返回时总有x->data == e

template <typename T>
static BinNodePosi(T) removeAt ( BinNodePosi(T) & x, BinNodePosi(T) & hot )
{
    BinNodePosi(T) w = x;  //实际被摘除的节点，初值同x
    BinNodePosi(T) succ = NULL;  //实际被删除节点的接替者
    if ( !HasLChild ( *x ) ) //若*x的左子树为空，则可
        succ = x = x->rc; //直接将*x替换为其右子树
    else if ( !HasRChild ( *x ) ) //若右子树为空，则可
        succ = x = x->lc; //对称地处理——注意：此时succ != NULL
    else { //若左右子树均存在，则选择x的直接后继作为实际被摘除节点，为此需要
        w = w->succ(); //（在右子树中）找到*x的直接后继*w
        swap ( x->data, w->data ); //交换*x和*w的数据元素
        BinNodePosi(T) u = w->parent;
        ( ( u == x ) ? u->rc : u->lc ) = succ = w->rc; //隔离节点*w
    }
    hot = w->parent; //记录实际被删除节点的父亲
    if ( succ ) succ->parent = hot; //并将被删除节点的接替者与hot相联
    release ( w->data ); release ( w ); return succ; //释放被摘除节点，返回接替者
}

template <typename T> bool BST<T>::remove ( const T& e ) //从BST树中删除关键码e
{
    BinNodePosi(T) & x = search ( e ); if ( !x ) return false; //确认目标存在（留意_hot的设置）
    removeAt ( x, _hot ); _size--; //实施删除
    updateHeightAbove ( _hot ); //更新_hot及其历代祖先的高度
    return true;
} //删除成功与否，由返回值指示


template <typename T>
BinNodePosi(T) BST<T>::connect34(BinNode<T> *a, BinNode<T> *b, BinNode<T> *c, BinNode<T> *T0, BinNode<T> *T1, BinNode<T> *T2,
                                 BinNode<T> *T3)
{
    a->lc=T0; if(T0) T0->parent=a;
    a->rc=T1; if(T1) T1->parent=a;
    //updateHeightAbove(a);
    updateHeight(a);
    c->lc=T2; if(T2) T2->parent=c;
    c->rc=T3; if(T3) T3->parent=c;
    //updateHeightAbove(c);
    updateHeight(c);
    b->lc=a; b->rc=c;
    a->parent=b; c->parent=b;
    updateHeight(b);
    return b;
}

template <typename T>
BinNodePosi(T) BST<T>::rotateAt(BinNode<T> *v)  //旋转
{
    BinNodePosi(T) p=v->parent;BinNodePosi(T) g=p->parent; //v,p,g有四种情况
    if(IsLChild(*p)) //zig
    {
        if(IsLChild(*v)) //zig-zig
        {
            p->parent=g->parent; //认爹
            return connect34(v,p,g,v->lc,v->rc,p->rc,g->rc);
        }
        else //zig-zag
        {
            v->parent=g->parent;
            return connect34(p,v,g,p->lc,v->lc,v->rc,g->rc);
        }
    }
    else //zag
    {
        if(IsRChild(*v)) //zag-zag
        {
            p->parent=g->parent;
            return connect34(g,p,v,g->lc,p->lc,v->lc,v->rc);
        }
        else //zag-zig
        {
             v->parent=g->parent;
            return connect34(g,v,p,g->lc,v->lc,v->rc,p->rc);
        }
    }
}

//AVL Tree
template <typename T>
BinNodePosi(T) AVL<T>::insert(const T &e)
{
    BinNodePosi(T) &x=search(e);if(x) return x;
    BinNodePosi(T) xx=x=new BinNode<T> (e,_hot); _size++; //创建新节点xx，并且认_hot为爹
    for(BinNodePosi(T) g=_hot;g;g=g->parent)
    {
        if(!AvlBalanced(*g))
        {
            FromParentTo(*g)=rotateAt(tallerChild(tallerChild(g)));
            break;
        }
        else
            updateHeight(g);
    }
    return xx; //返回新节点的位置
}
template<typename T>
bool AVL<T>::remove(const T &e)
{
    BinNodePosi(T) &x=search(e); if(!x) return false; //确保目标存在
    removeAt(x,_hot); _size--; //先将其删除
    //然后再旋转调整
    for(BinNodePosi(T) g=_hot;g;g=g->parent)
    {
        if(!AvlBalanced(*g)) //一旦发现g失衡
          g=FromParentTo(*g)=rotateAt(tallerChild(tallerChild(g))); //复衡
        updateHeight(g); //删除的时候高度可能会降低，所以要时刻更新高度
        //发现失衡不能立刻终止循环，因为删除操作导致的失衡会向上传播
    }
    return true;
}

template<typename NodePosi> inline
void attachAsLChild (NodePosi p,NodePosi lc)
{
    p->lc=lc;if(lc) lc->parent=p;
}
template<typename NodePosi> inline
void attachAsRChild(NodePosi p,NodePosi rc)
{
    p->rc=rc;if(rc) rc->parent=p;
}

//Splay Tree
template<typename T>
BinNodePosi(T) Splay<T>::splay(BinNode<T> *v) //伸展算法
{
    if(!v) return NULL;
    BinNodePosi(T) p;BinNodePosi(T) g; //父亲、祖父
    while((p=v->parent)&&(g=p->parent)) //自下而上反复对节点v做双层伸展
    {
        BinNodePosi(T) gg=g->parent; //曾祖父
        if(IsLChild(*v))
        {
            if(IsLChild(*p))  //zig-zig,同侧，爷转爹再转
            {
                //各个子树的顺序其实没有发生变化
                 attachAsLChild(g,p->rc); attachAsLChild(p,v->rc);
                 attachAsRChild(p,g); attachAsRChild(v,p);
            }
            else //zig-zag，异侧，爹转爷再转
            {
                attachAsLChild(p,v->rc); attachAsRChild(g,v->lc);
                attachAsLChild(v,g); attachAsRChild(v,p);
            }
        }
        else
        {
            if(IsRChild(*p)) //zag-zag，同侧，爷转爹再转
            {
                attachAsRChild(g,p->lc); attachAsRChild(p,v->lc);
                attachAsLChild(p,g); attachAsLChild(v,g);
            }
            else //zag-zig,异侧，爹转爷再转
            {
                attachAsRChild(p,v->lc); attachAsLChild(g,v->rc);
                attachAsRChild(v,g); attachAsLChild(v,p);
            }

        }
        if(!gg) v->parent=NULL; //此时v在此条件（原先的曾祖父不存在）下应为树根
        else
        {
            (g==gg->lc)?(attachAsLChild(gg,v)):(attachAsRChild(gg,v));
        }
        updateHeight(g); updateHeight(p); updateHeight(v);
    }
    if(p=v->parent) //如果父亲仍旧存在，还需要一次单旋
    {
        if(IsLChild(*v)) {attachAsLChild(p,v->rc);attachAsRChild(v,p);}
        else {attachAsRChild(p,v->lc);attachAsLChild(v,p);}
        updateHeight(p);updateHeight(v);
    }
    v->parent=NULL;return v;
}

template<typename T>
BinNodePosi(T)& Splay<T>::search(const T &e)
{
    BinNodePosi(T) p=searchIn(_root,e,_hot=NULL);
    _root=splay(p?p:_hot); //无论找到与否，将最后一个被访问的节点伸展至根
    return _root;
}

template<typename T>
BinNodePosi(T) Splay<T>::insert(const T &e)
{
    if(!_root)
    {
        _size++; return _root=new BinNode<T> (e);
    }
    if(e==search(e)->data) return _root; //确保不存在
    _size++; BinNodePosi(T) t=_root;
    if(_root->data<e)
    {
        t->parent=_root=new BinNode<T>(e,NULL,t,t->rc);
        if(HasRChild(*t)) {t->rc->parent=_root;t->rc=NULL;} //存在的孩子重新认爹
    }
    else
    {
        t->parent=_root=new BinNode<T>(e,NULL,t->lc,t);
        if(HasLChild(*t)) {t->lc->parent=_root;t->lc=NULL;}
    }
    updateHeightAbove(t);
    return _root;
}

template<typename T>
bool Splay<T>::remove(const T &e)
{
    if(!_root||e!=search(e)->data) return false; //若树空或者目标不存在，则无法删除
    BinNodePosi(T) w=_root; //临时变量
    if(!HasLChild(*_root)) //如果树根没有左孩子
    {
        _root=_root->rc; if(_root) _root->parent=NULL; //直接删除
    }
    else if(!HasRChild(*_root))
    {
        _root=_root->lc; if(_root) _root->parent=NULL;
    }
    else  //双分支的情况下
    {
        BinNodePosi(T) lTree=_root->lc;
        lTree->parent=NULL; _root->lc=NULL; //暂时将左子树切除
        _root=_root->rc; //切掉，顺便删除
        _root->parent=NULL;
        search(w->data);
        _root->lc=lTree; lTree->parent=_root; //再接上去
    }
    release(w->data); release(w); _size--;
    if(_root) updateHeight(_root); //只有根节点的高度需要更新
    return true;
}


//Red-Black Tree
template<typename T>
int RedBlack<T>::updateHeight(BinNode<T> *x)  //更新节点的黑高度
{
    x->height=max(stature(x->lc),stature(x->rc)); //孩子一般黑高度相等，除非出现双黑现象
    return IsBlack(x)?x->height+1:x->height; //若当前节点为黑，则黑高度加1
}

//双红修正
template<typename T>
void RedBlack<T>::solveDoubleRed(BinNode<T> *x)  //解决双红问题
{
    if(IsRoot(*x)) //若已递归到树根，则将其转黑，整树黑高度增一
    {
        _root->color=RB_BLACK; _root->height++;
         return ;
    }
    BinNodePosi(T) p=x->parent; if(IsBlack(p)) return; //若父节点为黑，则可以终止调整
    BinNodePosi(T) g=p->parent; //既然p为红，则可知x的祖父必然存在，且一定为黑色
    BinNodePosi(T) u=uncle(x); //u为x的叔父
    //分两种情况讨论
    if(IsBlack(u)) //若叔父u为黑色
    {
        //旋转并染色，不用上升
        if(IsLChild(*x)==IsLChild(*p)) //若x与p同侧，zig-zig||zag-zag
        {
            //即p在中间，染成黑色，x和g在两侧，染成红色
            p->color=RB_BLACK;
        }
        else //否则，异侧，zig-zag||zag-zig
        {
            //即x在中间，染成黑色，p和g在两侧，染成红色
            x->color=RB_BLACK;
        }
        g->color=RB_RED; //g的颜色必定染成红色
        BinNodePosi(T) gg=g->parent; //x的曾祖父
        BinNodePosi(T) r=FromParentTo(*g)=rotateAt(x); //旋转接入
        r->parent=gg;
    }
    else //若u为红色
    {
        p->color=RB_BLACK; p->height++;
        u->color=RB_BLACK; u->height++;
        if(!IsRoot(*g)) g->color=RB_RED; //若g非根，则转红
        solveDoubleRed(g); //继续调整
    }
}

//双黑修正
/******************************************************************
 * RedBlack双黑调整算法:解决节点s与被其替代的节点皆为黑色的问题
 * 分为三大类四种情况：
 * BB-1（黑s有红子t）:2次颜色翻转，2次黑高度更新，1-2次旋转，不再递归
 * BB-2R（黑s无红子，p红）:2次颜色翻转，2次黑高度更新，1-2次旋转，不再递归
 * BB-2B（黑s有红子，p黑）:1次颜色翻转，1次黑高度跟新，0次旋转，需要递归
 * BB-3（红s）:2次颜色翻转，2次黑高度更新，1次旋转，转为BB-1或BB-2R
 *******************************************************************/
template<typename T>
void RedBlack<T>::solveDoubleBlack(BinNode<T> *r)
{
    BinNodePosi(T) p=r?r->parent:_hot; //why? //r的父亲
    BinNodePosi(T) s=(r==p->lc)?p->rc:p->lc; //r的兄弟
    if(IsBlack(s)) //如果兄弟是黑色的
    {
       BinNodePosi(T) t=NULL; //s的红孩子，若左右孩子皆为红色，左孩子优先；如果都是黑色的，则为NULL
       if(IsRed(s->rc)) t=s->rc;
       if(IsRed(s->lc)) t=s->lc;
       if(t) //如果黑s有红孩子，BB-1
       {
           //t变黑，p变黑，s变为p的颜色
           RBColor OldColor=p->color;
           //通过旋转重平衡，并将新子树的左右孩子都染成黑色
           BinNodePosi(T) b=FromParentTo(*p)=rotateAt(t);
           if(HasLChild(*p)) {p->lc->color=RB_BLACK;updateHeight(p->lc);}
           if(HasRChild(*p)) {p->rc->color=RB_BLACK;updateHeight(p->rc);}
           b->color=OldColor; updateHeight(b);
       }
       else //如果黑s没有红孩子
       {
           s->color=RB_RED; //s都是转为红色
           if(IsRed(p))  //如果p是红色的，BB-2R
           {
               p->color=RB_BLACK; //p转成黑色
           }
           else  //BB-2B，需要上溯
           {
               //p保持黑色，高度减一
               p->height--;
               solveDoubleBlack(p); //递归上溯
           }
       }
    }
    else //如果兄弟是红色的，BB-3
    {
        //转换成BB-1或者BB-2R的情况
        s->color=RB_BLACK;p->color=RB_RED;
        BinNodePosi(T) t=IsLChild(*s)?s->lc:s->rc; //取t与s同侧，用于旋转
        _hot=p;FromParentTo(*p)=rotateAt(t);
        solveDoubleBlack(r); //继续修正，只递归一次，因为p已经转为红色
    }
}

template<typename T>
BinNodePosi(T) RedBlack<T>::insert(const T &e)
{
    BinNodePosi(T) &x=search(e);
    if(x) return x;
    x=new BinNode<T> (e,_hot,NULL,NULL,-1); _size++;
    solveDoubleRed(x);
    return x?x:_hot->parent; //why?
}

template <typename T>
bool RedBlack<T>::remove(const T &e)
{
    BinNodePosi(T) &x=search(e); if(!x) return false; //确保目标存在
    BinNodePosi(T) r=removeAt(x,_hot); //r为实际删除者的后继者
    //此时hot已经变为实际删除者的父亲
    if(!(--_size)) return true;
    if(_hot) //若刚被删除的点是根节点，则将其置黑，并更新根节点的黑高度
    {
        _root->color=RB_BLACK; updateHeight(_root); return true;
    }
    if(BlackHeightUpdated(*_hot)) return true; //若所有的祖先的黑深度依旧平衡，则说明所删除的点是红节点，无需变动
    if(IsRed(r)) //如果节点r是红色的，只需转成黑色的即可
    {
        r->color=RB_BLACK; r->height++; return true;
    }
    solveDoubleBlack(r); //否则需要进行双黑修正
    return true;
}
