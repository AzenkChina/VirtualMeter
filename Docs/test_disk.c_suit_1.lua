--lua script language

print("tested file name: information")

--测试 information 文件
function test_file_information_case_1()
	print("test case 1")
	print(string.format("file size is %d", basis.file.parameter.size("information")))
end

--测试 information 文件
function test_file_information_case_2()
	print("test case 2")

	local strtest = "test string 01234567890 abcdefgh"
	local offset = 7
	local count = string.len(strtest)

	print(string.format("read write at %d count %d", offset, count))
	local strorig = basis.file.parameter.read("information", offset, count)

	if(basis.file.parameter.write("information", offset, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.parameter.read("information", offset, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end

	if(basis.file.parameter.write("information", offset, count, strorig) ~= count)
	then
		print("test recovery write FAILD")
	else
		local recovery = basis.file.parameter.read("information", offset, count)
		if(recovery ~= strorig)
		then
			print("test recovery compare FAILD")
		else
			print("test recovery compare PASS")
		end
	end
end

--测试 information 文件
function test_file_information_case_3()
	print("test case 3")

	local strtest = "test string 01234567890 abcdefgh"
	local offset = 62
	local count = string.len(strtest)

	print(string.format("read write at %d count %d", offset, count))
	local strorig = basis.file.parameter.read("information", offset, count)

	if(basis.file.parameter.write("information", offset, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.parameter.read("information", offset, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end

	if(basis.file.parameter.write("information", offset, count, strorig) ~= count)
	then
		print("test recovery write FAILD")
	else
		local recovery = basis.file.parameter.read("information", offset, count)
		if(recovery ~= strorig)
		then
			print("test recovery compare FAILD")
		else
			print("test recovery compare PASS")
		end
	end
end

--测试 information 文件
function test_file_information_case_4()
	print("test case 4")

	local strtest = "test string 01234567890 abcdefgh"
	local offset = basis.file.parameter.size("information") - string.len(strtest)
	local count = string.len(strtest)

	print(string.format("read write at %d count %d", offset, count))
	local strorig = basis.file.parameter.read("information", offset, count)

	if(basis.file.parameter.write("information", offset, count, strtest) ~= string.len(strtest))
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.parameter.read("information", offset, count)

	if(string.len(stread) ~= string.len(strtest))
	then
		print("test read FAILD")
	else
		if(string.find(strtest, stread, 1) ~= 1)
		then
			print("test read FAILD")
		else
			print("test read PASS")
		end
	end

	if(basis.file.parameter.write("information", offset, count, strorig) ~= string.len(strtest))
	then
		print("test recovery write FAILD")
	else
		local recovery = basis.file.parameter.read("information", offset, string.len(strtest))
		if(recovery ~= strorig)
		then
			print("test recovery compare FAILD")
		else
			print("test recovery compare PASS")
		end
	end
end




test_file_information_case_1()
test_file_information_case_2()
test_file_information_case_3()
test_file_information_case_4()
