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

module("read_write_gui", package.seeall)

require("select_plugin")


local m_dialog = nil

local m_textAddress = nil
local m_textValue = nil
local m_plugin = nil


-- generate window ids
local m_ID = wx.wxID_HIGHEST
function nextID()
	m_ID = m_ID+1
	return m_ID
end

local ID_READGUI_BUTTON_READ	= nextID()
local ID_READGUI_BUTTON_WRITE	= nextID()



local function createControls()
	local mainSizer
	local staticBoxAddress
	local staticBoxValue
	local buttonSizer
	local buttonRead
	local buttonWrite
	local buttonQuit
	local minSize
	local maxSize


	-- init the controls

	-- create the main sizer
	mainSizer = wx.wxBoxSizer(wx.wxVERTICAL)
	m_dialog:SetSizer(mainSizer)

	-- create address sizer
	staticBoxAddress = wx.wxStaticBoxSizer(wx.wxVERTICAL, m_dialog, "netX Address: ")
	mainSizer:Add(staticBoxAddress, 0, wx.wxEXPAND)

	-- create the spin control for the netx address
	m_textAddress = wx.wxTextCtrl(m_dialog, wx.wxID_ANY, "0")
	staticBoxAddress:Add(m_textAddress, 0, wx.wxEXPAND)

	-- create value sizer
	staticBoxValue = wx.wxStaticBoxSizer(wx.wxVERTICAL, m_dialog, "Value: ")
	mainSizer:Add(staticBoxValue, 0, wx.wxEXPAND)

	-- create the spin control for the netx address
	m_textValue = wx.wxTextCtrl(m_dialog, wx.wxID_ANY, "0")
	staticBoxValue:Add(m_textValue, 0, wx.wxEXPAND)

	mainSizer:AddSpacer(6)

	-- create the button sizer
	buttonSizer = wx.wxBoxSizer(wx.wxHORIZONTAL)
	mainSizer:Add(buttonSizer, 0, wx.wxEXPAND)

	buttonRead = wx.wxButton(m_dialog, ID_READGUI_BUTTON_READ, "Read")
	buttonWrite = wx.wxButton(m_dialog, ID_READGUI_BUTTON_WRITE, "Write")
	buttonQuit = wx.wxButton(m_dialog, wx.wxID_OK, "Quit")
	buttonSizer:AddStretchSpacer(1)
	buttonSizer:Add(buttonRead)
	buttonSizer:AddStretchSpacer(1)
	buttonSizer:Add(buttonWrite)
	buttonSizer:AddStretchSpacer(1)
	buttonSizer:Add(buttonQuit)
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

---------------------------------------

local function OnButtonRead()
	local address
	local value


	address = tonumber(m_textAddress:GetValue())
	if not address then
		wx.wxMessageBox("Failed to convert the address field to a number!", "Input error!", wx.wxOK + wx.wxICON_ERROR, m_dialog)
	else
		value = m_plugin:read_data32(address)
		m_textValue:SetValue( string.format("0x%08x", value) )
	end
end


local function OnButtonWrite()
	local address
	local value

	address = tonumber(m_textAddress:GetValue())
	if not address then
		wx.wxMessageBox("Failed to convert the address field to a number!", "Input error!", wx.wxOK + wx.wxICON_ERROR, m_dialog)
	else
		value = tonumber(m_textValue:GetValue())
		if not value then
			wx.wxMessageBox("Failed to convert the value field to a number!", "Input error!", wx.wxOK + wx.wxICON_ERROR, m_dialog)
		else
			m_plugin:write_data32(address, value)
		end
	end
end


---------------------------------------

function run(iSerialNo, iBoardCount)
	-- show the plugin selector with all plugins starting with "romloader"
	m_plugin = select_plugin.SelectPlugin("romloader.*")

	-- if no plugin was selected, 'plugin' is nil
	if not m_plugin then
		-- no plugin selected
		print("canceled!")
	else
		-- a plugin was selected, now try to connect
		print("trying to connect")
		local fOk, strError = pcall(m_plugin.connect, m_plugin)
		if not fOk then
			-- failed to connect
			print("could not connect: " .. strError)
		else
			-- ok, connected
			print("connected")

			-- create the dialog
			m_dialog = wx.wxDialog(wx.NULL, wx.wxID_ANY, "Romloader Read/Write Demo", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxDEFAULT_DIALOG_STYLE+wx.wxRESIZE_BORDER)

			-- create the controls
			createControls()

			-- connect some controls
			m_dialog:Connect(ID_READGUI_BUTTON_READ,	wx.wxEVT_COMMAND_BUTTON_CLICKED,		OnButtonRead)
			m_dialog:Connect(ID_READGUI_BUTTON_WRITE,	wx.wxEVT_COMMAND_BUTTON_CLICKED,		OnButtonWrite)

			-- Show the dialog
			m_dialog:ShowModal(true)

			-- close the plugin
			m_plugin:disconnect()
			m_plugin:delete()
		end
	end
end

