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

#define M 5 // B+���Ľ���
#define MIN_IND M/2	// ���Ǹ��������ڵ�����Ԫ�ظ���
#define MAX_IND M-1	// �����ڵ����Ԫ�ظ���

template<typename Key, typename Value>	// ��ֵ
struct Node{
	bool leaf;	//	�����ڵ��־
	vector<Key> indices;	// �洢ָ���ӽڵ�����������M-1��
	vector<Node*> children;	//	ָ���ӽڵ㣬���M��
	vector<Value> values;	// �洢ֵ������Ҷ�ӽڵ�ʱ��Ч�������M-1��
	Node* parent;	//	ָ����
	Node* nxt;	//	Ҷ�ӽڵ���Ч��ָ����һ��Ҷ��
	Node* prev;	//	Ҷ�ӽڵ���Ч��ָ����һ��Ҷ��
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

template<typename Key, typename Value>	// ��ֵ
class BPlusTree {
private:
	Node<Key,Value>* root;
	// �ݹ���������ڵ㣬ֱ�����ڵ�
	void split(Node<Key,Value>* node);
	// �ݹ���������ڵ�
	void adjust(Node<Key,Value>* node);
	// ��α�����ʾB+��
	void show(Node<Key,Value>* node, int level);
	// �ͷ��ڴ�
	void free(Node<Key,Value>* node);
public:
	Node<Key,Value>* getRoot();
	BPlusTree();
	~BPlusTree();
	// �����ֵ��
	void insert(Key k, Value val);
	// ɾ����
	void remove(Key k);
	// ���Ҽ�
	Value search(Key k);
	// �ⲿ���ã���ʾB+��
	void show();
};

#endif