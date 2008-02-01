-----------------------------------------------------------------------------
--   Copyright (C) 2008 by Christoph Thelen                                --
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

module("serialnr", package.seeall)

local m_dialog = nil

local m_spinBoardCount = nil
local m_spinSerial = nil
local m_spinYear = nil
local m_spinWeek = nil

local m_iBoardCount = -1
local m_iSerial = -1
local m_iYear = -1
local m_iWeek = -1


-- generate window ids
local m_ID = wx.wxID_HIGHEST
function nextID()
	m_ID = m_ID+1
	return m_ID
end

local ID_SERIALNODIALOG_SPINBOARDCOUNT	= nextID()
local ID_SERIALNODIALOG_SPINSERIAL	= nextID()
local ID_SERIALNODIALOG_SPINYEAR	= nextID()
local ID_SERIALNODIALOG_SPINWEEK	= nextID()


local function createControls()
	local mainSizer
	local staticBoxBoardCount
	local staticBoxSecMemInfos
	local serialSizer
	local labelSerial
	local productionDateSizer
	local labelProductionDate
	local buttonSizer
	local buttonOk
	local buttonCancel
	local minSize
	local maxSize


	-- init the controls

	-- create the main sizer
	mainSizer = wx.wxBoxSizer(wx.wxVERTICAL)
	m_dialog:SetSizer(mainSizer)

	-- create the board count controls
	staticBoxBoardCount = wx.wxStaticBoxSizer(wx.wxVERTICAL, m_dialog, "Number of boards: ")
	mainSizer:Add(staticBoxBoardCount, 0, wx.wxEXPAND)

	m_spinBoardCount = wx.wxSpinCtrl(m_dialog, ID_SERIALNODIALOG_SPINBOARDCOUNT)
	staticBoxBoardCount:Add(m_spinBoardCount, 0, wx.wxEXPAND)

	mainSizer:AddSpacer(6)

	-- create security memory info wxStaticBoxSizer
	staticBoxSecMemInfos = wx.wxStaticBoxSizer(wx.wxVERTICAL, m_dialog, "Security memory settings: ")
	mainSizer:Add(staticBoxSecMemInfos, 0, wx.wxEXPAND)

	-- create serial number controls
	serialSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	labelSerial = wx.wxStaticText(m_dialog, wx.wxID_ANY, "Serial Number(dec) of 1st board:")
	m_spinSerial = wx.wxSpinCtrl(m_dialog, ID_SERIALNODIALOG_SPINSERIAL)
	serialSizer:Add(labelSerial, 0, wx.wxALIGN_CENTER_VERTICAL)
	serialSizer:Add(m_spinSerial, 0, wx.wxALIGN_CENTER_VERTICAL)
	staticBoxSecMemInfos:Add(serialSizer, 0, wx.wxEXPAND)
	staticBoxSecMemInfos:AddSpacer(6)

	-- create ProductionDate controls
	productionDateSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	labelProductionDate = wx.wxStaticText(m_dialog, wx.wxID_ANY, "Production Date:    YYWW(dec)  ")
	m_spinYear = wx.wxSpinCtrl(m_dialog, ID_SERIALNODIALOG_SPINYEAR)
	m_spinWeek = wx.wxSpinCtrl(m_dialog, ID_SERIALNODIALOG_SPINWEEK)
        productionDateSizer:Add(labelProductionDate, 0, wx.wxALIGN_CENTER_VERTICAL)
	productionDateSizer:Add(m_spinYear, 0, wx.wxALIGN_CENTER_VERTICAL)
	productionDateSizer:Add(m_spinWeek, 0, wx.wxALIGN_CENTER_VERTICAL)
	staticBoxSecMemInfos:Add(productionDateSizer, 0, wx.wxEXPAND)
	staticBoxSecMemInfos:AddSpacer(8)

	mainSizer:AddSpacer(6)

	-- create the button sizer
	buttonSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	mainSizer:Add(buttonSizer, 0, wx.wxEXPAND)

	buttonOk = wx.wxButton(m_dialog, wx.wxID_OK, "Ok")
	buttonCancel = wx.wxButton(m_dialog, wx.wxID_CANCEL, "Cancel")
	buttonSizer:AddStretchSpacer(1)
	buttonSizer:Add(buttonOk)
	buttonSizer:AddStretchSpacer(1)
	buttonSizer:Add(buttonCancel)
	buttonSizer:AddStretchSpacer(1)
	mainSizer:AddSpacer(4)

	-- set minimum size
	mainSizer:SetSizeHints(m_dialog)
	-- x and y are not expandable
	minSize = m_dialog:GetMinSize()
	maxSize = m_dialog:GetMaxSize()
	maxSize:SetHeight( minSize:GetHeight() )
	maxSize:SetWidth( minSize:GetWidth() )
	m_dialog:SetMaxSize(maxSize)
end


local function OnSpinBoardCount(event)
	m_iBoardCount = event:GetInt()
end


local function OnSpinSerial(event)
	m_iSerial = event:GetInt()
end


local function OnSpinYear(event)
	m_iYear = event:GetInt()
end


local function OnSpinWeek(event)
	m_iWeek = event:GetInt()
end


function run(iSerialNo, iBoardCount)
	local strMacAddrByte
	local dToday
	local strYear
	local strWeek


	-- create the dialog
	m_dialog = wx.wxDialog(wx.NULL, wx.wxID_ANY, "Select the plugin", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxDEFAULT_DIALOG_STYLE+wx.wxRESIZE_BORDER)

	-- get datecode
	dToday = wx.wxDateTime()
	dToday:SetToCurrent()
	-- get year number without century
	strYear = dToday:Format("%y")
	-- get week number
	strWeek = dToday:Format("%W")

	-- create the controls
	createControls()

	-- assign the values
	m_iBoardCount = iBoardCount
	m_spinBoardCount:SetRange(1, 1000)
	m_spinBoardCount:SetValue(m_iBoardCount)

	m_iSerial = iSerialNo
	m_spinSerial:SetRange(1, 999999)
	m_spinSerial:SetValue(m_iSerial)

	m_iYear = strYear
	m_spinYear:SetRange(0, 20)
	m_spinYear:SetValue(m_iYear)
	m_iWeek = strWeek
	m_spinWeek:SetRange(01, 53)
	m_spinWeek:SetValue(m_iWeek)

	-- connect some controls
	m_dialog:Connect(ID_SERIALNODIALOG_SPINBOARDCOUNT,	wx.wxEVT_COMMAND_SPINCTRL_UPDATED,		OnSpinBoardCount)
	m_dialog:Connect(ID_SERIALNODIALOG_SPINSERIAL,		wx.wxEVT_COMMAND_SPINCTRL_UPDATED,		OnSpinSerial)
	m_dialog:Connect(ID_SERIALNODIALOG_SPINYEAR,		wx.wxEVT_COMMAND_SPINCTRL_UPDATED,		OnSpinYear)
	m_dialog:Connect(ID_SERIALNODIALOG_SPINWEEK,		wx.wxEVT_COMMAND_SPINCTRL_UPDATED,		OnSpinWeek)

	-- Show the dialog
	if m_dialog:ShowModal(true)==wx.wxID_OK then
		if _G.__MUHKUH_PARAMETERS==nil then
			_G.__MUHKUH_PARAMETERS = {}
		end
		_G.__MUHKUH_PARAMETERS.BoardCount = m_iBoardCount
		_G.__MUHKUH_PARAMETERS.SerialNumber = m_iSerial
		_G.__MUHKUH_PARAMETERS.ProductionDate = m_iYear*256 + m_iWeek
		return true
	else
		return false
	end

end

