#pragma once

#include <set>
#include <unordered_set>

namespace CE
{
	template<typename KeyT, typename Pr = std::less<KeyT>, typename Alloc = std::allocator<KeyT>>
	using Set = std::set<KeyT, Pr, Alloc>;

	template<typename KeyT, typename Hasher = std::hash<KeyT>, typename KeyEqual = std::equal_to<KeyT>, typename Alloc = std::allocator<KeyT>>
	using UnorderedSet = std::unordered_set<KeyT, Hasher, KeyEqual, Alloc>;
}
