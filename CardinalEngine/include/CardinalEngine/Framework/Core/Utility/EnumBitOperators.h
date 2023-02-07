#pragma once
#include "../Definitions.h"

#include <type_traits>

#define CE_ENUM_BIT(X) 1 << X

#define CE_ENUM_BIT_OPERATORS(ENUM) \
inline std::underlying_type_t<ENUM> operator &(const ENUM& First, const ENUM& Second) \
{ \
	return static_cast<std::underlying_type_t<ENUM>>(First) & static_cast<std::underlying_type_t<ENUM>>(Second); \
} \
inline ENUM operator |(const ENUM& First, const ENUM& Second) \
{ \
	return static_cast<ENUM>(static_cast<std::underlying_type_t<ENUM>>(First) | static_cast<std::underlying_type_t<ENUM>>(Second)); \
} \
inline ENUM& operator |=(ENUM& First, const ENUM& Second) \
{ \
	return (ENUM&)((std::underlying_type_t<ENUM>&)First |= (std::underlying_type_t<ENUM>)Second); \
} \
inline ENUM& operator &=(ENUM& First, const ENUM& Second) \
{ \
	return (ENUM&)((std::underlying_type_t<ENUM>&)First &= (std::underlying_type_t<ENUM>)Second); \
} \
inline ENUM operator ~(const ENUM& First) \
{ \
	return static_cast<ENUM>(~static_cast<std::underlying_type_t<ENUM>>(First)); \
} \
inline bool operator ==(const ENUM& First, const std::underlying_type_t<ENUM>& Second) \
{ \
	return static_cast<std::underlying_type_t<ENUM>>(First) == Second; \
} \
inline bool operator ==(const std::underlying_type_t<ENUM>& First, const ENUM& Second) \
{ \
	return First == static_cast<std::underlying_type_t<ENUM>>(Second); \
}
