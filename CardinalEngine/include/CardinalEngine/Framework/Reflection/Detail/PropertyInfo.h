#pragma once
#include "../../Core/Headers.h"
#include "../ReflectionID.h"
#include "../Type.h"

namespace CE
{
	class Handle;
	class Argument;

	namespace Detail_Reflection
	{
		// Base class for Property Wrappers
		class CE_API PropertyWrapperBase
		{
		public:
			// Returns the ReflectionID of the Type
			virtual ReflectionID TypeReflectionID() const = 0;

			// Returns true if the Method is static
			virtual bool IsStatic() const = 0;

			// Returns the number of arguments of the Method
			virtual bool IsReadOnly() const = 0;

			// Get a Handle to the Value
			virtual Handle Get(const Handle& Handle) const = 0;

			// Set the Value of the Property
			virtual bool Set(const Handle& Handle, const Argument& Value) const = 0;

		public:
			Type PropType;
		};

		// Contains the Reflected information of a Property
		struct PropertyInfo
		{
			PropertyInfo(const PropertyInfo&) = delete;
			PropertyInfo(PropertyInfo&&) = delete;

			const ReflectionID ID;
			const String Name;
			UniquePtr<PropertyWrapperBase> Wrapper;
			Type Owner;

			Map<String, String> Metas;
		};
	}
}
