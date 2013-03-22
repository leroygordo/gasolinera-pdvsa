struct ticket {
  int nro_ticket;
  string hostname<50>;
  string fecha<8>;
  string hora<6>;
  string nombre_bomba<100>;
};

struct desafio {
  string pregunta<32>;
  string respuesta<64>;
  string nombre_bomba<100>;
};

program PDVSA_PROG {
  version PDVSA_VER {
    string preguntar (string) = 1;
    ticket responder (desafio) = 2;
    int pedir_gasolina (ticket) = 3;
    int pedir_tiempo (ticket) = 4;
  }=1;
}=0x31111111;
