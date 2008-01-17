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

module("select_plugin", package.seeall)

local m_dialog = nil
local m_list_devices = nil
local m_button_ok = nil
local m_devices = {}
local m_selected_device = nil
local m_strPattern = nil

-- IDs of the controls in the dialog
local ID_BUTTON_RESCAN   = 1
local ID_LIST_DEVICES    = 2


local function create_controls(size, iCol0Width, iCol1Width, iCol2Width)
	-- size for images
	local size_images = wx.wxSize(16, 16)

	-- cross and tick images
	local imageList = wx.wxImageList(size_images:GetWidth(), size_images:GetHeight())
	imageList:Add(wx.wxArtProvider.GetBitmap(wx.wxART_CROSS_MARK, wx.wxART_TOOLBAR, size_images))
	imageList:Add(wx.wxArtProvider.GetBitmap(wx.wxART_TICK_MARK, wx.wxART_TOOLBAR, size_images))

	-- the main sizer contains the listctrl and the button sizer
	local sizer_main = wx.wxBoxSizer(wx.wxVERTICAL)
	local sizer_buttons = wx.wxBoxSizer(wx.wxHORIZONTAL)

	-- create the quick action list
	local list_devices_style = wx.wxLC_REPORT+wx.wxLC_SINGLE_SEL+wx.wxLC_HRULES+wx.wxSUNKEN_BORDER+wx.wxLC_EDIT_LABELS
	m_list_devices = wx.wxListCtrl(m_dialog, ID_LIST_DEVICES, wx.wxDefaultPosition, wx.wxDefaultSize, list_devices_style)
	m_list_devices:SetImageList(imageList, wx.wxIMAGE_LIST_SMALL)
	m_list_devices:InsertColumn(0, "Name")
	m_list_devices:InsertColumn(1, "Typ")
	m_list_devices:InsertColumn(2, "Comment")
	sizer_main:Add(m_list_devices, 1, wx.wxGROW)

	-- create the buttons below the devicelist
	m_button_ok		= wx.wxButton(m_dialog, wx.wxID_OK, "Ok")
	local button_rescan	= wx.wxButton(m_dialog, ID_BUTTON_RESCAN, "Rescan")
	local button_cancel	= wx.wxButton(m_dialog, wx.wxID_CANCEL, "Cancel")
	sizer_buttons:AddStretchSpacer(1)
	sizer_buttons:Add(m_button_ok)
	sizer_buttons:AddStretchSpacer(1)
	sizer_buttons:Add(button_rescan)
	sizer_buttons:AddStretchSpacer(1)
	sizer_buttons:Add(button_cancel)
	sizer_buttons:AddStretchSpacer(1)
	sizer_main:Add(sizer_buttons, 0, wx.wxEXPAND)

	m_dialog:SetSizer(sizer_main)

	-- get the minimum possible size without loosing items
	local minWinSize = sizer_main:GetMinSize()
	-- set this as the minimum possible size
	m_dialog:SetMinSize(minWinSize)

	-- now validate the requested size
	local iSize0 = size:GetWidth()
	local iSize1 = minWinSize:GetWidth()
	if iSize0<iSize1 then
		iSize0 = iSize1
	end
	minWinSize:SetWidth(iSize0)
	iSize0 = size:GetHeight()
	iSize1 = minWinSize:GetHeight()
	if iSize0<iSize1 then
		iSize0 = iSize1
	end
	minWinSize:SetHeight(iSize0)

	-- set the resulting size
	m_dialog:SetSize(minWinSize)

	-- run the layout routine to update the client size
	m_dialog:Layout()


	-- set the size of the list control
	if iCol0Width==wx.wxLIST_AUTOSIZE then
		-- get size of the device list
		iCol0Width = m_list_devices:GetClientSize():GetWidth()
		iCol0Width = iCol0Width / 3
		iCol1Width = iCol0Width
		iCol2Width = iCol0Width
	end

	m_list_devices:SetColumnWidth(0, iCol0Width)
	m_list_devices:SetColumnWidth(1, iCol1Width)
	m_list_devices:SetColumnWidth(2, iCol2Width)
end


local function get_device_from_idx(index)
	local tableindex = nil
	local device = nil


	if index>=0 then
		-- get the selected element's data
		tableindex = m_list_devices:GetItemData(index)
		if tableindex~=nil and tableindex>0 and tableindex<=#m_devices then
			device = m_devices[tableindex]
		end
	end

	return device
end


local function is_device_ok(device)
	local fIsOk = false


	if device~=nil and device:IsValid()==true and device:IsUsed()==false then
		fIsOk = true
	end

	return fIsOk
end


local function device_selected(index)
	local device


	device = get_device_from_idx(index)
	m_button_ok:Enable(is_device_ok(device))
end


local function on_ok()
	local lIdx
	local device


	-- is a device selected?
	lIdx = m_list_devices:GetNextItem(-1, wx.wxLIST_NEXT_ALL, wx.wxLIST_STATE_FOCUSED)
	device = get_device_from_idx(lIdx)
	if is_device_ok(device)==true then
		m_selected_device = device
		m_dialog:EndModal(wx.wxID_OK)
	end
end


local function on_rescan()
	local plugin
	local list_item = wx.wxListItem()
	local item_id
	local comment


	-- clear all entries in the list
	m_list_devices:DeleteAllItems()
	-- clear all entries in the table
	m_devices = {}

	-- scan for the pattern
	muhkuh.ScanPlugins(m_strPattern)

	-- enter all devices into the list
	while true do
		plugin = muhkuh.GetNextPlugin()
		if plugin==nil then
			break
		elseif plugin:IsValid()==false then
			break
		else
			-- append the item to the table
			table.insert(m_devices, plugin)
			-- append new item at the end of the list
			item_id = m_list_devices:GetItemCount()
			-- create the new list item
			list_item:Clear()
			list_item:SetMask(wx.wxLIST_MASK_TEXT+wx.wxLIST_MASK_IMAGE+wx.wxLIST_MASK_DATA)
			list_item:SetId(item_id)
			list_item:SetData(#m_devices)
			list_item:SetText(plugin:GetName())
			if plugin:IsUsed()==false then
				list_item:SetImage(1)
				comment = "free"
			else
				list_item:SetImage(0)
				comment = "in use"
			end
			item_id = m_list_devices:InsertItem(list_item)
			m_list_devices:SetItem(item_id, 1, plugin:GetTyp())
			m_list_devices:SetItem(item_id, 2, comment)
		end
	end
end


local function on_item_selected(event)
	local lIdx;


	-- get the selected element
	lIdx = event:GetIndex()
	device_selected(lIdx);
end


local function on_item_deselected(event)
	device_selected(-1)
end


local function on_item_activated(event)
	local lIdx
	local device


	-- get the selected element
	lIdx = event:GetIndex()
	device = get_device_from_idx(lIdx)
	if is_device_ok(device)==true then
		m_selected_device = device
		m_dialog:EndModal(wx.wxID_OK)
	end
end


function SelectPlugin(pattern, dialog_size, dialog_iCol0Width, dialog_iCol1Width, dialog_iCol2Width)
	local plugin_instance = nil


	-- set default arguments
	m_strPattern		= pattern or ".*"
	dialog_size		= dialog_size or wx.wxSize(800,200)
	dialog_iCol0Width	= dialog_iCol0Width or wx.wxLIST_AUTOSIZE
	dialog_iCol1Width	= dialog_iCol1Width or wx.wxLIST_AUTOSIZE
	dialog_iCol2Width	= dialog_iCol2Width or wx.wxLIST_AUTOSIZE


	-- create the dialog
	m_dialog = wx.wxDialog(wx.NULL, wx.wxID_ANY, "Select the plugin", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxDEFAULT_DIALOG_STYLE+wx.wxRESIZE_BORDER)
	-- create dialog controls
	create_controls(dialog_size, dialog_iCol0Width, dialog_iCol1Width, dialog_iCol2Width)

	m_dialog:Connect(wx.wxID_OK,		wx.wxEVT_COMMAND_BUTTON_CLICKED,	on_ok)
	m_dialog:Connect(ID_BUTTON_RESCAN,	wx.wxEVT_COMMAND_BUTTON_CLICKED,	on_rescan)
	m_dialog:Connect(ID_LIST_DEVICES,	wx.wxEVT_COMMAND_LIST_ITEM_SELECTED,	on_item_selected)
	m_dialog:Connect(ID_LIST_DEVICES,	wx.wxEVT_COMMAND_LIST_ITEM_DESELECTED,	on_item_deselected)
	m_dialog:Connect(ID_LIST_DEVICES,	wx.wxEVT_COMMAND_LIST_ITEM_ACTIVATED,	on_item_activated)

	-- initial scan
	on_rescan()

	-- nothing selected at start
	device_selected(-1)

	-- Show the dialog
	if m_dialog:ShowModal(true)==wx.wxID_OK then
		local env = _G
		local fullname = m_selected_device:GetLuaCreateFn()
		for w in string.gmatch(fullname, "[^\.]+") do
			env = env[w]
			if env==nil then
				break
			end
		end

		if env~=nil and type(env)=="function" then
			plugin_instance = env(m_selected_device:GetHandle())
		else
			error("plugin '"..m_selected_device:GetName().."' specified an invalid contructor: '"..fullname.."'")
		end
	end

	return plugin_instance
end

