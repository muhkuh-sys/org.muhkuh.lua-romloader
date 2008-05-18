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

module("muhkuh_system", package.seeall)


-----------------------------------------------------------------------------
-- parse_code
--   Find the "Code" child of a node and return the contents as a string.
--   The "Code" node is mandatory.
--
-- Parameter:
--   parent_node : node containing the "Code" child
--
-- Returns:
--   string with code node contents on success
--   nil on error
--
local function parse_code(parent_node)
	local node
	local foundNode
	local code


	-- look for the first node named "Code"
	node = parent_node:GetChildren()
	foundNode = nil
	while node~=nil
	do
		if node:GetType()==wx.wxXML_ELEMENT_NODE and node:GetName()=="Code" then
			foundNode = node
			break
		end
		node = node:GetNext()
	end

	-- not found (node is nil) ?
	if not foundNode then
		print("muhkuh_system error: code node not found!")
		return nil
	end

	-- get the code
	code = foundNode:GetChildren():GetContent()

	return code
end


-----------------------------------------------------------------------------
-- parse_parameters
--   Find and parse all "Parameter" child of a node and return the contents as a string.
--   "Parameter" nodes are optional.
--
-- Parameter:
--   parent_node : node containing the "Parameter" children
--
-- Returns:
--   table with the parameter names as keys and the parameter values as values
--   nil on error
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
			strParameterValue = node:GetChildren():GetContent()
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
-- parse_test
--   Parse a "Test" node.
--
-- Parameter:
--   node : The "Test" node.
--
-- Returns:
--   table with name, version, code and parameters
--   nil on error
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


-----------------------------------------------------------------------------
-- parse_xml
--   Parse a complete xml file
--
-- Parameter:
--   -
--
-- Returns:
--   table with all tests
--   nil on error
--
local function parse_xml()
	local rootNode
	local node
	local foundNode
	local test
	local tests = {}


	-- look for the first root node named "TestDescription"
	rootNode = __MUHKUH_TEST_XML:GetRoot()
	node = rootNode
	foundNode = nil
	while node~=nil
	do
		if node:GetType()==wx.wxXML_ELEMENT_NODE and node:GetName()=="TestDescription" then
			foundNode = node
			break
		end
		node = node:GetNext()
	end

	-- not found (node is nil) ?
	if not foundNode then
		print("muhkuh_system error: no TestDescription node found")
		return nil
	end

	-- parse the test description
	test = parse_test(foundNode)
	if not test then
		print("muhkuh_system error: failed to get TestDescription code")
		return nil
	end
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

	return tests
end


-----------------------------------------------------------------------------
-- boot_xml
--   Boot a test from a xml description.
--
-- Parameter:
--   -
--
-- Returns:
--   -
--
function boot_xml()
	local alltests = nil


	alltests = parse_xml()
	if not alltests then
		wx.wxMessageBox("Failed to parse the XML file. Please send the log to the developer.", "Parse Error", wx.wxOK+wx.wxICON_ERROR, wx.NULL)
		return
	end


	-- set the global variable
	_G.__MUHKUH_ALL_TESTS = alltests
	-- run the main code
	assert(loadstring(alltests[1].code))()
end

