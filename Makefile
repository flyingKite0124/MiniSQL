CPP = clang++
CPPFLAGS = --std=c++11 -O2

all: db

# base

string.o: base/string.cc base/string.h
	$(CPP) $(CPPFLAGS) -c base/string.cc -o base/string.o

command_line.o: base/command_line.cc base/command_line.h
	$(CPP) $(CPPFLAGS) -c base/command_line.cc -o base/command_line.o

# db

db_type.o: db/db_type.cc db/db_type.h
	$(CPP) $(CPPFLAGS) -c db/db_type.cc -o db/db_type.o

db_main.o: db/db_main.cc db/db_main.h
	$(CPP) $(CPPFLAGS) -c db/db_main.cc -o db/db_main.o

db_help.o: db/db_main.cc db/db_main.h
	$(CPP) $(CPPFLAGS) -c db/db_help.cc -o db/db_help.o

db_repl.o: db/db_repl.cc db/db_repl.h
	$(CPP) $(CPPFLAGS) -c db/db_repl.cc -o db/db_repl.o

db_process.o: db/db_process.cc db/db_process.h
	$(CPP) $(CPPFLAGS) -c db/db_process.cc -o db/db_process.o

db: string.o command_line.o db_type.o db_main.o db_help.o db_repl.o db_process.o
	$(CPP) $(CPPFLAGS) base/string.o base/command_line.o db/db_type.o db/db_main.o db/db_help.o db/db_repl.o db/db_process.o -o minisql
