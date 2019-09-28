--lua script language

--file to read
local image = "cosem"
--file to write to
local system = "lexicon"

print(string.format("load image <%s> to system memory.", image))

local file = io.open(image, "rb")
local offset = 0
io.input(file)

repeat
	local data = io.read(96)
	if(data ~= nil)
	then
		basis.file.write(system, offset, 96, data)
		offset = offset + 96
	end
until(data == nil)

print(string.format("write %d bytes to system memory.", offset))

io.close(file)