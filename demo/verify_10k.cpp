#include "basic_db.h"

#include <iostream>


int main() {

  basic_db::Client client(8080);

  bool fail = false;

  for (int i = 0; i < 10000; ++i) {
    if (client.query({basic_db::GET, i}).result) { // Get doesnt fail in this model
      std::cout << "[Client] Fail at position " << i << ".\n";
      fail = true;
    }  
  }

  if (!fail) {
    std::cout << "[Client] Pass for all 10k\n";
  }

  return 0;
}