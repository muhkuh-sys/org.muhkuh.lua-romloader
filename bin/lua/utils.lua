-----------------------------------------------------------------------------
--   Copyright (C) 2008 by Stephan Lesch                                   --
--   Stephan_Lesch@users.sourceforge.net                                   --
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
module("utils", package.seeall)

-- trace print command 
m_fVerbose = true
function tprint(...)
	if m_fVerbose then 
		print(...)
	end
end

---------------------------------------
-- getlocalfile
--   Read a (possibly remote) file with muhkuh.load and write it to a temporary local file.
--
-- Parameters:
--   name : source filename
--   suffix : optional suffix for the temporary file
--
-- Returns:
--   the filename of the temporary file or nil on error
--
function getlocalfile(name, suffix)
	local tmpfilename = os.tmpname()
	local bin
	local filehandle

	if suffix then 
		tmpfilename=tmpfilename .. "." .. suffix
		tprint("---" .. tmpfilename .. "---")
	end

	bin = muhkuh.load(name)
	if bin:len() == 0 then
		print("Failed to load file "..name)
		return nil
	end

	filehandle = io.open(tmpfilename, "wb")
	if not filehandle then
		print("Failed to open temp file for exe")
		return nil
	end
	filehandle:write(bin)
	filehandle:close()

	return tmpfilename
end


---------------------------------------
-- runcommand
--   Execute a command and print its output. The output is also returned.
--
-- Parameters:
--   cmd : command to execute
--
-- Returns:
--   result, text
--   result : return code from the command
--   text : output from the command

function runcommand(cmd)
	tprint("Running command: ", cmd)
	local lRet, astrOutput, astrErrors = wx.wxExecuteStdoutStderr(cmd, wx.wxEXEC_SYNC)
	local strOutput = astrOutput and table.concat(astrOutput, "\n") or ""
	local strErrors = astrErrors and table.concat(astrErrors, "\n") or ""
	local strOutput = strOutput .. strErrors
	tprint("returncode " .. lRet)
	tprint("Output:")
	tprint(strOutput)
	return lRet, strOutput
end
--[=[
function runcommand(cmd)
	local outputfilename
	local text
	outputfilename = os.tmpname()

	--cmd = cmd .. " >" .. outputfilename .. " 2>&1"
	cmd = string.format([["%s >"%s" 2>&1"]], cmd, outputfilename)
	print("Running command: ", cmd)

	result = os.execute(cmd)
	print("returncode " .. result)

	-- read and print output file
	filehandle = io.open(outputfilename, "r")
	if not filehandle then
		print("Failed to open logfile!")
	else
		text = filehandle:read("*all")
		print("Output:")
		print(text)
		filehandle:close()
	end

	os.remove(outputfilename)
	return result, text
end
--]=]
----------------------------------------------------------------------------
-- run a local copy of a command located in the test directory
-- returns either nil, error message 
-- or return value, output, error output

function runLocalCopy(cmd, suffix, args)
	local exetmpfile = utils.getlocalfile(cmd, suffix)
	if not exetmpfile then
		local msg = "file " .. cmd .. " not found"
		tprint(msg)
		return nil, msg
	end	
	local retVal, strOutput = utils.runcommand(exetmpfile ..  " " .. args)
	os.remove(exetmpfile)
	return retVal, strOutput
end



---------------------------------------

-- read binary file into string
-- returns the file or nil, message

function loadBin(strName)
	tprint("reading file " .. strName)
	local f = wx.wxFile(strName)
	if not f:IsOpened() then 
		return nil, "Cannot open file " .. strName 
	end
	
	local iLen = f:Length()
	local iBytesRead, bin = f:Read(iLen)
	f:Close()
	tprint(iBytesRead .. " bytes read")
	if iBytesRead ~= iLen then
		local msg = "Error reading file " .. strName
		tprint(msg)
		return nil, msg
	else
		return bin
	end
end

--[[
function loadBin(strName)
	print("reading file " .. strName)
	local bin
	local f, msg = io.open(strName, "rb")
	if f then
		bin = f:read("*all")
		f:close()
		print(bin:len() .. " bytes read")
		return bin
	else
		return nil, msg
	end
end
--]]

-- write binary file into string
-- returns true or false, message
function writeBin(strName, bin, accessMode)
	tprint("writing to file " .. strName)
	local f = wx.wxFile(strName, accessMode or wx.wxFile.write)
	if not f:IsOpened() then 
		return false, "Error opening file " .. strName .. " for writing"
	end

	local iBytesWritten = f:Write(bin, bin:len())
	f:Close()
	tprint(iBytesWritten .. " bytes written")
	if iBytesWritten ~= bin:len() then
		msg = "Error while writing to file " .. strName
		tprint(msg)
		return false, msg
	else
		return true
	end
end

--[[
function writeBin(strName, bin)
	local f, msg = io.open(strName, "wb")
	if f then 
		f:write(bin)
		f:close()
		return true
	else
		print("Failed to open file for writing")
		return false, msg
	end
end
--]]


-- Append string to the end of the file.
-- Creates the file if it does not exist.
function appendBin(strName, bin)
	return writeBin(strName, bin, wx.wxFile.write_append) 
end



-- Remove all files in a directory, but not the directory itself
-- returns true or false and an error message
function removeDirFiles(dir)
	return removeDir(dir, false)
end

-- Remove all files in dir,
-- then remove the directory itself, unless fRmDir==false
-- returns true or false and an error message
function removeDir(strDirName, fRmDir)
	local dir = wx.wxDir(strDirName)
	if not dir:IsOpened() then
		return false, "could not open directory" .. strDirName
	end
	
	local filename = wx.wxFileName()
	filename:AssignDir(strDirName)
	local strFullPath
	
	local fFile, strName = dir:GetFirst()
	while fFile do
		filename:SetFullName(strName)
		strFullPath = filename:GetFullPath()
		tprint("removing file: " .. strFullPath)
		if not wx.wxRemoveFile(strFullPath) then
			return false, "error removing file: "..strFullPath
		end
		fFile, strName = dir:GetNext()
	end
	
	if fRmDir ~= false then
		-- remove handle to directory
		dir = nil
		collectgarbage("collect")
		tprint("removing directory " .. strDirName)
		if wx.wxRmdir(strDirName) then
			tprint("OK")
		else
			return false, "failed to remove dir: " .. strDirName
		end
	end
	
	return true
end


---------------------------------------
-- load_localfile_lines
-- gets a local copy of a text file, reads its lines and removes the local file

function load_localfile_lines(strFilename)
	tprint("getting local copy of file "..strFilename)
	local lines = {}
	local strLocalfile = utils.getlocalfile(strFilename)
	if not strLocalfile then
		print("failed to get a local copy of " .. strFilename)
	else
		tprint("reading local copy: "..strLocalfile)
		for line in io.lines(strLocalfile) do
			lines[#lines+1]=line
		end
		local res, msg = os.remove(strLocalfile)
		if not res then print(msg) end
	end
	tprint(#lines .. " lines read")
	return lines
end


----------------------------------------------------------------------------
-- create temp dir in the system temp directory.
-- strDirName the name to use for the directory. Default is muhkuh_yyyymmdd_hhmmss
-- If the directory exists, dirname_1, dirname_2 etc. are tried.

function createTempDir(strDirName)
	
	local tempfile = wx.wxFileName()
	local standardPaths = wx.wxStandardPaths.Get()
	local strTmpDir = standardPaths:GetTempDir()
	strDirName = strDirName or os.date("muhkuh_%Y%m%d_%H%M%S")
	
	tempfile:AssignDir(strTmpDir)
	tempfile:AppendDir(strDirName)
	local cnt = 2
	while not tempfile:Mkdir() do
		tempfile:AssignDir(strTmpDir)
		tempfile:AppendDir(strDirName .. "_" .. cnt)
		cnt = cnt+1
	end
	tprint("tempdir: "..tempfile:GetFullPath())
	
	-- If the directory is not readable/writeable although
	-- we just created it, there is a problem with the 
	-- access privileges
	if not tempfile:IsDirWritable() then
		return false, "dir is not writable"
	end
	
	if not tempfile:IsDirReadable() then
		return false, "dir is not readable"
	end
	
	m_tTempFileName = tempfile
	return true
end


function createTempDir_old()
	local tempfile = wx.wxFileName()
	tempfile:AssignTempFileName("muhkuh_report")
	tprint("tempfile: "..tempfile:GetFullPath())
	
	if not wx.wxRemoveFile(tempfile:GetFullPath()) then
		return false, "failed to remove tmp file"
	end
	
	tempfile:AppendDir(tempfile:GetName())
	tempfile:SetName("")
	tempfile:ClearExt()
	tprint("tempdir: "..tempfile:GetFullPath())
	
	if not tempfile:Mkdir() then
		return false, "failed to make dir"
	end
	
	if not tempfile:IsDirWritable() then
		return false, "dir is not writable"
	end
	
	if not tempfile:IsDirReadable() then
		return false, "dir is not readable"
	end
	
	m_tTempFileName = tempfile
	return true
end

----------------------------------------------------------------------------
-- delete temp dir and all files in it
-- returns true or false and an error message
function removeTempDir()
	local strDirName = m_tTempFileName:GetPath(wx.wxPATH_GET_VOLUME)
	local fOk, strError = removeDir(strDirName)
	if fOk then m_tTempFileName = nil end
	return fOk, strError
end

function removeTempDir_old()
	local strDirName = m_tTempFileName:GetPath(wx.wxPATH_GET_VOLUME)
	local dir = wx.wxDir(strDirName)
	if not dir:IsOpened() then
		return false, "could not open temp directory" .. strDirName
	end
	
	local strFullPath
	local fFile, strName = dir:GetFirst()
	while fFile do
		m_tTempFileName:SetFullName(strName)
		strFullPath = m_tTempFileName:GetFullPath()
		tprint("removing file: " .. strFullPath)
		if not wx.wxRemoveFile(strFullPath) then
			return false, "error removing file"..strFullPath
		end
		fFile, strName = dir:GetNext()
	end
	
	dir = nil
	m_tTempFileName = nil
	collectgarbage("collect")
	
	tprint("removing temp dir " .. strDirName)
	if wx.wxRmdir(strDirName) then
		tprint("OK")
	else
		return false, "failed to remove dir" .. strDirName
	end
	return true
end

----------------------------------------------------------------------------
-- return the wxFileName object pointing to the temp file (which must exist)

function getTempFileName(strName, strExt)
	m_tTempFileName:SetName(strName)
	m_tTempFileName:SetExt(strExt)
	return m_tTempFileName
end

----------------------------------------------------------------------------
-- store a string in a tmp file
-- returns true or false and an error message

function writeTempFile(strName, strExt, strFileContent)
	m_tTempFileName:SetName(strName)
	m_tTempFileName:SetExt(strExt)
	local strName = m_tTempFileName:GetFullPath()
	return utils.writeBin(strName, strFileContent)
end

----------------------------------------------------------------------------
-- append to the file
-- returns true or false and an error message

function appendTempFile(strName, strExt, strFileContent)
	m_tTempFileName:SetName(strName)
	m_tTempFileName:SetExt(strExt)
	local strName = m_tTempFileName:GetFullPath()
	return utils.appendBin(strName, strFileContent)
end

----------------------------------------------------------------------------
-- load tmp file into a string
-- returns contents of the file or nil plus an error message
function readTempFile(strName, strExt)
	m_tTempFileName:SetName(strName)
	m_tTempFileName:SetExt(strExt)
	local strName = m_tTempFileName:GetFullPath()
	return utils.loadBin(strName)
end

----------------------------------------------------------------------------
-- check whether the temp file exists
function tempFileExists(strName, strExt)
	m_tTempFileName:SetName(strName)
	m_tTempFileName:SetExt(strExt)
	return m_tTempFileName:FileExists()
end

----------------------------------------------------------------------------
-- delete temp file if it exists
-- returns true or false and an error message

function deleteTempFile(strName, strExt)
	m_tTempFileName:SetName(strName)
	m_tTempFileName:SetExt(strExt)
	if m_tTempFileName:FileExists() then
		local strFullPath = m_tTempFileName:GetFullPath()
		tprint("removing file: " .. strFullPath)
		if not wx.wxRemoveFile(strFullPath) then
			return nil, "error removing file" .. strFullPath
		end
		fFile, strName = dir:GetNext()
	end
end


----------------------------------------------------------------------------
-- generate a single line of hexdump
function hexDumpLine(abData, iPos, iBytes, strAddrFormat, strByteSep, iHexDataLen, fAscii)
	iPos = iPos or 0
	
	local strAddr = ""
	if strAddrFormat then
		strAddr = string.format(strAddrFormat, iPos)
	end
	
	local strHex = ""
	local strChars = ""
	
	for iXPos = 1, iBytes do
		bByte = abData:byte(iPos + iXPos)
		if bByte then
			strHex = strHex..string.format("%02x", bByte)
			if iXPos<iBytes then
				strHex = strHex..strByteSep
			end
			strChars = strChars.. (bByte>=32 and string.char(bByte) or ".")
		end
	end
	
	if fAscii then
		return strAddr .. padString(strHex, iHexDataLen, " ") .. " " .. strChars
	else
		return strAddr .. strHex
	end
end

----------------------------------------------------------------------------
-- pad string str up to length iLen with character cPad
function padString(str, iLen, cPad)
	if str:len()<iLen then
		return str .. string.rep(" ", iLen-str:len())
	else 
		return str
	end
end

----------------------------------------------------------------------------
-- print hexdump or generate as a string
-- abData          the data to be dumped
-- strOut          if non-nil, hexdump is appended to this string
--                 if nil, hexdump is printed
-- strAddrFormat   format string for the address
-- iBytePerLine    bytes per line, default is to print the whole data in one line
-- fAscii          if true, a character representation is printed next to the hex dump
-- strByteSep      separator for the bytes in the hex dump, default is a space
function doHexDump(abData, strOut, strAddrFormat, iBytesPerLine, fAscii, strByteSep)
	local strAddrFormat = strAddrFormat or ""
	local iBytesPerLine = iBytesPerLine or abData:len()
	local iHexDataLen = iBytesPerLine * 3 - 1
	local strByteSeparatorChar = strByteSeparatorChar or " "
	
	local iTotalLen = abData:len()
	for iLinePos=0, iTotalLen-1, iBytesPerLine do
		local strLine = hexDumpLine(abData, iLinePos, iBytesPerLine, 
			strAddrFormat, strByteSeparatorChar, iHexDataLen, fShowAscii)
		if strOut == nil then
			print(strLine)
		elseif strOut == "" then
			strOut = strLine
		else
			strOut = strOut .. "\n" .. strLine
		end
	end
	return strOut
end

----------------------------------------------------------------------------
-- print the array abData as a hexdump
function printHex(abData, strAddrFormat, iBytesPerLine, fShowAscii, strByteSeparatorChar)
	doHexDump(abData, nil, strAddrFormat, iBytesPerLine, fShowAscii, strByteSeparatorChar)
end

----------------------------------------------------------------------------
-- construct hexdump as a string
function hexString(abData, strAddrFormat, iBytesPerLine, fShowAscii, strByteSeparatorChar)
	return doHexDump(abData, "", strAddrFormat, iBytesPerLine, fShowAscii, strByteSeparatorChar)
end

