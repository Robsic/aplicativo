/*
Golfinho Teleop Android Node - Android Studio and ROS2 Foxy Integration
Author: José Gabriel de Jesus Flores - gabrielfloresbassanello@gmail.com
Github: https://github.com/jgabrielflores
Laboratory of Robotics, Intelligent and Complex Systems - RobSIC
Universidade Federal de Itajubá
Created: 2022/04/28
*/

#include <chrono>
#include <memory>
#include <string.h>
#include <cstring>
#include <string>
#include <iostream>
#include <termios.h>
#include <assert.h>
#include <sstream>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace std;

using namespace std::chrono_literals;

#define VEL_ANG_MAX 1
#define VEL_LIN_MAX 1
#define VEL_ANG_INC 0.02
#define VEL_LIN_INC 0.08

double vel_ang = 0;
double vel_lin = 0;

int main(int argc, char *argv[]) {

    //Inicializa o ROS2
    rclcpp::init(argc, argv);

    //Criação de um rclcpp::Node chamado teleop
    auto node = rclcpp::Node::make_shared("teleop");

    //Os dados da mensagem são definidos
    auto default_qos = rclcpp::QoS(rclcpp::SystemDefaultsQoS());
    auto cmd_msg = std::make_shared<geometry_msgs::msg::Twist>();

    // Declaração do publisher_
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub;

    /*Publish é incializado*/
    cmd_pub = node->create_publisher<geometry_msgs::msg::Twist>("/golfinho/cmd_vel", default_qos); // topic publisher

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

    memset(host, 0, NI_MAXHOST); 
    memset(service, 0, NI_MAXSERV);

    char buf[4096];

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

        // as sub-strings separadas vão ser colocadas neste vetor
        vector<string> tokens;
        // stream de strings de input inicializado com a string a ser separada
        istringstream tokenizer {buf};
        // variável de trabalho
        string token;
        // separa as coordenadas por ponto e vírgula (;), em seguida aloca no vetor tokens[0] = coordenadas x e tokens[1] = coordenadas y
        while (getline(tokenizer, token, ';'))
        tokens.push_back(token);

        vel_ang = stof(tokens[0]);  //original vel_lin
        vel_lin = stof(tokens[1]);  //original vel_ang

        RCLCPP_INFO(node->get_logger(), "KEY PRESSED: '%f %f'",vel_lin, vel_ang);

        cmd_msg->linear.x = vel_lin;
        cmd_msg->angular.z = vel_ang;
        cmd_pub->publish(*cmd_msg);
        // std::this_thread::sleep_for(15ms);

    }

    close(listening);
    rclcpp::shutdown();

return 0;
}