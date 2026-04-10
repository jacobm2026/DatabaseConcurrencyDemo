#include "./demo/basic_db.h"

using namespace basic_db;


int main() {

  // Port 8080
  Host host(8080);

  host.begin_listen();


  __builtin_unreachable();
}