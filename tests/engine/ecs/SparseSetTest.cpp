#include <ecs/SparseSet.hpp>

#include <gtest/gtest.h>

TEST(SparseSetTest, Type_IdPerType)
{


    EXPECT_EQ(idFloat1, 1);
}

void SparseSetTest_EmptySet() {
  SparseSet sut;

  auto isEmpty = sut.Empty();
  auto size = sut.Size();
  auto capacity = sut.Capacity();

  assert(isEmpty);
  assert(size == 0);
  assert(capacity == 0);
}

void SparseSetTest_Reserved_EmptySet() {
  SparseSet sut;
  sut.Reserve(10);

  auto isEmpty = sut.Empty();
  auto size = sut.Size();
  auto capacity = sut.Capacity();

  assert(isEmpty);
  assert(size == 0);
  assert(capacity == 10);
}

void SparseSetTest_Push_EmptySet() {
  SparseSet sut;
  Entity entity{1};

  sut.Push(1);

  auto isEmpty = sut.Empty();
  auto size = sut.Size();
  auto capacity = sut.Capacity();

  assert(!isEmpty);
  assert(size == 1);
  assert(capacity == 1);
}

void SparseSetTest_Push_Reserved() {
  SparseSet sut;
  sut.Reserve(10);

  sut.Push(1);

  auto isEmpty = sut.Empty();
  auto size = sut.Size();
  auto capacity = sut.Capacity();

  assert(!isEmpty);
  assert(size == 1);
  assert(capacity == 10);
}

void SparseSetTest_Contains_EmptySet() {
  SparseSet sut;

  auto actual1 = sut.Contains(0);
  auto actual2 = sut.Contains(1);
  auto actual3 = sut.Contains(2);

  assert(!actual1);
  assert(!actual2);
  assert(!actual3);
}

void SparseSetTest_Contains_Multiple() {
  SparseSet sut;

  sut.Push(0);
  sut.Push(1);
  sut.Push(5);

  auto isEmpty = sut.Empty();
  auto size = sut.Size();
  auto capacity = sut.Capacity();
  auto data = sut.Data();

  assert(!isEmpty);
  assert(size == 3);
  assert(capacity == 4);

  assert(sut.Contains(0));
  assert(sut.Contains(1));
  assert(sut.Contains(5));
  assert(!sut.Contains(2));

  assert(data[0] == 0);
  assert(data[1] == 1);
  assert(data[2] == 5);
}

void SparseSetTest_Index() {
  SparseSet sut;

  sut.Push(0);
  sut.Push(1);
  sut.Push(5);

  assert(sut.Index(0) == 0);
  assert(sut.Index(1) == 1);
  assert(sut.Index(5) == 2);
}

void SparsSetTest_Clear() {
  SparseSet sut;
  sut.Push(0);
  sut.Push(1);
  sut.Push(5);

  sut.Clear();

  auto isEmpty = sut.Empty();
  auto size = sut.Size();

  assert(isEmpty);
  assert(size == 0);
  assert(!sut.Contains(0));
  assert(!sut.Contains(1));
  assert(!sut.Contains(2));
  assert(!sut.Contains(5));
}

void SparsSetTest_Pop() {
  SparseSet sut;
  sut.Push(0);
  sut.Push(1);
  sut.Push(5);

  sut.Pop(1);

  assert(!sut.Contains(1));
  assert(sut.Size() == 2);
  assert(!sut.Empty());

  sut.Pop(0);

  assert(!sut.Contains(0));
  assert(sut.Size() == 1);
  assert(!sut.Empty());

  sut.Pop(5);

  assert(!sut.Contains(5));
  assert(sut.Size() == 0);
  assert(sut.Empty());
}

void ComponentSparseSet_Push() {
  ComponentSparseSet<TestComp> sut;
  Entity entity{0};

  sut.Push(entity);

  auto empty = sut.Empty();
  auto capacity = sut.Capacity();
  auto size = sut.Size();
  auto contains = sut.Contains(entity);
  auto &actual = sut.Get(entity);

  assert(!empty);
  assert(capacity == 1);
  assert(size == 1);
  assert(contains);

  assert(actual.x == 0);
  assert(actual.y == 0);
}

