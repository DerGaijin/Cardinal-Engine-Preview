#include "CardinalEngine-Editor/Game/EditorViewportEntity.h"
#include "CardinalEngine/Engine/Engine.h"
#include "CardinalEngine/Game/World.h"
#include "CardinalEngine/Game/Components/CameraComponent.h"
#include "CardinalEngine/Game/Components/InputComponent.h"

#include "CardinalEngine-Editor/ImGuiInterface/Headers.h"

namespace CE
{
	EditorViewportEntity::EditorViewportEntity()
	{
		EditorName = "Editor Viewport";
		IsEditorEntity = true;

		m_Camera = SharedPtr<CameraComponent>(new CameraComponent());
		m_Camera->EditorName = "Viewport Camera";
		AddComponent(m_Camera);

		m_Input = SharedPtr<InputComponent>(new InputComponent());
		m_Input->EditorName = "Input";
		AddComponent(m_Input);
	}

	void EditorViewportEntity::Tick(float DeltaTime)
	{
		// Movement
		{
			float MoveForward = m_Input->GetAxisValue("CEE_MoveForward");
			float MoveRight = m_Input->GetAxisValue("CEE_MoveRight");
			float MoveUp = m_Input->GetAxisValue("CEE_MoveUp");

			float SpeedMultiplier = m_Input->IsInputPressed(EInput::Keyboard_Left_Shift) ? 3.0f : 1.0f;
			if (MoveForward != 0.0f || MoveRight != 0.0f || MoveUp != 0.0f)
			{
				Vec3f Location = GetRootComponent()->GetRelativeLocation();

				Location += GetRootComponent()->GetForwardVector() * MoveForward * DeltaTime * SpeedMultiplier;
				Location += GetRootComponent()->GetRightVector() * MoveRight * DeltaTime * SpeedMultiplier;
				Location.Z += MoveUp * DeltaTime * SpeedMultiplier;

				GetRootComponent()->SetRelativeLocation(Location);
			}
		}

		// Look around
		{
			float LookRight = m_Input->GetAxisValue("CEE_LookRight");
			float LookUp = m_Input->GetAxisValue("CEE_LookUp");

			float LookSpeed = 50.0f;
			if (LookRight != 0.0f || LookUp != 0.0f)
			{
				Vec3f Rotation = GetRootComponent()->GetRelativeRotation();

				Rotation.X += LookUp * DeltaTime * LookSpeed;
				Rotation.Z += LookRight * DeltaTime * LookSpeed;

				GetRootComponent()->SetRelativeRotation(Rotation);
			}
		}
	}
}
