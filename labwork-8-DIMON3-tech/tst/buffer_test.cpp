#include "D:\ITMO Programming\lab8\labwork-8-DIMON3-tech\lib\buffer.h"
#include "D:\ITMO Programming\lab8\labwork-8-DIMON3-tech\lib\buffer_ext.h"
#include "gtest/gtest.h"

// iterator tests

TEST(IteratorTest, PointerDereference) {
    CircularBuffer<int> buffer(2);
    buffer.push_back(1);
    buffer.push_back(2);
    CircularBuffer<int>::Iterator it = buffer.begin();
    ASSERT_EQ(*it, 1);
    ASSERT_EQ(*(it + 1), 2);
    ++it;
    ASSERT_EQ(*it, 2);
    it += 1;

}

TEST(IteratorTest, ArithmeticOperations) {
    CircularBuffer<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    buffer.push_back(5);
    CircularBuffer<int>::Iterator it = buffer.begin();
    it++;
    ASSERT_EQ(*it, 2);
    it += 3;
    ASSERT_EQ(*it, 5);
    it -= 3;
    ASSERT_EQ(*it, 2);
    it = buffer.begin();
    CircularBuffer<int>::Iterator it2 = buffer.begin() + 4;
    it += it2;
    ASSERT_EQ(*it, 5);
    CircularBuffer<int>::Iterator it3;
    it = buffer.begin() + 1;
    it2 = buffer.begin() + 2;
    it3 = it + it2;
    ASSERT_EQ(*it3, 4);
    it = buffer.begin() + 1;
    it2 = buffer.begin();
    it3 = it - it2;
    ASSERT_EQ(*it3, 2);

}

TEST(IteratorTest, Overflow) {
    CircularBuffer<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    CircularBuffer<int>::Iterator it = buffer.begin() + 2;
    ++it;
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);

}

TEST(CircularBufferExtTest, Iterator) {
    CircularBufferExt<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    auto it = buffer.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    --it;
    ASSERT_EQ(*it, 1);
}


// const iterator tests

TEST(CircularBufferExtTest, ConstIterator) {
    CircularBufferExt<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    auto it = buffer.cbegin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    --it;
    ASSERT_EQ(*it, 1);
}

TEST(ConstIteratorExtTest, DefaultConstructor) {
    CircularBufferExt<int>::ConstIteratorExt it(nullptr, 0, 0);
    ASSERT_EQ(it.operator->(), nullptr);
}

TEST(CircularBufferExtConstIterator, DefaultConstructor) {
    CircularBufferExt<int> buffer(5);
    CircularBufferExt<int>::ConstIteratorExt it = buffer.cbegin();
    ASSERT_TRUE(it == buffer.cbegin());
}

TEST(CircularBufferExtConstIterator, CopyConstructor) {
    CircularBufferExt<int> buffer(5);
    CircularBufferExt<int>::ConstIteratorExt it1 = buffer.cbegin();
    CircularBufferExt<int>::ConstIteratorExt it2(it1);
    ASSERT_TRUE(it1 == it2);
}

TEST(CircularBufferExtConstIterator, ArithmeticOperators) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    CircularBufferExt<int>::ConstIteratorExt it = buffer.cbegin();

    it = it + 2;
    ASSERT_EQ(*it, 3);
    it = it - 1;
    ASSERT_EQ(*it, 2);
}

TEST(CircularBufferExtConstIterator, IndexOperator) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    CircularBufferExt<int>::ConstIteratorExt it = buffer.cbegin();
    ASSERT_EQ(it[1], 2);
}

// no-extend buffer tests

TEST(CircularBufferTest, DefaultConstructor) {
    CircularBuffer<int> buffer;
    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.capacity(), 0);
    ASSERT_EQ(buffer.size(), 0);
}

TEST(CircularBufferTest, ConstructorWithCapacity) {
    CircularBuffer<int> buffer(5);
    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.capacity(), 5);
    ASSERT_EQ(buffer.size(), 0);
}

TEST(CircularBufferTest, PushBackAndFrontBackAccess) {
    CircularBuffer<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.capacity(), 3);
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.front(), 1);
    ASSERT_EQ(buffer.back(), 3);
}

TEST(CircularBufferTest, Overflow) {
    CircularBuffer<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    ASSERT_FALSE(buffer.empty());
    ASSERT_EQ(buffer.capacity(), 3);
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.front(), 2);
    ASSERT_EQ(buffer.back(), 4);
}

TEST(CircularBufferTest, Swap) {
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

    ASSERT_EQ(box[0], 1);
    ASSERT_EQ(*box.end(), 4);

    ASSERT_EQ(buffer[0], 45);
    ASSERT_EQ(*buffer.end(), 48);

    ASSERT_EQ(buffer.front(), 45);

    auto it = box.begin();
    ASSERT_EQ(*it, 1);

}

TEST(CircularBufferTest, OverflowIterator) {
    CircularBuffer<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.push_back(4);
    CircularBuffer<int>::Iterator it = buffer.begin();
    ASSERT_EQ(*it, 2); // _head is next element
}

TEST(CircularBufferTest, IteratorAndInsertion) {
    CircularBuffer<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    auto it = buffer.begin();
    ASSERT_EQ(*it, 1);
    ++it;
    ASSERT_EQ(*it, 2);
    buffer.insert(it, 1, 4);
    ASSERT_EQ(buffer.capacity(), 3);
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.front(), 3);
    ASSERT_EQ(buffer.back(), 2);
}

TEST(CircularBufferTest, EraseAndClear) {
    CircularBuffer<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    auto it = buffer.begin();
    ++it;
    buffer.erase(it);
    ASSERT_EQ(buffer.capacity(), 3);
    ASSERT_EQ(buffer.size(), 2);
    ASSERT_EQ(buffer.front(), 1);
    ASSERT_EQ(buffer.back(), 3);
    buffer.clear();
    ASSERT_TRUE(buffer.empty());
    ASSERT_EQ(buffer.capacity(), 3);
    ASSERT_EQ(buffer.size(), 0);
}

TEST(CircularBufferTest, Accumulate) {
    CircularBuffer<int> int_buffer(3);
    int_buffer.push_back(1);
    int_buffer.push_back(2);
    int_buffer.push_back(3);
    int sum = 0;
    auto int_begin = int_buffer.begin();
    auto int_end = int_buffer.end();
    ASSERT_EQ(int_buffer.accumulate(int_begin, int_end, sum), 6);
}

TEST(CircularBufferTest, InitializerListTest) {
    CircularBuffer<int> buffer = {1, 2, 3};
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.capacity(), 3);
    ASSERT_EQ(*buffer.begin(), 1);
    ASSERT_EQ(*buffer.end(), 3);
}

TEST(CircularBufferTest, Assign) {
    CircularBufferExt<int> extend_buffer(3);
    extend_buffer.push_back(1);
    extend_buffer.push_back(2);
    extend_buffer.push_back(3);

    CircularBufferExt<int> extend_box = {41, 42, 43, 44, 45};
    extend_buffer.assign(extend_box.begin(), extend_box.end());

    ASSERT_EQ(extend_buffer.size(), 5);
    ASSERT_EQ(extend_buffer[0], 41);
    ASSERT_EQ(*extend_buffer.end(), 45);

}
// extend buffer tests

TEST(CircularBufferExtTest, DefaultConstructor) {
    CircularBufferExt<int> buffer;
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), 0);
    ASSERT_TRUE(buffer.empty());
}

TEST(CircularBufferExtTest, ConstructorWithCapacity) {
    CircularBufferExt<int> buffer(5);
    ASSERT_EQ(buffer.size(), 0);
    ASSERT_EQ(buffer.capacity(), 5);
    ASSERT_TRUE(buffer.empty());
}

TEST(CircularBufferExtTest, PushBack) {
    CircularBufferExt<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.front(), 1);
    ASSERT_EQ(buffer.back(), 3);
}

TEST(CircularBufferExtTest, Resize) {
    CircularBufferExt<int> buffer(2);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    ASSERT_EQ(buffer.size(), 3);
    ASSERT_EQ(buffer.capacity(), 4);
}

TEST(CircularBufferExtTest, Emplace) {
    CircularBufferExt<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    auto it = buffer.emplace(buffer.begin() + 1, 3);
    ASSERT_EQ(*it, 3);
    ASSERT_EQ(buffer.size(), 3);
}

TEST(CircularBufferExtTest, Insert) {
    CircularBufferExt<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(3);
    auto it = buffer.insert(buffer.begin() + 1, 1, 2);
    ASSERT_EQ(*it, 2);
    ASSERT_EQ(buffer.size(), 3);
}

TEST(CircularBufferExtTest, Erase) {
    CircularBufferExt<int> buffer(3);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    auto it = buffer.erase(buffer.begin() + 1);
    ASSERT_EQ(*it, 3);
    ASSERT_EQ(buffer.size(), 2);
}

TEST(CircularBufferExtTest, AccessOperatorsTest) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    EXPECT_EQ(buffer[1], 2);
    EXPECT_EQ(buffer.at(1), 2);
}

TEST(CircularBufferExtTest, ClearTest) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.clear();
    EXPECT_EQ(0, buffer.size());
    EXPECT_TRUE(buffer.empty());
}

TEST(CircularBufferExtTest, ResizeTest) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);
    buffer.resize(10);
    EXPECT_EQ(10, buffer.capacity());
}

TEST(CircularBufferExtTest, IteratorOperatorsTest) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);

    auto iter1 = buffer.begin();
    auto iter2 = buffer.begin() + 1;
    EXPECT_TRUE(iter1 < iter2);
    EXPECT_TRUE(iter1 <= iter2);
    EXPECT_TRUE(iter2 > iter1);
    EXPECT_TRUE(iter2 >= iter1);
    EXPECT_TRUE(iter1 != iter2);
    EXPECT_TRUE(iter1 + 1 == iter2);
}

TEST(CircularBufferExt, ComparingTwoBuffers) {
    CircularBufferExt<int> buffer(5);
    buffer.push_back(1);
    buffer.push_back(2);
    buffer.push_back(3);

    CircularBufferExt<int> box(5);
    box.push_back(1);
    box.push_back(2);
    box.push_back(3);

    ASSERT_TRUE(box == buffer);
}





