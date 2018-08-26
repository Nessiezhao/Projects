#pragma once
#include<iostream>
#include<string>
#include<assert.h>
#include"./huffman.hpp"
using namespace std;

struct CharInfo//每一个字符的信息
{
    CharInfo(size_t count = 0)
        :_count(count)
    {}

    bool operator!=(const CharInfo& info)
    {
        return _count != info._count;
    }
    bool operator==(const CharInfo& info)
    {
        return _count == info._count;
    }
    CharInfo operator+(const CharInfo& info)
    {
        CharInfo temp(*this);
        temp._count += info._count;
        return temp;
    }
    bool operator<(const CharInfo& info)
    {
        return _count < info._count;
    }
    char _ch;
    long long _count;//当前字符出现的次数
    string _strCode; //当前字符对应的编码
};


class FileCompress
{
public:
    FileCompress()
    {
        for(size_t i = 0; i < 256; ++i)
        {
            _charInfo[i]._ch = i;
            _charInfo[i]._count = 0;
        }
    }

    void GetHuffManCode(HuffmanTreeNode<CharInfo>* pRoot)
    {
        if(pRoot)
        {
            GetHuffManCode(pRoot->_pLeft);
            GetHuffManCode(pRoot->_pRight);

            if(pRoot->_pLeft == NULL && pRoot->_pRight == NULL)//说明此时的root在叶子节点的位置
            {
                HuffmanTreeNode<CharInfo>* pCur = pRoot;
                HuffmanTreeNode<CharInfo>* pParent = pCur->_pParent;

                string& strCode = _charInfo[pCur->_weight._ch]._strCode;
                while(pParent)
                {
                    if(pCur == pParent->_pLeft)
                        strCode += '0';
                    else
                        strCode += '1';
                    pCur = pParent;
                    pParent = pCur->_pParent;
                }
                reverse(strCode.begin(),strCode.end());
            }
        }
    }

    void CompressFile(const string& filePath)
    {
        //1.读取源文件，统计每个字符出现的次数
        FILE* fIn = fopen(filePath.c_str,"r");
        if(fIn == NULL)
        {
            cout<< "找不到该文件"<<endl;
            return;
        }

        char* pReadBuff = new char[1024];
        while(true)
        {
            size_t readSize = fread(pReadBuff,1,1024,fIn);
            if(readSize == 0)
                break;

            for(size_t i = 0; i < readSize; ++i)
            {
                _charInfo[pReadBuff[i]]._count++;//或取到每个字符出现的次数
            }

        }

        //2.以每个字符出现的次数为权值创建huffman树
        HuffmanTree<CharInfo> ht(_charInfo,256,CharInfo(0));

        //3.根据huffman树来或取huffman编码
        GetHuffManCode(ht.GetRoot());

        //写压缩文件的头部信息
        //文件的后缀
        string filePostFix = GetFilePostFix(filePath);
        //编码的信息
        string strCodeInfo;
        char strCount[32] = {0}//用来存放每个字符出现的次数
        size_t LineCount = 0;//有效字符的行数
        for(size_t i = 0; i < 256; ++i)
        {
            if(_charInfo[i]._count != 0)
            {
                strCodeInfo += _charInfo[i]._ch;
                strCodeInfo += ',';
                itoa(_charInfo[i]._count,strCount,10);
                strCodeInfo += strCount;
                strCodeInfo += '\n';
                LineCount++;
            }
        }
        string strHeadInfo;
        strHeadInfo += filePostFix;
        strHeadInfo += '\n';
        itoa(LineCount,strCount,10);
        strHeadInfo += strCount;
        strHeadInfo += '\n';
        strHeadInfo += strCodeInfo;

        //4.用每个字符的编码重新改写源文件
        FILE* fOut = fopen("2.hzp","w");
        assert(fOut);
        //写压缩编码的信息
        fwrite(strHeadInfo.c_str(),1,strHeadInfo.length(),fOut);
        char* pWriteBuff = new char[1024];
        char c = 0;
        char pos = 0;
        size_t writeSize = 0;
        fseek(fIn,0,SEEK_SET);
        while(true)
        {
            size_t readSize = fread(pReadBuff,1,1024,fIn);
            if(readSize == 0)
                break;

            for(size_t i = 0; i < readSize; ++i)
            {
                string& strCode = _charInfo[pReadBuff[i]]._strCode;
                for(size_t j = 0; j < strCode.size(); ++j)
                {
                    c <<= 1;
                    pos++;
                    if(strCode[j] == '1')
                        c |= 1;
                    if(pos == 8)
                    {
                        pWriteBuff[writeSize++] = c;
                        if(writeSize == 1024)
                        {
                            fwrite(pWriteBuff,1,1024,fOut);
                            writeSize = 0;
                        }
                        pos = 0;
                    }
                }
            }
        }
        if(pos < 8)
        {
            pWriteBuff[writeSize++] = (c<<(8-pos));
        }
        fwrite(pWriteBuff,1,1024,fOut);
        fclose(fIn);
        fclose(fOut);

        delete[] pReadBuff;
        delete[] pWriteBuff;
    }

    string GetFilePostFix(string FilePath)
    {
        size_t pos = FilePath.find_last_of('.');
        return FilePath.substr(pos);
    }

    string GetFilePath(string FilePath)
    {
        size_t pos = FilePath.find_last_of('.');
        return FilePath.substr(0,pos);
    }

    //解压缩
    void UnCompressFile(const string filePath)
    {
        FILE* fIn = fopen(filePath.c_str(),"r");
        assert(fIn);

        string strFilePostFix;
        ReadLine(fIn,strFilePostFix);
        string strLineCount;
        ReadLine(fIn,strLineCount);
        size_t lineCount = atoi(strLineCount.c_str());
        string strCodeInfo;
        for(size_t i = 0; i < lineCount; ++i)
        {
            strCodeInfo = "";
            ReadLine(fIn,strCodeInfo);
            _charInfo[strCodeInfo[0]]._count = atoi(strCodeInfo.c_str() + 2);
        }

        HuffmanTree<CharInfo> ht(_charInfo,256,CharInfo(0));
       HuffmanTree<CharInfo>* pCur = ht.GetRoot();
       string compressFilePath = GetFilePath(filePath);
       compressFilePath += strFilePostFix;

       FILE* fOut = fopen(compressFilePath.c_str(),"w");
       assert(fOut);

        //解压缩
       //读取压缩信息
       char* pReadBuff = new char[1024];
       char* pWriteBuff = new char[1024];
       szie_t writeSize = 0;
       size_t pos = 8;
       size_t fileSize = pCur->_weight._count;

       while(true)
       {
           size_t readSize = fread(pReadBuff,1,1024,fIn);
           if(readSize == 0)
               break;
           for(size_t i = 0; i < readSize; ++i)
           {
               pos = 8;
               while(pos--)
               {
                    if(pReadBuff[i] & (1<<pos))
                        pCur = pCur->_pRight;
                    else
                        pCur = pCur->_pLeft;
                    if(pCur->_pLeft == NULL && pCur->_pRight == NULL)
                    {
                        pWriteBuff[writeSize++] = pCur->_weight._ch;
                        if(writeSize == 1024)
                        {
                            fwrite(pWriteBuff,1,1024,fOut);
                            writeSize = 0;
                        }
                        if(--fileSize == 0)
                        {
                            fwrite(pWriteBuff,1,writeSize,fOut);
                            break;
                        }

                        pCur = ht.GetRoot();
                    }
               }
           }
       }
       
       delete[] pReadBuff;
       delete[] pWriteBuff;

       fclose(fIn);
       fclose(fOut);
    }


    void ReadLine(FILE* fIn,string& strInfo)
    {
        char c = fgetc(fIn);
        if(c == EOF)
            return;
        while(c != '\n')
        {
            strInfo += c;
            c = fgetc(fIn);
            if(c == EOF)
                return;
        }
    }
private:
    CharInfo _charInfo[256]; 
};
