#include <zmq.hpp>
#include <string>
#include <iostream>

int main() {
   // Prepare context and socket
   zmq::context_t context;
   zmq::socket_t socket(context, ZMQ_REQ);
   socket.connect("ipc:///tmp/lab5_5555");

   // Send "Hello" to the server
   std::string message = "Hello";
   zmq::message_t request(message.size());
   memcpy(request.data(), message.c_str(), message.size());
   socket.send(request, zmq::send_flags::none);

   std::cout << "Sent request: " << message << std::endl;

   // Receive the reply from the server
   zmq::message_t reply;
   auto _ = socket.recv(reply, zmq::recv_flags::none);

   std::string replyMessage = std::string(static_cast<char*>(reply.data()), reply.size());
   std::cout << "Received reply: " << replyMessage << std::endl;

   return 0;
}
