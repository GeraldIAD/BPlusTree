#include "BPlusTree.h"
#include "BPlusTree.cpp"
#include <string>
#include <vector>

using namespace std;
template<typename Key, typename Value>
void insertTest(BPlusTree<Key, Value> &bpt,vector<Key> keys, vector<Value> values);
template<typename Key, typename Value>
void removeTest(BPlusTree<Key, Value> &bpt,vector<Key> keys);
template<typename Key, typename Value>
void connectTest(BPlusTree<Key, Value> &bpt);


int main() {
    BPlusTree<int, string> bpt;
    vector<int> keys1 = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
    vector<string> values1 = {"1", "2", "3", "4", "5", "6", "7", "8", "9","10","11","12","13","14","15","16","17","18","19"};
    vector<int> keys2 = {19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
    vector<string> values2 = {"19","18","17","16","15","14","13","12","11","10","9","8","7","6","5","4","3","2","1"};
    
    cout<<"测试从左向右插入和删除，以及查找键"<<endl;
    insertTest(bpt,keys1,values1);
    bpt.show();
    bpt.search(1);
    bpt.search(-1);
    removeTest(bpt,keys1);
    
    // cout<<"测试从右向左插入和删除"<<endl;
    // insertTest(bpt,keys1,values1);
    // removeTest(bpt,keys1);

    // cout<<"测试从左向右插入，从右向左删除"<<endl;
    // insertTest(bpt,keys1,values1);
    // removeTest(bpt,keys2);

    // cout<<"测试从右向左插入，从左向右删除"<<endl;
    // insertTest(bpt,keys2,values2);
    // removeTest(bpt,keys1);
    return 0;
}

template<typename Key, typename Value>
void insertTest(BPlusTree<Key, Value> &bpt,vector<Key> keys, vector<Value> values){
    for (int i = 0; i < keys.size(); i++){
        bpt.insert(keys[i], values[i]);
        bpt.show();
    }
    printf("插入测试完毕\n\n\n\n");
}

template<typename Key, typename Value>
void removeTest(BPlusTree<Key, Value> &bpt,vector<Key> keys){
    for (int i = 0; i < keys.size(); i++){
        bpt.remove(keys[i]);
        bpt.show();
    }
    printf("删除测试完毕\n\n\n\n");
}

template<typename Key, typename Value>
void connectTest(BPlusTree<Key, Value> &bpt){
    cout << "Connect Test: " << endl;
    Node<Key, Value>* node = bpt.getRoot();
    while (!node->leaf){
        node = node->children[0];
    }
    while (node != NULL){
        cout << "Node: ";
        for (int i = 0; i < node->values.size(); i++){
            cout << node->indices[i] << ":" << node->values[i] << " ";
        }
        cout << endl;
        node = node->nxt;
    }
}