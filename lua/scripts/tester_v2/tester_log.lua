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


atComponentTestLog = {}


-- Create the test control window.
function atComponentTestLog:create(tParent)
	-- Set parent.
	self.tParent = tParent
	
	self.this = wxaui.wxAuiNotebook(tParent.this, wx.wxID_ANY)
	local this = self.this
	
	-- Add a demo page to the notebook.
	m_welcomeHtml = wx.wxHtmlWindow(this, wx.wxID_ANY, wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxSUNKEN_BORDER)
	m_welcomeHtml:SetPage("<html><head><title>Welcome</title></head><body><h1>Welcome to the new tester!</h1></body></html>")
	--m_welcomeHtml:SetRelatedFrame(m_mainDialog, "lalala - %s")
	m_welcomeHtml:SetRelatedStatusBar(0)
	this:AddPage(m_welcomeHtml, "Welcome", false)

	return self
end


function atComponentTestLog:update_style()
	local tStyle = g_atConfiguration.tStyle

end


function atComponentTestLog:update_fonts()
end


function atComponentTestLog:update_test()
end


function atComponentTestLog:update_test_results(uiSerialIdx, uiSingleTestIdx)
end


function atComponentTestLog:event_select_test(uiSerial, uiTestIdx)
	local this = self.this
	
	-- Only process events for a subtest.
	if uiSerial~=nil and uiTestIdx~=nil then
		local atResults = g_atTestResults[uiSerial].atResults
		
		local tLogWindow
		
		-- Does the test already have a log?
		tLogWindow = atResults[uiTestIdx].tNotebook
		if not tLogWindow then
			aTestAttr = g_atDeviceTest.atSingleTests[uiTestIdx]
			if aTestAttr~=nil then
				strTestName = aTestAttr.strName
				-- Add another demo page to the notebook.
				tLogWindow = wx.wxTextCtrl(this, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxTE_MULTILINE+wx.wxTE_READONLY)
				tLogWindow:AppendText(string.format("Log for Serial %d, %s", uiSerial, strTestName))
				strCaption = string.format("#%d: %s", uiSerial, strTestName)
				this:AddPage(tLogWindow, strCaption, false)
		
				-- Remember this page.
				atResults[uiTestIdx].tNotebook = tLogWindow
			end
		end
		
		if tLogWindow then
			local iIdx = self.this:GetPageIndex(tLogWindow)
			if iIdx~=wx.wxNOT_FOUND then
				self.this:SetSelection(iIdx)
			end
		end
	end
end


g_atComponents["log"] = atComponentTestLog

