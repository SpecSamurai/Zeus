#include "EventTest_types.hpp"

#include <events/Event.hpp>
#include <events/EventDispatcher.hpp>

#include <gtest/gtest.h>

class EventDispatcherTestClass
{
public:
    EventDispatcherTestClass(
        Zeus::EventDispatcher<TestEvent1, TestEvent2>& dispatcher)
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
    Zeus::EventDispatcher<TestEvent1, TestEvent2>& dispatcher;
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

TEST(EventDispatcherTest, DispatchStatic_NoHandlersRegistered_False)
{
    Zeus::EventDispatcher<TestEvent1> sut(1);

    auto result = sut.Dispatch(TestEvent1{});

    EXPECT_FALSE(result);
}

TEST(EventDispatcherTest, DispatchStatic_OneEvent)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(1);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    auto result = sut.Dispatch(TestEvent1{});

    EXPECT_TRUE(result);
    EXPECT_TRUE(IsHandled);
    EXPECT_EQ(TestValue, 0);
}

TEST(EventDispatcherTest, DispatchStatic_MultipleEvents)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(2);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    auto result1 = sut.Dispatch(TestEvent1{});
    auto result2 = sut.Dispatch(TestEvent2{ .value = 10 });

    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_TRUE(IsHandled);
    EXPECT_EQ(TestValue, 10);
}

TEST(EventDispatcherTest, DispatchStatic_AfterUnregistration_False)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(2);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    sut.Unregister<TestEvent1>("testEventHandler1");

    auto result1 = sut.Dispatch(TestEvent1{});
    auto result2 = sut.Dispatch(TestEvent2{ .value = 10 });

    EXPECT_FALSE(result1);
    EXPECT_TRUE(result2);
    EXPECT_FALSE(IsHandled);
    EXPECT_EQ(TestValue, 10);
}

TEST(EventDispatcherTest, DispatchClassMember_OneEvent)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(2);
    EventDispatcherTestClass testObj(sut);

    auto result2 = sut.Dispatch(TestEvent2{ .value = 10 });

    EXPECT_TRUE(result2);
    EXPECT_FALSE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}

TEST(EventDispatcherTest, DispatchClassMember_MultipleEvents)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(2);
    EventDispatcherTestClass testObj(sut);

    auto result1 = sut.Dispatch(TestEvent1{});
    auto result2 = sut.Dispatch(TestEvent2{ .value = 10 });

    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_TRUE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}

TEST(EventDispatcherTest, DispatchClassMember_AfterUnregistration_False)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(2);
    EventDispatcherTestClass testObj(sut);

    sut.Unregister<TestEvent1>("TestClass::TestEvent1");

    auto result1 = sut.Dispatch(TestEvent1{});
    auto result2 = sut.Dispatch(TestEvent2{ .value = 10 });

    EXPECT_FALSE(result1);
    EXPECT_TRUE(result2);
    EXPECT_FALSE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}

TEST(EventDispatcherTest, DispatchStaticAndClassMembers)
{
    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(2);
    EventDispatcherTestClass testObj(sut);
    IsHandled = false;
    TestValue = 0;

    sut.Register("testEventHandler1", testEventHandler1);
    sut.Register("testEventHandler2", testEventHandler2);

    auto result1 = sut.Dispatch(TestEvent1{});
    auto result2 = sut.Dispatch(TestEvent2{ .value = 10 });

    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_TRUE(IsHandled);
    EXPECT_EQ(TestValue, 10);
    EXPECT_TRUE(testObj.isHandled);
    EXPECT_EQ(testObj.value, 10);
}
