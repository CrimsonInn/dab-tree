#! /usr/bin/env python

import data_pb2
import sys

def write():
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], "BATCH_DATA")
    sys.exit(-1)

  batch_data = data_pb2.Batch()

  try:
    with open(sys.argv[1], "rb") as f:
      batch_data.ParseFromString(f.read())
  except IOError:
    print (sys.argv[1] + ": File not found.  Creating a new file.")

  # insert new type
  dab_type = batch_data.fea_types
  dab_type.append(data_pb2.DISC)
  dab_type.append(data_pb2.DISC)
  dab_type.append(data_pb2.DISC)

  # def rows
  batch_data.rows = 1000
  # def cols
  batch_data.cols = 1000
  # insert new value
  new_data_value = batch_data.samples.add()
  new_data_value.v = 0.1;
  new_data_value.id = 2; # the last value will be the union type value

  with open(sys.argv[1], "wb") as f:
    f.write(batch_data.SerializeToString())


def read():
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], "BATCH_DATA")
    sys.exit(-1)

  batch_data = data_pb2.Batch()

  with open(sys.argv[1], "rb") as f:
    batch_data.ParseFromString(f.read())

  print("rows: ", batch_data.rows)
  print("cols: ", batch_data.cols)
  print("samples size: ", len(batch_data.samples))
  p_value = batch_data.samples[-1]
  print(" - last value: ", p_value, end="")
  print("fea_types size: ", len(batch_data.fea_types))
  p_type = batch_data.fea_types[-1]
  if p_type == data_pb2.CONT:
    print(" - CONT")
  elif p_type == data_pb2.DISC:
    print(" - DISC")
  elif p_type == data_pb2.RANK:
    print(" - RANK")
  else:
    print("ERROR")


def main():
  #write()
   read()


if __name__ == "__main__":
    main()


