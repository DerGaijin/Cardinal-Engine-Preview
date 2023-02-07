#pragma once
#include "../../Core/Headers.h"
#include "../ReflectionID.h"
#include "../Type.h"
#include "../Any.h"
#include "../Handle.h"
#include "../Argument.h"

namespace CE
{
	namespace Detail_Reflection
	{
		// Base class for Method Wrappers
		class CE_API MethodWrapperBase
		{
		public:
			// Default destructor
			virtual ~MethodWrapperBase() = default;

			// Returns true if the Method is static
			virtual bool IsStatic() const = 0;

			// Returns the number of arguments of the Method
			virtual size_t ArgumentCount() const = 0;

			// Returns the ReflectionID of the Argument at Index
			virtual ReflectionID GetArgumentReflectionID(size_t Index) const = 0;

			// Returns the ReflectionID of the return Type
			virtual ReflectionID GetReturnReflectionID() const = 0;

			// Executes the Method on Handle with given Arguments
			virtual Any Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const = 0;

			// Checks if the Arguments can be used with the Method
			bool DoArgumentsMatch(const Array<Argument>& Arguments) const
			{
				if (ArgumentTypes.Size() != Arguments.Size())
				{
					return false;
				}

				auto It1 = ArgumentTypes.begin();
				auto It2 = Arguments.begin();
				while (It1 != ArgumentTypes.end() && It2 != Arguments.end())
				{
					if (!It2->IsSuitable(*It1))
					{
						return false;
					}

					It1++;
					It2++;
				}

				return true;
			}

		public:
			Array<Type> ArgumentTypes;
			Type ReturnType;
		};



		// Contains the Reflected information of a Method
		struct MethodInfo
		{
			MethodInfo(const MethodInfo&) = delete;
			MethodInfo(MethodInfo&&) = delete;

			const ReflectionID ID;
			const String Name;
			UniquePtr<MethodWrapperBase> Wrapper;
			Type Owner;

			Map<String, String> Metas;
		};
	}
}
