#include <events/Event.hpp>
#include <events/EventDispatcher.hpp>

#include <gtest/gtest.h>

class TestEvent1
{
public:
    // TestEvent1()
    // {
    // }

    int x;
    int y;
};

class TestEvent2
{
};

bool Listener1(const TestEvent1& event)
{
    std::cout << event.x << std::endl;
    return true;
}

bool Listener2(const TestEvent1& event)
{
    std::cout << event.x << std::endl;
    return true;
}

bool Listener3(const TestEvent2& event)
{
    std::cout << "TestEvent2" << std::endl;
    return true;
}

void* operator new(std::size_t size)
{
    static int c = 0;

    std::cout << "Allocateed: " << ++c << std::endl;

    return malloc(size);
}

TEST(EventTest, EventDispatcher_ListenerRegistration)
{
    printf("Disaptcher\n");

    Zeus::EventDispatcher<TestEvent1, TestEvent2> sut(100);

    //Zeus::EventListener<TestEvent1> listener1{
    //    [&](const TestEvent1& event) -> bool {
    //        std::cout << event.x << std::endl;
    //        return true;
    //    }
    //};

    //Zeus::EventListener<TestEvent2> listener2{
    //    [](const TestEvent2& event) -> bool {
    //        std::cout << "TestEvent2\n";
    //        return true;
    //    }
    //};

    sut.Register(Listener1);
    sut.Register(Listener2);
    sut.Register(Listener3);

    TestEvent1 event1
    {
        .x = 1, .y = 2
    };
    sut.Dispatch(event1);

    sut.Unregister(Listener1);

    TestEvent2 event2{};
    sut.Dispatch(event2);

    sut.Unregister(Listener2);

    printf("Disaptcher\n");

    EXPECT_FALSE(true);
}

TEST(EventTest, Push_Overrides_ExistingEntries)
{

    printf("EventTest\n");

    //Zeus::EventQueue<TestEvent1, TestEvent2> sut;

    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);
    //sut.Publish<TestEvent1>(1, 2);

    // EXPECT_FALSE(sut.Empty());
    // EXPECT_EQ(sut.Count(), 5);
}
