#include "basic_db.h"

#include <iostream>


int main() {

  basic_db::Client client(8080);

  for (int i = 0; i < 10000; ++i) {
    if (client.query({basic_db::GET, i}).result) { // Get doesnt fail in this model
      client.query({basic_db::REMOVE, i}); // Assume these succeed
    } else {
      client.query({basic_db::ADD, i});
    }
  }

  return 0;
}