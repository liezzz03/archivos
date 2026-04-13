#include <stdlib.h>
#include "sala.h"

static int* asientos = NULL; // Puntero al arreglo de asientos (NULL si no hay sala creada)
static int n_asientos = 0; //  número total de asientos en la sala

//Crea una nueva sala con la capacidad especificada
//Retorna el número de asientos si se crea correctamente, -1 en caso de error
int crea_sala(int capacidad) {
    // No permitir crear una sala si ya existe una o si la capacidad es inválida
    if (asientos != NULL || capacidad < 1) return -1;
    asientos = (int*) malloc(capacidad * sizeof(int));
    if (asientos == NULL) return -1; // Error de memoria

    n_asientos = capacidad;
    // Inicializar todos los asientos como libres (-1 indica libre)
    for (int i = 0; i < n_asientos; i++) {
        asientos[i] = -1;
    }
    return n_asientos;
}

// Retorna la capacidad total de la sala, o -1 si no existe
int capacidad_sala() {
    if (asientos == NULL) return -1;
    return n_asientos;
}

// Retorna el número de asientos ocupados, o -1 si no existe sala
int asientos_ocupados() {
    if (asientos == NULL) return -1; //
    int ocupados = 0;
    for (int i = 0; i < n_asientos; i++) {
        // Asiento ocupado si no es -1
        if (asientos[i] != -1) {
            ocupados++;
        }
    }
    return ocupados;
}

// Retorna el número de asientos libres, o -1 si no existe la sala
int asientos_libres() {
    if (asientos == NULL) return -1;

    int ocupados = asientos_ocupados();
    if (ocupados == -1) return -1;

    return n_asientos - ocupados;
}

//Reserva el primer asiento libre para la persona con id_persona
//Retorna el id del asiento reservado, o -1 si falla
int reserva_asiento(int id_persona) {
    if (asientos == NULL || id_persona <= 0) return -1;

    for (int i = 0; i < n_asientos; i++) {
        if (asientos[i] == -1) { // Encontrar primer asiento libre
            asientos[i] = id_persona;
            return (i + 1); // Retornar id en rango [1, n_asientos]
        }
    }
    return -1; // Sala llena
}

// Librea el asiento especificado por id_asiento
// Retorna el id de la persona que lo ocupaba, o -1 si falla
int libera_asiento(int id_asiento) {
    if (asientos == NULL || id_asiento < 1 || id_asiento > n_asientos) return -1;

    int index = id_asiento - 1;
    if (asientos[index] == -1) return -1; // Asiento ya libre

    int id_persona = asientos[index];
    asientos[index] = -1; // Liberar asiento
    return id_persona;
}

// Libera el asiento dependiendo del id de la persona
int libera_persona(int id_persona) {
    if (asientos == NULL || id_persona <= 0) return -1;
    for (int i = 0; i < n_asientos; i++) {
        if (asientos[i] == id_persona) {
            asientos[i] = -1; // Liberamos el asiento
            return (i + 1);    // Devolvemos el número de asiento que ocupaba (1..N)
        }
    }
    return -1; // No se encontró a esa persona en la sala
}

// Elimina la sala y libera la memoria
// Retorna 0 si se elimina correctamente, -1 si no existe la sala
int elimina_sala() {
    if (asientos == NULL) return -1;
    free(asientos);
    asientos = NULL;
    n_asientos = 0;
    return 0;
}

// Retorna el estado del asiento: 0 si libre, id de persona si ocupado, -1 si error
int estado_asiento(int id_asiento) {
    if (asientos == NULL || id_asiento < 1 || id_asiento > n_asientos) return -1;
    int val = asientos[id_asiento - 1];
    return (val == -1) ? 0 : val;
}

// Reserva un asiento específico para la persona con id_persona
// Retorna el id del asiento si se reserva, -1 si falla
int reserva_asiento_especifico(int id_asiento, int id_persona) {
    if (asientos == NULL || id_persona <= 0) return -1;
    if (id_asiento < 1 || id_asiento > n_asientos) return -1;

    if (asientos[id_asiento - 1] != -1) {
        return -1; // Asiento ya ocupado
    }

    asientos[id_asiento - 1] = id_persona;
    return id_asiento;
}

// Implementación de la prueba de reserva múltiple (Todo o nada)
int reserva_multiple(int npersonas, int* lista_id) {
    // Control de robustez: npersonas <= 0 o lista nula
    if (npersonas <= 0 || lista_id == NULL) return -1;

    // REQUISITO: Todo o nada. Si no hay sitio para todos, no se sienta nadie
    if (asientos_libres() < npersonas) return -1;

    int exitos = 0;
    for (int i = 0; i < npersonas; i++) {
        if (reserva_asiento(lista_id[i]) != -1) exitos++;
    }
    return exitos;
}