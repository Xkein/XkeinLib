#pragma once

#include "..\DataStruct.h"

XKEINNAMESPACE_START
	class Control
	{
	protected:
		DynamicArray<Control*> childs;
		Control* parent;
		bool focused;
		bool onHovered;
		Rectangle<int> rectangle;

	public:
		virtual ~Control() _NOEXCEPT
		{
			childs.Clear();
		}

		virtual void Update() _NOEXCEPT
		{
			for (Control* child : childs) {
				child->Update();
			}
		}

		virtual void Draw() _NOEXCEPT
		{
			for (Control* child : childs) {
				child->Draw();
			}
		}

		virtual void AddChild(Control* child) _NOEXCEPT
		{
			childs.PushBack(child);
		}

	};

XKEINNAMESPACE_END