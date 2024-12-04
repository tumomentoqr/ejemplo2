#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definición de tipos de tokens
typedef enum {
    TOKEN_PALABRA_RESERVADA,
    TOKEN_IDENTIFICADOR,
    TOKEN_LITERAL_ENTERO,
    TOKEN_LITERAL_DECIMAL,
    TOKEN_LITERAL_CADENA,
    TOKEN_OPERADOR,
    TOKEN_DELIMITADOR,
    TOKEN_ERROR,
    TOKEN_FIN_ARCHIVO
} TipoToken;

typedef struct {
    char valor[100];
    TipoToken tipo;
} Token;

// Palabras reservadas
const char *palabras_reservadas[] = {
    "si", "sino", "mientras", "entero", "decimal", "cadena", "retorno",
    NULL
};

// Operadores válidos
const char *operadores[] = {
    "+", "-", "*", "/", "%", "++", "--",
    "==", "!=", "<", "<=", ">", ">=", "&&", "||", "=", "&", "|", "^", "~",
    NULL
};

// Delimitadores válidos
const char delimitadores[] = "{}[]();,";

// Funciones auxiliares

// Verifica si una palabra es reservada
int es_palabra_reservada(const char *palabra) {
    for (int i = 0; palabras_reservadas[i] != NULL; i++) {
        if (strcmp(palabra, palabras_reservadas[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Verifica si es un operador
int es_operador(const char *cadena) {
    for (int i = 0; operadores[i] != NULL; i++) {
        if (strcmp(cadena, operadores[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Verifica si es un delimitador
int es_delimitador(char c) {
    return strchr(delimitadores, c) != NULL;
}

// Analizador léxico
Token siguiente_token(FILE *archivo) {
    Token token;
    token.valor[0] = '\0';
    token.tipo = TOKEN_ERROR;

    int c = fgetc(archivo);

    // Ignorar espacios en blanco y saltos de línea
    while (isspace(c)) {
        c = fgetc(archivo);
    }

    if (c == EOF) {
        token.tipo = TOKEN_FIN_ARCHIVO;
        return token;
    }

    // Identificar delimitadores
    if (es_delimitador(c)) {
        token.valor[0] = c;
        token.valor[1] = '\0';
        token.tipo = TOKEN_DELIMITADOR;
        return token;
    }

    // Identificar operadores
    char operador[3] = {c, '\0', '\0'};
    int siguiente = fgetc(archivo);
    operador[1] = siguiente;

    if (es_operador(operador)) {
        strcpy(token.valor, operador);
        token.tipo = TOKEN_OPERADOR;
        return token;
    } else {
        ungetc(siguiente, archivo);
        operador[1] = '\0';
        if (es_operador(operador)) {
            strcpy(token.valor, operador);
            token.tipo = TOKEN_OPERADOR;
            return token;
        }
    }

    // Identificar números
    if (isdigit(c)) {
        int i = 0;
        int es_decimal = 0;
        do {
            if (c == '.') es_decimal = 1;
            token.valor[i++] = c;
            c = fgetc(archivo);
        } while (isdigit(c) || (c == '.' && !es_decimal));
        token.valor[i] = '\0';
        ungetc(c, archivo);
        token.tipo = es_decimal ? TOKEN_LITERAL_DECIMAL : TOKEN_LITERAL_ENTERO;
        return token;
    }

    // Identificar cadenas
    if (c == '"') {
        int i = 0;
        c = fgetc(archivo);
        while (c != '"' && c != EOF) {
            token.valor[i++] = c;
            c = fgetc(archivo);
        }
        if (c == '"') {
            token.valor[i] = '\0';
            token.tipo = TOKEN_LITERAL_CADENA;
        } else {
            token.tipo = TOKEN_ERROR;
        }
        return token;
    }

    // Identificar identificadores y palabras reservadas
    if (isalpha(c) || c == '_') {
        int i = 0;
        do {
            token.valor[i++] = c;
            c = fgetc(archivo);
        } while (isalnum(c) || c == '_');
        token.valor[i] = '\0';
        ungetc(c, archivo);
        token.tipo = es_palabra_reservada(token.valor) ? TOKEN_PALABRA_RESERVADA : TOKEN_IDENTIFICADOR;
        return token;
    }

    // Si nada coincide, es un error
    token.valor[0] = c;
    token.valor[1] = '\0';
    token.tipo = TOKEN_ERROR;
    return token;
}

// Validar balanceo de delimitadores y punto y coma
int validar_codigo(FILE *archivo) {
    char pila[1000];
    int tope = -1;
    int requiere_punto_y_coma = 0; // Bandera para validar el punto y coma

    rewind(archivo);
    Token token = siguiente_token(archivo);
    while (token.tipo != TOKEN_FIN_ARCHIVO) {
        if (token.tipo == TOKEN_DELIMITADOR) {
            if (strchr("{[(", token.valor[0])) {
                pila[++tope] = token.valor[0];
                requiere_punto_y_coma = 0; // No se requiere punto y coma después de un bloque
            } else if (strchr("}])", token.valor[0])) {
                if (tope == -1) {
                    return 0; // Error: delimitador de cierre sin apertura
                }
                char esperado = pila[tope--];
                if ((token.valor[0] == '}' && esperado != '{') ||
                    (token.valor[0] == ']' && esperado != '[') ||
                    (token.valor[0] == ')' && esperado != '(')) {
                    return 0; // Error: delimitador de cierre no coincide
                }
                requiere_punto_y_coma = 1; // Bloque cerrado, puede requerir punto y coma
            } else if (token.valor[0] == ';') {
                requiere_punto_y_coma = 0; // Punto y coma correcto
            }
        } else if (token.tipo == TOKEN_IDENTIFICADOR || token.tipo == TOKEN_LITERAL_ENTERO || token.tipo == TOKEN_LITERAL_DECIMAL || token.tipo == TOKEN_LITERAL_CADENA) {
            requiere_punto_y_coma = 1; // Identificador o literal que requiere punto y coma
        }

        token = siguiente_token(archivo);
    }

    return (tope == -1 && !requiere_punto_y_coma); // La pila debe estar vacía y no faltar punto y coma
}

int main() {
    FILE *archivo = fopen("codigo.txt", "r");
    if (!archivo) {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    int resultado = validar_codigo(archivo);
    if (resultado) {
        printf("El código es válido.\n");
    } else {
        printf("El código contiene errores  o falta de punto y coma.\n");
    }

    fclose(archivo);
    return 0;
}
