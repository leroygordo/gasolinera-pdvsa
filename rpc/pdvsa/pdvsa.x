struct ticket {
  int nro_ticket;
  int ip_centro;
  string fecha<8>;
  string hora<6>;
};

program PDVSA_PROG {
  version PDVSA_VER {
    int preguntar (string) = 1;
    int responder (string) = 2;
    int validar (ticket) = 3;
    int pedir_gasolina (string) = 4;
    int pedir_tiempo () = 5;
  }=1;
}=0x20740983;
