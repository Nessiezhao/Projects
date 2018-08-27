#pragma once
#include<iostream>
#include<vector>
using namespace std;

template< class T>
struct Less
{
    bool operator()(const T& l,const T& r)
    {
        return l < r;
    }
};

template< class T>
struct Greater
{
    bool operator()(const T& l,const T& r)
    {
        return l > r;
    }
};

template<class T,class Comper = Greater<T> > 
class Heap
{
public:
    Heap()
        :_a(0)
    {}

    Heap(T* a,size_t n)
    {
        _a.resize(n);
        //构建一个堆分为存储数据和调整结构
        for(size_t i = 0; i < n; i++)
        {
            _a[i] = a[i];
        }
        //进行下沉式调整，有多少个非叶子节点就调整多少次
        //并且从最后一个非叶子节点开始调整
        for(int i = ((int)n -2) / 2; i >= 0; i--)
        {
            _Adjustdown(i);
        }
    }

    size_t size()
    {
        return _a.size();
    }

    void Push(const T& x)
    {
        _a.push_back(x);
        _Adjustup(_a.size()-1);
    }

    void Pop()
    {
        swap(_a[0],_a[_a.size()-1]);
        _a.pop_back();
        _Adjustdown(0);
    }

    const T& Top()
    {
        return _a[0];
    }

protected:
    void _Adjustup(int child)
    {
        Comper com;
        int parent = (child-1)/2;
        int _child = child;
        while(parent >= 0)
        {
            if(com(_a[_child],_a[parent]))
            {
                swap(_a[_child],_a[parent]);
                _child = parent;
                parent = (_child -1)/2;
            }
            else
            {
                break;
            }
        }
    }

    void _Adjustdown(int root)//遍历一次的要求是将最后一个数找到
    {
        //创建对象
        Comper com;
        int parent = root;
        int child = root*2 + 1;
        while(child < (int)_a.size())
        {
            if(child+1 < (int)_a.size() && com(_a[child+1],_a[child]))
            {
                ++child;
            }
            if(com(_a[child],_a[parent]))
            {
                swap(_a[parent],_a[child]);
                parent = child;
                child = parent*2 + 1;
            }
            else
            {
                break;
            }
        }
    }
protected:
    vector<T> _a;
};
