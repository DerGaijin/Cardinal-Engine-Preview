#include "CardinalEngine-Editor/AssetEditors/AssetEditor.h"

namespace CE
{
	String AssetEditor::GetSuffix() const
	{
		return String::Format("%llu:%s", StorageHash(), Path().u8string().c_str());
	}
}
