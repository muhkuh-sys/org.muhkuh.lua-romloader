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


local atComponentDetails = {}


function atComponentDetails:create(tParent)
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
	this.tTextTestname = wx.wxStaticText(this, wx.wxID_ANY, "NXDM10 S301 test")
	tMainSizer:Add(this.tTextTestname, 0, wx.wxALIGN_CENTER)
	tMainSizer:AddSpacer(6)


	return self
end


function atComponentDetails:event_update_style()
	local tStyle = tester.g_atConfiguration.tStyle

end


function atComponentDetails:event_update_fonts()
end


function atComponentDetails:event_update_test()
end


function atComponentDetails:event_update_test_results(uiSerialIdx, uiSingleTestIdx)
end


function atComponentDetails:event_select_test(uiSerial, uiTestIdx)
end


tester.g_atComponents["details"] = atComponentDetails

