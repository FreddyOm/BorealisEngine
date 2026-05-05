#pragma once
#include "../../types/string_id.h"
#include "../../types/types.h"
#include "imgui/imgui.h"

#include <vector>

namespace Borealis::Runtime::Debug
{
	struct IGUIDrawable;

	struct DebugCategoryButton
	{
	public:
		DebugCategoryButton(ImVec2 size, std::vector<IGUIDrawable*>* pDrawables, Types::StringId id)
			: size(size), pDrawables(pDrawables), id(id)
		{ }

		void Draw(Types::uint64 buttonIndex) const;

	private:
		ImVec2 size{};
		std::vector<IGUIDrawable*>* pDrawables;
		Types::StringId id = Types::String("default");

		static ImVec4 normalColor;
		static ImVec4 selectedColor;
	};
}