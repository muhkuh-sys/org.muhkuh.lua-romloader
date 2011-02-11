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


local atComponentControl = {
	ID_BUTTON_BROWSE_SAVE_LOGS = NewID()
}


-- Create the test control window.
function atComponentControl:create(tParent)
	-- Set parent.
	self.tParent = tParent
	
	-- Create the panel.
	self.this = wx.wxScrolledWindow(tParent.this, wx.wxID_ANY)
	local this = self.this
	this:SetScrollbars(20, 20, 50, 50)

	-- Create the main sizer.
	local tMainSizer = wx.wxBoxSizer(wx.wxVERTICAL)
	this:SetSizer(tMainSizer)

	-- Create the option box and sizer.
--	local tOptionsBox = wx.wxStaticBox(this, wx.wxID_ANY, "Options")
--	tMainSizer:Add(tOptionsBox)

	local tOptionsSizer = wx.wxFlexGridSizer(4, 3, 4, 4)
	tOptionsSizer:AddGrowableCol(1,1)
--	tOptionsBox:SetSizer(tOptionsSizer)
	tMainSizer:Add(tOptionsSizer, 0, wx.wxEXPAND)
	tMainSizer:AddSpacer(6)

	tOptionsSizer:Add(wx.wxStaticText(this, wx.wxID_ANY, "Save log after a test: "), 0, wx.wxALIGN_CENTER_VERTICAL)
	aStrChoices = {"never", "always", "ask"}
	local tSaveLogPolicy = wx.wxComboBox(this, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize, aStrChoices, wx.wxCB_DROPDOWN+wx.wxCB_READONLY)
	tSaveLogPolicy:SetSelection(2)
	tOptionsSizer:Add(tSaveLogPolicy, 0, wx.wxALIGN_CENTER_VERTICAL)
	tOptionsSizer:AddSpacer(0)

	self.tButtonBrowseSaveLogs = wx.wxBitmapButton(this, self.ID_BUTTON_BROWSE_SAVE_LOGS, wx.wxBitmap())
	tOptionsSizer:Add(wx.wxStaticText(this, wx.wxID_ANY, "Standard save path: "), 0, wx.wxALIGN_CENTER_VERTICAL)
	local tSaveLogPath = wx.wxTextCtrl(this, wx.wxID_ANY, "/tmp/muhkuh_logs/")
	tOptionsSizer:Add(tSaveLogPath, 0, wx.wxEXPAND+wx.wxALIGN_CENTER_VERTICAL)
	tOptionsSizer:Add(self.tButtonBrowseSaveLogs)

	tOptionsSizer:Add(wx.wxStaticText(this, wx.wxID_ANY, "Standard save filename: "), 0, wx.wxALIGN_CENTER_VERTICAL)
	local tSaveLogFilename = wx.wxTextCtrl(this, wx.wxID_ANY, "{TESTNAME}/{DATE}/{SERIAL}.html")
	tOptionsSizer:Add(tSaveLogFilename, 0, wx.wxEXPAND+wx.wxALIGN_CENTER_VERTICAL)
	tOptionsSizer:AddSpacer(0)

	tOptionsSizer:Add(wx.wxStaticText(this, wx.wxID_ANY, "Stop after an error: "), 0, wx.wxALIGN_CENTER_VERTICAL)
	aStrChoices = {"never", "always", "ask"}
	local tStopAfterErrorPolicy = wx.wxComboBox(this, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize, aStrChoices, wx.wxCB_DROPDOWN+wx.wxCB_READONLY)
	tStopAfterErrorPolicy:SetSelection(2)
	tOptionsSizer:Add(tStopAfterErrorPolicy, 0, wx.wxALIGN_CENTER_VERTICAL)
	tOptionsSizer:AddSpacer(0)

	local tButtonSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	tMainSizer:Add(tButtonSizer, 0, wx.wxEXPAND)

	tButtonSizer:AddStretchSpacer(1)
	tButtonSizer:Add(wx.wxButton(this, wx.wxID_ANY, "Start Test Sequence"))
	tButtonSizer:AddStretchSpacer(1)
	tButtonSizer:Add(wx.wxButton(this, wx.wxID_ANY, "Continue Test Sequence"))
	tButtonSizer:AddStretchSpacer(1)

	return self
end


function atComponentControl:update_style()
	local tStyle = g_atConfiguration.tStyle

	self.tButtonBrowseSaveLogs:SetBitmapLabel(tStyle.tImageList:GetBitmap(tStyle.IMAGE_IDX_Folder))
end


function atComponentControl:update_fonts()
end


function atComponentControl:update_test()
end


function atComponentControl:update_test_results(uiSerialIdx, uiSingleTestIdx)
end


function atComponentControl:event_select_test(uiSerial, uiTestIdx)
end


g_atComponents["control"] = atComponentControl

