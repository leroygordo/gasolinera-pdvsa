struct ticket {
  int nro_ticket;
  int ip_centro;
  string fecha<8>;
  string hora<6>;
};
program CENTROPROG {
  version CENTRO_VER {
    string preguntar (string) = 1;
    ticket responder (string) = 2;
    int validar (ticket) = 3;
    int pedir_gasolina (string) = 4;
    int pedir_tiempo () = 5;
  }=1;
}=0x31111111;
