#include<iostream>
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
    typedef HuffmanTreeNode<W>* PNode;
public:
    HuffmanTree()
        :_pRoot(NULL)
    {}

    HuffmanTree(W* array,size_t size,const W& invalid)
    {
        _CreateHuffmanTree(array,size,invalid);
    }

    ~HuffmanTree()
    {
        _Destroy(_pRoot);
    }

    PNode GetRoot()
    {
        return _pRoot;
    }
private:
    PNode* _pRoot;
};
