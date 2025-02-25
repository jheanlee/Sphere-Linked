//
// Created by Jhean Lee on 2024/11/26.
//

#ifndef TUNNEL_SHARED_HPP
  #define TUNNEL_SHARED_HPP
  #include <mutex>

  namespace shared_resources {
    extern std::mutex ssl_send_mutex;
  }

  extern bool verbose;

#endif //TUNNEL_SHARED_HPP
