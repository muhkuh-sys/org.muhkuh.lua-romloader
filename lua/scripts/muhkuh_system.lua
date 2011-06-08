-----------------------------------------------------------------------------
-- Copyright (C) 2008 by Christoph Thelen                               <br/>
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


-----------------------------------------------------------------------------
-- @description Get the first child of a node with the name 'strName'.
--
-- @param node search this xml node's children
-- @param strName name of the child node (note: no wildcarts or regexp, it's the plain name)
--
-- @return xml node of the first matching child node or <code>nil</code> if no matching node could be found
--
local function get_matching_node(node, strName)
	local xml_iter
	local xml_found = nil


	xml_iter = node
	-- loop over all nodes
	while xml_iter~=nil do
		if xml_iter:GetType()==wx.wxXML_ELEMENT_NODE and xml_iter:GetName()==strName then
			xml_found = xml_iter
			break
		else
			xml_iter = xml_iter:GetNext()
		end
	end

	-- return the node or nil
	return xml_found
end

-----------------------------------------------------------------------------
-- @description Get the contents of the first child textnode.
--
-- @param node search this xml node's children
--
-- @return Textnode's contents as string, <code>nil</code> on error
--
local function get_node_contents(node, strName)
	local xml_iter
	local xml_found = nil
	local typ
	local strText = nil


	xml_iter = node:GetChildren()
	-- loop over all nodes
	while xml_iter~=nil do
		typ = xml_iter:GetType()
		if typ==wx.wxXML_TEXT_NODE or typ==wx.wxXML_CDATA_SECTION_NODE then
			strText = xml_iter:GetContent()
			break
		else
			xml_iter = xml_iter:GetNext()
		end
	end

	-- return the node or nil
	return strText
end


-----------------------------------------------------------------------------
-- @description Find the "Code" child of a node and return the contents as a string. The "Code" node is mandatory.
--
-- @param parent_node node containing the "Code" child
--
-- @return string with code node contents on success, <code>nil</code> on error
--
local function parse_code(parent_node)
	local foundNode
	local code


	-- look for the first node named "Code"
	foundNode = get_matching_node(parent_node:GetChildren(), "Code")

	-- found node?
	if not foundNode then
		-- no
		print("muhkuh_system error: code node not found!")
	else
		-- yes -> get the code
		code = get_node_contents(foundNode)
	end

	return code
end


-----------------------------------------------------------------------------
-- @description Find and parse all "Parameter" child of a node and return the contents as a string. "Parameter" nodes are optional.
--
-- @param parent_node node containing the "Parameter" children
--
-- @return table with the parameter names as keys and the parameter values as values, <code>nil</code> on error
--
local function parse_parameters(parent_node)
	local parameters = {}
	local node
	local foundNode
	local strParameterName
	local strParameterTyp
	local strParameterValue
	local iParStart
	local iParEnd


	-- get all parameters
	node = parent_node:GetChildren()
	while node~=nil
	do
		if node:GetType()==wx.wxXML_ELEMENT_NODE and node:GetName()=="Parameter" then
			-- get parameter name
			strParameterName = node:GetPropVal("name", "")
			if strParameterName==nil or strParameterName=="" then
				print("muhkuh_system error: Parameter node has no name attribute")
				parameters = nil
				break
			end
			-- get parameter value
			strParameterValue = get_node_contents(node)
			if strParameterValue==nil or strParameterValue=="" then
				print("muhkuh_system error: Parameter node has no value")
				parameters = nil
				break
			end
			iParStart = string.find(strParameterValue, "[^ \t\n\r]", 1, false)
			iParEnd = string.find(string.reverse(strParameterValue), "[^ \t\n\r]", 1, false)

			-- add parameter to table
			if parameters[strParameterName] then
				print("muhkuh_system warning: overwriting old value of '"..strParameterName.."'")
			end
			parameters[strParameterName] = string.sub(strParameterValue, iParStart, -iParEnd)
		end

		node = node:GetNext()
	end

	return parameters
end


-----------------------------------------------------------------------------
-- @description Parse a "Test" node.
--
-- @param node The "Test" node.
--
-- @return table with name, version, code and parameters, <code>nil</code> on error
--
local function parse_test(node)
	local strName
	local strVersion
	local strCode
	local aParams


	-- get the name
	strName = node:GetPropVal("name", "")
	if strName=="" then
		print("muhkuh_system error: missing name attribute")
		return nil
	end

	-- get the version
	strVersion = node:GetPropVal("version", "")
	if strVersion=="" then
		print("muhkuh_system error: missing version attribute")
		return nil
	end

	-- parse the code
	strCode = parse_code(node)
	if not strCode then
		print("muhkuh_system error: failed to read the code section")
		return nil
	end

	-- parse the parameters
	aParams = parse_parameters(node)
	if not aParams then
		print("muhkuh_system error: failed to read the parameters")
		return nil
	end

	return { name=strName, version=strVersion, code=strCode, parameter=aParams }
end







local function get_imported_startup_code(node)
	local strName
	local strVersion
	local strCode = [[
		require("serialnr")
		require("tester")
		if serialnr.run(20000, 10)==false then
			-- cancel
			muhkuh.TestHasFinished()
		end
		tester.run()
			]]


	-- get the name
	strName = node:GetPropVal("name", "")
	if strName=="" then
		print("muhkuh_system error: missing name attribute")
		return nil
	end

	-- get the version
	strVersion = node:GetPropVal("version", "")
	if strVersion=="" then
		print("muhkuh_system error: missing version attribute")
		return nil
	end

	return { name=strName, version=strVersion, code=strCode, parameter={} }
end


local translate_cmd =
{
	{ s = "fill (%x+) (%x+) byte",		r = "\t\t\tplugin:write_data08(0x%1, 0x%2)"},
	{ s = "fill (%x+) (%x+) word",		r = "\t\t\tplugin:write_data16(0x%1, 0x%2)"},
	{ s = "fill (%x+) (%x+) long",		r = "\t\t\tplugin:write_data32(0x%1, 0x%2)"},
	{ s = "fill (%x+) (%x+)",		r = "\t\t\tplugin:write_data32(0x%1, 0x%2)"},

	{ s = "dump (%x+)",			r = "\t\t\tulValue = plugin:read_data32(0x%1)"},

	{ s = "!l ([^ ]+) (%x+)",		r = "\t\t\tbin = muhkuh.load(\"%1\")\n\t\t\ttester.stdWrite(panel, plugin, 0x%2, bin)"},

	{ s = "call (%x+) (%x+)",		r = "\t\t\ttester.stdCall(panel, plugin, 0x%1, 0x%2)"}
}

local function translate_step_cmd(parent_node, aCode)
	local fResult
	local foundNode
	local strCmd
	local strCode
	local iMatches


	fResult = nil

	-- look for the first node named "Cmd"
	foundNode = get_matching_node(parent_node:GetChildren(), "Cmd")

	-- found node?
	if not foundNode then
		-- no
		print("muhkuh_system error: Cmd node not found!")
	else
		-- yes -> get the command
		strCmd = get_node_contents(foundNode)

		-- translate the command
		for i,p in pairs(translate_cmd) do
			strCode, iMatches = string.gsub(strCmd, p.s, p.r)
			if iMatches>0 then
				break
			end
		end
		if iMatches<=0 then
			print("muhkuh_system error: unknown command: " .. strCmd)
		else
			table.insert(aCode, strCode)
			fResult = true
		end
	end

	return fResult
end


local function translate_step_response(parent_node, aCode)
	local fResult
	local node
	local responseNode
	local foundNode
	local strCanAbort
	local fCanAbort
	local strText
	local strMatch
	local iMatch


	fResult = nil

	-- look for the first node named "Response"
	responseNode = get_matching_node(parent_node:GetChildren(), "Response")

	-- not found (node is nil) ?
	if responseNode then
		-- look for the first node named "RegExp"
		foundNode = get_matching_node(responseNode:GetChildren(), "RegExp")
		if not foundNode then
			print("muhkuh_system error: Response node has no RegExp!")
			return nil
		end

		-- check retpos attribute for default value
		strVal = foundNode:GetPropVal("retpos", "1")
		if strVal~="1" then
			print("muhkuh_system error: strange retpos in RegExp: " .. strVal)
			return nil
		end

		strVal = get_node_contents(foundNode)
		if not strVal:match("%^%[0%-9a%-fA%-F%]%{8%}%[ \\t%]%*\\:%[ \\t%]%*%(%[0%-9a%-fA%-F%]%{8%}%)") then
			print("muhkuh_system error: strange regular expression in RegExp: '" .. strVal .. "'")
			return nil
		end


		-- look for the first node named "Match"
		foundNode = get_matching_node(responseNode:GetChildren(), "Match")
		if not foundNode then
			print("muhkuh_system error: Response node has no Match!")
			return nil
		end

		strVal = get_node_contents(foundNode)
		if not strVal then
			print("muhkuh_system error: Match node has no data!")
			return nil
		end

		strMatch = strVal:match("(%x+)")
		if not strMatch then
			print("muhkuh_system error: strange value in Match: " .. strVal)
			return nil
		end

		iMatch = tonumber(strMatch, 16)
		if not iMatch then
			print("muhkuh_system error: strange value in Match: " .. strVal)
			return nil
		end

		-- generate code
		table.insert(aCode, string.format("if ulValue~=0x%08X then", iMatch))
		table.insert(aCode, "\treturn __MUHKUH_TEST_RESULT_FAIL")
		table.insert(aCode, "end")
	end

	return true
end


local function translate_step_interaction(parent_node, aCode)
	local fResult
	local node
	local foundNode
	local strCanAbort
	local fCanAbort
	local strText


	fResult = nil

	-- look for the first node named "Interaction"
	foundNode = get_matching_node(parent_node:GetChildren(), "Interaction")
	-- found node?
	if foundNode then
		-- yes -> can the interaction cancel the test?
		strCanAbort = foundNode:GetPropVal("Abort", "0")
		if strCanAbort=="1" then
			fCanAbort = true
		else
			fCanAbort = false
		end

		-- get the command
		strText = get_node_contents(foundNode)

		-- generate code
		table.insert(aCode, "strMsg = [["..strText.."]]")
		table.insert(aCode, "iResult = wx.wxMessageBox(strMsg, \"Interaction required\", wx.wxOK + wx.wxCANCEL + wx.wxICON_INFORMATION, panel)")
		if fCanAbort then
			table.insert(aCode, "if iResult==wx.wxCANCEL then")
			table.insert(aCode, "\tprint(\"user pressed cancel\")")
			table.insert(aCode, "\treturn __MUHKUH_TEST_RESULT_FAIL")
			table.insert(aCode, "end")
		end
	end

	return true
end


local function translate_step(parent_node, aCode)
	local fResult


	-- translate the command
	fResult = translate_step_cmd(parent_node, aCode)
	if not fResult then
		print("muhkuh_system error: failed to translate command")
	else
		-- translate the response
		fResult = translate_step_response(parent_node, aCode)
		if not fResult then
			print("muhkuh_system error: failed to translate response")
		else
			-- translate the interaction
			fResult = translate_step_interaction(parent_node, aCode)
			if not fResult then
				print("muhkuh_system error: failed to translate interaction")
			end
		end
	end

	return fResult
end


local function import_steps(parent_node)
	local aCode = {}
	local node
	local strAllCode


	-- start with common code
	table.insert(aCode, "\t\t\tlocal plugin")
	table.insert(aCode, "\t\t\tlocal panel")
	table.insert(aCode, "\t\t\tlocal ulValue")
	table.insert(aCode, "\t\t\tlocal iResult")
	table.insert(aCode, "\t\t\tlocal strMsg")
	table.insert(aCode, "")
	table.insert(aCode, "")
	table.insert(aCode, "\t\t\tpanel = tester.getPanel()")
	table.insert(aCode, "")
	table.insert(aCode, "\t\t\tplugin = tester.getCommonPlugin()")
	table.insert(aCode, "\t\t\tif not plugin then")
	table.insert(aCode, "\t\t\t\tprint(\"no netx selected, test canceled!\")")
	table.insert(aCode, "\t\t\t\treturn __MUHKUH_TEST_RESULT_CANCEL")
	table.insert(aCode, "\t\t\tend")

	-- get all parameters
	node = parent_node:GetChildren()
	while node~=nil
	do
		if node:GetType()==wx.wxXML_ELEMENT_NODE and node:GetName()=="Step" then
			if not translate_step(node, aCode) then
				print("muhkuh_system error: failed to translate step")
				return nil
			end
		end

		node = node:GetNext()
	end

	-- add exit code
	table.insert(aCode, "\treturn __MUHKUH_TEST_RESULT_OK")

	-- concat all table entries
	strAllCode = table.concat(aCode, "\n")

	print("muhkuh_system: complete code:")
	print(strAllCode)

	return strAllCode
end


-----------------------------------------------------------------------------
-- @description Import a subtest with old xml command syntax.
--
-- @param node Xml node of the old test description.
-- @return table with the converted subtest, <code>nil</code> on error
--
local function import_old_test(node)
	local strName
	local strVersion
	local strCode


	-- get the name
	strName = node:GetPropVal("name", "")
	if strName=="" then
		print("muhkuh_system error: missing name attribute")
		return nil
	end

	-- get the version
	strVersion = node:GetPropVal("version", "")
	if strVersion=="" then
		print("muhkuh_system warning: missing version attribute")
	end

	-- import the old steps
	strCode = import_steps(node)
	if not strCode then
		print("muhkuh_system error: failed to import the steps")
		return nil
	end

	return { name=strName, version=strVersion, code=strCode, parameter={} }
end


-----------------------------------------------------------------------------
-- @description Parse a complete xml file
--
-- @return table with all tests, <code>nil</code> on error
--
local function parse_xml()
	local tXmlDocument
	local rootNode
	local foundNode
	local test
	local tests = {}


	-- load the xml file
	local strFile = muhkuh.load(__MUHKUH_TEST_XML)

	-- create a local temp file
	local tTmpName = wx.wxFileName()
	tTmpName:AssignTempFileName("muhkuh_system")
	if not tTmpName:IsOk() then
		print("muhkuh_system_error: failed to create a temp file.")
		return nil
	end
	local strTmpFileName = tTmpName:GetFullPath()
	local tFile = wx.wxFile()
	if not tFile:Create(strTmpFileName, true) then
		print("muhkuh_system_error: failed to create temp file '" .. strTmpFileName .. "'")
		return nil
	end
	tFile:Write(strFile)
	tFile:Close()

	tXmlDocument = wx.wxXmlDocument()
	if not tXmlDocument:Load(strTmpFileName) then
		wx.wxRemoveFile(strTmpFileName)
		print("muhkuh_system_error: failed to parse xml file '" .. __MUHKUH_TEST_XML .. "'")
		return nil
	end

	-- remove the temp file
	wx.wxRemoveFile(strTmpFileName)

	-- look for the first root node named "TestDescription"
	rootNode = tXmlDocument:GetRoot()
	foundNode = get_matching_node(rootNode, "TestDescription")
	-- found node?
	if not foundNode then
		print("muhkuh_system error: no TestDescription node found")
		return nil
	end

	-- parse the TestDescription Code node
	test = parse_test(foundNode)
	if test then
		table.insert(tests, test)

		-- get all tests
		node = rootNode:GetChildren()
		while node~=nil
		do
			if node:GetType()==wx.wxXML_ELEMENT_NODE and node:GetName()=="Test" then
				test = parse_test(node)
				if not test then
					print("muhkuh_system error: failed to read Test node")
					return nil
				end

				table.insert(tests, test)
			end
			node = node:GetNext()
		end
	else
		-- failed to get code node, is this a test with old syntax?
		print("muhkuh_system error: failed to get TestDescription code, trying to import old syntax")

		-- get the startup code for imported tests
		test = get_imported_startup_code(foundNode)
		table.insert(tests, test)

		-- get all tests
		node = rootNode:GetChildren()
		while node~=nil
		do
			if node:GetType()==wx.wxXML_ELEMENT_NODE and node:GetName()=="Test" then
				test = import_old_test(node)
				if not test then
					print("muhkuh_system error: failed to import old Test node")
					return nil
				end

				table.insert(tests, test)
			end
			node = node:GetNext()
		end
	end

	return tests
end


function catch_prints(...)
	m_textCtrl:AppendText(table.concat(arg,"\t"))
	m_textCtrl:AppendText("\n")
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


	alltests = parse_xml()
	if not alltests then
		wx.wxMessageBox("Failed to parse the XML file. Please send the log to the developer.", "Parse Error", wx.wxOK+wx.wxICON_ERROR, wx.NULL)
	else
		-- set the global variable
		_G.__MUHKUH_ALL_TESTS = alltests
		-- run the main code
		assert(loadstring(alltests[1].code, "@@0"))()
	end

	wx.wxGetApp():MainLoop()
end

