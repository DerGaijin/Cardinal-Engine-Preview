#pragma once
#include "../../Framework/Core/Headers.h"

namespace CE
{
	class DynamicRHI;
	template<typename T>
	struct RHIObjectDeleter;

	class CE_API RHIObject : public SharedFromThis<RHIObject>
	{
		template<typename T>
		friend class RHIObjectDeleter;

	public:
		template<typename T, typename Obj>
		static T* Cast(Obj* ToCast, DynamicRHI* Owner)
		{
			static_assert(std::is_base_of_v<RHIObject, Obj>, "Obj does not inherit RHIObject");
			static_assert(std::is_base_of_v<Obj, T>, "T does not inherit Obj");

			if (ToCast == nullptr)
			{
				return nullptr;
			}

			if (!ToCast->IsValid())
			{
				return nullptr;
			}

			if (ToCast->Owner() != Owner)
			{
				return nullptr;
			}

			return dynamic_cast<T*>(ToCast);;
		}


	public:
		virtual ~RHIObject() = default;

		// Returns true if the Object is valid
		virtual bool IsValid() const = 0;

		// Returns the Owning DynamicRHI
		virtual DynamicRHI* Owner() const = 0;

	protected:
		// Destroys the Object
		virtual void Destroy() = 0;
	};

	template<typename T>
	struct RHIObjectDeleter
	{
		static_assert(std::is_base_of_v<RHIObject, T>, "T is not a RHIObject");
		constexpr RHIObjectDeleter() noexcept = default;

		template<typename T2, std::enable_if_t<std::is_convertible_v<T2*, T*>, int> = 0>
		RHIObjectDeleter(const RHIObjectDeleter<T2>&) noexcept {}

		void operator()(T* Ptr) const noexcept
		{
			static_assert(0 < sizeof(T), "can't delete an incomplete type");
			RHIObject* RHIPtr = dynamic_cast<RHIObject*>(Ptr);
			if (RHIPtr != nullptr)
			{
				RHIPtr->Destroy();
			}
			delete Ptr;
		}
	};
}
