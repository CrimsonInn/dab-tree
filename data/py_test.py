#! /usr/bin/env python

import dabTree_pb2
import sys

def write():
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], "DAB_TREE_DATA")
    sys.exit(-1)

  dab_tree_data = dabTree_pb2.DabData()

  try:
    with open(sys.argv[1], "rb") as f:
      dab_tree_data.ParseFromString(f.read())
  except IOError:
    print (sys.argv[1] + ": File not found.  Creating a new file.")

  # insert new type
  dab_type = dab_tree_data.types
  dab_type.append(dabTree_pb2.DISC)

  # def rows
  dab_tree_data.rows = 1000
  # def cols
  dab_tree_data.cols = 1000
  # insert new value
  new_data_value = dab_tree_data.values.add()
  new_data_value.v = 0.1;
  new_data_value.id = 2;
  new_data_value.n = 10;

  with open(sys.argv[1], "wb") as f:
    f.write(dab_tree_data.SerializeToString())

def read():
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], "DAB_TREE_DATA")
    sys.exit(-1)

  dab_tree_data = dabTree_pb2.DabData()

  with open(sys.argv[1], "rb") as f:
    dab_tree_data.ParseFromString(f.read())

  print("rows: ", dab_tree_data.rows)
  print("cols: ", dab_tree_data.cols)
  print("values size: ", len(dab_tree_data.values))
  p_value = dab_tree_data.values[1]
  print("value: ", p_value.v, p_value.id, p_value.n)
  print("types size: ", len(dab_tree_data.types))
  p_type = dab_tree_data.types[0]
  if p_type == dabTree_pb2.CONT:
    print("CONT")
  elif p_type == dabTree_pb2.DISC:
    print("DISC")
  elif p_type == dabTree_pb2.RANK:
    print("RANK")
  else:
    print("ERROR")

#write()
read()

