//
// Created by Jhean Lee on 2024/10/2.
//
#include <string>
#include <cstring>
#include <unistd.h>

#include "../common/shared.hpp"
#include "message.hpp"
#include "../common/console.hpp"

void Message::load(char *buffer) {
  if (strlen(buffer) == 0) throw -1;
  if (strlen(buffer) > MESSAGE_MAX_STRING_SIZE + 1) throw -1;

  type = buffer[0];
  string = std::string(buffer + 1);
}

void Message::dump(char *buffer) const {
  if (type == '\0') throw -1;
  if (string.size() > MESSAGE_MAX_STRING_SIZE) throw -1;

  buffer[0] = type;
  strcat(buffer, string.c_str());
}

int ssl_send_message(SSL *ssl, char *buffer, size_t buffer_size, Message &message) {
  std::lock_guard<std::mutex> lock(shared_resources::ssl_send_mutex);

  try {
    std::memset(buffer, '\0', buffer_size);
    message.dump(buffer);
  } catch (int err) {
    console(WARNING, MESSAGE_DUMP_FAILED, nullptr, "message::message::dump");
    return -1;
  }

  return SSL_write(ssl, buffer, buffer_size);
}

int ssl_recv_message(SSL *ssl, char *buffer, size_t buffer_size, Message &message) {
  std::memset(buffer, '\0', buffer_size);
  int nbytes = SSL_read(ssl, buffer, buffer_size);
  if (nbytes <= 0) return nbytes;

  try {
    message.load(buffer);
  } catch (int err) {
    console(WARNING, MESSAGE_LOAD_FAILED, nullptr, "message::message::load");
    return -1;
  }

  return nbytes;
}

int ssl_read_message_non_block(SSL *ssl, pollfd *pfds, char *buffer, size_t buffer_size, Message &message) {
  pfds[0] = {.fd = SSL_get_fd(ssl), .events = POLLIN | POLLPRI};
  int ready_for_call = poll(pfds, 1, timeout_session_millisec);
  if (ready_for_call < 0) {
    console(ERROR, SOCK_POLL_ERR, nullptr, "message::read_message_non_block");
    return -1;
  } else if (ready_for_call == 0) {
    return 0;
  } else {
    int recv_status = ssl_recv_message(ssl, buffer, buffer_size, message);
    if (recv_status == 0) return -1;
    return recv_status;
  }
}