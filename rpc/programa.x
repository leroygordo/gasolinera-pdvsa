struct estructura {
  int tiempo;
  int inventario;
};
program CENTROPROG {
  version CENTRO_VER {
  	  int pedir_gasolina (estructura) = 1;
	  int pedir_tiempo (estructura) = 2;
  }=1;
}=0x31111111;
