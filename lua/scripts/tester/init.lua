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

module("tester", package.seeall)


require("wx")
require("muhkuh")


local TESTER_ID_COUNTER = nil
function NewID()
	if not TESTER_ID_COUNTER then
		TESTER_ID_COUNTER = wx.wxID_HIGHEST
	end
	TESTER_ID_COUNTER = TESTER_ID_COUNTER + 1
	return TESTER_ID_COUNTER
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

g_pfnOriginalPrintFunction = nil



-----------------------------------------------------------------------------
--
-- Exported functions for tests.
--

local m_commonPlugin = nil

_G.__MUHKUH_TEST_RESULT_OK          = 1
_G.__MUHKUH_TEST_RESULT_CANCEL      = 2
_G.__MUHKUH_TEST_RESULT_FAIL        = -1
_G.__MUHKUH_TEST_RESULT_FATALERROR  = -2


function getCommonPlugin(pattern)
	local pattern = pattern or ".*"
	if not m_commonPlugin then
		local plugin = select_plugin.SelectPlugin(pattern)
		if plugin then
			print("tester.getCommonPlugin: trying to connect")
			local fOk, strError = pcall(plugin.Connect, plugin)
			if fOk then
				print("connected")
				m_commonPlugin = plugin
			else
				print(strError)
				print("could not connect")
				print(debug.traceback())
			end
		end
	end
	return m_commonPlugin
end

function closeCommonPlugin()
	if m_commonPlugin then
		m_commonPlugin:Disconnect()
		m_commonPlugin = nil
	end
end

function hexdump(strData, iBytesPerRow)
	local iCnt
	local iByteCnt
	local strDump


	if not iBytesPerRow then
		iBytesPerRow = 16
	end

	iByteCnt = 0
	for iCnt=1,strData:len() do
		if iByteCnt==0 then
			strDump = string.format("%08X :", iCnt-1)
		end
		strDump = strDump .. string.format(" %02X", strData:byte(iCnt))
		iByteCnt = iByteCnt + 1
		if iByteCnt==iBytesPerRow then
			iByteCnt = 0
			print(strDump)
		end
	end
	if iByteCnt~=0 then
		print(strDump)
	end
end


-----------------------------------------------------------------------------
--
-- Main frame
--

local atMainFrame = {
	ID_MAIN_FRAME = NewID(),
	ID_MENU_TEST_START = NewID(),
	ID_MENU_TEST_CANCEL = NewID(),
	ID_MENU_TEST_SAVE_REPORT = NewID(),
	ID_TESTER_PROCESS = NewID(),


	m_atStyleFiles = {
		"tester.style.supercow"
	},

	m_atComponentFiles = {
		"tester.overview",
		"tester.control",
		"tester.details",
		"tester.log",
		"tester.resultgrid"
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
	},

	m_uiRunningTest_Serial_First = nil,
	m_uiRunningTest_Serial_Last  = nil,

	m_uiRunningTest_SingleTest_First = nil,
	m_uiRunningTest_SingleTest_Last  = nil
}


function atMainFrame:load_styles()
	print("loading styles...")
	for tKey,strFilename in pairs(self.m_atStyleFiles) do
		print(string.format("\t'%s'", strFilename))
		require(strFilename)
	end
	print("finished loading styles!")
end


function atMainFrame:load_components()
	print("loading components...")
	for tKey,strFilename in pairs(self.m_atComponentFiles) do
		print(string.format("\t'%s'", strFilename))
		require(strFilename)
	end
	print("finished loading components!")
end


function atMainFrame:init()
	wx.wxLog.SetVerbose(true)
	wx.wxLog.SetLogLevel(wx.wxLOG_Debug)

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
	self.this = wx.wxFrame(wx.NULL, self.ID_MAIN_FRAME, "lalala", wx.wxDefaultPosition, wx.wxSize(640,480), wx.wxCAPTION+wx.wxRESIZE_BORDER+wx.wxCLOSE_BOX)
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
	self:event_update_fonts(nil)
	self:event_update_style(nil)

	m_tAuiManager:Update()

	this:Connect(self.ID_TESTER_PROCESS, wx.wxEVT_END_PROCESS, function(tEvent) atMainFrame:event_test_process_finished(tEvent) end)

	return self
end


function atMainFrame:set_test(atDeviceTest)
	-- Remember the device test.
	g_atDeviceTest = atDeviceTest

	-- Clear the test results.
	local atTestResults = {}

	-- Create a table with one entry for each serial number with the
	-- result, the logged text and the associated notebook control.
	for uiSerialCnt=atDeviceTest.uiSerialFirst,atDeviceTest.uiSerialLast do
		-- Create the result for one serial number.
		local atBoardResult = {}
		for iCnt=1,atDeviceTest.sizSingleTests do
			table.insert(atBoardResult, { eResult=TESTRESULT_OPEN, strLog="", tNotebook=nil })
		end
		atTestResults[uiSerialCnt] = { uiSerial=uiSerialCnt, atResults=atBoardResult}
	end

	g_atTestResults = atTestResults

	self:event_update_test(nil)
	self:event_update_test_results(nil, nil, nil)
end


function atMainFrame:exit()
	atMainFrame.this:Close()
end

-----------------------------------------------------------------------------
--
-- Main Frame Menu.
--

function atMainFrame:create_menu()
	local this = self.this

	-- Create the "File" menu.
	local tMenu_File = wx.wxMenu()
	tMenu_File:Append(wx.wxID_EXIT, "E&xit", "Quit the program")

	-- Create the "Test" menu.
	local tMenu_Test = wx.wxMenu()
	tMenu_Test:Append(self.ID_MENU_TEST_START, "Start", "Start the test")
	tMenu_Test:Append(self.ID_MENU_TEST_CANCEL, "Cancel", "Cancel the running test")
	tMenu_Test:Append(self.ID_MENU_TEST_SAVE_REPORT, "Save Report", "Save the test report")

	-- Create the "Help" menu.
	local tMenu_Help = wx.wxMenu()
	tMenu_Help:Append(wx.wxID_ABOUT, "&About", "About the tester.")

	-- Put all menus in the menu bar.
	local tMenuBar = wx.wxMenuBar()
	tMenuBar:Append(tMenu_File, "&File")
	tMenuBar:Append(tMenu_Test, "&Test")
	tMenuBar:Append(tMenu_Help, "&Help")
	this:SetMenuBar(tMenuBar)

	-- TODO: connect all menu events to the functions.
	this:Connect(wx.wxID_EXIT, wx.wxEVT_COMMAND_MENU_SELECTED, self.exit )

	this:Connect(self.ID_MENU_TEST_START, wx.wxEVT_COMMAND_MENU_SELECTED, function(tEvent) atMainFrame:test_start(tEvent) end )

	-- Create the status bar.
	this:CreateStatusBar(1)
	this:SetStatusText("Welcome to tester.")
end


-----------------------------------------------------------------------------
--
-- Event Interface.
--

function atMainFrame:event_update_test(tSource)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_update_test then
				tComponent:event_update_test()
			end
		end
	end
end


function atMainFrame:event_update_test_results(tSource, uiSerial, uiTestIdx)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_update_test_results then
				tComponent:event_update_test_results(uiSerial, uiTestIdx)
			end
		end
	end
end


function atMainFrame:event_update_fonts(tSource)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_update_fonts then
				tComponent:event_update_fonts()
			end
		end
	end
end


function atMainFrame:event_update_style(tSource)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_update_style then
				tComponent:event_update_style()
			end
		end
	end
end


function atMainFrame:event_select_test(tSource, uiSerial, uiTestIdx)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_select_test then
				tComponent:event_select_test(uiSerial, uiTestIdx)
			end
		end
	end
end


function atMainFrame:event_start_test(tSource, uiSerial, uiTestIdx)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_start_test then
				tComponent:event_start_test(uiSerial, uiTestIdx)
			end
		end
	end
end


function atMainFrame:event_test_finished(tSource, uiSerial, uiTestIdx)
	-- Loop over all components.
	for tKey,tComponent in pairs(g_atComponents) do
		-- Send the event to the component if it is not the source.
		if tComponent~=tSource then
			-- Does the event handler exist?
			if tComponent.event_test_finished then
				tComponent:event_test_finished(uiSerial, uiTestIdx)
			end
		end
	end
end


-----------------------------------------------------------------------------
--
-- Test functions.
--

-- Read the test description from the file 'test_description.lua'. This file
-- must be in the current working folder. It contains a simple table definition,
-- which maps the sub test numbers to a 'name' and 'version' pair.
function atMainFrame:test_read(uiSerial_First, uiSerial_Last)
	local atDeviceTest = {
		-- The name of the test.
		strName = "",

		-- The first and last serial number included in this test.
		uiSerialFirst = uiSerial_First,
		uiSerialLast  = uiSerial_Last,

		-- A list of all single tests in this device test.
		sizSingleTests = 0,
		atSingleTests = {}
	}

	-- Try to execute the file.
	local fStatus,aResult = pcall(dofile, "test_description.lua")
	if fStatus==false then
		print("Failed to read the file test_description.lua !")
		print(aResult)
	elseif type(_G.__MUHKUH_ALL_TESTS)~="table" then
		print("The test description did not define a global array named __MUHKUH_ALL_TESTS !")
		fStatus = false
	elseif type(_G.__MUHKUH_ALL_TESTS[0])~="table" then
		print("The list of tests has no root entry!")
		fStatus = false
	elseif #_G.__MUHKUH_ALL_TESTS==0 then
		print("The list of tests is empty!")
		fStatus = false
	end

	if fStatus==true then
		atDeviceTest.strName = _G.__MUHKUH_ALL_TESTS[0].name
		atDeviceTest.sizSingleTests = #_G.__MUHKUH_ALL_TESTS
		for iCnt,aAttr in ipairs(_G.__MUHKUH_ALL_TESTS) do
			local aSubTest = {
				strName = aAttr.name,
				strVersion = aAttr.version
			}
			table.insert(atDeviceTest.atSingleTests, aSubTest)
		end
	end

	return atDeviceTest
end


function atMainFrame:test_start(tEvent)
	print("start the test")


	-- run all tests on all boards
	local uiBoardIdx = nil
	local uiSingleTestIdx = nil

	self.m_uiRunningTest_Serial_First = uiBoardIdx or g_atDeviceTest.uiSerialFirst
	self.m_uiRunningTest_Serial_Last  = uiBoardIdx or g_atDeviceTest.uiSerialLast

	self.m_uiRunningTest_SingleTest_First = uiSingleTestIdx or 1
	self.m_uiRunningTest_SingleTest_Last  = uiSingleTestIdx or g_atDeviceTest.sizSingleTests

	self:run_one_single_test(self.m_uiRunningTest_Serial_First, self.m_uiRunningTest_SingleTest_First)

--	-- Run more tests?
--	if uiSerial<self.m_uiRunningTest_Serial_Last or uiSingleTest<self.m_uiRunningTest_SingleTest_Last then
--		-- TODO: ask the user if we should move on.
--
--		-- Run the next test.
--		if uiSingleTest<self.m_uiRunningTest_SingleTest_Last then
--			uiSingleTest = uiSingleTest + 1
--		else
--			uiSerial = uiSerial + 1
--			uiSingleTest = self.m_uiRunningTest_SingleTest_First
--		end
--
--		self:run_one_single_test(uiSerial, uiSingleTest)
--	end
end


function atMainFrame:run_one_single_test(uiSerial, uiSingleTest)
	-- Clear the test result.
	print(uiSerial, uiSingleTest)
	g_atTestResults[uiSerial].atResults[uiSingleTest].eResult = TESTRESULT_OPEN
	self:event_update_test_results(nil, uiSerial, uiSingleTest)

	self.m_tRunningTest_Serial = uiSerial
	self.m_tRunningTest_SingleTest = uiSingleTest

	self:event_select_test(nil, uiSerial, uiSingleTest)
	self:event_start_test(nil, uiSerial, uiSingleTest)

	-- This is the filename of the base parameters.
	local strParametersBase = "test_description_0_par.lua"
	-- This is the filename of the subtest parameters.
	local strParametersSubtest = string.format("test_description_%d_par.lua", uiSingleTest)
	-- This is the filename of the subtest code.
	local strCodeSubtest = string.format("test_description_%d_code.lua", uiSingleTest)

	-- This is the test result.
	local eResult
	
	_G.__MUHKUH_TEST_PARAMETER = {}
	fResult,tResult = pcall(dofile, strParametersBase)
	if fResult~=true then
		print("*** ERROR ***")
		print(string.format("*** Failed to execute the parameter file '%s'.", strParametersBase))
		print(string.format("*** Error: %s", tResult))
		eResult = TESTRESULT_FATAL_ERROR
	else
		fResult,tResult = pcall(dofile, strParametersSubtest)
		if fResult~=true then
			print("*** ERROR ***")
			print(string.format("*** Failed to execute the parameter file '%s'.", strParametersSubtest))
			print(string.format("*** Error: %s", tResult))
			eResult = TESTRESULT_FATAL_ERROR
		else
			fResult,tResult = pcall(dofile, strCodeSubtest)
			if fResult~=true then
				print("*** ERROR ***")
				print(string.format("*** Failed to execute the code file '%s'.", strCodeSubtest))
				print(string.format("*** Error: %s", tResult))
				eResult = TESTRESULT_FATAL_ERROR
			else
				-- Translate the Muhkuh result to the internal values.
				if tResult==__MUHKUH_TEST_RESULT_OK then
					eResult = TESTRESULT_OK
				elseif tResult==__MUHKUH_TEST_RESULT_FAIL then
					eResult = TESTRESULT_FAILED
				elseif tResult==__MUHKUH_TEST_RESULT_CANCEL then
					eResult = TESTRESULT_CANCELED
				else
					eResult = TESTRESULT_FATAL_ERROR
				end
			end
		end
	end

	g_atTestResults[uiSerial].atResults[uiSingleTest].eResult = eResult
	self:event_update_test_results(nil, uiSerial, uiSingleTest)

	wx.wxLogMessage("Test finished.")

	self:event_test_finished(nil, uiSerial, uiSingleTest)
end


-----------------------------------------------------------------------------
--
-- Application
--

local tApplication = {}
function tApplication:OnInit()
	local tMainFrame = atMainFrame:create()
	tMainFrame:create_menu()

	local uiSerial_First = 20000
	local uiSerial_Last  = 20000

	-- set the test.
	local atDeviceTest = tMainFrame:test_read(uiSerial_First, uiSerial_Last)
	tMainFrame:set_test(atDeviceTest)

	local tFrame = tMainFrame.this
	wx.wxGetApp():SetTopWindow(tFrame)
	tFrame:Show()
	return true
end


function run()
	-- Save the original print function. Maybe we can restore it later.
	g_pfnOriginalPrintFunction = _G.print

	tApplication:OnInit()

	wx.wxGetApp():MainLoop()
end

