module("utils", package.seeall)

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
--   Execute a command and print it's output. The output is also returned.
--
-- Parameters:
--   cmd : command to execute
--
-- Returns:
--   result, text
--   result : return code from the command
--   text : output from the command
--
function runcommand(cmd)
	local outputfilename
	local text
	outputfilename = os.tmpname()

	cmd = cmd .. " 2>" .. outputfilename

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
