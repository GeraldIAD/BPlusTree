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
	}
};

template<typename Key, typename Value>	// 键值
class BPlusTree {
private:
	// 插入非满节点
	//void insertNotFull(Node<Key,Value>* node, Key k, Value val);
	// 分裂节点
	void split(Node<Key,Value>* node);
	Node<Key,Value>* root;
	void show(Node<Key,Value>* node, int level);
	void free(Node<Key,Value>* node);
public:
	BPlusTree();
	~BPlusTree();
	void insert(Key k, Value val);
	void show();
};

#endif