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

struct pase {
  string nombre_bomba<100>;
  ticket ticket_;
};


program PDVSA_PROG {
  version PDVSA_VER {
    string preguntar (string) = 1;
    ticket responder (desafio) = 2;
    int pedir_gasolina (pase) = 3;
    int pedir_tiempo (pase) = 4;
  }=1;
}=0x31111111;
