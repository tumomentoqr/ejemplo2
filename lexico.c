

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Definiciones de los tokens
#define TOKEN_PALABRA_RESERVADA_SI 0
#define TOKEN_PALABRA_RESERVADA_SINO 1
#define TOKEN_PALABRA_RESERVADA_MIENTRAS 2 
#define TOKEN_PALABRA_RESERVADA_PARA 3
#define TOKEN_PALABRA_RESERVADA_HACER 4
#define TOKEN_PALABRA_RESERVADA_ENTERO 5
#define TOKEN_PALABRA_RESERVADA_DECIMAL 6
#define TOKEN_PALABRA_RESERVADA_CADENA 7
#define TOKEN_PALABRA_RESERVADA_BOOLEANO 8
#define TOKEN_PALABRA_RESERVADA_VERDADERO 9
#define TOKEN_PALABRA_RESERVADA_FALSO 10
#define TOKEN_PALABRA_RESERVADA_REGRESAR 11
#define TOKEN_PALABRA_RESERVADA_ROMPER 12
#define TOKEN_PALABRA_RESERVADA_CONTINUAR 13
#define TOKEN_PALABRA_RESERVADA_ELEGIR 14
#define TOKEN_PALABRA_RESERVADA_CASO 15
#define TOKEN_PALABRA_RESERVADA_CONSTANTE 16
#define TOKEN_PALABRA_RESERVADA_FUNCION 17


#define TOKEN_IDENTIFICADOR 100
#define TOKEN_ENTERO 101
#define TOKEN_DECIMAL 102
#define TOKEN_CADENA 103
#define TOKEN_SUMA 104
#define TOKEN_RESTA 105
#define TOKEN_MULT 106
#define TOKEN_DIV 107
#define TOKEN_MOD 108
#define TOKEN_ASIGNACION 109
#define TOKEN_IGUAL 110
#define TOKEN_NOT 111
#define TOKEN_DISTINTO 112
#define TOKEN_MENOR 113
#define TOKEN_MENOR_IGUAL 114
#define TOKEN_MAYOR 115
#define TOKEN_MAYOR_IGUAL 116
#define TOKEN_AND 117
#define TOKEN_OR 118
#define TOKEN_LLAVE_IZQUIERDA 119
#define TOKEN_LLAVE_DERECHA 120
#define TOKEN_CORCHETE_IZQUIERDO 121
#define TOKEN_CORCHETE_DERECHO 122
#define TOKEN_PARENTESIS_IZQUIERDO 123
#define TOKEN_PARENTESIS_DERECHO 124
#define TOKEN_PUNTO_COMA 125
#define TOKEN_COMA 126
#define TOKEN_ERROR 127
#define TOKEN_EOF -1

// Estructura para representar un token
typedef struct {
    int tipo;
    char valor[256];
} Token;

// Lista de palabras reservadas
const char* palabras_reservadas[] = {
    "si", "sino", "mientras", "para", "hacer",
    "entero", "decimal", "cadena", "booleano", "verdadero",
    "falso", "regresar", "romper", "continuar", "elegir",
    "caso", "constante", "funcion"
};

const int tokens_palabras_reservadas[] = {
    TOKEN_PALABRA_RESERVADA_SI, TOKEN_PALABRA_RESERVADA_SINO, TOKEN_PALABRA_RESERVADA_MIENTRAS,
    TOKEN_PALABRA_RESERVADA_PARA, TOKEN_PALABRA_RESERVADA_HACER, TOKEN_PALABRA_RESERVADA_ENTERO,
    TOKEN_PALABRA_RESERVADA_DECIMAL, TOKEN_PALABRA_RESERVADA_CADENA, TOKEN_PALABRA_RESERVADA_BOOLEANO,
    TOKEN_PALABRA_RESERVADA_VERDADERO, TOKEN_PALABRA_RESERVADA_FALSO, TOKEN_PALABRA_RESERVADA_REGRESAR,
    TOKEN_PALABRA_RESERVADA_ROMPER, TOKEN_PALABRA_RESERVADA_CONTINUAR, TOKEN_PALABRA_RESERVADA_ELEGIR,
    TOKEN_PALABRA_RESERVADA_CASO, TOKEN_PALABRA_RESERVADA_CONSTANTE, TOKEN_PALABRA_RESERVADA_FUNCION
};

const int num_palabras_reservadas = 18;

// Función para verificar si una cadena es palabra reservada
int es_palabra_reservada(const char* palabra) {
    for (int i = 0; i < num_palabras_reservadas; i++) {
        if (strcmp(palabra, palabras_reservadas[i]) == 0) {
            return tokens_palabras_reservadas[i];
        }
    }
    return -1;
}

// Función para obtener el siguiente token
Token obtener_siguiente_token(FILE* fuente) {
    Token token;
    token.tipo = TOKEN_ERROR;
    token.valor[0] = '\0';

    char c;
    int i = 0;

    // Ignorar espacios en blanco
    while ((c = fgetc(fuente)) != EOF && isspace(c));

    if (c == EOF) {
        token.tipo = TOKEN_EOF;
        return token;
    }

    // Identificadores o palabras reservadas
    if (isalpha(c)) {
        do {
            token.valor[i++] = c;
            c = fgetc(fuente);
        } while (isalnum(c) || c == '_');
        ungetc(c, fuente);
        token.valor[i] = '\0';

        int tipo = es_palabra_reservada(token.valor);
        token.tipo = (tipo != -1) ? tipo : TOKEN_IDENTIFICADOR;
    }
    // Números
    else if (isdigit(c)) {
        do {
            token.valor[i++] = c;
            c = fgetc(fuente);
        } while (isdigit(c));
        ungetc(c, fuente);
        token.valor[i] = '\0';
        token.tipo = TOKEN_ENTERO;
    }
    // Operadores simples
    else if (strchr("+-*/=<>!&|{}[]();,", c)) {
        token.valor[i++] = c;
        token.valor[i] = '\0';
        switch (c) {
            case '+': token.tipo = TOKEN_SUMA; break;
            case '-': token.tipo = TOKEN_RESTA; break;
            case '*': token.tipo = TOKEN_MULT; break;
            case '/': token.tipo = TOKEN_DIV; break;
            case '=': token.tipo = TOKEN_ASIGNACION; break;
            case '{': token.tipo = TOKEN_LLAVE_IZQUIERDA; break;
            case '}': token.tipo = TOKEN_LLAVE_DERECHA; break;
            case '(': token.tipo = TOKEN_PARENTESIS_IZQUIERDO; break;
            case ')': token.tipo = TOKEN_PARENTESIS_DERECHO; break;
            case ';': token.tipo = TOKEN_PUNTO_COMA; break;
            case ',': token.tipo = TOKEN_COMA; break;
            default: token.tipo = TOKEN_ERROR; break;
        }
    } else {
        token.valor[i++] = c;
        token.valor[i] = '\0';
        token.tipo = TOKEN_ERROR;
    }

    return token;
}

// Función principal
int main() {
    FILE* fuente = fopen("codigo.txt", "r");
    if (fuente == NULL) {
        printf("Error al abrir el archivo fuente.\n");
        return 1;
    }

    Token token;
    do {
        token = obtener_siguiente_token(fuente);
        printf("Token: %-20s Tipo: %d\n", token.valor, token.tipo);
    } while (token.tipo != TOKEN_EOF);

    fclose(fuente);
    return 0;
}
