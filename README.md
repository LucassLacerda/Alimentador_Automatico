# Alimentador_Automatico
Projeto que consiste numa integração entre a Google Firebase, Android Studio, uma ESP32 e um servo motor. 

O intuito do protótipo é fazer com que um servo motor gire num momento específico, definido pelo usuário no aplicativo, liberando a ração em um recipiente para alimentar o pet.

No aplicativo, o usuário define o horário no qual quer que a ração seja despejada. Esse horário então é enviado para o banco de dados da Google Firebase, que por sua vez é lido pela ESP 32, que, através do NTP, compara com o horário atul e se forem os mesmos, o motor é ativado.
