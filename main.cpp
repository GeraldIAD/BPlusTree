#include "BPlusTree.h"
#include "BPlusTree.cpp"
 
using namespace std;
void insertTest();

int main() {
    insertTest();
    return 0;
}

void insertTest(){
    BPlusTree<int, int> bpt;
    bpt.insert(1, 1);
    bpt.show();
    bpt.insert(2, 2);
    bpt.show();
    bpt.insert(3, 3);
    bpt.show();
    bpt.insert(4, 4);
    bpt.show();
    bpt.insert(5, 5);
    bpt.show();
    bpt.insert(6, 6);
    bpt.show();
    bpt.insert(7, 7);
    bpt.show();
    bpt.insert(8, 8);
    bpt.show();
    bpt.insert(9, 9);
    bpt.show();
    bpt.insert(10, 10);
    bpt.show();
    bpt.insert(11, 11);
    bpt.show();
    bpt.insert(12, 12);
    bpt.show();
    bpt.insert(13, 13);
    bpt.show();
    bpt.insert(14, 14);
    bpt.show();
    bpt.insert(15, 15);
    bpt.show();
    bpt.insert(16, 16);
    bpt.show();
    bpt.insert(17, 17);
    bpt.show();
    bpt.insert(18, 18);
    bpt.show();
    bpt.insert(19, 19);
    bpt.show();
}