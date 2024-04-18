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
        printf("叶子节点已满，分裂\n");
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
                printf("父节点超出，递归分裂\n");
                split(parent);
            }
        }
    }
}

template<typename Key, typename Value>
void showLeaf(Node<Key, Value>* node){
    if (node == NULL) return;
    cout<<"show Leaf"<<endl;
        for(int i = 0; i < node->indices.size(); i++){
            cout<<node->indices[i]<<" ";
            cout<<node->values[i]<<" ";
        }
    cout<<endl;
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

// 删除键，总是从叶子节点开始删除
template<typename Key, typename Value>
void BPlusTree<Key, Value>::remove(Key k){
    cout<<"remove key: "<<k<<endl;
    // 根节点为空，直接返回
    if(root == NULL) {
        printf("树空，无法删除键\n");
        return;
    }
    // 针对叶子结点，根据key找到叶子节点，删除记录
    Node<Key, Value>* cur = root;
    while(!cur->leaf){  // 找到要删除的叶子节点cur
        int i = 0;
        while(i < cur->indices.size() && k > cur->indices[i]) i++;
        cur = cur->children[i];
    }
    // 找到要删除的键值对
    int i = 0;
    while(i < cur->indices.size() && k > cur->indices[i]) i++;
    // 没找到要删除的键值对，直接返回
    if(i == cur->indices.size() || cur->indices[i] != k) {
        cout<<"没有找到要删除的键 k: "<<k<<endl;
        return;
    }

    // 找到要删除的键值对，删除
    cout<<"删除Key:"<<cur->indices[i]<<" Value:"<<cur->values[i];
    cur->indices.erase(cur->indices.begin() + i);
    cur->values.erase(cur->values.begin() + i);
    cout<<"后，当前叶子节点元素个数为"<<cur->indices.size()<<endl;


    // 如果当前节点是根节点，则如果根节点的元素个数为0，需要删除根节点
    if(cur == root){
        printf("当前节点是根节点\n");
        if(cur->indices.size() == 0){
            printf("根节点元素个数为0，删除根节点\n");
            delete cur;
            root = NULL;
        }
        // 否则直接返回
        return;
    }
    

    Node<Key, Value>* parent = cur->parent;
    Node<Key, Value>* left = cur->prev;
    Node<Key, Value>* right = cur->nxt;
        

    // 如果删除后叶子节点的元素个数小于MIN_IND，需要调整
    if(cur->indices.size() < MIN_IND){
        cout<<"叶子节点元素个数小于MIN_IND，需要调整"<<endl;

        // 从左兄弟节点借
        if (left != NULL && left->indices.size() > MIN_IND){
            cout<<"从左兄弟节点借"<<endl;
            // 从左兄弟节点借一个元素
            Key borrow_key = left->indices[left->indices.size()-1];
            Value borrow_val = left->values[left->values.size()-1];
            // 把父节点的索引值更新为借来的键值对
            int i = 0;
            while(i < parent->children.size() && parent->children[i] != cur) i++;
            parent->indices[i-1] = borrow_key;
            // 把左兄弟节点的最后一个键值对插入到当前节点
            cur->indices.insert(cur->indices.begin(), borrow_key);
            cur->values.insert(cur->values.begin(), borrow_val);
            // 把左兄弟节点的最后一个键值对删除
            left->indices.erase(left->indices.begin() + left->indices.size()-1);
            left->values.erase(left->values.begin() + left->values.size()-1);
        }

        // 从右兄弟节点借
        else if (right != NULL && right->indices.size() > MIN_IND){
            cout<<"从右兄弟节点借"<<endl;
            // 从右兄弟节点借一个元素
            Key borrow_key = right->indices[0];
            Value borrow_val = right->values[0];
            // 把父节点的索引值更新为借来的键值对
            int i = 0;
            while(i < parent->indices.size() && parent->indices[i] != borrow_key) i++;
            parent->indices[i] = borrow_key;
            // 把右兄弟节点的第一个键值对插入到当前节点
            cur->indices.push_back(borrow_key);
            cur->values.push_back(borrow_val);
            // 把右兄弟节点的第一个键值对删除
            right->indices.erase(right->indices.begin());
            right->values.erase(right->values.begin());
        }

        // 左右兄弟节点都不能借，只能合并
        else {
            cout<<"左右兄弟节点都不能借，只能合并"<<endl;
            // 合并与自己在同一个父节点下的左右兄弟节点，优先合并左兄弟节点
            if (left!=NULL && left->parent == cur->parent)
            {
                cout<<"合并左兄弟叶子节点"<<endl;
                // 当前节点的索引值插入到左兄弟节点的末尾
                left->indices.insert(left->indices.end(), cur->indices.begin(), cur->indices.end());
                left->values.insert(left->values.end(), cur->values.begin(), cur->values.end());
                // 把父节点的指向当前节点的指针和索引删除
                int i = 0;
                while(i < parent->children.size() && parent->children[i] != cur) i++;
                parent->children.erase(parent->children.begin() + i);
                parent->indices.erase(parent->indices.begin() + i - 1);
                // 调整指针
                left->nxt = cur->nxt;
                if(cur->nxt != NULL) cur->nxt->prev = left;
                delete cur;
                //cout<<"合并左兄弟后，合并节点父亲是"<<left->parent->indices[0]<<"大小为"<<left->parent->indices.size()<<endl;
            }
            // 合并右兄弟节点
            else if (right!=NULL && right->parent == cur->parent)
            {   
                cout<<"合并右兄弟叶子节点"<<endl;
                // 右兄弟节点的索引值插入到当前节点的末尾
                cur->indices.insert(cur->indices.end(), right->indices.begin(), right->indices.end());
                cur->values.insert(cur->values.end(), right->values.begin(), right->values.end());
                // 把父节点的指向右兄弟节点的指针和索引删除
                int i = 0;
                while(i < parent->children.size() && parent->children[i] != right) i++;
                parent->children.erase(parent->children.begin() + i);
                parent->indices.erase(parent->indices.begin() + i - 1);
                // 调整指针
                cur->nxt = right->nxt;
                if(right->nxt != NULL) right->nxt->prev = cur;
                delete right;
            }
        }
    }
    // 如果父节点（此时必然是索引节点）的元素个数小于MIN_IND，需要调整
    if(parent->indices.size() < MIN_IND){
        cout<<"叶子的父节点元素个数小于MIN_IND，为 "<<parent->indices.size()<<" 个，需要调整"<<endl;
        adjust(parent);
    }
}

// 调整索引节点
template<typename Key, typename Value>
void BPlusTree<Key, Value>::adjust(Node<Key, Value>* node){
    // 如果当前节点是根节点，直接返回
    if(node->parent == NULL) {
        printf("当前节点是根节点，允许元素个数小于MIN_IND\n");
        if (node->children.size() == 0)
        {
            printf("根节点孩子指针个数为0，删除根节点\n");
            root = NULL;
            delete node;
        }
        else if (node->children.size() == 1)
        {
            printf("根节点孩子指针个数为1，根节点指向唯一的子节点，更新根节点\n");
            root = node->children[0];
            root->parent = NULL;
            delete node;
        }
        return;
    }
    // 否则寻找兄弟节点，是否有多余的元素可以借
    Node<Key, Value>* parent = node->parent;
    Node<Key, Value>* left = NULL;
    Node<Key, Value>* right = NULL;
    int i = 0;
    while(i < parent->children.size() && parent->children[i] != node) i++;
    // 优先左兄弟节点
    if(i > 0) left = parent->children[i-1];
    // 其次右兄弟节点
    if(i < parent->children.size()-1) right = parent->children[i+1];

    // 从左兄弟节点借
    if (left != NULL && left->indices.size() > MIN_IND){
        printf("从左兄弟索引节点借，左兄弟第一个索引为");
        cout<<left->indices[left->indices[0]]<<endl;
        // 从左兄弟节点借一个元素
        Key borrow_key = left->indices[left->indices.size()-1];
        Node<Key, Value>* borrow_child = left->children[left->children.size()-1];
        // 把父节点的索引值更新为借来的键值对
        int j = 0;
        while(j < parent->indices.size() && parent->indices[j] != borrow_key) j++;
        Key parent_key = parent->indices[j];
        parent->indices[j] = borrow_key;
        // 右节点插入parent_key，和borrow_child指针
        node->indices.insert(node->indices.begin(), parent_key);
        node->children.insert(node->children.begin(), borrow_child);
        // 删除左兄弟节点的最后一个键值对和指针
        left->indices.erase(left->indices.begin() + left->indices.size()-1);
        left->children.erase(left->children.begin() + left->children.size()-1);
    }
    // 从右兄弟节点借
    else if (right != NULL && right->indices.size() > MIN_IND)
    {
        printf("从右兄弟索引节点借，右兄弟第一个索引为");
        cout<<right->indices[0]<<endl;
        // 从右兄弟节点借一个元素
        Key borrow_key = right->indices[0];
        Node<Key, Value>* borrow_child = right->children[0];
        // 把父节点的索引值更新为借来的键值对
        int j = 0;
        while(j < parent->indices.size() && parent->indices[j] != borrow_key) j++;
        Key parent_key = parent->indices[j];
        parent->indices[j] = borrow_key;
        // 左节点插入parent_key，和borrow_child指针
        node->indices.push_back(parent_key);
        node->children.push_back(borrow_child);
        // 删除右兄弟节点的第一个键值对和指针
        right->indices.erase(right->indices.begin());
        right->children.erase(right->children.begin());
    }
    // 左右兄弟节点都不能借，只能合并
    else {
        printf("索引的左右兄弟节点都不能借，只能合并\n");
        // 当前节点和父节点key下移形成新的索引节点
        //优先合并左兄弟节点
        if (left!=NULL && left->parent == node->parent)
        {
            cout<<"合并左兄弟索引节点，左兄弟第一个索引为"<<left->indices[0]<<endl;
            Key parent_key = parent->indices[i-1];
            printf("从父亲节点下放的key: %d\n", parent_key);
            Node<Key, Value>* new_node = new Node<Key,Value>;
            new_node->leaf = false;
            // 添加left的所有索引和指针，parent_key索引，node的所有索引和指针
            new_node->indices.insert(new_node->indices.end(), left->indices.begin(), left->indices.end());
            new_node->indices.push_back(parent_key);
            new_node->indices.insert(new_node->indices.end(), node->indices.begin(), node->indices.end());

            new_node->children.insert(new_node->children.end(), left->children.begin(), left->children.end());
            new_node->children.insert(new_node->children.end(), node->children.begin(), node->children.end());
            new_node->parent = parent;
            // 删除parent_key和指向left和node的指针
            parent->indices.erase(parent->indices.begin() + i - 1);
            parent->children.erase(parent->children.begin() + i - 1);
            parent->children.erase(parent->children.begin() + i - 1);
            // parent指向新的索引节点
            parent->children.insert(parent->children.begin() + i - 1, new_node);
            // 更新所有子节点的父节点
            for (int j=0;j<new_node->children.size();j++)
                new_node->children[j]->parent = new_node;


            // 删除left节点
            delete left;
            // 删除node节点
            delete node;
            // 递归调整父节点
            adjust(parent);
        }
        // 合并右兄弟节点
        else if (right!=NULL && right->parent == node->parent)
        {
            cout<<"合并右兄弟索引节点，右兄弟第一个索引为"<<right->indices[0]<<endl;
            Key parent_key = parent->indices[i];
            printf("从父亲节点下放的key: %d\n", parent_key);
            Node<Key, Value>* new_node = new Node<Key,Value>;
            new_node->leaf = false;
            // 添加node的所有索引和指针，parent_key索引，right的所有索引和指针
            new_node->indices.insert(new_node->indices.end(), node->indices.begin(), node->indices.end());
            new_node->indices.push_back(parent_key);
            new_node->indices.insert(new_node->indices.end(), right->indices.begin(), right->indices.end());
            new_node->children.insert(new_node->children.end(), node->children.begin(), node->children.end());
            new_node->children.insert(new_node->children.end(), right->children.begin(), right->children.end());
            new_node->parent = parent;
            // 删除parent_key和指向node和right的指针
            parent->indices.erase(parent->indices.begin() + i);
            parent->children.erase(parent->children.begin() + i);
            parent->children.erase(parent->children.begin() + i);
            // parent指向新的索引节点
            parent->children.insert(parent->children.begin() + i, new_node);
            // 更新所有子节点的父节点
            for (int j=0;j<new_node->children.size();j++)
                new_node->children[j]->parent = new_node;

            // 删除right节点
            delete right;
            // 删除node节点
            delete node;
            // 递归调整父节点
            adjust(parent);
        }
    }

}



// 获取根节点
template<typename Key, typename Value>
Node<Key, Value>* BPlusTree<Key, Value>::getRoot(){
    return root;
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
            cout<<"depth:"<<cur_depth<<"  ";
            last_depth = cur_depth;
        }
        if (cur->leaf){
            for(int i = 0; i < cur->indices.size(); i++){
                cout<<cur->indices[i]<<" ";
                //cout<<cur->values[i]<<" ";
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

// 查找键
template<typename Key, typename Value>
Value BPlusTree<Key, Value>::search(Key k){
    cout<<"search key: "<<k<<endl;
    // 根节点为空，直接返回
    if(root == NULL) {
        printf("树空，无法查找键\n");
        return Value();
    }
    // 针对叶子结点，根据key找到叶子节点，查找记录
    Node<Key, Value>* cur = root;
    while(!cur->leaf){  // 找到要查找的叶子节点cur
        int i = 0;
        while(i < cur->indices.size() && k > cur->indices[i]) i++;
        cur = cur->children[i];
    }
    // 在叶子节点中查找
    int i = 0;
    while(i < cur->indices.size() && k > cur->indices[i]) i++;
    if(i == cur->indices.size() || cur->indices[i] != k) {
        cout<<"没有找到要查找的键 k: "<<k<<endl;
        return Value();
    }
    cout<<"找到键 k: "<<k<<" 对应的值为: "<<cur->values[i]<<endl;
    return cur->values[i];
}