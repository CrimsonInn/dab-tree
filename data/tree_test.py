#! /usr/bin/env python

import tree_pb2
import sys

def write():
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], "DAB_TREE_DATA")
    sys.exit(-1)

  dab_tree_data = tree_pb2.RegTree()

  try:
    with open(sys.argv[1], "rb") as f:
      dab_tree_data.ParseFromString(f.read())
  except IOError:
    print (sys.argv[1] + ": File not found.  Creating a new file.")

  # id
  dab_tree_data.id = 100

  # insert new type
  dab_type = dab_tree_data.fea_types
  dab_type.append(tree_pb2.DISC)

  # features
  dab_split = dab_tree_data.split_fea
  dab_split.append(20)

  # values
  new_tree_value = dab_tree_data.split_value.add()
  new_tree_value.v = 0.1;
  new_tree_value.id = 2; # the last value will be the union type value

  # leaf
  dab_leaf = dab_tree_data.is_leaf
  dab_leaf.append(True)

  with open(sys.argv[1], "wb") as f:
    f.write(dab_tree_data.SerializeToString())


def read():
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], "DAB_TREE_DATA")
    sys.exit(-1)

  dab_tree_data = tree_pb2.RegTree()

  with open(sys.argv[1], "rb") as f:
    dab_tree_data.ParseFromString(f.read())

  print("id: ", dab_tree_data.id)
  nums = len(dab_tree_data.fea_types)
  print("size: ", nums)
  print("Last element:")
  p_type = dab_tree_data.fea_types[-1]
  if p_type == tree_pb2.CONT:
    print(" - feature types: CONT")
  elif p_type == tree_pb2.DISC:
    print(" - feature types: DISC")
  elif p_type == tree_pb2.RANK:
    print(" - feature types: RANK")
  else:
    print("ERROR")
  print(" - split feature: ", dab_tree_data.split_fea[-1])
  print(" - split value: ", dab_tree_data.split_value[-1], end="")
  print(" - is leaf: ", dab_tree_data.is_leaf[-1])

def main():
  #write()
  read()

if __name__ == "__main__":
    main()


