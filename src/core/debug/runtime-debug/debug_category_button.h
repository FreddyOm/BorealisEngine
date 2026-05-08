#pragma once
#include "../../helpers/macros.h"
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

		~DebugCategoryButton() = default;

		BOREALIS_DEFAULT_COPY_CONSTRUCT(DebugCategoryButton)
		BOREALIS_DEFAULT_MOVE_CONSTRUCT(DebugCategoryButton)
		BOREALIS_DEFAULT_COPY_ASSIGN(DebugCategoryButton)
		BOREALIS_DELETE_MOVE_ASSIGN(DebugCategoryButton)

		void Draw(Types::uint64 buttonIndex) const;

	private:
		ImVec2 size{};
		std::vector<IGUIDrawable*>* pDrawables;
		Types::StringId id = Types::String("default");

		static ImVec4 normalColor;
		static ImVec4 selectedColor;
	};
}
