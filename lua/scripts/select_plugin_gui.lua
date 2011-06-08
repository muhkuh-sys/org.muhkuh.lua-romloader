-----------------------------------------------------------------------------
--   Copyright (C) 2010 by Christoph Thelen                                --
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
local m_aDetectedInterfaces = nil
local m_list_references = nil
local m_button_ok = nil
local m_references = {}
local m_selected_reference = nil
local m_strPattern = nil

-- IDs of the controls in the dialog
local ID_BUTTON_RESCAN		= 1
local ID_LIST_REFERENCES	= 2


famfamfam_cross_xpm = { 
"16 16 120 2",
"   c #FF6263",
" . c #FE7679",
" X c #E21D1F",
" o c #E83032",
" O c #EF4346",
" + c #F6585A",
" @ c #F55659",
" # c #FB696C",
" $ c #FF7A7D",
" % c #F0474A",
" & c #F75C5E",
" * c #FF878A",
" = c #F65A5D",
" - c #FB6D6F",
" ; c #FF7E80",
" : c #EB383B",
" > c #F24D4F",
" , c #FF7376",
" < c #FD7174",
" 1 c #E92D2F",
" 2 c #F75557",
" 3 c #ED3E40",
" 4 c #FA6467",
" 5 c #FF7779",
" 6 c #F64A4C",
" 7 c #F4484A",
" 8 c #FC7376",
" 9 c #E21C1E",
" 0 c #E72D30",
" q c #FF8286",
" w c #FF797C",
" e c #E32022",
" r c #F04649",
" t c #FF585B",
" y c #F75B5D",
" u c #FF7072",
" i c #F6595C",
" p c #FA6A6D",
" a c #FF6768",
" s c #FA6163",
" d c #F75D60",
" f c #ED3D3F",
" g c #EC3B3E",
" h c #FA6366",
" j c #F86164",
" k c #F24547",
" l c #EE4143",
" z c #E31F21",
" x c #EB3436",
" c c #F6585B",
" v c #FC6B6E",
" b c #FF7C7E",
" n c #FE7A7D",
" m c #F75C5F",
" M c #FF7174",
" N c #FD6F72",
" B c #FB6D70",
" V c #FF7E81",
" C c #EC3A3D",
" Z c #FA6265",
" A c #F86063",
" S c #EA2F31",
" D c #EE4042",
" F c #F35154",
" G c #FF777A",
" H c #FF5659",
" J c #FF6E70",
" K c #EE4245",
" L c #F9686A",
" P c #FF797D",
" I c #E42224",
" U c #FF7073",
" Y c #FD6E71",
" T c #FA6C6E",
" R c #F13F41",
" E c #F24C4F",
" W c #F96163",
" Q c #F85F62",
" ! c #F75F61",
" ~ c #FF7476",
" ^ c #E62A2C",
" / c #F96366",
" ( c #FF7679",
" ) c #FD7477",
" _ c #EE4144",
" ` c #F65659",
" ' c #F55658",
" ] c #E93235",
" [ c #EF4548",
" { c #F65A5C",
" } c #E52527",
" | c #F44D50",
".  c #F85E61",
".. c #FF5B5E",
".X c #FE7174",
".o c #FFFFFF",
".O c #E5272A",
".+ c #FE7E81",
".@ c #EC3C3E",
".# c #F86264",
".$ c #FF5457",
".% c #FF6C6E",
".& c #F65558",
".* c #FF797B",
".= c #F04648",
".- c #FF7071",
".; c #EF4447",
".: c #F6595B",
".> c #F5575A",
"., c #FF7B7E",
".< c #F54E50",
".1 c #FD797C",
".2 c #EB3739",
".3 c #FF7274",
".4 c #EE3D3F",
".5 c #EB393C",
".6 c #FF7477",
".7 c #ED3F41",
".8 c #FB6769",
".9 c None",
".9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9",
".9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9",
".9.9.9 G ..9.9.9.9.9.9 j L F.9.9",
".9.9 G V.+ <.9.9.9.9 Q - b Q.9.9",
".9.9.X n *.1 #.9.9.  T $ ! r.9.9",
".9.9.9 v ) q 8.# d p.* d [.9.9.9",
".9.9.9.9 / B ;., w 5 &.;.9.9.9.9",
".9.9.9.9.9 = P.. t ~ O.9.9.9.9.9",
".9.9.9.9.9 y ( H.$ u.=.9.9.9.9.9",
".9.9.9.9 { 4.3 U J.% 2.4.9.9.9.9",
".9.9.9.: h M + l.@ | a.< x.9.9.9",
".9.9 c Z.- ` D.9.9 ] k   7 1.9.9",
".9.9 ' s.&.7.9.9.9.9 ^ R 6 S.9.9",
".9.9.9 E 3.9.9.9.9.9.9 I S.9.9.9",
".9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9",
".9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9"}


famfamfam_tick_xpm = {
"16 16 83 1",
"  c #61A758",
". c #5FA556",
"X c #5CA953",
"o c #5CA353",
"O c #367A31",
"+ c #56A14D",
"@ c #33762E",
"# c #549F4B",
"$ c #86CC7C",
"% c #7EC874",
"& c #6DB463",
"* c #6CB262",
"= c #6AB260",
"- c #6AB060",
"; c #66B65C",
": c #68AE5E",
"> c #63B059",
", c #3D8337",
"< c #5FAC55",
"1 c #3C8136",
"2 c #3A7F34",
"3 c #397D33",
"4 c #377B31",
"5 c #34772E",
"6 c #87CD7C",
"7 c #85C97A",
"8 c #83C978",
"9 c #81C976",
"0 c #76BD6B",
"q c #499242",
"w c #6ABB5F",
"e c #69B95E",
"r c #478E40",
"t c #67B75C",
"y c #66B55B",
"u c #67AD5C",
"i c #3F8438",
"p c #90D284",
"a c #87CC7B",
"s c #5CA354",
"d c #579F4F",
"f c #79C06D",
"g c #529D4A",
"h c #499141",
"j c #8FD182",
"k c #64AC5B",
"l c #86C979",
"z c #60A657",
"x c #5DA454",
"c c #5AA651",
"v c #357930",
"b c #57A24E",
"n c #55A04C",
"m c #32752D",
"M c #82CB78",
"N c #6DB563",
"B c #6DB363",
"V c #69AF5F",
"C c #3D8237",
"Z c #3B8035",
"A c #3A7E34",
"S c #387C32",
"D c #35782F",
"F c #8ACE7F",
"G c #89CE7E",
"H c #85C87A",
"J c #82CA77",
"K c #81CA76",
"L c #6ABA5F",
"P c #FFFFFF",
"I c #65B45A",
"U c #408539",
"Y c #8DCF81",
"T c #8BCF7F",
"R c #89CD7D",
"E c #85CB79",
"W c #5AA252",
"Q c #59A051",
"! c #539E4B",
"~ c #519C49",
"^ c #519A49",
"/ c #8DCE80",
"( c None",
"((((((((((((((((",
"((((((((((((((((",
"((((((((((((b(((",
"(((((((((((+BV((",
"((((((((((n*jHz(",
"(((((((((#-YT U(",
"((((((((!VFGzC((",
"((;f(((g:R6.1(((",
"(yl/c(~u$ExZ((((",
"(>0p7=&8Mo2(((((",
"((cNGaK9WA((((((",
"(((^kJ%Q3(((((((",
"((((rsdS((((((((",
"(((((,4(((((((((",
"((((((((((((((((",
"(((((((((((((((("}


local function create_controls(size, iCol0Width, iCol1Width, iCol2Width)
	-- Size for images.
	local size_images = wx.wxSize(16, 16)

	-- Cross and tick images.
	local imageList = wx.wxImageList(size_images:GetWidth(), size_images:GetHeight())
	imageList:Add(wx.wxBitmap(famfamfam_cross_xpm))
	imageList:Add(wx.wxBitmap(famfamfam_tick_xpm))

	-- the main sizer contains the listctrl and the button sizer
	local sizer_main = wx.wxBoxSizer(wx.wxVERTICAL)
	local sizer_buttons = wx.wxBoxSizer(wx.wxHORIZONTAL)

	-- create the quick action list
	local list_references_style = wx.wxLC_REPORT+wx.wxLC_SINGLE_SEL+wx.wxLC_HRULES+wx.wxSUNKEN_BORDER+wx.wxLC_EDIT_LABELS
	m_list_references = wx.wxListCtrl(m_dialog, ID_LIST_REFERENCES, wx.wxDefaultPosition, wx.wxDefaultSize, list_references_style)
	m_list_references:SetImageList(imageList, wx.wxIMAGE_LIST_SMALL)
	m_list_references:InsertColumn(0, "Name")
	m_list_references:InsertColumn(1, "Typ")
	m_list_references:InsertColumn(2, "Comment")
	sizer_main:Add(m_list_references, 1, wx.wxGROW)

	-- create the buttons below the referencelist
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
		-- get size of the reference list
		iCol0Width = m_list_references:GetClientSize():GetWidth()
		iCol0Width = iCol0Width / 3
		iCol1Width = iCol0Width
		iCol2Width = iCol0Width
	end

	m_list_references:SetColumnWidth(0, iCol0Width)
	m_list_references:SetColumnWidth(1, iCol1Width)
	m_list_references:SetColumnWidth(2, iCol2Width)
end


local function get_reference_from_idx(iIndex)
	local iTableIndex = nil
	local tReference = nil


	if iIndex>=0 then
		-- get the selected element's data
		iTableIndex = m_list_references:GetItemData(iIndex)
		if iTableIndex~=nil and iTableIndex>0 and iTableIndex<=#m_references then
			tReference = m_references[iTableIndex]
		end
	end

	return tReference
end


local function is_reference_ok(tReference)
	local fIsOk = false


	if tReference~=nil and tReference:IsValid()==true and tReference:IsUsed()==false then
		fIsOk = true
	end

	return fIsOk
end


local function reference_selected(iIndex)
	local tReference


	tReference = get_reference_from_idx(iIndex)
	m_button_ok:Enable(is_reference_ok(tReference))
end


local function on_ok()
	local lIdx
	local tReference


	-- is a reference selected?
	lIdx = m_list_references:GetNextItem(-1, wx.wxLIST_NEXT_ALL, wx.wxLIST_STATE_FOCUSED)
	tReference = get_reference_from_idx(lIdx)
	if is_reference_ok(tReference)==true then
		m_selected_reference = tReference
		m_dialog:EndModal(wx.wxID_OK)
	end
end


local function on_rescan()
	local plugin
	local list_item = wx.wxListItem()
	local item_id
	local comment


	-- Clear all entries in the list.
	m_list_references:DeleteAllItems()
	-- Clear all entries in the table.
	m_aDetectedInterfaces = {}

	-- Detect all interfaces.

	for i,v in ipairs(__MUHKUH_PLUGINS) do
		local iDetected
		local strId = v:GetID()
		if strId:match(m_strPattern) then
			print(string.format("Detecting interfaces with plugin %s", strId))
			iDetected = v:DetectInterfaces(m_aDetectedInterfaces)
			print(string.format("Found %d interfaces with plugin %s", iDetected, strId))
		else
			print(string.format("Ignoring plugin %s. It does not match the pattern.", strId))
		end
	end
	print(string.format("Found a total of %d interfaces with %d plugins", #m_aDetectedInterfaces, #__MUHKUH_PLUGINS))
	print("")

	-- Show all items in the list.
	for iIndex,tReference in pairs(m_aDetectedInterfaces) do
		if tReference:IsValid()==true then
			m_references[iIndex] = tReference
			-- append new item at the end of the list
			item_id = m_list_references:GetItemCount()
			-- create the new list item
			list_item:Clear()
			list_item:SetMask(wx.wxLIST_MASK_TEXT+wx.wxLIST_MASK_IMAGE+wx.wxLIST_MASK_DATA)
			list_item:SetId(item_id)
			list_item:SetData(iIndex)
			list_item:SetText(tReference:GetName())
			if tReference:IsUsed()==false then
				list_item:SetImage(1)
				comment = "free"
			else
				list_item:SetImage(0)
				comment = "in use"
			end
			item_id = m_list_references:InsertItem(list_item)
			m_list_references:SetItem(item_id, 1, tReference:GetTyp())
			m_list_references:SetItem(item_id, 2, comment)
		end
	end
end


local function on_item_selected(event)
	local lIdx;


	-- get the selected element
	lIdx = event:GetIndex()
	reference_selected(lIdx);
end


local function on_item_deselected(event)
	reference_selected(-1)
end


local function on_item_activated(event)
	local lIdx
	local tReference


	-- get the selected element
	lIdx = event:GetIndex()
	tReference = get_reference_from_idx(lIdx)
	if is_reference_ok(tReference)==true then
		m_selected_reference = tReference
		m_dialog:EndModal(wx.wxID_OK)
	end
end


function SelectPlugin(strPattern)
	local iInterfaceIdx
	local tPlugin


	m_strPattern       = strPattern or ".*"
	dialog_size        = dialog_size or wx.wxSize(800,200)
	dialog_iCol0Width  = dialog_iCol0Width or wx.wxLIST_AUTOSIZE
	dialog_iCol1Width  = dialog_iCol1Width or wx.wxLIST_AUTOSIZE
	dialog_iCol2Width  = dialog_iCol2Width or wx.wxLIST_AUTOSIZE

	-- Create the dialog.
	m_dialog = wx.wxDialog(wx.NULL, wx.wxID_ANY, "Select the plugin", wx.wxDefaultPosition, wx.wxDefaultSize, wx.wxDEFAULT_DIALOG_STYLE+wx.wxRESIZE_BORDER)
	-- Create dialog controls.
	create_controls(dialog_size, dialog_iCol0Width, dialog_iCol1Width, dialog_iCol2Width)

	m_dialog:Connect(wx.wxID_OK,          wx.wxEVT_COMMAND_BUTTON_CLICKED,        on_ok)
	m_dialog:Connect(ID_BUTTON_RESCAN,    wx.wxEVT_COMMAND_BUTTON_CLICKED,        on_rescan)
	m_dialog:Connect(ID_LIST_REFERENCES,  wx.wxEVT_COMMAND_LIST_ITEM_SELECTED,    on_item_selected)
	m_dialog:Connect(ID_LIST_REFERENCES,  wx.wxEVT_COMMAND_LIST_ITEM_DESELECTED,  on_item_deselected)
	m_dialog:Connect(ID_LIST_REFERENCES,  wx.wxEVT_COMMAND_LIST_ITEM_ACTIVATED,   on_item_activated)

	-- initial scan
	on_rescan()

	-- nothing selected at start
	reference_selected(-1)

	-- Show the dialog
	if m_dialog:ShowModal(true)==wx.wxID_OK then
		tPlugin = m_selected_reference:Create()
	end

	return tPlugin
end
