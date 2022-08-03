# Aplicativo Golfinho

O aplicativo tem o objetivo de controlar os movimentos lineares e ângulares do carrinho de golfe, por meio, de um joystick que possui uma conexão via socket do android para o ROS 2. Simultaneamente, há um nó de teleoperação que publica os dados enviados pelo joystick.

## Dependências:
ROS 2 Foxy e Android Studio

## Contrua o pacote do ROS 2
Realize o download do arquivo, renomeie para "aplicativo" e envie para o diretório "home". Em seguida, abra o terminal e execute o comando:
```bash
source /opt/ros/foxy/setup.bash
cd ~/aplicativo/golfinho_app_ws
colcon build
```

# Comunicação do Adroid e ROS 2
#### Passo 1: execute o aplicativo android no seu telefone
1) Abra seu Android Studio
2) Abra o projeto em "my_places"
3) Vai para app->java->com.example.myplaces->MainActivity e desça para a linha 74 **ALTERE o IP da classe para o IP do seu computador** (você pode verificá-lo ao executar "ifconfig" no terminal)
4) Execute o aplicativo em seu telefone 

#### Passo 2: rodando o /golfinho/cmd_vel
Abra um terminal e forneça os seguintes comandos
```bash
source /opt/ros/foxy/setup.bash
cd ~/aplicativo/golfinho_app_ws
. install/local_setup.bash
ros2 run cpp_pub golfinho_teleop
```
