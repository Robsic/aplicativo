#include <chrono>
#include <memory>
#include <string.h>
#include <string>
#include <iostream>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {

//socket(domínio, tipo, protocolo);
  int listening = socket(AF_INET, SOCK_STREAM, 0);

  if (listening == -1)
  {
      cerr << "Can't create a socket! Quitting" << endl;
      return -1;
  }

  // Vincula o endereço IP e a porta a um socket
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(6000);
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

  bind(listening, (sockaddr*)&hint, sizeof(hint));

  // Diz ao Winsock que o socket é para escutar
  listen(listening, SOMAXCONN);

  //Aguarda uma conexão
  sockaddr_in client;
  socklen_t clientSize = sizeof(client);

  int clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

  char host[NI_MAXHOST];      // Nome remoto do cliente
  char service[NI_MAXSERV];   // Serviço (i.e. porta) em que o cliente está conectado

  memset(host, 0, NI_MAXHOST); // igual o memset(host, 0, NI_MAXHOST);
  memset(service, 0, NI_MAXSERV);
  //Fecha o socket de escuta
  close(listening);
 
// While loop: aceita e ecoa a mensagem de volta para o cliente
  char buf[4096];

//Inicializa o ROS2
  rclcpp::init(argc, argv);


//auto = std::shared_ptr<rclcpp::Node>

//Criação de um rclcpp::Node chamado minimal_publisher
  auto node = rclcpp::Node::make_shared("minimal_publisher");
//Os dados da mensagem são definidos
  auto message = std_msgs::msg::String();
// Declaração do publisher_
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
/*o publicador é inicializado com o tipo de mensagem String
  o tópico do nome do tópico e o tamanho de fila necessário para limitar as mensagens */
  publisher_ = node->create_publisher<std_msgs::msg::String>("topic", 10);



while (true)
    {
        memset(buf, 0, 4096);
 
        // Aguarda o cliente enviar dados
        int bytesReceived = recv(clientSocket, buf, 4096, 0);
        if (bytesReceived == -1)
        {
            //cout << "Error in recv(). Quitting" << endl;
            break;
        }
 
        if (bytesReceived == 0)
        {
            //cout << "Client disconnected " << endl;
            break;
        }
       // Passando os dados do buf que estão sendo recebido pelo android para o message  
        message.data = buf;

        //RCLCPP_INFO(node->get_logger(), buf);
        //A macro RCLCPP_INFO garante que todas as mensagens publicadas sejam impressas no console.
        RCLCPP_INFO(node->get_logger(), "Publishing: '%s'", message.data.c_str());
        publisher_->publish(message);
         
        // Echo message back to client
        //send(clientSocket, buf, bytesReceived + 1, 0);
    }


 
  rclcpp::shutdown();
  return 0;
}