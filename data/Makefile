# See README.txt.

all: protoc

clean:
	rm -f protoc_middleman data.pb.cc data.pb.h data_pb2.py
	rm -f *.pyc
	rmdir tutorial 2>/dev/null || true
	rmdir com/example/tutorial 2>/dev/null || true
	rmdir com/example 2>/dev/null || true
	rmdir com 2>/dev/null || true

data: 
	protoc -I=. --cpp_out=. --python_out=. data.proto

tree: 
	protoc -I=. --cpp_out=. --python_out=. tree.proto