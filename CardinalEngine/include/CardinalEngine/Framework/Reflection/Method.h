#pragma once
#include "../Core/Headers.h"
#include "ReflectionID.h"
#include "Type.h"

namespace CE
{
	namespace Detail_Reflection
	{
		struct MethodInfo;
	}

	class Any;
	class Handle;
	class Argument;

	// Contains all Informations about a Reflected Method
	class CE_API Method
	{
		friend class ReflectionManager;
		friend class ReflectionModule;
		friend class Type;

	public:
		// Helper Function to search for a Method Overload
		static const Method& FindMethodOverload(MultiMap<ReflectionID, Method>& Container, const ReflectionID& ID, const Array<Type>& ArgumentTypes);

		// Returns a Multi map of all registered Global Methods
		static MultiMap<ReflectionID, Method> All();

		// Searches for a Method with the ID
		static Method Find(const ReflectionID& ID);

		// Searches for a Method with the ID and matching ArgumentTypes
		static Method Find(const ReflectionID& ID, const Array<Type>& ArgumentTypes);

		// Searches for matching Method and Executes it
		static Any StaticExecute(const ReflectionID& ID, const Array<Argument>& Arguments = {});
		
		// Searches for matching Method and Executes it
		static Any StaticExecute(const ReflectionID& ID, const Array<Argument>& Arguments, bool& Result);

	public:
		// Default Constructor
		Method() = default;

		// MethodInfo Constructor
		Method(const WeakPtr<Detail_Reflection::MethodInfo>& Info);

	public:
		// Returns true if the Method is Valid
		bool IsValid() const;

		// Returns the ID
		const ReflectionID& ID() const;

		// Returns the Name
		const String& Name() const;

		// Returns true if the Method is Static
		bool IsStatic() const;

		// Returns the Number of Arguments
		size_t ArgumentCount() const;

		// Returns the Argument Types
		Array<Type> ArgumentTypes() const;

		// Returns true if the Argument Types match
		bool DoArgumentTypesMatch(const Array<Type>& ArgumentTypes) const;

		// Returns the Type of the Return
		Type ReturnType() const;

		// Returns the Type of the Owner of the Method
		Type Owner() const;

		// Execute the Method if it is Static with Arguments
		Any Execute(const Array<Argument>& Arguments = {}) const;

		// Execute the Method if it is Static with Arguments
		Any Execute(const Array<Argument>& Arguments, bool& Result) const;
		
		// Execute the Method with Arguments
		Any Execute(const Handle& Handle, const Array<Argument>& Arguments = {}) const;
		
		// Execute the Method with Arguments
		Any Execute(const Handle& Handle, const Array<Argument>& Arguments, bool& Result) const;

		// Returns number of Metadata
		size_t MetadataCount() const;

		// Returns all Metadatas
		Map<String, String> Metadatas() const;

		// Returns true if Metadata contains Key
		bool HasMetadata(const String& Key) const;

		// Returns the Value of Metadata Key
		String Metadata(const String& Key) const;

	private:
		ReflectionID m_ID;
		WeakPtr<Detail_Reflection::MethodInfo> m_Info;
	};
}
