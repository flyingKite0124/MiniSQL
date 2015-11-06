CPP = clang++
CPPFLAGS = --std=c++11 -O2

all: db

# base

io.o: base/io.cc base/io.h
	$(CPP) $(CPPFLAGS) -c base/io.cc -o base/io.o

string.o: base/string.cc base/string.h
	$(CPP) $(CPPFLAGS) -c base/string.cc -o base/string.o

command_line.o: base/command_line.cc base/command_line.h
	$(CPP) $(CPPFLAGS) -c base/command_line.cc -o base/command_line.o

# db

db_type.o: db/db_type.cc db/db_type.h
	$(CPP) $(CPPFLAGS) -c db/db_type.cc -o db/db_type.o

db_global.o: db/db_global.cc db/db_global.h
	$(CPP) $(CPPFLAGS) -c db/db_global.cc -o db/db_global.o

db_buffer.o: db/db_buffer.cc db/db_buffer.h
	$(CPP) $(CPPFLAGS) -c db/db_buffer.cc -o db/db_buffer.o

db_catalog.o: db/db_catalog.cc db/db_catalog.h
	$(CPP) $(CPPFLAGS) -c db/db_catalog.cc -o db/db_catalog.o

db_index.o: db/db_index.cc db/db_index.h
	$(CPP) $(CPPFLAGS) -c db/db_index.cc -o db/db_index.o

db_main.o: db/db_main.cc db/db_main.h
	$(CPP) $(CPPFLAGS) -c db/db_main.cc -o db/db_main.o

db_help.o: db/db_main.cc db/db_main.h
	$(CPP) $(CPPFLAGS) -c db/db_help.cc -o db/db_help.o

db_repl.o: db/db_repl.cc db/db_repl.h
	$(CPP) $(CPPFLAGS) -c db/db_repl.cc -o db/db_repl.o

db_process.o: db/db_process.cc db/db_process.h
	$(CPP) $(CPPFLAGS) -c db/db_process.cc -o db/db_process.o

db_record.o: db/db_record.cc db/db_record.h
	$(CPP) $(CPPFLAGS) -c db/db_record.cc -o db/db_record.o

db: io.o string.o command_line.o db_type.o db_buffer.o db_catalog.o db_index.o db_global.o db_main.o db_help.o db_repl.o db_process.o db_record.o
	$(CPP) $(CPPFLAGS) base/io.o base/string.o base/command_line.o db/db_type.o \
	db/db_buffer.o db/db_catalog.o db/db_index.o db/db_global.o db/db_main.o db/db_help.o db/db_repl.o \
	db/db_process.o db/db_record.o \
	-o minisql -lreadline
