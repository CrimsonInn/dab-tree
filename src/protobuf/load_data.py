#! /usr/bin/env python

import sys
sys.path.append("./")
import include.data_pb2 as data_pb2
import pandas as pd 
import numpy as np 

READ_DATA_SIZE = 100000
WORKER_NUM = 5
WRITE_BATCH_FILE = 1

def load_raw_data(file_name):
  data_ = pd.read_csv(file_name)

  # clean null data
  data_na = ['cd', 'hd', 'dbdistance', 'vccdistance']
  data_filled = pd.DataFrame(data_)
  data_filled[data_na] = data_filled[data_na].apply(lambda x:x.fillna(x.mean()))

  # transform data
  fea_types = ['DISC','DISC','DISC','DISC',
               'CONT','CONT','CONT','DISC',
               'DISC','CONT','CONT','CONT',
               'CONT','CONT','CONT','CONT','CONT']
  fea_types = np.array(fea_types)
  data_filled['gender']=(data_filled.gender == 'M')*1
  data_filled['voted']=(data_filled.voted == 'Y')*1
  party_mapping = {
    'D':0,
    'G':1,
    'L':2,
    'O':3,
    'R':4,
    'U':5
  }
  data_filled['party'] = data_filled['party'].map(party_mapping)
  race_mapping = {
    'African-American':0,
    'Caucasian':1,
    'Central Asian':2,
    'East Asian':3,
    'Hispanic':4,
    'Jewish':5,
    'Middle Eastern':6,
    'Native American':7,
    'Pacific Islander':8,
    'South Asian':9,
    'Uncoded':10
  }
  data_filled['racename'] = data_filled['racename'].map(race_mapping)
  data_matrix = data_filled.values
  np.random.shuffle(data_matrix)
  data_matrix = np.array(data_matrix)
  return fea_types, data_matrix[0:READ_DATA_SIZE, :]


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

  for i in range(height):
    for j in range(width):
      new_data_value = batch_data.data.add()
      if fea_types[j] == "CONT":
        new_data_value.v = float(value_matrix[i][j])
      elif fea_types[j] == "DISC":
        new_data_value.cls = int(value_matrix[i][j])
      elif fea_types[j] == "RANK":
        new_data_value.level = int(value_matrix[i][j])
      else:
        new_data_value.cls = value_matrix[i][j]
        print("Unknown feature type; leaving as default value.")

  with open(file_name, "wb") as f:
    f.write(batch_data.SerializeToString())
  print("protobuf file %s store success!" % file_name)


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
  # fea_types = ["CONT", "CONT", "DISC", "RANK"]
  # value_matrix = [[0,1,0,2],[0,1,0,0],[0,0.4,0,0],[0,0.4,1,0]]

  
  data_file_name = "train.csv"
  print("Begin load data ... ")
  fea_types, value_matrix = load_raw_data(data_file_name)
  print("Fea_type shape: ", fea_types.shape)
  print("Value shape: ", value_matrix.shape)
  if WRITE_BATCH_FILE > 0:
    file_name = "BATCH_DATA_FILE"
    write(fea_types, value_matrix, file_name)
  else:
    slice_size = READ_DATA_SIZE // WORKER_NUM
    for i in range(WORKER_NUM):
      write(fea_types, value_matrix[(i * slice_size) : ((i + 1) * slice_size), :], str(i + 1))


if __name__ == "__main__":
    main()

