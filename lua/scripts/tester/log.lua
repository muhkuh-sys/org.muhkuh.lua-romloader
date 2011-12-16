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
require("muhkuh")


g_tActiveLogWindow = nil

atComponentTestLog = {
	m_tStyleMessage = nil,
	m_tStyleError = nil
}

function atComponentTestLog:local_get_log_window(uiSerial, uiTestIdx)
	local atResults = tester.g_atTestResults[uiSerial].atResults
	-- Does the test already have a log?
	local tLogWindow = atResults[uiTestIdx].tNotebook
	if not tLogWindow then
		-- No log window yet -> create a new one.
		local aTestAttr = tester.g_atDeviceTest.atSingleTests[uiTestIdx]
		if aTestAttr~=nil then
			local strTestName = aTestAttr.strName
			-- Add another demo page to the notebook.
			local lStyle = wx.wxTE_MULTILINE + wx. wxTE_READONLY + wx.wxTE_DONTWRAP
			tLogWindow = wx.wxTextCtrl(self.this, wx.wxID_ANY, "haha!\n", wx.wxDefaultPosition, wx.wxDefaultSize, lStyle)
			local strCaption = string.format("#%d: %s", uiSerial, strTestName)
			self.this:AddPage(tLogWindow, strCaption, false)
			
			-- Remember this page.
			atResults[uiTestIdx].tNotebook = tLogWindow
		end
	end

	return tLogWindow
end


-- Create the test control window.
function atComponentTestLog:create(tParent)
	-- Set parent.
	self.tParent = tParent
	
	local lStyle = wxaui.wxAUI_NB_TOP + wxaui.wxAUI_NB_TAB_SPLIT + wxaui.wxAUI_NB_TAB_MOVE + wxaui.wxAUI_NB_SCROLL_BUTTONS + wxaui.wxAUI_NB_CLOSE_ON_ACTIVE_TAB + wxaui.wxAUI_NB_WINDOWLIST_BUTTON
	self.this = wxaui.wxAuiNotebook(tParent.this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, lStyle)
	local this = self.this
	
	-- Add a demo page to the notebook.
	self.m_welcomeLog = wx.wxTextCtrl(this, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTE_MULTILINE+wx.wxTE_READONLY)
	self.m_welcomeLog:AppendText("Welcome to the new tester!")
	this:AddPage(self.m_welcomeLog, "Welcome", true)
	
	local tLogTarget = wx.wxLogTextCtrl(self.m_welcomeLog)
	wx.wxLog.SetActiveTarget(tLogTarget)
	
	-- Create the default styles for the captures.
	self.m_tStyleMessage = wx.wxTextAttr(wx.wxBLACK)
	self.m_tStyleError = wx.wxTextAttr(wx.wxRED)
	
	return self
end


function atComponentTestLog:event_update_style()
	local tStyle = tester.g_atConfiguration.tStyle

end


function atComponentTestLog:event_update_fonts()
	-- TODO: update fonts in logs
end


function atComponentTestLog:event_update_test()
end


function atComponentTestLog:event_update_test_results(uiSerialIdx, uiSingleTestIdx)
end


function atComponentTestLog:event_select_test(uiSerial, uiTestIdx)
	-- Only process events for a subtest.
	if uiSerial~=nil and uiTestIdx~=nil then
		local tLogWindow = self:local_get_log_window(uiSerial, uiTestIdx)
		if tLogWindow then
			local iIdx = self.this:GetPageIndex(tLogWindow)
			if iIdx~=wx.wxNOT_FOUND then
				self.this:SetSelection(iIdx)
			end
		end
	end
end


function atComponentTestLog:event_start_test(uiSerial, uiTestIdx)
	-- Only process events for a subtest.
	if uiSerial~=nil and uiTestIdx~=nil then
		g_tActiveLogWindow = self:local_get_log_window(uiSerial, uiTestIdx)
	else
		g_tActiveLogWindow = nil
	end
end


function atComponentTestLog:event_test_finished(uiSerial, uiTestIdx)
	g_tActiveLogWindow = nil
end


function catch_prints(...)
	if g_tActiveLogWindow~=nil then
		-- Loop over all arguments and combine them to one big string.
		astrMsg = {}
		for iCnt=1,select("#",...) do
			-- Get one argument.
			table.insert(astrMsg, tostring(select(iCnt,...)))
		end
		table.insert(astrMsg, "\n")
		g_tActiveLogWindow:AppendText(table.concat(astrMsg, "\t"))
	else
		tester.g_pfnOriginalPrintFunction(...)
	end
end


-- Catch prints in the output window.
_G.print = catch_prints

tester.g_atComponents["log"] = atComponentTestLog

