# Makefile for protobuf

DIR_DATA_SRC = ./data
DIR_DATA_DST = ./include
DIR_INC = ./include
DIR_PROTO_SRC = ./src/protobuf
DIR_SRC = ./src
DIR_BUILD = ./build

CC = c++
CFLAGS = -std=gnu++11 `pkg-config --cflags --libs protobuf` -lglog

objects = $(DIR_BUILD)/main.o $(DIR_BUILD)/write_proto.o $(DIR_BUILD)/read_proto.o\
		  $(DIR_INC)/data.pb.cc $(DIR_INC)/tree.pb.cc $(DIR_INC)/matrix.pb.cc 

.PHONY: all
all: _PRE output
	@echo "END!"

proto: $(DIR_DATA_SRC)/data.proto $(DIR_DATA_SRC)/tree.proto
	protoc -I=$(DIR_DATA_SRC) --cpp_out=$(DIR_INC) $(DIR_DATA_SRC)/data.proto
	protoc -I=$(DIR_DATA_SRC) --cpp_out=$(DIR_INC) $(DIR_DATA_SRC)/tree.proto
	protoc -I=$(DIR_DATA_SRC) --cpp_out=$(DIR_INC) $(DIR_DATA_SRC)/matrix.proto

output: $(objects) proto
	$(CC) $(CFLAGS) $(objects) -o output 

$(DIR_BUILD)/main.o: $(DIR_SRC)/main.cc $(DIR_INC)/*.h
	$(CC) -std=gnu++11 -c -I$(DIR_INC) $(DIR_SRC)/main.cc -o $(DIR_BUILD)/main.o

$(DIR_BUILD)/write_proto.o: $(DIR_PROTO_SRC)/write_proto.cc $(DIR_INC)/*.h
	$(CC) -std=gnu++11 -c -I$(DIR_INC) $(DIR_PROTO_SRC)/write_proto.cc -o $(DIR_BUILD)/write_proto.o

$(DIR_BUILD)/read_proto.o: $(DIR_PROTO_SRC)/write_proto.cc $(DIR_INC)/*.h
	$(CC) -std=gnu++11 -c -I$(DIR_INC) $(DIR_PROTO_SRC)/read_proto.cc -o $(DIR_BUILD)/read_proto.o

.PHONY: _PRE
_PRE:
	mkdir -p $(DIR_BUILD)

.PHONY: clean
clean:
	rm -fr build

