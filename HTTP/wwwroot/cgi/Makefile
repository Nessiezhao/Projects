.PHONY:all
all:insertData selectData test selecttest
insertData:insertData.c
	gcc -o $@ $^ -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient

selectData:selectData.c
	gcc -o $@ $^ -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient
selecttest:selecttest.c
	g++ selecttest.c -o selecttest -lctemplate -pthread -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient
test:test.c
	gcc -o $@ $^ -I/usr/include/mysql/ -L/usr/lib64/mysql/ -lmysqlclient
.PHONY:clean
clean:
	rm -f insertData selectData
