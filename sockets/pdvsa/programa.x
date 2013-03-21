struct ticket {
  int nro_ticket;
  string nombre_centro<50>;
  string hora<6>;
  string nombre_bomba<50>;
};

struct desafio {
  string pregunta<32>;
  string respuesta<64>;
  string nombre_bomba<50>;
};

program PROGRAMA_PROG {
  version PROGRAMA_VER {
    string preguntar (string) = 1;
    ticket responder (desafio) = 2;
    int pedir_gasolina (ticket) = 3;
    int pedir_tiempo (ticket) = 4;
  }=1;
}=0x31111111;
