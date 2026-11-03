//-----------------------------------------------------------------------------
// @file    DragAndDrop.cpp
// @author  BrainModular team
// @brief   Accepts the files dragged on the panel and shows the names of the dropped ones.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "DragAndDrop.h"
#include "ModuleDefinition.h"

#include <string_view>

namespace
{
	constexpr float MARGIN      = 0.04f;
	constexpr float LINE_HEIGHT = 0.12f;
	constexpr float FONT_SIZE   = 0.1f;

	constexpr TRectF WHOLE_PANEL = { .left = 0.0f, .top = 0.0f, .right = 1.0f, .bottom = 1.0f };

	// Usine sends the files as a Delphi comma-text: a name holding a space,
	// a comma or a quote is quoted, and its quotes are doubled
	std::vector<std::string> splitCommaText(AnsiCharPtr commaText)
	{
		std::vector<std::string> items;
		const std::string_view text = (commaText != nullptr) ? commaText : "";

		size_t pos = 0;
		while (pos < text.size())
		{
			std::string item;
			if (text[pos] == '"')
			{
				for (++pos; pos < text.size(); ++pos)
				{
					if (text[pos] == '"')
					{
						if ((pos + 1 >= text.size()) || (text[pos + 1] != '"'))
							break;
						++pos; // doubled quote
					}
					item += text[pos];
				}
				pos = text.find(',', pos);
			}
			else
			{
				const size_t comma = text.find(',', pos);
				item = text.substr(pos, comma - pos);
				pos  = comma;
			}

			if (!item.empty())
				items.push_back(std::move(item));

			if (pos == std::string_view::npos)
				break;
			++pos;
		}
		return items;
	}

	// the files come with their full path
	std::string fileNameOf(const std::string& path)
	{
		const size_t separator = path.find_last_of("/\\");
		return (separator == std::string::npos) ? path : path.substr(separator + 1);
	}
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DragAndDrop();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DragAndDrop*>(pModule);
}

void DragAndDrop::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType    = mtControl;
	pModuleInfo->BackColor     = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DefaultWidth  = DEFAULT_MODULE_WIDTH;
	pModuleInfo->DefaultHeight = DEFAULT_MODULE_HEIGHT;
	pModuleInfo->DontProcess   = TRUE;
}

void DragAndDrop::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	colorDragOver = sdkAlphaColor(40, 160, 70);
	colorText     = sdkGetUsineColor(clFontLight);
}

void DragAndDrop::onPaint()
{
	if (isDraggedOver)
		sdkFillRect(WHOLE_PANEL, colorDragOver, 0, colorDragOver, 0);

	if (droppedFileNames.empty())
	{
		sdkFillText(WHOLE_PANEL, "drop files here", colorText, FONT_SIZE);
		return;
	}

	// one line per file, the last line counts the files that do not fit
	const int maxLines = static_cast<int>((1.0f - 2.0f * MARGIN) / LINE_HEIGHT);
	const int numFiles = static_cast<int>(droppedFileNames.size());
	const int numLines = std::min(numFiles, maxLines);

	for (int i = 0; i < numLines; ++i)
	{
		const TRectF lineRect = {
			.left   = MARGIN,
			.top    = MARGIN + static_cast<float>(i) * LINE_HEIGHT,
			.right  = 1.0f - MARGIN,
			.bottom = MARGIN + static_cast<float>(i + 1) * LINE_HEIGHT
		};
		const bool        isLastLine = (i == numLines - 1) && (numFiles > numLines);
		const std::string lineText   = isLastLine ? "+ " + std::to_string(numFiles - i) + " more" : droppedFileNames[i];

		sdkFillText(lineRect, lineText.c_str(), colorText, FONT_SIZE, FALSE, FALSE, taLeading, taCenter);
	}
}

void DragAndDrop::onMouseMove(TShiftState Shift, float X, float Y)
{
	// there is no drag leave callback: a drag that leaves the panel without
	// a drop keeps it green until the mouse moves over it again
	setDraggedOver(false);
}

LongBool DragAndDrop::onAcceptDragOver(AnsiCharPtr FilesList, float X, float Y)
{
	// called at each move of the drag, the list is empty when no file is dragged
	const bool hasFiles = (FilesList != nullptr) && (FilesList[0] != '\0');
	setDraggedOver(hasFiles);

	return hasFiles ? TRUE : FALSE;
}

void DragAndDrop::onDragDrop(AnsiCharPtr FilesList, float X, float Y)
{
	// FilesList is only valid during the call, the names are copied
	droppedFileNames.clear();
	for (const std::string& path : splitCommaText(FilesList))
		droppedFileNames.push_back(fileNameOf(path));

	isDraggedOver = false;
	sdkRepaintPanel();
}

void DragAndDrop::setDraggedOver(bool draggedOver)
{
	if (draggedOver == isDraggedOver)
		return;

	isDraggedOver = draggedOver;
	sdkRepaintPanel();
}
