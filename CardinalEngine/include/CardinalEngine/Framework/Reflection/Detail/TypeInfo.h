#pragma once
#include "../../Core/Headers.h"
#include "../ReflectionID.h"
#include "../Type.h"
#include "../Method.h"
#include "../Property.h"

namespace CE
{
	namespace Detail_Reflection
	{
		// Contains the Reflected information of a Type/Class/Enum
		struct TypeInfo
		{
			TypeInfo(const TypeInfo&) = delete;
			TypeInfo(TypeInfo&&) = delete;

			const ReflectionID ID;
			const ETypeCategory Category;
			const String TypeName;
			const size_t Size;

			Type Underlying;

			Array<Method> Constructors;

			Array<Type> Bases;

			MultiMap<ReflectionID, Method> Methods;
			Map<ReflectionID, Property> Properties;

			Map<String, ReflectionEnumEntry> EnumEntries;

			Map<String, String> Metas;
		};
	}
}
