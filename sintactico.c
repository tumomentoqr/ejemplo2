#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Definición de tipos de tokens
typedef enum
{
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

typedef struct
{
    char valor[100];
    TipoToken tipo;
} Token;

// Estructura para manejar variables declaradas
typedef struct
{
    char nombre[100];
    char tipo[20];
} Variable;

Variable variables_declaradas[100];
int num_variables = 0;

// Palabras reservadas
const char *palabras_reservadas[] = {
    "si", "sino", "mientras", "entero", "decimal", "cadena", "retorno",
    NULL};

// Operadores válidos
const char *operadores[] = {
    "+", "-", "*", "/", "%", "++", "--",
    "==", "!=", "<", "<=", ">", ">=", "&&", "||", "=", "&", "|", "^", "~",
    NULL};

// Delimitadores válidos
const char delimitadores[] = "{}[]();,";

// Verifica si una palabra es reservada
int es_palabra_reservada(const char *palabra)
{
    for (int i = 0; palabras_reservadas[i] != NULL; i++)
    {
        if (strcmp(palabra, palabras_reservadas[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Verifica si es un operador
int es_operador(const char *cadena)
{
    for (int i = 0; operadores[i] != NULL; i++)
    {
        if (strcmp(cadena, operadores[i]) == 0)
        {
            return 1;
        }
    }
    return 0;
}

// Verifica si es un delimitador
int es_delimitador(char c)
{
    return strchr(delimitadores, c) != NULL;
}

// Verifica si es un tipo de dato válido
int es_tipo_dato(const char *palabra)
{
    return (strcmp(palabra, "entero") == 0 ||
            strcmp(palabra, "decimal") == 0 ||
            strcmp(palabra, "cadena") == 0);
}

// Analizador léxico
Token siguiente_token(FILE *archivo)
{
    Token token;
    token.valor[0] = '\0';
    token.tipo = TOKEN_ERROR;

    int c = fgetc(archivo);

    // Ignorar espacios en blanco y saltos de línea
    while (isspace(c))
    {
        c = fgetc(archivo);
    }

    if (c == EOF)
    {
        token.tipo = TOKEN_FIN_ARCHIVO;
        return token;
    }

    // Identificar delimitadores
    if (es_delimitador(c))
    {
        token.valor[0] = c;
        token.valor[1] = '\0';
        token.tipo = TOKEN_DELIMITADOR;
        return token;
    }

    // Identificar cadenas con comillas simples
    if (c == '\'')
    {
        int i = 0;
        c = fgetc(archivo);
        while (c != '\'' && c != EOF)
        {
            token.valor[i++] = c;
            c = fgetc(archivo);
        }
        if (c == '\'')
        {
            token.valor[i] = '\0';
            token.tipo = TOKEN_LITERAL_CADENA;
        }
        else
        {
            token.tipo = TOKEN_ERROR;
        }
        return token;
    }

    // Identificar operadores
    char operador[3] = {c, '\0', '\0'};
    int siguiente = fgetc(archivo);
    operador[1] = siguiente;

    if (es_operador(operador))
    {
        strcpy(token.valor, operador);
        token.tipo = TOKEN_OPERADOR;
        return token;
    }
    else
    {
        ungetc(siguiente, archivo);
        operador[1] = '\0';
        if (es_operador(operador))
        {
            strcpy(token.valor, operador);
            token.tipo = TOKEN_OPERADOR;
            return token;
        }
    }

    // Identificar números
    if (isdigit(c))
    {
        int i = 0;
        int es_decimal = 0;
        do
        {
            if (c == '.')
                es_decimal = 1;
            token.valor[i++] = c;
            c = fgetc(archivo);
        } while (isdigit(c) || (c == '.' && !es_decimal));
        token.valor[i] = '\0';
        ungetc(c, archivo);
        token.tipo = es_decimal ? TOKEN_LITERAL_DECIMAL : TOKEN_LITERAL_ENTERO;
        return token;
    }

    // Identificar identificadores y palabras reservadas
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        do
        {
            token.valor[i++] = c;
            c = fgetc(archivo);
        } while (isalnum(c) || c == '_');
        token.valor[i] = '\0';
        ungetc(c, archivo);

        if (es_palabra_reservada(token.valor))
        {
            token.tipo = TOKEN_PALABRA_RESERVADA;
        }
        else
        {
            token.tipo = TOKEN_IDENTIFICADOR;
        }
        return token;
    }

    // Si nada coincide, es un error
    token.valor[0] = c;
    token.valor[1] = '\0';
    token.tipo = TOKEN_ERROR;
    return token;
}

// Validar declaración de variable
int validar_declaracion(Token *tokens, int *pos, int num_tokens)
{
    // Verificar tipo de dato
    if (!es_tipo_dato(tokens[*pos].valor))
    {
        printf("Error: Tipo de dato inválido '%s'\n", tokens[*pos].valor);
        return 0;
    }
    char tipo[20];
    strcpy(tipo, tokens[*pos].valor);
    (*pos)++;

    // Verificar identificador
    if (*pos >= num_tokens || tokens[*pos].tipo != TOKEN_IDENTIFICADOR)
    {
        printf("Error: Se esperaba un identificador\n");
        return 0;
    }

    // Guardar variable declarada
    strcpy(variables_declaradas[num_variables].nombre, tokens[*pos].valor);
    strcpy(variables_declaradas[num_variables].tipo, tipo);
    num_variables++;

    (*pos)++;

    // Verificar operador de asignación
    if (*pos >= num_tokens || strcmp(tokens[*pos].valor, "=") != 0)
    {
        printf("Error: Se esperaba '='\n");
        return 0;
    }
    (*pos)++;

    // Verificar valor según el tipo
    if (*pos >= num_tokens)
    {
        printf("Error: Se esperaba un valor\n");
        return 0;
    }

    if (strcmp(tipo, "cadena") == 0)
    {
        if (tokens[*pos].tipo != TOKEN_LITERAL_CADENA)
        {
            printf("Error: Se esperaba una cadena\n");
            return 0;
        }
    }
    else if (strcmp(tipo, "entero") == 0)
    {
        if (tokens[*pos].tipo != TOKEN_LITERAL_ENTERO)
        {
            printf("Error: Se esperaba un entero\n");
            return 0;
        }
    }
    else if (strcmp(tipo, "decimal") == 0)
    {
        if (tokens[*pos].tipo != TOKEN_LITERAL_DECIMAL &&
            tokens[*pos].tipo != TOKEN_LITERAL_ENTERO)
        {
            printf("Error: Se esperaba un número\n");
            return 0;
        }
    }
    (*pos)++;

    // Verificar punto y coma
    if (*pos >= num_tokens || strcmp(tokens[*pos].valor, ";") != 0)
    {
        printf("Error: Se esperaba ';'\n");
        return 0;
    }
    (*pos)++;

    return 1;
}

// Validar estructura si
int validar_si(Token *tokens, int *pos, int num_tokens)
{
    // Ya verificamos que es "si", avanzamos al siguiente token
    (*pos)++;

    // Verificar paréntesis de apertura
    if (*pos >= num_tokens || strcmp(tokens[*pos].valor, "(") != 0)
    {
        printf("Error: Se esperaba '(' después de 'si'\n");
        return 0;
    }
    (*pos)++;

    // Verificar condición
    if (*pos >= num_tokens || tokens[*pos].tipo != TOKEN_IDENTIFICADOR)
    {
        printf("Error: Se esperaba un identificador en la condición\n");
        return 0;
    }
    (*pos)++;

    // Verificar operador de comparación
    if (*pos >= num_tokens || !es_operador(tokens[*pos].valor))
    {
        printf("Error: Se esperaba un operador de comparación\n");
        return 0;
    }
    (*pos)++;

    // Verificar valor de comparación
    if (*pos >= num_tokens ||
        (tokens[*pos].tipo != TOKEN_LITERAL_ENTERO &&
         tokens[*pos].tipo != TOKEN_LITERAL_DECIMAL))
    {
        printf("Error: Se esperaba un valor numérico\n");
        return 0;
    }
    (*pos)++;

    // Verificar paréntesis de cierre
    if (*pos >= num_tokens || strcmp(tokens[*pos].valor, ")") != 0)
    {
        printf("Error: Se esperaba ')'\n");
        return 0;
    }
    (*pos)++;

    // Verificar llave de apertura
    if (*pos >= num_tokens || strcmp(tokens[*pos].valor, "{") != 0)
    {
        printf("Error: Se esperaba '{'\n");
        return 0;
    }
    (*pos)++;

    // Procesar el bloque hasta encontrar la llave de cierre
    int contador_llaves = 1;
    while (*pos < num_tokens && contador_llaves > 0)
    {
        if (strcmp(tokens[*pos].valor, "{") == 0)
            contador_llaves++;
        if (strcmp(tokens[*pos].valor, "}") == 0)
            contador_llaves--;
        (*pos)++;
    }

    if (contador_llaves > 0)
    {
        printf("Error: Falta llave de cierre '}'\n");
        return 0;
    }

    return 1;
}

// Validar retorno
int validar_retorno(Token *tokens, int *pos, int num_tokens)
{
    // Ya verificamos que es "retorno", avanzamos al siguiente token
    (*pos)++;

    // Verificar identificador o literal
    if (*pos >= num_tokens ||
        (tokens[*pos].tipo != TOKEN_IDENTIFICADOR &&
         tokens[*pos].tipo != TOKEN_LITERAL_ENTERO &&
         tokens[*pos].tipo != TOKEN_LITERAL_DECIMAL))
    {
        printf("Error: Se esperaba un identificador o valor después de 'retorno'\n");
        return 0;
    }
    (*pos)++;

    // Verificar punto y coma
    if (*pos >= num_tokens || strcmp(tokens[*pos].valor, ";") != 0)
    {
        printf("Error: Se esperaba ';' después del retorno\n");
        return 0;
    }
    (*pos)++;

    return 1;
}

int validar_sintaxis(Token *tokens, int num_tokens)
{
    int pos = 0;

    printf("\n=== Iniciando validación sintáctica ===\n");
    printf("Número total de tokens: %d\n", num_tokens);

    // Imprimir tokens encontrados
    printf("\nTokens encontrados:\n");
    for (int i = 0; i < num_tokens && tokens[i].tipo != TOKEN_FIN_ARCHIVO; i++)
    {
        printf("Token[%d]: '%s' (tipo=%d)\n", i, tokens[i].valor, tokens[i].tipo);
    }
    printf("\nIniciando análisis...\n");

    while (pos < num_tokens && tokens[pos].tipo != TOKEN_FIN_ARCHIVO)
    {
        printf("Procesando token en posición %d: '%s' (tipo=%d)\n",
               pos, tokens[pos].valor, tokens[pos].tipo);

        if (es_tipo_dato(tokens[pos].valor))
        {
            if (!validar_declaracion(tokens, &pos, num_tokens))
                return 0;
        }
        else if (strcmp(tokens[pos].valor, "si") == 0)
        {
            if (!validar_si(tokens, &pos, num_tokens))
                return 0;
        }
        else if (strcmp(tokens[pos].valor, "retorno") == 0)
        {
            if (!validar_retorno(tokens, &pos, num_tokens))
                return 0;
        }
        else
        {
            printf("Error: Token inesperado '%s'\n", tokens[pos].valor);
            return 0;
        }
    }

    return 1;
}

int main()
{
    printf("=== Iniciando compilador ===\n\n");

    FILE *archivo = fopen("codigo.txt", "r");
    if (!archivo)
    {
        printf("Error al abrir el archivo.\n");
        return 1;
    }

    Token tokens[1000];
    int num_tokens = 0;
    Token token;

    // Leer todos los tokens
    do
    {
        token = siguiente_token(archivo);
        tokens[num_tokens++] = token;
    } while (token.tipo != TOKEN_FIN_ARCHIVO);

    // Validar la sintaxis
    if (validar_sintaxis(tokens, num_tokens))
    {
        printf("\nEl código es válido.\n");
    }
    else
    {
        printf("\nEl código contiene errores de sintaxis.\n");
    }

    fclose(archivo);
    return 0;
}