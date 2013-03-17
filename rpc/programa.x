struct ticket {
  int nro_ticket;
  int ip_centro;
  string fecha<8>;
  string hora<6>;
};

struct desafio {
  string pregunta<32>;
  string respuesta<64>;
};


program CENTROPROG {
  version CENTRO_VER {
    string<32> preguntar (string) = 1;
    ticket responder (desafio) = 2;
    int validar (ticket) = 3;
    int pedir_gasolina (string) = 4;
    int pedir_tiempo () = 5;
  }=1;
}=0x31111111;
