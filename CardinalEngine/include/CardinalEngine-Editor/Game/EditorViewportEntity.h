#pragma once
#include "../../CardinalEngine/Framework/Core/Headers.h"
#include "../../CardinalEngine/Game/Entity.h"
#include "../../CardinalEngine/Framework/Delegates/DelegateSingle.h"

namespace CE
{
	class CameraComponent;
	class InputComponent;

	class CE_API EditorViewportEntity : public Entity
	{
	public:
		EditorViewportEntity();

		virtual void Tick(float DeltaTime) override;

	public:
		const SharedPtr<CameraComponent>& Camera() const
		{
			return m_Camera;
		}

		const SharedPtr<InputComponent>& Input() const
		{
			return m_Input;
		}

	private:
		SharedPtr<CameraComponent> m_Camera;
		SharedPtr<InputComponent> m_Input;
	};
}
