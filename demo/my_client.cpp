#include "basic_db.h"

using namespace basic_db;


int main() {

  // Port 8080
  Client client(8080);

  client.query_verbose({ADD, 1});
  client.query_verbose({GET, 1});
  client.query_verbose({GET, 2});
  client.query_verbose({REMOVE, 1});
  client.query_verbose({GET, 1});



  return 0;
}