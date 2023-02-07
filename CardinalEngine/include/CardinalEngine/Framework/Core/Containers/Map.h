#pragma once

#include <map>
#include <unordered_map>

namespace CE
{
	template<typename KeyT, typename ValueT, typename Pr = std::less<KeyT>, typename Alloc = std::allocator<std::pair<const KeyT, ValueT>>>
	using Map = std::map<KeyT, ValueT, Pr, Alloc>;

	template<typename KeyT, typename ValueT, typename Pr = std::less<KeyT>, typename Alloc = std::allocator<std::pair<const KeyT, ValueT>>>
	using MultiMap = std::multimap<KeyT, ValueT, Pr, Alloc>;

	template<typename KeyT, typename ValueT, typename Hasher = std::hash<KeyT>, typename KeyEqual = std::equal_to<KeyT>, typename Alloc = std::allocator<std::pair<const KeyT, ValueT>>>
	using UnorderedMap = std::unordered_map<KeyT, ValueT, Hasher, KeyEqual, Alloc>;

	template<typename KeyT, typename ValueT, typename Hasher = std::hash<KeyT>, typename KeyEqual = std::equal_to<KeyT>, typename Alloc = std::allocator<std::pair<const KeyT, ValueT>>>
	using UnorderedMultiMap = std::unordered_multimap<KeyT, ValueT, Hasher, KeyEqual, Alloc>;
}
