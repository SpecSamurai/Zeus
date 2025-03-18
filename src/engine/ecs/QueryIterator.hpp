#pragma once

namespace Zeus::ECS
{
template <typename Container>
class QueryIterator
{
public:
    using value_type = typename Container::value_type;
    using pointer = typename Container::const_pointer;
    using const_reference = typename Container::const_reference;
    using difference_type = typename Container::difference_type;
    using size_type = typename Container::size_type;
};
}
