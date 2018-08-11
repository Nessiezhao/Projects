.PHONY:all
all:insertData selectData test
insertData:insertData.c
	gcc -o $@ $^ -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient

selectData:selectData.c
	gcc -o $@ $^ -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient

test:test.c
	gcc -o $@ $^ -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient
.PHONY:clean
clean:
	rm -f insertData selectData
