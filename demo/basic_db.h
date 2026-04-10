#pragma once

#include <string>
#include <unordered_set>


namespace basic_db {

// ------------ Database Query + Responses ------------

// Instruction parameters for the query (should be an enum class)
constexpr int GET = 0;
constexpr int ADD = 1;
constexpr int REMOVE = 2;

struct Query {
  int instruction;
  int data;
};

constexpr int OK = 0;
constexpr int BAD_INSTRUCTION = 1;
constexpr int INCOMPLETE_REQUEST = 2;
constexpr int ALREADY_EXISTS = 3;
constexpr int DOES_NOT_EXIST = 4;
constexpr int BAD_RESPONSE = 5;

struct Response {
  int status;
  int result;
};

// This would break the implementations if this is violated
static_assert(sizeof(Query) == sizeof(Response));


// ------------ Database Host ------------

class Host {


public:

  void begin_listen();

  Host(int port);

private:
  int port_;
  std::unordered_set<int> database_;
};


// ------------ Database Client ------------

class Client {
public:
  Client(int port);

  // Blocks execution until a response is recieved.
  Response query(Query query);

  // Query with info printed to stdout
  Response query_verbose(Query query);

  ~Client();

private:
  int fd_;
};

// ------------ Database stdout operations ------------
std::string instruction_to_string(int instruction);
std::string status_to_string(int status);
void print_response(Response r);

}