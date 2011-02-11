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



local IDCounter = nil
function NewID()
	if not IDCounter then
		IDCounter = wx.wxID_HIGHEST
	end
	IDCounter = IDCounter + 1
	return IDCounter
end


TESTRESULT_OPEN         = 0
TESTRESULT_OK           = 1
TESTRESULT_FAILED       = 2
TESTRESULT_CANCELED     = 3
TESTRESULT_FATAL_ERROR  = 4

astrTestResult = {
	[TESTRESULT_OPEN]         = "Open",
	[TESTRESULT_OK]           = "OK",
	[TESTRESULT_FAILED]       = "Failed",
	[TESTRESULT_CANCELED]     = "Canceled",
	[TESTRESULT_FATAL_ERROR]  = "Fatal Error"
}

atTestResultColor = {
	[TESTRESULT_OPEN]         = wx.wxWHITE,
	[TESTRESULT_OK]           = wx.wxGREEN,
	[TESTRESULT_FAILED]       = wx.wxRED,
	[TESTRESULT_CANCELED]     = wx.wxWHITE,
	[TESTRESULT_FATAL_ERROR]  = wx.wxRED
}


g_atStyles = {}
g_atComponents = {}

g_atConfiguration = {}
g_atDeviceTest = {}
g_atTestResults = {}


-----------------------------------------------------------------------------
--
-- Main frame
--

local atMainFrame = {
	m_atStyleFiles = {
		"tester_style_supercow.lua"
	},

	m_atComponentFiles = {
		"tester_overview.lua",
		"tester_control.lua",
		"tester_details.lua",
		"tester_log.lua",
		"tester_resultgrid.lua"
	},

	-- The default layout
	m_atMainWindowDefaultLayout = {
		{
			class = "overview",
			visible = true,
			caption = "Test Overview",
			direction = wxaui.wxAUI_DOCK_TOP,
			layer = 0,
			row = 0,
			position = 0
		},
		{
			class = "control",
			visible = true,
			caption = "Test Control",
			direction = wxaui.wxAUI_DOCK_TOP,
			layer = 0,
			row = 0,
			position = 1
		},
		{
			class = "details",
			visible = true,
			caption = "Test Details",
			direction = wxaui.wxAUI_DOCK_TOP,
			layer = 0,
			row = 1,
			position = 0
		},
		{
			class = "result_grid",
			visible = true,
			caption = "Test Results",
			direction = wxaui.wxAUI_DOCK_TOP,
			layer = 0,
			row = 1,
			position = 1
		},
		{
			class = "log",
			visible = true,
			caption = none,
			direction = wxaui.wxAUI_DOCK_CENTER,
			layer = 0,
			row = 0,
			position = 0
		}
	}
}


function atMainFrame:load_styles()
	print("loading styles...")
	for tKey,strFilename in pairs(self.m_atStyleFiles) do
		print(string.format("\t'%s'", strFilename))
		local fnChunk,strError = loadfile(strFilename)
		assert(fnChunk, string.format("failed to load style '%s':\n%s", strFilename, strError or ""))()
	end
	print("finished loading styles!")
end


function atMainFrame:load_components()
	print("loading components...")
	for tKey,strFilename in pairs(self.m_atComponentFiles) do
		print(string.format("\t'%s'", strFilename))
		local fnChunk,strError = loadfile(strFilename)
		assert(fnChunk, string.format("failed to load component '%s':\n%s", strFilename, strError or ""))()
	end
	print("finished loading components!")
end


function atMainFrame:init()
	self:load_styles()
	self:load_components()


	g_atConfiguration.tFontNormal = wx.wxFont(12, wx.wxFONTFAMILY_SWISS, wx.wxFONTSTYLE_NORMAL, wx.wxFONTWEIGHT_NORMAL)
	g_atConfiguration.tFontHeadline = wx.wxFont(20, wx.wxFONTFAMILY_SWISS, wx.wxFONTSTYLE_NORMAL, wx.wxFONTWEIGHT_BOLD)
	g_atConfiguration.tFontImportantData = wx.wxFont(16, wx.wxFONTFAMILY_MODERN, wx.wxFONTSTYLE_NORMAL, wx.wxFONTWEIGHT_BOLD)

	g_atConfiguration.tStyle = g_atStyles[1]
end


function atMainFrame:create()
	-- Init all elements.
	self:init()

	-- Create the main Frame.
	self.this = wx.wxFrame(wx.NULL, wx.wxID_ANY, "lalala", wx.wxDefaultPosition, wx.wxSize(640,480), wx.wxCAPTION+wx.wxRESIZE_BORDER+wx.wxCLOSE_BOX)
	local this = self.this

	m_tAuiManager = wxaui.wxAuiManager()
	m_tAuiManager:SetManagedWindow(this)

	-- Create all components.
	for tKey,atAttributes in pairs(self.m_atMainWindowDefaultLayout) do
		-- Call the create function of the component. The returned value is something like a pannel.
		local tClass = g_atComponents[atAttributes.class]
		local tComponent = tClass:create(self)
		-- Get the best size (i.e. nothing is truncated) for the component.
		local tBestSize = tComponent.this:GetBestSize()
		-- Setup the pane information.
		local tPaneInfo = wxaui.wxAuiPaneInfo()
		if atAttributes.caption then
			tPaneInfo:CaptionVisible(true)
			tPaneInfo:Caption(atAttributes.caption)
		else
			tPaneInfo:CaptionVisible(false)
		end
		tPaneInfo:Direction(atAttributes.direction)
		tPaneInfo:Row(atAttributes.row)
		tPaneInfo:Position(atAttributes.position)
		tPaneInfo:BestSize(tBestSize)
		m_tAuiManager:AddPane(tComponent.this, tPaneInfo)
	end

	-- Set the style and font to all components.
	self:update_fonts()
	self:update_style()

	m_tAuiManager:Update()

	return self
end


function atMainFrame:update_fonts()
	for tKey,tComponent in pairs(g_atComponents) do
		tComponent:update_fonts()
	end
end


function atMainFrame:update_style()
	for tKey,tComponent in pairs(g_atComponents) do
		tComponent:update_style()
	end
end


function atMainFrame:set_test(atDeviceTest, uiSerial_First, uiSerial_Last)
	-- Remember the device test.
	g_atDeviceTest = atDeviceTest

	-- Clear the test results.
	local atTestResults = {}

	-- Create a table with one entry for each serial number with the
	-- result, the logged text and the associated notebook control.
	for uiSerialCnt=atDeviceTest.uiSerialFirst,atDeviceTest.uiSerialLast do
		-- Create the result for one serial number.
		local atBoardResult = {}
		for tKey,atTest in ipairs(atDeviceTest.atSingleTests) do
			table.insert(atBoardResult, { eResult=TESTRESULT_OPEN, strLog="", tNotebook=nil })
		end
		atTestResults[uiSerialCnt] = { uiSerial=uiSerialCnt, atResults=atBoardResult}
	end

	g_atTestResults = atTestResults

	for tKey,tComponent in pairs(g_atComponents) do
		tComponent:update_test()
		tComponent:update_test_results(nil, nil)
	end
end


-----------------------------------------------------------------------------
--
-- Event Interface.
--

function atMainFrame:event_select_test(tSource, uiSerial, uiTestIdx)
	for tKey,tComponent in pairs(g_atComponents) do
		if tComponent~=tSource then
			tComponent:event_select_test(uiSerial, uiTestIdx)
		end
	end
end


-----------------------------------------------------------------------------
--
-- Application
--

local tApplication = {}
function tApplication:OnInit()
	local tMainFrame = atMainFrame:create()


	-- set the test.
	local atDeviceTest = {
		-- The name of the test.
		strName = "NXDM10 S301 test",

		-- The first and last serial number included in this test.
		uiSerialFirst = 20000,
		uiSerialLast  = 20009,

		-- A list of all single tests in this device test.
		atSingleTests = {
			{
				strName = "Uart Test",
				strParameter = {},
				strCode = "return 0"
			},
			{
				strName = "SPI Flash Test",
				strParameter = {},
				strCode = "return 0"
			}
		}
	}
	tMainFrame:set_test(atDeviceTest)


	local tFrame = tMainFrame.this
	wx.wxGetApp():SetTopWindow(tFrame)
	tFrame:Show()
	return true
end

tApplication:OnInit()


wx.wxGetApp():MainLoop()

