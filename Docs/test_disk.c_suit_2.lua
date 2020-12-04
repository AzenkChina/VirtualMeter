--lua script language

print("tested file name: firmware")

--测试 firmware 文件
function test_file_information_case_1()
	print("test case 1")
	
	if(basis.file.parallel.init("firmware", 128) ~= 0)
	then
		print("test init PASS")
	else
		print("test init FAILD")
	end
end

--测试 firmware 文件
function test_file_information_case_2()
	print("test case 2")
	
	local strtest = "test string 01234567890 abcdefgh"
	local index = 7
	local count = string.len(strtest)

	print(string.format("read write at index %d count %d", index, count))
	
	if(basis.file.parallel.write("firmware", index, count, strtest) ~= count)
	then
		print("test write FAILD")
	else
		print("test write PASS")
	end

	local stread = basis.file.parallel.read("firmware", index, count)

	if(strtest ~= stread)
	then
		print("test read FAILD")
	else
		print("test read PASS")
	end
end

test_file_information_case_1()
test_file_information_case_2()
