#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "sala.h"

//función para centralizar errores por stderr
void error(const char *mensaje) {
    fprintf(stderr, "Error: %s\n", mensaje);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <orden> <argumentos>\n", argv[0]);
        return 1;
    }
    char *orden = argv[1];
    //orden: crea
    if (strcmp(orden, "crea") == 0) {
        char *ruta = NULL;
        int capacidad = -1;
        int sobreescribir = 0;
        int opt;
        //reset de getopt para procesar desde el tercer argumento
        optind = 2; 
        while ((opt = getopt(argc, argv, "f:c:o")) != -1) {
            switch (opt) {
                case 'f': ruta = optarg; break;
                case 'c': capacidad = atoi(optarg); break;
                case 'o': sobreescribir = 1; break;
            }
        }
        if (!ruta || capacidad <= 0) {
            error("Faltan parametros (ruta o capacidad valida).");
            return 1;
        }
        //comprobación de existencia si no hay -o
        if (access(ruta, F_OK) == 0 && !sobreescribir) {
            error("El fichero ya existe. Usa -o para sobreescribir.");
            return 1;
        }
        if (crea_sala(capacidad) == -1) {
            error("No se pudo crear la sala.");
            return 1;
        }
        if (guarda_estado_sala(ruta) == -1) {
            perror("Error al guardar el fichero");
            return 1;
        }
    }

    //orden: estado
    else if (strcmp(orden, "estado") == 0) {
        char *ruta = NULL;
        optind = 2;
        int opt;
        while ((opt = getopt(argc, argv, "f:")) != -1) {
            if (opt == 'f') ruta = optarg;
        }
        if (!ruta) {
            error("Debe especificar la ruta con -f.");
            return 1;
        }
        int fd = open(ruta, O_RDONLY);
        if (fd == -1) {
            perror("Error abriendo fichero");
            return 1;
        }
        int cap;
        read(fd, &cap, sizeof(int));
        close(fd);
        crea_sala(cap);
        if (recupera_estado_sala(ruta) == -1) {
            error("No se pudo recuperar el estado.");
            return 1;
        }
        printf("Estado de la sala en %s (Capacidad: %d)\n", ruta, cap);
        for (int i = 1; i <= cap; i++) {
            int p = estado_asiento(i);
            if (p > 0) printf("Asiento %d: Persona %d\n", i, p);
            else printf("Asiento %d: Libre\n", i);
        }
    }

    //orden: reserva (todo o nada)
    else if (strcmp(orden, "reserva") == 0) {
        char *ruta = NULL;
        optind = 2;
        int opt;
        while ((opt = getopt(argc, argv, "f:")) != -1) {
            if (opt == 'f') ruta = optarg;
        }
        if (!ruta || optind >= argc) {
            error("Uso: misala reserva -f ruta id1 id2...");
            return 1;
        }
        //leer capacidad primero para crear sala
        int fd = open(ruta, O_RDONLY);
        if (fd == -1) { perror("Error"); return 1; }
        int cap; read(fd, &cap, sizeof(int)); close(fd);
        crea_sala(cap);
        recupera_estado_sala(ruta);
        int num_personas = argc - optind;
        if (asientos_libres() < num_personas) {
            error("No hay asientos suficientes para todas las reservas.");
            return 1;
        }
        //realizamos reservas en memoria
        for (int i = optind; i < argc; i++) {
            reserva_asiento(atoi(argv[i]));
        }
        //si llegamos aqui, guardamos el cambio persistente
        guarda_estado_sala(ruta);
    }

    //orden: anula
    else if (strcmp(orden, "anula") == 0) {
        char *ruta = NULL;
        int idx_asientos = -1;
        //buscamos los parametros manualmente por la complejidad de -asientos
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) ruta = argv[++i];
            else if (strcmp(argv[i], "-asientos") == 0) idx_asientos = i + 1;
        }
        if (!ruta || idx_asientos == -1 || idx_asientos >= argc) {
            error("Uso: misala anula -f ruta -asientos id1 id2...");
            return 1;
        }
        int fd = open(ruta, O_RDONLY);
        if (fd == -1) { perror("Error"); return 1; }
        int cap; read(fd, &cap, sizeof(int)); close(fd);
        crea_sala(cap);
        recupera_estado_sala(ruta);
        for (int i = idx_asientos; i < argc; i++) {
            int id_a = atoi(argv[i]);
            if (id_a < 1 || id_a > cap) {
                fprintf(stderr, "Error: Asiento %d no valido (fuera de rango).\n", id_a);
            } else {
                libera_asiento(id_a);
            }
        }
        guarda_estado_sala(ruta);
    }
    else {
        error("Orden desconocida.");
        return 1;
    }
    return 0;
}
