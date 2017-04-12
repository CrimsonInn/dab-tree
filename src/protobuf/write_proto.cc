#include <iostream>
#include <fstream>
#include <string>
#include "proto_func.h"
#include "data.h"
#include "tree.h"
#include <glog/logging.h>

void write_batch_data() {

}

void write_tree() {

}



void read_protobuf() {
    
  cout << "Test read protobuf ... " << endl;
  
  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  // Test for protobuf
  // read out a person message
  tutorial::AddressBook read_address_book;

  string file_name = "DEMO_ADDRESS_BOOK_FILE"; //protobuf file name

  // Read the existing address book.
  fstream input_read(file_name, ios::in | ios::binary);
  if (!input_read) {
    cout << file_name << ": File not found. Creating a new file." << endl;
  } else if (!read_address_book.ParseFromIstream(&input_read)) {
    cerr << "Failed to parse address book." << endl;
    return ;
  }

  // print the last person's info
  if (read_address_book.people_size() > 0) {
    int num = read_address_book.people_size();
    const tutorial::Person& first_person = read_address_book.people(num - 1);
    cout << "Begin to print the last person's info: " << endl;
    cout << " Person ID: " << first_person.id() << endl;
    cout << " Name: " << first_person.name() << endl;
    cout << " E-mail address: " << first_person.email() << endl;
    if (first_person.phones_size() > 0) {
        const tutorial::Person::PhoneNumber& phone_number = first_person.phones(0);
        switch (phone_number.type()) {
          case tutorial::Person::MOBILE:
            cout << " Mobile phone #: ";
            break;
          case tutorial::Person::HOME:
            cout << " Home phone #: ";
            break;
          case tutorial::Person::WORK:
            cout << " Work phone #: ";
            break;
        }
        cout << phone_number.number() << endl;
   }
    cout << "Done!" << endl;
  } else {
    cout << "Empty address book!" << endl;
  }

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

}