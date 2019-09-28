--lua script language

print("tested file name: firmware")

--测试 firmware 文件
function test_file_information_case_1()
	print("test case : 1")
	print("file size is", basis.file.size("firmware"))
	print("file cluster is", basis.file.cluster("firmware"))
end

--测试 firmware 文件
function test_file_information_case_2()
	print("test case : 2")
	
	local strtest = "test string 01234567890 abcdefgh"
	local offset = 7
	local count = string.len(strtest)

	print("read write address", offset, "count", count)

	if(basis.file.write("firmware", offset, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.read("firmware", offset, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end

	local strwrite = "\xff\xff\xff\xff"
	
	if(basis.file.write("firmware", (offset - (offset % basis.file.cluster("firmware"))), string.len(strwrite), strwrite) ~= string.len(strwrite))
	then
		print("test recovery FAILD")
	else
		local recovery = basis.file.read("firmware", offset, string.len(strwrite))
		if(recovery ~= strwrite)
		then
			print("test recovery FAILD")
		else
			print("test recovery PASS")
		end
	end
end

--测试 firmware 文件
function test_file_information_case_3()
	print("test case : 3")
	
	local strtest = "test string 01234567890 abcdefgh"
	local offset = 400000
	local count = string.len(strtest)

	print("read write address", offset, "count", count)

	if(basis.file.write("firmware", offset, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.read("firmware", offset, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end

	local strwrite = "\xff\xff\xff\xff"
	
	if(basis.file.write("firmware", (offset - (offset % basis.file.cluster("firmware"))), string.len(strwrite), strwrite) ~= string.len(strwrite))
	then
		print("test recovery FAILD")
	else
		local recovery = basis.file.read("firmware", offset, string.len(strwrite))
		if(recovery ~= strwrite)
		then
			print("test recovery FAILD")
		else
			print("test recovery PASS")
		end
	end
end

--测试 firmware 文件
function test_file_information_case_4()
	print("test case : 4")

	print("file size is", basis.file.size("firmware"))
	print("file cluster is", basis.file.cluster("firmware"))

	local strtest = "test string 01234567890 abcdefgh"
	local offset = basis.file.size("firmware") - 9
	local count = string.len(strtest)

	print("read write address", offset, "count", count)

	if(basis.file.write("firmware", offset, count, strtest) ~= 9)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.read("firmware", offset, count)

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

	local strwrite = "\xff\xff\xff\xff"
	
	if(basis.file.write("firmware", (offset - (offset % basis.file.cluster("firmware"))), string.len(strwrite), strwrite) ~= string.len(strwrite))
	then
		print("test recovery FAILD")
	else
		local recovery = basis.file.read("firmware", offset, string.len(strwrite))
		if(recovery ~= strwrite)
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
