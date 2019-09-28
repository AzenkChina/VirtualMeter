--lua script language

print("tested file name: information")

--测试 information 文件
function test_file_information_case_1()
	print("test case : 1")
	print("file size is", basis.file.size("information"))
	print("file cluster is", basis.file.cluster("information"))
end

--测试 information 文件
function test_file_information_case_2()
	print("test case : 2")
	
	local strtest = "test string 01234567890 abcdefgh"
	local offset = 7
	local count = string.len(strtest)

	print("read write address", offset, "count", count)
	local strorig = basis.file.read("information", offset, count)

	if(basis.file.write("information", offset, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.read("information", offset, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end

	if(basis.file.write("information", offset, count, strorig) ~= count)
	then
		print("test recovery FAILD")
	else
		local recovery = basis.file.read("information", offset, count)
		if(recovery ~= strorig)
		then
			print("test recovery FAILD")
		else
			print("test recovery PASS")
		end
	end
end

--测试 information 文件
function test_file_information_case_3()
	print("test case : 3")

	local strtest = "test string 01234567890 abcdefgh"
	local offset = 62
	local count = string.len(strtest)

	print("read write address", offset, "count", count)
	local strorig = basis.file.read("information", offset, count)

	if(basis.file.write("information", offset, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.read("information", offset, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end

	if(basis.file.write("information", offset, count, strorig) ~= count)
	then
		print("test recovery FAILD")
	else
		local recovery = basis.file.read("information", offset, count)
		if(recovery ~= strorig)
		then
			print("test recovery FAILD")
		else
			print("test recovery PASS")
		end
	end
end

--测试 information 文件
function test_file_information_case_4()
	print("test case : 4")

	print("file size is", basis.file.size("information"))
	print("file cluster is", basis.file.cluster("information"))

	local strtest = "test string 01234567890 abcdefgh"
	local offset = basis.file.size("information") - 9
	local count = string.len(strtest)

	print("read write address", offset, "count", count)
	local strorig = basis.file.read("information", offset, count)

	if(basis.file.write("information", offset, count, strtest) ~= 9)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.read("information", offset, count)

	if(string.len(stread) ~= 9)
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

	if(basis.file.write("information", offset, count, strorig) ~= 9)
	then
		print("test recovery FAILD")
	else
		local recovery = basis.file.read("information", offset, 9)
		if(recovery ~= strorig)
		then
			print("test recovery FAILD")
		else
			print("test recovery PASS")
		end
	end
end




test_file_information_case_1()
test_file_information_case_2()
test_file_information_case_3()
test_file_information_case_4()
