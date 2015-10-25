CPP = clang++
CPPFLAGS = --std=c++11 -O2

all: db

command_line.o: src/base/command_line.cc src/base/command_line.h
	$(CPP) $(CPPFLAGS) -c src/base/command_line.cc -o src/base/command_line.o

db_main.o: src/db/db_main.cc src/db/db_main.h
	$(CPP) $(CPPFLAGS) -c src/db/db_main.cc -o src/db/db_main.o

db_help.o: src/db/db_main.cc src/db/db_main.h
	$(CPP) $(CPPFLAGS) -c src/db/db_help.cc -o src/db/db_help.o

db: db_main.o command_line.o db_help.o
	$(CPP) $(CPPFLAGS) src/base/command_line.o src/db/db_main.o src/db/db_help.o -o out/minisql
