#include<iostream>
#include"./Heap.hpp"
template<class W>
struct HuffmanTreeNode
{
    HuffmanTreeNode(const W& weight)
        :_pLeft(NULL)
         ,_pRight(NULL)
         ,_pParent(NULL)
         ,_weight(weight)
    {}

    HuffmanTreeNode<W>* _pLeft;
    HuffmanTreeNode<W>* _pRight;
    HuffmanTreeNode<W>* _pParent;
    W _weight;
};

template<class W>
class HuffmanTree
{
    typedef HuffmanTreeNode<W> PNode;
public:
    HuffmanTree()
        :_pRoot(NULL)
    {}

    HuffmanTree(W* array,size_t size,const W& invalid)
    {
        _CreateHuffmanTree(array,size,invalid);
    }

    void _Destroy(PNode* root)
    {
        if(root == NULL)
            return;
        _Destroy(root->_pLeft);
        _Destroy(root->_pRight);
        delete root;
    }

    ~HuffmanTree()
    {
        _Destroy(_pRoot);
        _pRoot = NULL;
    }

    PNode* GetRoot()
    {
        return _pRoot;
    }
public:
    void _CreateHuffmanTree(W* array,size_t size,const W& invalid)
    {
        //贪心算法
        struct NodeCompare
        {
            bool operator()(PNode* l,PNode* r)
            //bool operator()(const HuffmanTreeNode<W>* l,const HuffmanTreeNode<W>* r)const
            {
                return l->_weight < r->_weight;
            }
        };
        //建小堆
        Heap<PNode*,NodeCompare> min;
        for(size_t i = 0; i < size; i++)
        {
            if(array[i] != invalid)
            {
                min.Push(new PNode(array[i]));
            }
        }

        //每次取最小，直到只剩一个节点时
        while(min.size() > 1)
        {
            PNode* num1 = min.Top();
            min.Pop();
            PNode* num2 = min.Top();
            min.Pop();

            PNode* tmp = new PNode(num1->_weight + num2->_weight);
            tmp->_pLeft = num1;
            tmp->_pRight = num2;
            num1->_pParent = tmp;
            num2->_pParent = tmp;
            min.Push(tmp);
        }
        _pRoot = min.Top();
    }
protected:
    HuffmanTree(const HuffmanTree&);//防拷贝
    HuffmanTree& operator=(const HuffmanTree&);
protected:
    PNode* _pRoot;
};
