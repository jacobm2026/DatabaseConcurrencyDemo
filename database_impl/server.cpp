#include "../demo/basic_db.h"

#include <iostream>
#include <thread>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


namespace basic_db {

Host::Host(int port) : port_(port) {}

void listener_main(int fd, std::unordered_set<int>* database_);


void Host::begin_listen() {

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port_);


  bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
  listen(server_fd, 10);
  std::cout << "Database listening on port " << port_ << "...\n";

  // Just listen for every connections


  std::vector<std::thread> open_connections;

  while (true) {
    int fd = accept(server_fd, reinterpret_cast<sockaddr*>(&address), reinterpret_cast<socklen_t*>(&addrlen));
    if (fd < 0) {
      std::cout << "[Server] Incoming connection failed.\b";
      continue;
    } else {
      std::cout << "[Server] Incoming connection success.\n";
    }

    open_connections.emplace_back(listener_main, fd, &database_);
  }
}

void listener_main(int fd, std::unordered_set<int>* database_) {

  char buffer[sizeof(Query)] = {0};
  int bytes_read;

  while ((bytes_read = read(fd, buffer, sizeof(Query))) > 0) {
    Query* request = reinterpret_cast<Query*>(buffer);
    Response* response = reinterpret_cast<Response*>(buffer);
    if (bytes_read != sizeof(Query)) {
      // This is interesting
      response->status = INCOMPLETE_REQUEST;
      response->result = bytes_read;
      send(fd, response, sizeof(Response), 0);
      continue;
    }

    // Process the request
    switch (request->instruction) {
      case GET:
        response->result = static_cast<int>(database_->find(request->data) != database_->end());
        response->status = OK;
        break;
      case ADD:
        if (database_->find(request->data) != database_->end()) {
          response->status = ALREADY_EXISTS;
        } else {
          database_->insert(request->data);
          response->status = OK;
        }
        break;
      case REMOVE:
        if (database_->find(request->data) == database_->end()) {
          response->status = DOES_NOT_EXIST;
        } else {
          database_->erase(request->data);
          response->status = OK;
        }
        break;
      default:
        response->result = request->instruction;
        response->status = BAD_INSTRUCTION;
        break;
    }
    send(fd, response, sizeof(Response), 0);
  }

  close(fd);
}

}