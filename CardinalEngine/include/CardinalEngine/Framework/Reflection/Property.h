#pragma once
#include "../Core/Headers.h"
#include "ReflectionID.h"
#include "Type.h"

namespace CE
{
	namespace Detail_Reflection
	{
		struct PropertyInfo;
	}

	class Handle;
	class Argument;

	// Contains all Informations about a Reflected Property
	class CE_API Property
	{
		friend class ReflectionManager;
		friend class ReflectionModule;

	public:
		// Returns a Map of all registered Global Properties
		static Map<ReflectionID, Property> All();

		// Searches for a Property with the ID
		static Property Find(const ReflectionID& ID);

		// Returns a Handle to the Value of the Static Property with ID
		static Handle StaticGetValue(const ReflectionID& ID);
		
		// Sets the Value of the Static Property with ID
		static bool StaticSetValue(const ReflectionID& ID, const Argument& Value);

	public:
		// Default Constructor
		Property() = default;

		// PropertyInfo Constructor
		Property(const WeakPtr<Detail_Reflection::PropertyInfo>& Info);

	public:
		// Returns true if the Property is Valid
		bool IsValid() const;

		// Returns the ID
		const ReflectionID& ID() const;

		// Returns the Name
		const String& Name() const;

		// Returns true if the Property is Static
		bool IsStatic() const;

		// Returns true if the Property is Read Only
		bool IsReadOnly() const;

		// Returns the Type
		Type Type() const;

		// Returns the Owner of the Property
		CE::Type Owner() const;

		// Returns Value of the Property if it is Static
		Handle GetValue() const;
		
		// Returns Value of the Property
		Handle GetValue(const Handle& Handle) const;

		// Sets the Value of the Property if it is Static
		bool SetValue(const Argument& Value) const;

		// Sets the Value of the Property
		bool SetValue(const Handle& Handle, const Argument& Value) const;

		// Returns the number of Metadatas
		size_t MetadataCount() const;

		// Returns all Metadatas
		Map<String, String> Metadatas() const;

		// Returns true if Metadata contains Key
		bool HasMetadata(const String& Key) const;

		// Returns the Value of Metadata Key
		String Metadata(const String& Key) const;

	private:
		ReflectionID m_ID;
		WeakPtr<Detail_Reflection::PropertyInfo> m_Info;
	};
}
