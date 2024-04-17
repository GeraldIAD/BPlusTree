#include "BPlusTree.h"

using namespace std;

// 插入键值对
template<typename Key, typename Value>
void BPlusTree<Key, Value>::insert(Key k, Value val){
    cout<<"insert key: "<<k<<" value: "<<val<<endl;
    // 根节点为空，创建一个叶节点，插入记录，结束
    if(root == NULL){
        root = new Node<Key, Value>;
        root->leaf = true;
        root->indices.push_back(k);
        root->values.push_back(val);
        return;
    }
    // 针对叶子结点，根据key找到叶子节点，向其插入记录
    Node<Key, Value>* cur = root;
    while(!cur->leaf){  // 找到要插入的叶子节点cur
        int i = 0;
        while(i < cur->indices.size() && k > cur->indices[i]) i++;
        cur = cur->children[i];
    }
    if(cur->values.size() < MAX_IND){ // 叶子节点未满，直接插入
        int i = 0;
        while(i < cur->indices.size() && k > cur->indices[i]) i++;  // 找到插入位置，插入键值对
        cur->indices.insert(cur->indices.begin() + i, k);
        cur->values.insert(cur->values.begin() + i, val);
    }
    else {  // 当前叶子节点已满，分裂为cur和new_node两个节点
        Node<Key, Value>* new_node = new Node<Key, Value>;
        new_node->leaf = true;
        // 调整指针
        new_node->nxt = cur->nxt;
        cur->nxt = new_node;
        new_node->prev = cur;
        if(new_node->nxt != NULL) new_node->nxt->prev = new_node;
        
        // 先把要插入的键值对插入到原节点中，下面再分裂
        int i = 0;
        while(i < cur->indices.size() && k > cur->indices[i]) i++;
        cur->indices.insert(cur->indices.begin() + i, k);
        cur->values.insert(cur->values.begin() + i, val);

        // 把原节点的后一半数据拷贝到新节点
        // 此处使用 <= 符号，右结点存储的数据比左结点多一个
        for(int i = MIN_IND; i <= MAX_IND; i++){
            new_node->indices.push_back(cur->indices[i]);
            new_node->values.push_back(cur->values[i]);
        }
        cur->indices.erase(cur->indices.begin() + MIN_IND, cur->indices.end());
        cur->values.erase(cur->values.begin() + MIN_IND, cur->values.end());

        // 调整父节点，有两种情况，一种是原节点是根节点，一种是原节点不是根节点
        if(cur->parent == NULL){  // 原节点是根节点，创建新根节点，不需要递归调整
            Node<Key, Value>* new_root = new Node<Key, Value>;
            new_root->leaf = false;
            // 注意索引值！！！由于只存M-1个索引值，所以新根节点的第一个索引值是右结点的第一个键
            new_root->indices.push_back(new_node->indices[0]);
            // children[0]是左结点，children[1]是右结点
            new_root->children.push_back(cur);
            new_root->children.push_back(new_node);
            cur->parent = new_root;
            new_node->parent = new_root;
            root = new_root;
        }
        else {  // 原节点不是根节点，递归调整父节点
            // 找到原节点的父节点，把新节点的第一个键插入到父节点中
            Node<Key, Value>* parent = cur->parent;
            int i = 0;
            while(i < parent->children.size() && parent->children[i] != cur) i++;
            // 把新节点的第一个键插入到父节点中
            parent->indices.insert(parent->indices.begin() + i, new_node->indices[0]);
            parent->children.insert(parent->children.begin() + i + 1, new_node);
            new_node->parent = parent;
            if(parent->indices.size() > MAX_IND){  // 父节点超出，递归分裂
                split(parent);
            }
        }   
    }
}

// 分裂一个已经满了的索引节点
template<typename Key, typename Value>
void BPlusTree<Key, Value>::split(Node<Key, Value>* node){
    // 分裂后的左右两个节点
    Node<Key, Value>* new_node = new Node<Key, Value>;
    new_node->leaf = false;
    // 这里是精细操作，需要注意indices和children的分配
    // 对于children指针
    // 以m=5阶为例子，0~2分配给左边，3~5分配给右边
    // 以m=6阶为例子，0~2分配给左边，3~6分配给右边
    // 对于indices索引
    // 以m=5阶为例子，0~1分配给左边，2分配给父节点，3~4分配给右边
    // 以m=6阶为例子，0~1分配给左边，2分配给父节点，3~5分配给右边

    // 对于children指针
    for (int i= (M+1)/2; i <= M; i++){
        new_node->children.push_back(node->children[i]);
        node->children[i]->parent = new_node;
    }
    node->children.erase(node->children.begin() + (M+1)/2, node->children.end());
    // 对于indices索引
    for (int i = (M+1)/2; i < M; i++){
        new_node->indices.push_back(node->indices[i]);
    }
    // 暂存父节点的索引值
    Key k = node->indices[(M+1)/2 - 1];
    node->indices.erase(node->indices.begin() + (M+1)/2 - 1, node->indices.end());
    // 调整父节点，有两种情况，一种是原节点是根节点，一种是原节点不是根节点
    if (node->parent == NULL){  // 原节点是根节点，创建新根节点，不需要递归调整
        Node<Key, Value>* new_root = new Node<Key, Value>;
        new_root->leaf = false;
        new_root->indices.push_back(k);
        new_root->children.push_back(node);
        new_root->children.push_back(new_node);
        node->parent = new_root;
        new_node->parent = new_root;
        root = new_root;
    }
    else {  // 原节点不是根节点，递归调整父节点
        // 把k插入到父节点中，同时更改父节点的children指针
        Node<Key, Value>* parent = node->parent;
        int i = 0;
        while(i < parent->children.size() && parent->children[i] != node) i++;
        parent->indices.insert(parent->indices.begin() + i, k);
        parent->children.insert(parent->children.begin() + i + 1, new_node);
        new_node->parent = parent;
        if(parent->indices.size() > MAX_IND){  // 父节点超出，递归分裂
            split(parent);
        }
    }
}


// 构造函数
template<typename Key, typename Value>
BPlusTree<Key, Value>::BPlusTree(){
    printf("创建B+树\n");
    root = NULL;
}

// 析构函数
template<typename Key, typename Value>
BPlusTree<Key, Value>::~BPlusTree(){
    printf("递归销毁B+树，清理内存\n");
    // 递归释放内存
    if(root != NULL){
        free(root);
    }
}

// 释放内存
template<typename Key, typename Value>
void BPlusTree<Key, Value>::free(Node<Key, Value>* node){
    if(node == NULL) return;
    if(node->leaf){
        delete node;
    }
    else {
        for(int i = 0; i < node->children.size(); i++){
            free(node->children[i]);
        }
        delete node;
    }
}

// 展示B+树
template<typename Key, typename Value>
void BPlusTree<Key, Value>::show(){
    cout<<"----------show B+ Tree----------"<<endl;
    show(root,0);
    cout<<endl<<endl;
}

// 按照层次展示B+树
template<typename Key, typename Value>
void BPlusTree<Key, Value>::show(Node<Key, Value>* node, int depth){
    if(node == NULL) return;
    // 用队列，便于层次访问
    queue< pair<Node<Key, Value>*, int> > q;
    q.push(make_pair(node, depth));
    int last_depth = depth-1;
    while (!q.empty()){
        pair<Node<Key, Value>*, int> p = q.front();
        q.pop();
        // 当前节点，当前深度
        Node<Key, Value>* cur = p.first;
        int cur_depth = p.second;
        if (cur_depth != last_depth){
            cout<<endl;
            cout<<"depth:"<<cur_depth<<"   ";
            last_depth = cur_depth;
        }
        if (cur->leaf){
            for(int i = 0; i < cur->indices.size(); i++){
                cout<<cur->indices[i]<<" ";
            }
            cout<<"  ";
        }
        else {
            for(int i = 0; i < cur->indices.size(); i++){
                cout<<cur->indices[i]<<" ";
            }
            cout<<"  ";
            for(int i = 0; i < cur->children.size(); i++){
                q.push(make_pair(cur->children[i], cur_depth + 1));
            }
        }
    }
    cout<<endl<<endl;
    cout<<"--------------------------------"<<endl;
}