#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstdio>
#include <queue>
#include <utility>

using namespace std;

#define M 5 // B+树的阶数
#define MIN_IND M/2	// （非根）索引节点最少元素个数
#define MAX_IND M-1	// 索引节点最多元素个数

template<typename Key, typename Value>	// 键值
struct Node{
	bool leaf;	//	索引节点标志
	vector<Key> indices;	// 存储指向子节点的索引，最多M-1个
	vector<Node*> children;	//	指向子节点，最多M个
	vector<Value> values;	// 存储值（仅在叶子节点时有效），最多M-1个
	Node* parent;	//	指向父亲
	Node* nxt;	//	叶子节点有效，指向下一个叶子
	Node* prev;	//	叶子节点有效，指向上一个叶子
	Node(){	
		leaf = false;
		parent = NULL;
		nxt = NULL;
		prev = NULL;
		indices.clear();
		children.clear();
		values.clear();
	}
};

template<typename Key, typename Value>	// 键值
class BPlusTree {
private:
	Node<Key,Value>* root;
	// 递归分裂索引节点，直到根节点
	void split(Node<Key,Value>* node);
	// 递归调整索引节点
	void adjust(Node<Key,Value>* node);
	// 层次遍历显示B+树
	void show(Node<Key,Value>* node, int level);
	// 释放内存
	void free(Node<Key,Value>* node);
public:
	Node<Key,Value>* getRoot();
	BPlusTree();
	~BPlusTree();
	// 插入键值对
	void insert(Key k, Value val);
	// 删除键
	void remove(Key k);
	// 查找键
	Value search(Key k);
	// 外部调用，显示B+树
	void show();
};

#endif