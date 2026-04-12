#include "../demo/basic_db.h"

#include <algorithm>
#include <iostream>
#include <thread>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


namespace basic_db {

Host::Host(int port) : port_(port) {}

void listener_main(int fd, std::vector<std::thread>* threads, std::unordered_set<int>* database_, std::mutex* database_mutex, std::mutex* threads_mutex);


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


  // Just listen for every connections and add to list
  std::vector<std::thread> open_connections;

  std::mutex threads_mutex;
  while (true) {
    int fd = accept(server_fd, reinterpret_cast<sockaddr*>(&address), reinterpret_cast<socklen_t*>(&addrlen));
    if (fd < 0) {
      std::cout << "[Server] Incoming connection failed.\n";
      continue;
    } else {
      std::cout << "[Server] Incoming connection success.\n";
    }

    open_connections.emplace_back(listener_main, fd, &open_connections, &database_, &database_mutex_, &threads_mutex);
  }
}

void listener_main(int fd, std::vector<std::thread>* threads, std::unordered_set<int>* database_, std::mutex* database_mutex, std::mutex* threads_mutex) {

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
    database_mutex->lock();
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
    database_mutex->unlock();
    send(fd, response, sizeof(Response), 0);
  }

  close(fd);

  // Remove the thread from the list of threads
  std::lock_guard<std::mutex> lock(*threads_mutex);
  auto it = std::find_if(threads->begin(), threads->end(), [](std::thread& t) {
    return t.get_id() == std::this_thread::get_id();
  });

  if (it != threads->end()) {
    it->detach(); // Must detach BEFORE erase, so destructor doesn't terminate
    threads->erase(it);
  }
}

}