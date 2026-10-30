
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_TEXTO 100
typedef struct Cancion {
    char titulo[MAX_TEXTO];
    char artista[MAX_TEXTO];
    int duracion;             
    struct Cancion *anterior;
    struct Cancion *siguiente;
} Cancion;
typedef struct {
    Cancion *cabeza;
    Cancion *cola;
    Cancion *actual;             
    int total;
} Playlist;
static void leer_linea(const char *mensaje, char *destino, size_t tam) {
    printf("%s", mensaje);
    if (fgets(destino, (int)tam, stdin) == NULL) {
        destino[0] = '\0';
        return;
    }
    size_t len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    } else {
        int c;               
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
}
static int leer_entero(const char *mensaje) {
    char buffer[MAX_TEXTO];
    char *fin;
    long valor;
    for (;;) {
        leer_linea(mensaje, buffer, sizeof(buffer));
        valor = strtol(buffer, &fin, 10);
        if (fin != buffer && *fin == '\0' && valor >= 0 && valor < 1000000)
            return (int)valor;
        printf("  Entrada no valida. Intente de nuevo.\n");
    }
}
static int iguales_ci(const char *a, const char *b) {
    while (*a && *b) {
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
            return 0;
        a++; b++;
    }
    return *a == *b;
} 
static void imprimir_cancion(const Cancion *c, int marcar) {
    printf("  %s %s - %s (%d:%02d)\n",
           marcar ? ">" : " ",
           c->titulo, c->artista, c->duracion / 60, c->duracion % 60);
}
static void iniciar(Playlist *p) {
    p->cabeza = p->cola = p->actual = NULL;
    p->total = 0;
}
static int agregar(Playlist *p, const char *titulo, const char *artista, int duracion) {
    Cancion *nueva = (Cancion *)malloc(sizeof(Cancion));
    if (nueva == NULL) return 0;
    strncpy(nueva->titulo, titulo, MAX_TEXTO - 1);
    nueva->titulo[MAX_TEXTO - 1] = '\0';
    strncpy(nueva->artista, artista, MAX_TEXTO - 1);
    nueva->artista[MAX_TEXTO - 1] = '\0';
    nueva->duracion = duracion;
    nueva->siguiente = NULL;
    nueva->anterior = p->cola;
    if (p->cola == NULL) {
        p->cabeza = nueva;
        p->actual = nueva;      
    } else {
        p->cola->siguiente = nueva;
    }
    p->cola = nueva;
    p->total++;
    return 1;
}
static Cancion *buscar(const Playlist *p, const char *titulo) {
    for (Cancion *c = p->cabeza; c != NULL; c = c->siguiente)
        if (iguales_ci(c->titulo, titulo))
            return c;
    return NULL;
} 
static int eliminar(Playlist *p, const char *titulo) {
    Cancion *c = buscar(p, titulo);
    if (c == NULL) return 0;
    if (p->actual == c)
        p->actual = (c->siguiente != NULL) ? c->siguiente : c->anterior;
 
    if (c->anterior != NULL) c->anterior->siguiente = c->siguiente;
    else                     p->cabeza = c->siguiente;
 
    if (c->siguiente != NULL) c->siguiente->anterior = c->anterior;
    else                      p->cola = c->anterior;
    free(c);
    p->total--;
    return 1;
}
static void mostrar_adelante(const Playlist *p) {
    if (p->cabeza == NULL) { printf("  La lista esta vacia.\n"); return; }
    printf("\n  --- Lista (inicio -> fin) ---\n");
    int i = 1;
    for (Cancion *c = p->cabeza; c != NULL; c = c->siguiente, i++) {
        printf("%3d.", i);
        imprimir_cancion(c, c == p->actual);
    }
}
static void mostrar_atras(const Playlist *p) {
    if (p->cola == NULL) { printf("  La lista esta vacia.\n"); return; }
    printf("\n  --- Lista (fin -> inicio) ---\n");
    int i = p->total;
    for (Cancion *c = p->cola; c != NULL; c = c->anterior, i--) {
        printf("%3d.", i);
        imprimir_cancion(c, c == p->actual);
    }
} 
static void siguiente_cancion(Playlist *p) {
    if (p->actual == NULL) { printf("  La lista esta vacia.\n"); return; }
    if (p->actual->siguiente == NULL) {
        printf("  Ya esta en la ultima cancion.\n");
        return;
    }
    p->actual = p->actual->siguiente;
} 
static void cancion_anterior(Playlist *p) {
    if (p->actual == NULL) { printf("  La lista esta vacia.\n"); return; }
    if (p->actual->anterior == NULL) {
        printf("  Ya esta en la primera cancion.\n");
        return;
    }
    p->actual = p->actual->anterior;
}
static void liberar(Playlist *p) {
    Cancion *c = p->cabeza;
    while (c != NULL) {
        Cancion *sig = c->siguiente;
        free(c);
        c = sig;
    }
    iniciar(p);
}
static void menu_navegar(Playlist *p) {
    int op;
    do {
        printf("\n  === Reproduciendo ===\n");
        if (p->actual != NULL) imprimir_cancion(p->actual, 1);
        else printf("  (lista vacia)\n");
        printf("  1. Siguiente\n  2. Anterior\n  3. Ir al inicio\n"
               "  4. Ir al final\n  0. Volver\n");
        op = leer_entero("  Opcion: ");
        switch (op) {
            case 1: siguiente_cancion(p); break;
            case 2: cancion_anterior(p); break;
            case 3: if (p->cabeza) p->actual = p->cabeza; break;
            case 4: if (p->cola)   p->actual = p->cola;   break;
            case 0: break;
            default: printf("  Opcion invalida.\n");
        }
    } while (op != 0);
}
static void menu_principal(void) {
    Playlist lista;
    char titulo[MAX_TEXTO], artista[MAX_TEXTO];
    int op;
    iniciar(&lista);
    do {
        printf("\n===== LISTA DE REPRODUCCION (%d canciones) =====\n", lista.total);
        printf("1. Agregar cancion\n");
        printf("2. Eliminar cancion\n");
        printf("3. Mostrar hacia adelante\n");
        printf("4. Mostrar hacia atras\n");
        printf("5. Buscar cancion por titulo\n");
        printf("6. Navegar entre canciones\n");
        printf("0. Salir\n");
        op = leer_entero("Opcion: ");
        switch (op) {
            case 1: {
                leer_linea("  Titulo: ", titulo, sizeof(titulo));
                if (titulo[0] == '\0') { printf("  El titulo no puede estar vacio.\n"); break; }
                leer_linea("  Artista: ", artista, sizeof(artista));
                int dur = leer_entero("  Duracion (segundos): ");
                if (agregar(&lista, titulo, artista, dur))
                    printf("  Cancion agregada.\n");
                else
                    printf("  Error: sin memoria.\n");
                break;
            }
            case 2:
                leer_linea("  Titulo a eliminar: ", titulo, sizeof(titulo));
                if (eliminar(&lista, titulo)) printf("  Cancion eliminada.\n");
                else printf("  No se encontro la cancion.\n");
                break;
            case 3: mostrar_adelante(&lista); break;
            case 4: mostrar_atras(&lista); break;
            case 5: {
                leer_linea("  Titulo a buscar: ", titulo, sizeof(titulo));
                Cancion *c = buscar(&lista, titulo);
                if (c != NULL) {
                    printf("  Encontrada:\n");
                    imprimir_cancion(c, 0);
                    lista.actual = c;    
                } else {
                    printf("  No se encontro la cancion.\n");
                }
                break;
            }
            case 6: menu_navegar(&lista); break;
            case 0: printf("Hasta luego.\n"); break;
            default: printf("  Opcion invalida.\n");
        }
    } while (op != 0);
    liberar(&lista);
} 
int main(void) {
    menu_principal();
    return 0;
}