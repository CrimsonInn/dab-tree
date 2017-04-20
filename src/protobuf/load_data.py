#! /usr/bin/env python

import sys
sys.path.append("./")
from include import data_pb2


def write(fea_types, value_matrix, file_name):
  # if len(sys.argv) != 2:
  #   print ("Usage:", sys.argv[0], file_name)
  #   sys.exit(-1)

  batch_data = data_pb2.Matrix()

  try:
    with open(file_name, "rb") as f:
      batch_data.ParseFromString(f.read())
  except IOError:
    print (file_name + ": File not found.  Creating a new file.")

  height = len(value_matrix)
  width = 0
  if height :
    width = len(value_matrix[0])

  batch_data.width = width
  batch_data.height = height

  for fy in fea_types:
    if fy == "CONT":
      batch_data.fea_types.append(data_pb2.CONT)
    elif fy == "DISC":
      batch_data.fea_types.append(data_pb2.DISC)
    elif fy == "RANK":
      batch_data.fea_types.append(data_pb2.RANK)
    else:
      batch_data.fea_types.append(data_pb2.DISC)
      print("Unknown feature type; leaving as default value.")

  for i in range(width):
    for j in range(height):
      new_data_value = batch_data.data.add()
      if fea_types[j] == "CONT":
        new_data_value.v = value_matrix[i][j]
      elif fea_types[j] == "DISC":
        new_data_value.cls = value_matrix[i][j]
      elif fea_types[j] == "RANK":
        new_data_value.level = value_matrix[i][j]
      else:
        new_data_value.cls = value_matrix[i][j]
        print("Unknown feature type; leaving as default value.")

  with open(file_name, "wb") as f:
    f.write(batch_data.SerializeToString())
  print("Success!")


def read(file_name):
  # TODO: fix bugs
  if len(sys.argv) != 2:
    print ("Usage:", sys.argv[0], file_name)
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
  fea_types = ["CONT", "CONT", "DISC", "RANK"]
  value_matrix = [[0,1,0,2],[0,1,0,0],[0,0.4,0,0],[0,0.4,1,0]]
  file_name = "BATCH_DATA_FILE"
  write(fea_types, value_matrix, file_name)


if __name__ == "__main__":
    main()

