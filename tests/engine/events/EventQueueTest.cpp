#include "EventTest_types.hpp"

#include <events/Event.hpp>
#include <events/EventQueue.hpp>

#include <gtest/gtest.h>

class EventQueueTestClass
{
public:
    EventQueueTestClass(Zeus::EventQueue<TestEvent1, TestEvent2>& dispatcher)
        : dispatcher{ dispatcher }
    {
        dispatcher.Register<TestEvent1>(
            "TestClass::TestEvent1",
            [this]([[maybe_unused]] const TestEvent1& event) -> bool {
                return OnEvent1();
            });

        dispatcher.Register<TestEvent2>(
            "TestClass::TestEvent2",
            [this](const TestEvent2& event) -> bool {
                return OnEvent2(event);
            });
    }

    bool OnEvent1()
    {
        return isHandled = true;
    }

    bool OnEvent2(const TestEvent2& event)
    {
        value = event.value;

        return true;
    }

    bool isHandled{ false };
    float value{ 0.f };
    Zeus::EventQueue<TestEvent1, TestEvent2>& dispatcher;
};

static bool IsHandled;
static float TestValue;

static bool testEventHandler1([[maybe_unused]] const TestEvent1& event)
{
    return IsHandled = true;
}

static bool testEventHandler2(const TestEvent2& event)
{
    TestValue = event.value;
    return true;
}

TEST(EventQueueTest, DispatchStatic_OneEvent)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 1);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    sut.Publish(TestEvent1{});
    sut.Dispatch();

    EXPECT_TRUE(IsHandled);
    EXPECT_EQ(TestValue, 0);
}

TEST(EventQueueTest, DispatchStatic_MultipleEvents)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 2);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    sut.Publish(TestEvent1{});
    sut.Publish(TestEvent2{ .value = 10 });
    sut.Dispatch();

    EXPECT_TRUE(IsHandled);
    EXPECT_EQ(TestValue, 10);
}

TEST(EventQueueTest, DispatchStatic_AfterUnregistration_False)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 2);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    sut.Unregister<TestEvent1>("testEventHandler1");

    sut.Publish(TestEvent1{});
    sut.Publish(TestEvent2{ .value = 10 });

    sut.Dispatch();

    EXPECT_FALSE(IsHandled);
    EXPECT_EQ(TestValue, 10);
}

TEST(EventQueueTest, DispatchClassMember_OneEvent)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 1);
    EventQueueTestClass testObj(sut);

    sut.Publish(TestEvent2{ .value = 10 });

    sut.Dispatch();

    EXPECT_FALSE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}

TEST(EventQueueTest, DispatchClassMember_MultipleEvents)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 2);
    EventQueueTestClass testObj(sut);

    sut.Publish(TestEvent1{});
    sut.Publish(TestEvent2{ .value = 10 });

    sut.Dispatch();

    EXPECT_TRUE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}

TEST(EventQueueTest, DispatchClassMember_AfterUnregistration_False)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 2);
    EventQueueTestClass testObj(sut);

    sut.Unregister<TestEvent1>("TestClass::TestEvent1");

    sut.Publish(TestEvent1{});
    sut.Publish(TestEvent2{ .value = 10 });

    sut.Dispatch();

    EXPECT_FALSE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}

TEST(EventQueueTest, DispatchStaticAndClassMembers)
{
    Zeus::EventQueue<TestEvent1, TestEvent2> sut(2, 2);
    EventQueueTestClass testObj(sut);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    sut.Publish(TestEvent1{});
    sut.Publish(TestEvent2{ .value = 10 });

    sut.Dispatch();

    EXPECT_TRUE(IsHandled);
    EXPECT_EQ(TestValue, 10);
    EXPECT_TRUE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}
