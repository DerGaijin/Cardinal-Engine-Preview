#pragma once
#include "../Framework/Core/Headers.h"
#include "Asset.h"
#include "AssetID.h"

namespace CE
{
	enum class EAssetState
	{
		Empty,
		Loading,
		Error,
		Ready,
	};


	struct AssetRefData
	{
		Spinlock Lock;
		SharedPtr<Asset> Asset;
		std::shared_future<bool> LoadState;

		static SharedPtr<AssetRefData> Create(const SharedPtr<CE::Asset>& Asset)
		{
			std::promise<bool> Tmp;
			SharedPtr<AssetRefData> Result = SharedPtr<AssetRefData>(new AssetRefData());
			Result->Asset = Asset;
			Result->LoadState = Tmp.get_future().share();
			Tmp.set_value(true);
			return Result;
		}
	};


	template<typename A>
	class CE_API AssetRef
	{
		friend class AssetSystem;

	public:
		AssetRef() = default;

		AssetRef(const AssetID& ID) : m_ID(ID) {}

		AssetRef(const SharedPtr<A>& Asset)
		{
			m_Data = AssetRefData::Create(Asset);
		}

		AssetRef(const SharedPtr<A>& Asset, const AssetID& ID) : m_ID(ID)
		{
			m_Data = AssetRefData::Create(Asset);
		}

		AssetRef(const AssetID& ID, const SharedPtr<AssetRefData>& Data) : m_ID(ID), m_Data(Data) {}

	public:
		// Clears the AssetRef
		void Clear()
		{
			m_ID = AssetID();
			m_Data.Reset();
		}

	public:
		// Returns the ID if the asset
		const AssetID& ID() const
		{
			return m_ID;
		}

		// Returns the current state
		EAssetState State() const
		{
			if (m_Data != nullptr)
			{
				std::lock_guard<Spinlock> Lock(m_Data->Lock);
				if (m_Data->LoadState.valid())
				{
					if (m_Data->LoadState._Is_ready())
					{
						return m_Data->LoadState.get() ? EAssetState::Ready : EAssetState::Error;
					}
					return EAssetState::Loading;
				}
			}

			EAssetState::Empty;
		}

		// Wait until the asset is loaded and ready to be used
		void Wait()
		{
			if (m_Data != nullptr)
			{
				std::shared_future<bool> Temp;
				{
					std::lock_guard<Spinlock> Lock(m_Data->Lock);
					Temp = m_Data->LoadState;
				}
				return Temp.wait();
			}
		}

		// Wait for time or until the asset is loaded and ready to be used
		template<typename Rep, typename Per>
		std::future_status WaitFor(const std::chrono::duration<Rep, Per>& Duration)
		{
			if (m_Data != nullptr)
			{
				std::shared_future<bool> Temp;
				{
					std::lock_guard<Spinlock> Lock(m_Data->Lock);
					Temp = m_Data->LoadState;
				}
				return Temp.wait_for(Duration);
			}
		}

		// Wait until time or until the asset is loaded and ready to be used
		template<typename Clock, typename Dur>
		std::future_status WaitUntil(const std::chrono::time_point<Clock, Dur>& TimePoint)
		{
			if (m_Data != nullptr)
			{
				std::shared_future<bool> Temp;
				{
					std::lock_guard<Spinlock> Lock(m_Data->Lock);
					Temp = m_Data->LoadState;
				}
				return Temp.wait_until(TimePoint);
			}
		}

		// Returns the asset
		SharedPtr<A> Get() const
		{
			return Get<A>();
		}

		// Returns the asset with cast
		template<typename T>
		std::enable_if_t<std::is_base_of_v<Asset, T>, SharedPtr<T>> Get() const
		{
			if (m_Data != nullptr)
			{
				std::lock_guard<Spinlock> Lock(m_Data->Lock);
				return SharedPtr<T>::DynamicCast(m_Data->Asset);
			}
			return nullptr;
		}

	private:
		AssetID m_ID;
		SharedPtr<AssetRefData> m_Data;
	};
}
