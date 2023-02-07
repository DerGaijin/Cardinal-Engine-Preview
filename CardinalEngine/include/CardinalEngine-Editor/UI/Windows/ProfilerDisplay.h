#pragma once
#include "../../../CardinalEngine/Framework/Core/Headers.h"
#include "../EditorWindow.h"

namespace CE
{
	class CE_API ProfilerDisplay : public EditorWindow
	{
	public:
		virtual void Draw(float DeltaTime) override;

	private:
		void DrawHierarchyBlocks(const Array<ProfiledHierarchyBlock>& Blocks);

	private:
		UniquePtr<ProfiledFrame> m_CachedFrame;
	};
}
