//
// Created by Jhean Lee on 2025/2/5.
//

#include <csignal>

#include "signal_handler.hpp"

void register_signal() {
  signal(SIGPIPE, SIG_IGN);
}