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


local atComponentOverview = {}


-----------------------------------------------------------------------------
--
-- Local functions.
--

function atComponentOverview:local_update_test_results(uiSerialIdx)
	local uiTestsUntested = 0
	local uiTestsOk = 0
	local uiTestsFailed = 0
	
	local atResults = g_atTestResults[uiSerialIdx].atResults
	for uiSingleTestCnt = 1,g_atDeviceTest.sizSingleTests do
		local eResult = atResults[uiSingleTestCnt].eResult
		if eResult==TESTRESULT_OPEN or eResult==TESTRESULT_CANCELED then
			uiTestsUntested = uiTestsUntested + 1
		elseif eResult==TESTRESULT_OK then
			uiTestsOk = uiTestsOk + 1
		else
			uiTestsFailed = uiTestsFailed + 1
		end
	end

	self.tTextTestsUntested1:SetLabel(string.format("%d", uiTestsUntested))
	self.tTextTestsOk1:SetLabel(string.format("%d", uiTestsOk))
	self.tTextTestsFailed1:SetLabel(string.format("%d", uiTestsFailed))
end


-- Create the test overview window.
function atComponentOverview:create(tParent)
	-- Set parent.
	self.tParent = tParent
	
	-- Create the panel.
	self.this = wx.wxScrolledWindow(tParent.this, wx.wxID_ANY)
	local this = self.this
	this:SetScrollbars(20, 20, 50, 50)

	-- Create the main sizer.
	local tMainSizer = wx.wxBoxSizer(wx.wxVERTICAL)
	this:SetSizer(tMainSizer)

	-- Create the Headline.
	self.tTextTestname = wx.wxStaticText(this, wx.wxID_ANY, "NXDM10 S301 test")
	tMainSizer:Add(self.tTextTestname, 0, wx.wxALIGN_CENTER)
	tMainSizer:AddSpacer(6)

	-- Create the serial number display.
	local tSerialNumberSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	tMainSizer:Add(tSerialNumberSizer, 0, wx.wxEXPAND)
	tMainSizer:AddSpacer(4)

	self.tTextSerialnumber0 = wx.wxStaticText(this, wx.wxID_ANY, "Serialnumber: ")
	self.tTextSerialnumber1 = wx.wxStaticText(this, wx.wxID_ANY, "0")
	self.tTextSerialnumber2 = wx.wxStaticText(this, wx.wxID_ANY, "from ")
	self.tTextSerialnumber3 = wx.wxStaticText(this, wx.wxID_ANY, "0")
	self.tTextSerialnumber4 = wx.wxStaticText(this, wx.wxID_ANY, " to ")
	self.tTextSerialnumber5 = wx.wxStaticText(this, wx.wxID_ANY, "0")
	tSerialNumberSizer:Add(self.tTextSerialnumber0, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tSerialNumberSizer:Add(self.tTextSerialnumber1, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tSerialNumberSizer:Add(self.tTextSerialnumber2, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tSerialNumberSizer:Add(self.tTextSerialnumber3, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tSerialNumberSizer:Add(self.tTextSerialnumber4, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tSerialNumberSizer:Add(self.tTextSerialnumber5, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)

	-- Create the result sizer.
	local tResultSizer = wx.wxFlexGridSizer(3, 4, 2, 4)
	tMainSizer:Add(tResultSizer, 0, wx.wxLEFT, 32)
	tMainSizer:AddSpacer(4)

	self.tIconTestsUntested = wx.wxStaticBitmap(this, wx.wxID_ANY, wx.wxBitmap())
	self.tTextTestsUntested0 = wx.wxStaticText(this, wx.wxID_ANY, ":")
	self.tTextTestsUntested1 = wx.wxStaticText(this, wx.wxID_ANY, "0")
	self.tTextTestsUntested2 = wx.wxStaticText(this, wx.wxID_ANY, "open")
	tResultSizer:Add(self.tIconTestsUntested, 0, wx.wxALIGN_CENTER)
	tResultSizer:Add(self.tTextTestsUntested0, 0, wx.wxALIGN_CENTER)
	tResultSizer:Add(self.tTextTestsUntested1, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tResultSizer:Add(self.tTextTestsUntested2, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_LEFT)

	self.tIconTestsOk = wx.wxStaticBitmap(this, wx.wxID_ANY, wx.wxBitmap())
	self.tTextTestsOk0 = wx.wxStaticText(this, wx.wxID_ANY, ":")
	self.tTextTestsOk1 = wx.wxStaticText(this, wx.wxID_ANY, "0")
	self.tTextTestsOk2 = wx.wxStaticText(this, wx.wxID_ANY, "ok")
	tResultSizer:Add(self.tIconTestsOk, 0, wx.wxALIGN_CENTER)
	tResultSizer:Add(self.tTextTestsOk0, 0, wx.wxALIGN_CENTER)
	tResultSizer:Add(self.tTextTestsOk1, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tResultSizer:Add(self.tTextTestsOk2, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_LEFT)

	self.tIconTestsFailed = wx.wxStaticBitmap(this, wx.wxID_ANY, wx.wxBitmap())
	self.tTextTestsFailed0 = wx.wxStaticText(this, wx.wxID_ANY, ":")
	self.tTextTestsFailed1 = wx.wxStaticText(this, wx.wxID_ANY, "0")
	self.tTextTestsFailed2 = wx.wxStaticText(this, wx.wxID_ANY, "failed")
	tResultSizer:Add(self.tIconTestsFailed, 0, wx.wxALIGN_CENTER)
	tResultSizer:Add(self.tTextTestsFailed0, 0, wx.wxALIGN_CENTER)
	tResultSizer:Add(self.tTextTestsFailed1, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_RIGHT)
	tResultSizer:Add(self.tTextTestsFailed2, 0, wx.wxALIGN_CENTER_VERTICAL+wx.wxALIGN_LEFT)

	-- Create the status display.
	local tStatusSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	tMainSizer:Add(tStatusSizer, 0, wx.wxEXPAND)
	self.tTextStatus0 = wx.wxStaticText(this, wx.wxID_ANY, "Status: ")
	self.tTextStatus1 = wx.wxStaticText(this, wx.wxID_ANY, "nothing to do...")
	tStatusSizer:Add(self.tTextStatus0)
	tStatusSizer:Add(self.tTextStatus1)

	return self
end


-----------------------------------------------------------------------------
--
-- Event Interface.
--

function atComponentOverview:event_update_style()
	local tStyle = g_atConfiguration.tStyle

	self.tIconTestsUntested:SetBitmap(tStyle.tImageList:GetBitmap(tStyle.IMAGE_IDX_TestsUntested))
	self.tIconTestsOk:SetBitmap(tStyle.tImageList:GetBitmap(tStyle.IMAGE_IDX_TestsOk))
	self.tIconTestsFailed:SetBitmap(tStyle.tImageList:GetBitmap(tStyle.IMAGE_IDX_TestsFailed))
end


function atComponentOverview:event_update_fonts()
	local tCfg = g_atConfiguration

	self.tTextTestname:SetFont(tCfg.tFontHeadline)
	self.tTextSerialnumber0:SetFont(tCfg.tFontNormal)
	self.tTextSerialnumber1:SetFont(tCfg.tFontImportantData)
	self.tTextSerialnumber2:SetFont(tCfg.tFontNormal)
	self.tTextSerialnumber3:SetFont(tCfg.tFontNormal)
	self.tTextSerialnumber4:SetFont(tCfg.tFontNormal)
	self.tTextSerialnumber5:SetFont(tCfg.tFontNormal)

	self.tTextTestsUntested0:SetFont(tCfg.tFontNormal)
	self.tTextTestsUntested1:SetFont(tCfg.tFontImportantData)
	self.tTextTestsUntested2:SetFont(tCfg.tFontNormal)

	self.tTextTestsOk0:SetFont(tCfg.tFontNormal)
	self.tTextTestsOk1:SetFont(tCfg.tFontImportantData)
	self.tTextTestsOk2:SetFont(tCfg.tFontNormal)

	self.tTextTestsFailed0:SetFont(tCfg.tFontNormal)
	self.tTextTestsFailed1:SetFont(tCfg.tFontImportantData)
	self.tTextTestsFailed2:SetFont(tCfg.tFontNormal)

	self.tTextStatus0:SetFont(tCfg.tFontNormal)
	self.tTextStatus1:SetFont(tCfg.tFontNormal)
end


function atComponentOverview:event_update_test()
	-- Get the number of boards.
	local uiSerialFirst = g_atDeviceTest.uiSerialFirst
	local uiSerialLast = g_atDeviceTest.uiSerialLast

	-- Display the new serial range.
	self.tTextSerialnumber3:SetLabel(string.format("%d", uiSerialFirst))
	self.tTextSerialnumber5:SetLabel(string.format("%d", uiSerialLast))

	-- Start with the first serial number.
	self:event_select_test(uiSerialFirst, nil)
end


function atComponentOverview:event_update_test_results(uiSerialIdx, uiSingleTestIdx)
	-- Get the number of boards and the number of tests.
	local uiSerialFirst = g_atDeviceTest.uiSerialFirst
	local uiSerialLast = g_atDeviceTest.uiSerialLast
	local uiBoards = g_atDeviceTest.uiSerialLast - uiSerialFirst + 1
	local uiTests  = g_atDeviceTest.sizSingleTests


	-- Is this update event for the complete table? -> Only update the current serial number.
	uiSerialIdx = uiSerialIdx or self.uiCurrentSerialIdx

	-- Is this a single test update event? -> Only display the results if it is the current serial number.
	if uiSerialIdx~=nil and uiSerialIdx==self.uiCurrentSerialIdx then
		self:local_update_test_results(uiSerialIdx)
	end
end


function atComponentOverview:event_select_test(uiSerialIdx, uiTestIdx)
	self.uiCurrentSerialIdx = uiSerialIdx
	self.tTextSerialnumber1:SetLabel(string.format("%d", uiSerialIdx))

	self:local_update_test_results(uiSerialIdx)
end


g_atComponents["overview"] = atComponentOverview

