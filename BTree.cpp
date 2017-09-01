//
// Created by Jeza on 2017/9/1.
//
#include "BTree.h"
using namespace std;

template<typename T>
static int binSearch(vector<T> A,T const& e,int lo,int hi)
{
    while(lo<hi)
    {
        int mi=(lo+hi)>>1;
        (e<A[mi])?hi=mi:lo=mi+1;
    } //查找成功不能提前终止
    return --lo; //循环结束时，lo为大于e的元素的最小秩，故lo-1即不大于e的元素的最大秩
}


template <typename T>
int searchIn(vector<T> v,const T& e) //在该向量中寻找不大于e的最大关键码
{
    /* int rank=-1;
     while(v[++rank]<=e);
     return rank-1;*/
    return binSearch(v,e,0,v.size()-1);
}

template<typename T>
BTNodePosi(T) BTree<T>::search(const T &e)
{
    BTNodePosi(T) v=_root;_hot=NULL;
    while(v)
    {
        int r=searchIn(v->key,e); //在当前节点中，找到不大于e的最大关键码
        if((0<=r)&&(e==v->key[r])) return v; //成功命中
        else
        {
            _hot=v;
            v=v->child[r+1];
        }
    }
    return NULL;
}

template <typename T>
void BTree<T>::solveOverflow(BTNodePosi(T) v)
{
    if(_order>=v->child.size()) return; //递归基，当前节点并未上溢
    int s=_order/2; //轴点，此时应有_order=key.size()=child.size()-1
    BTNodePosi(T) u=new BTNode<T>(); //值得注意的是新节点已经有一个空孩子
        for(int j=0;j<_order-s-1;j++) //v右侧_order-s-1个孩子及关键码分裂为右侧节点u
    {
        u->child.insert(u->child.begin()+j,v->child[s+1]); v->child.erase(v->child.begin()+s+1);
        u->key.insert(u->key.begin()+j,v->key[s+1]); v->child.erase(v->key.begin()+s+1);
    }
    u->child[_order-s-1]=v->child[s+1]; v->child.erase(v->child.begin()+s+1); //移动v最靠右的孩子
    if(u->child[0])
        for(int j=0;j<_order-s;j++)  u->child[j]->parent=u;
    BTNodePosi(T) p=v->parent;
    if(!p) //如果p为空
    {
        _root=p=new BTNode<T>();
        p->child[0]=v;v->parent=p;
    }
    int r=1+searchIn(p->key,v->key[0]); //r为p中指向u的指针的秩
    p->key.insert(p->key.begin()+r,v->key[s]);  //轴点关键码上升
    v->key.erase(v->key.begin()+s);
    p->child.insert(p->child.begin()+r+1,u); u->parent=p;
    solveOverflow(p); //上升一层
}

template<typename T>
void BTree<T>::solveUnderflow(BTNode<T> *v)
{
    if((_order+1)/2<=v->child.size()) return ;//递归基，当前节点并未下溢
    BTNodePosi(T) p=v->parent;
    if(!p) //递归基，已到根节点，没有孩子的下限
    {
        if(!v->key.size()&&v->child[0]) //倘若作为树根的v已经不含关键码，却有唯一的非空孩子
        {
            _root=v->child[0]; _root->parent=NULL;//v的唯一孩子变成根节点
            v->child[0]=NULL; release(v); //销毁v
        }
        return ;
    }
    int r=0; while(p->child[r]!=v) r++; //找到v的位置（第r个）
    //Situation1:向左兄弟借
    if(0<r) //若p不是最左边的孩子
    {
        BTNodePosi(T) ls=p->child[r-1]; //则左兄弟必然存在
        if((_order+1)<ls->child.size())  //若左兄弟足够胖
        {
            v->key.insert(v->key.begin(),p->key[r-1]); //p借出一个关键码给v作为最小关键码
            p->key[r-1]=ls->key[ls->key.size()-1]; ls->key.pop_back(); //ls的最大关键码转给p
            v->child.insert(v->child.begin(),ls->child[ls->child.size()-1]); ls->child.pop_back(); //同时ls的最右侧的孩子过继给v
            //作为最左侧的孩子
            return;
        }
    }
    //Situation2:向右兄弟借
    if(r<p->child.size()-1) //若v不是p最右侧的孩子
    {
        BTNodePosi(T) rs=p->child[r+1];
        if((_order+1)<rs->child.size()) //若右兄弟足够胖
        {
            v->key.insert(v->key.end(),p->key[r]); //p借出一个关键码给v作为最大关键码
            p->key[r]=rs->key[0]; rs->key.erase(rs->key.begin()); //rs的最小关键码
            v->child.insert(v->child.end(),rs->child[0]); rs->child.erase(rs->child.begin()); //同时rs的最左侧的孩子过继给v
            //作为最右侧的孩子
            return;
        }
    }
    //Situation3:左右兄弟要么都为空，要么都太瘦，采取合并操作
    if(0<r)  //与左兄弟合并
    {
        BTNodePosi(T) ls=p->child[r-1];
        ls->key.insert(ls->key.end(),p->key[r-1]); p->key.erase(p->key.begin()+r-1); //p的第r-1个关键码转入ls
        p->child.erase(p->child.begin()+r); //同时v不再是p的第r个孩子
        ls->child.insert(ls->child.end(),v->child[0]); v->child.erase(v->child.begin());
        if(ls->child[ls->child.size()-1])  //将v的最左侧孩子过继给ls做最右侧的孩子
            ls->child[ls->child.size()-1]->parent=ls;
        while(!v->key.empty())
        {
            ls->key.insert(ls->key.end(),v->key[0]); v->key.erase(v->key.begin());
            ls->child.insert(ls->child.end(),v->child[0]); v->child.erase(v->child.begin());
            if(ls->child[ls->child.size()-1]) ls->child[ls->child.size()-1]->parent=ls;
        }
        release(v);
    }
    else //与右兄弟合并
    {
        BTNodePosi(T) rs=p->child[r+1];
        rs->key.insert(rs->key.begin(),p->key[r]); p->child.erase(p->child.begin()+r);
        rs->child.insert(rs->child.begin(),v->child[v->child.size()-1]); v->child.pop_back();
        if(rs->child[0]) rs->child[0]->parent=rs;
        while(!v->key.empty())
        {
            rs->key.insert(rs->key.begin(),v->key[v->key.size()-1]); v->key.pop_back();
            rs->child.insert(rs->child.begin(),v->child[v->child.size()-1]); v->child.pop_back();
            if(rs->child[0]) rs->child[0]->parent=rs;
        }
        release(v);
    }
    solveUnderflow(p); //上升一层，如有必要则继续分裂
    return ;
}

template <typename T>
bool BTree<T>::insert(const T &e)
{
    BTNodePosi(T) v=search(e); if(v) return false; //确认目标节点不存在
    int r=searchIn(_hot->key,e); //在节点_hot的有关关键码向量中查找合适的插入位置
    _hot->key.insert(_hot->key.begin()+r+1,e); //将关键码插入相应的位置上
    _hot->child.insert(_hot->key.begin()+r+2,NULL); //创建一个空子树指针
    _size++; //更新规模
    solveOverflow(_hot);
    return true;
}

template<typename T>
bool BTree<T>::remove(const T &e)
{
    BTNodePosi(T) v=search(v); if(!v) return false; //确保删除目标存在
    int r=searchIn(v->key,e);
    if(v->child[0]) //如果v不是叶子，需要寻找e的后继
    {
        BTNodePosi(T) u=v->child[r+1];
        while(u->child[0]) u=u->child[0]; //一直向左
        v->key[r]=u->key[0]; v=u;r=0; //交换位置
    }
    v->key.erase(v->key.begin()+r); v->child.erase(v->child.begin()+r+1);_size--;
    solveUnderflow(v);
    return true;
}