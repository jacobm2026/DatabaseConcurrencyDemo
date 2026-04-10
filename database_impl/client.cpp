#include "../demo/basic_db.h"

#include <iostream>
#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace basic_db {


Client::Client(int port) {

  struct sockaddr_in server_addr;

  // 1. Create socket
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (fd_ < 0) {
    throw std::invalid_argument("Could not connect create the socket");
  }

  // 2. Specify the server to connect to
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);  // localhost

  // 3. Connect
  if (connect(fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    throw std::invalid_argument("Could not connect to port");
  }

  std::cout << "[Client] Connection to database server complete.\n";

}

Response Client::query(Query query) {
  // Write it to the file destination
  write(fd_, reinterpret_cast<void*>(&query), sizeof(Query));
  // Wait for the response
  // Copy into stack query buffer
  Response response;
  int bytes_read;
  if (bytes_read = read(fd_, reinterpret_cast<void*>(&response), sizeof(Response)) != sizeof(Response)) {
    response.status = BAD_RESPONSE;
    response.result = bytes_read;
  }

  return response;
}


std::string instruction_to_string(int instruction) {
  switch (instruction) {
    case GET:
      return "Get";
    case ADD:
      return "Add";
    case REMOVE:
      return "Remove";
    default:
      return std::to_string(instruction);
  }
}

std::string status_to_string(int status) {
  switch (status) {
    case OK:
      return "Ok";
    case BAD_INSTRUCTION:
      return "Bad instruction";
    case INCOMPLETE_REQUEST:
      return "Incomplete request";
    case ALREADY_EXISTS:
      return "Already exists";
    case DOES_NOT_EXIST:
      return "Does not exist";
    case BAD_RESPONSE:
      return "Bad response";
    default:
      return std::to_string(status);
  }
}

void print_response(Response r) {
  if (r.status == OK) {
    std::cout << "[Client] Query returned " << r.result << '\n';
  } else {
    std::cout << "[Client] Query failed with failure: " << status_to_string(r.status) << '\n';
  }
}


Response Client::query_verbose(Query query) {
  std::cout << "[Client] Sending " << instruction_to_string(query.instruction) << ' ' << query.data << '\n';
  Response r = this->query(query);
  print_response(r);
  return r;
}

Client::~Client() {
  close(fd_);
}


}