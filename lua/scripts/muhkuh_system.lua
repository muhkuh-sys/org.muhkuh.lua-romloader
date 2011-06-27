-----------------------------------------------------------------------------
-- Copyright (C) 2011 by Christoph Thelen                               <br/>
-- <a href="mailto:doc_bacardi@users.sourceforge.net">doc_bacardi@users.sourceforge.net</a><br/>
--                                                                      <br/>
-- This program is free software; you can redistribute it and/or modify <br/>
-- it under the terms of the GNU General Public License as published by <br/>
-- the Free Software Foundation; either version 2 of the License, or    <br/>
-- (at your option) any later version.                                  <br/>
--                                                                      <br/>
-- This program is distributed in the hope that it will be useful,      <br/>
-- but WITHOUT ANY WARRANTY; without even the implied warranty of       <br/>
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        <br/>
-- GNU General Public License for more details.                         <br/>
--                                                                      <br/>
-- You should have received a copy of the GNU General Public License    <br/>
-- along with this program; if not, write to the                        <br/>
-- Free Software Foundation, Inc.,                                      <br/>
-- 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.            <br/>
-----------------------------------------------------------------------------

module("muhkuh_system", package.seeall)


m_textCtrl = nil


function catch_prints(...)
	if type(...)=="string" then
		m_textCtrl:AppendText(...)
	elseif type(...)=="table" then
		m_textCtrl:AppendText(table.concat(arg,"\t"))
--		for iCnt,strArg in ipairs(...) do
--			m_textCtrl:AppendText(strArg)
--		end
	end
	m_textCtrl:AppendText("\n")
end


function test_has_finished()
	wx.wxGetApp():ExitMainLoop()
end


local function create_window()
	local m_frame = wx.wxFrame(wx.NULL, wx.wxID_ANY, "Muhkuh - Insert Testname here", wx.wxDefaultPosition, wx.wxSize(800, 600));

	local m_auiMgr = wxaui.wxAuiManager()
	m_auiMgr:SetManagedWindow(m_frame);

	m_frame:CreateStatusBar();
	m_frame:GetStatusBar():SetStatusText("Ready")

	m_frame:SetMinSize(wx.wxSize(400,300));

	local m_panel = wx.wxPanel(m_frame, wx.wxID_ANY)
	local m_paneInfo = wxaui.wxAuiPaneInfo()
	m_paneInfo:Name("testpanel")
	m_paneInfo:CaptionVisible(false)
	m_paneInfo:Center()
	m_paneInfo:Position(0)
	m_auiMgr:AddPane(m_panel, m_paneInfo);

	local style = wx.wxTE_MULTILINE + wx.wxSUNKEN_BORDER + wx.wxTE_READONLY
	m_textCtrl = wx.wxTextCtrl(m_frame, wx.wxID_ANY, "", wx.wxDefaultPosition, wx.wxDefaultSize, style)
	m_paneInfo:Name("message_log")
	m_paneInfo:CaptionVisible(true)
	m_paneInfo:Caption("Message Log")
	m_paneInfo:Bottom()
	m_paneInfo:Position(0)
	m_auiMgr:AddPane(m_textCtrl, m_paneInfo)

	m_auiMgr:Update();

	m_frame:Show(true)

	_G.print = catch_prints
	print("Hallo!")

	muhkuh.TestHasFinished = test_has_finished

	_G.__MUHKUH_PANEL = m_panel
	-- save this layout as the default perspective
	-- m_strDefaultPerspective = m_auiMgr.SavePerspective();
end


-----------------------------------------------------------------------------
-- @description Boot a test from a xml description.
--
function boot_xml()
	local alltests = nil


	-- create the window
	create_window()

	dofile("test_description_0_par.lua")
	dofile("test_description_0_code.lua")

	wx.wxGetApp():MainLoop()
end

