-----------------------------------------------------------------------------
--   Copyright (C) 2011 by Christoph Thelen                                --
--   doc_bacardi@users.sourceforge.net                                     --
--                                                                         --
--   This program is free software; you can redistribute it and/or modify  --
--   it under the terms of the GNU General Public License as published by  --
--   the Free Software Foundation; either version 2 of the License, or     --
--   (at your option) any later version.                                   --
--                                                                         --
--   This program is distributed in the hope that it will be useful,       --
--   but WITHOUT ANY WARRANTY; without even the implied warranty of        --
--   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         --
--   GNU General Public License for more details.                          --
--                                                                         --
--   You should have received a copy of the GNU General Public License     --
--   along with this program; if not, write to the                         --
--   Free Software Foundation, Inc.,                                       --
--   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             --
-----------------------------------------------------------------------------

require("wx")


local atResultGrid = {}

function atResultGrid.grid_select_cell(tEvent)
	local iRow = tEvent:GetRow()
	local iCol = tEvent:GetCol()

	local uiSerial = g_atDeviceTest.uiSerialFirst + iRow
	local uiTestIdx = iCol + 1

	atResultGrid.tParent:event_select_test(atResultGrid, uiSerial, uiTestIdx)
end

function atResultGrid.grid_select_label(tEvent)
	local iRow = tEvent:GetRow()
	local iCol = tEvent:GetCol()


	if iRow>=0 and iCol==-1 then
		local uiSerial = g_atDeviceTest.uiSerialFirst + iRow
		atResultGrid.tParent:event_select_test(atResultGrid, uiSerial, nil)
	else
		tEvent:Veto()
	end
end


function atResultGrid:create(tParent)
	-- Set parent.
	self.tParent = tParent
	
	self.this = wx.wxGrid(tParent.this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxNO_BORDER + wx.wxWANTS_CHARS)
	local this = self.this
	this:CreateGrid(0, 0)

	this:SetToolTip("Doubleclick a cell or a row label to select this test.")

	this:Connect(wx.wxID_ANY, wx.wxEVT_GRID_CELL_LEFT_DCLICK, self.grid_select_cell)
	this:Connect(wx.wxID_ANY, wx.wxEVT_GRID_LABEL_LEFT_DCLICK, self.grid_select_label)

	return self
end


function atResultGrid:event_update_style()
	local tStyle = g_atConfiguration.tStyle

end


function atResultGrid:event_update_fonts()
end


function atResultGrid:event_update_test()
	-- Get the number of boards and the number of tests.
	local uiSerialFirst = g_atDeviceTest.uiSerialFirst
	local uiSerialLast = g_atDeviceTest.uiSerialLast
	local uiBoards = g_atDeviceTest.uiSerialLast - uiSerialFirst + 1
	local uiTests  = #g_atDeviceTest.atSingleTests

	-- Delete all rows and columns.
	self.this:DeleteCols(0, self.this:GetNumberCols())
	self.this:DeleteRows(0, self.this:GetNumberRows())

	-- Create new Rows and Columns.
	self.this:AppendCols(uiTests)
	self.this:AppendRows(uiBoards)
	-- Set all cells to read only.
	self.this:EnableEditing(false)

	-- Set all row labels.
	for uiSerialCnt=uiSerialFirst,uiSerialLast do
		self.this:SetRowLabelValue(uiSerialCnt-uiSerialFirst, string.format("%d", uiSerialCnt))
	end

	-- Set all column labels.
	for uiCol=1,uiTests do
		self.this:SetColLabelValue(uiCol-1, g_atDeviceTest.atSingleTests[uiCol].strName)
	end
end


function atResultGrid:event_update_test_results(uiSerialIdx, uiSingleTestIdx)
	local uiSerialCntFirst = uiBoardIdx or g_atDeviceTest.uiSerialFirst
	local uiSerialCntLast  = uiBoardIdx or g_atDeviceTest.uiSerialLast

	local uiSingleTestCntFirst = uiSingleTestIdx or 1
	local uiSingleTestCntLast  = uiSingleTestIdx or #g_atDeviceTest.atSingleTests

	for uiSerialCnt = uiSerialCntFirst, uiSerialCntLast do
		local atResults = g_atTestResults[uiSerialCnt].atResults
		for uiSingleTestCnt = uiSingleTestCntFirst, uiSingleTestCntLast do
			local eResult = atResults[uiSingleTestCnt].eResult
			local uiRow = uiSerialCnt - g_atDeviceTest.uiSerialFirst
			local uiCol = uiSingleTestCnt - 1
			self.this:SetCellValue(uiRow, uiCol, astrTestResult[eResult])
			self.this:SetCellBackgroundColour(uiRow, uiCol, atTestResultColor[eResult])
		end
	end
end


function atResultGrid:event_select_test(uiSerial, uiTestIdx)
end


g_atComponents["result_grid"] = atResultGrid

