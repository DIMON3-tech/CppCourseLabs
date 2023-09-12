#include <iostream>
#include "../lib/buffer.h"
#include "../lib/buffer_ext.h"
#include <vector>


int main() {
    CircularBuffer<int> buffer(4);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    CircularBuffer<int> box(4);
    box.push_back(45);
    box.push_back(46);
    box.push_back(47);
    box.push_back(48);

    buffer.swap(box);

    auto it = box.begin();
    std::cout << box.front();

    













}