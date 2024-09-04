#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LEN 100
FILE *file;

typedef enum {
  TOKEN_INT,        // int
  TOKEN_IDENTIFIER, // [a-zA-Z]
  TOKEN_NUMBER,     // [0-9]+
  TOKEN_ASSIGN,     // =
  TOKEN_PLUS,       // +
  TOKEN_MINUS,      // -
  TOKEN_IF,         // if
  TOKEN_EQUAL,      // ==
  TOKEN_LBRACE,     // {
  TOKEN_RBRACE,     // }
  TOKEN_LPAREN,     // (
  TOKEN_RPAREN,     // )
  TOKEN_SEMICOLON,  // ;
  TOKEN_UNKNOWN,
  TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  char text[MAX_TOKEN_LEN];
} Token;
Token current_token;

Token getNextToken() {
  Token token;
  int c;
  while ((c = fgetc(file)) != EOF) {
    if (isspace(c))
      continue;

    if (isalpha(c)) {
      int len = 0;
      token.text[len++] = c;
      while (isalnum(c = fgetc(file))) {
        if (len < MAX_TOKEN_LEN - 1)
          token.text[len++] = c;
      }
      ungetc(c, file);
      token.text[len] = '\0';
      if (strcmp(token.text, "int") == 0)
        token.type = TOKEN_INT;
      else if (strcmp(token.text, "if") == 0)
        token.type = TOKEN_IF;
      else
        token.type = TOKEN_IDENTIFIER;
    }

    if (isdigit(c)) {
      int len = 0;
      token.text[len++] = c;
      while (isdigit(c = fgetc(file))) {
        if (len < MAX_TOKEN_LEN - 1)
          token.text[len++] = c;
      }
      ungetc(c, file);
      token.text[len] = '\0';
      token.type = TOKEN_NUMBER;
    }

    switch (c) {
    case '=':
      if ((c = fgetc(file)) == '=') {
        token.type = TOKEN_EQUAL;
        token.text[0] = '=';
        token.text[1] = '=';
        token.text[2] = '\0';
      } else {
        ungetc(c, file);
        token.type = TOKEN_ASSIGN;
        token.text[0] = '=';
        token.text[1] = '\0';
      }
      break;
    case '{':
      token.type = TOKEN_LBRACE;
      token.text[0] = '{';
      token.text[1] = '\0';
      break;
    case '}':
      token.type = TOKEN_RBRACE;
      token.text[0] = '}';
      token.text[1] = '\0';
      break;
    case '+':
      token.type = TOKEN_PLUS;
      token.text[0] = '+';
      token.text[1] = '\0';
      break;
    case '-':
      token.type = TOKEN_MINUS;
      token.text[0] = '-';
      token.text[1] = '\0';
      break;
    case '(':
      token.type = TOKEN_LPAREN;
      token.text[0] = '(';
      token.text[1] = '\0';
      break;
    case ')':
      token.type = TOKEN_RPAREN;
      token.text[0] = ')';
      token.text[1] = '\0';
      break;
    case ';':
      token.type = TOKEN_SEMICOLON;
      token.text[0] = ';';
      token.text[1] = '\0';
      break;
    }
    printf("Token: %d, Text: %s\n", token.type, token.text);
    return token;
  }
  token.type = TOKEN_EOF;
  token.text[0] = '\0';
  printf("Token: %d, Text: %s\n", token.type, token.text);
  return token;
}

void parse_declaration();
void parse_assignment();
void parse_if_statement();
void parse_expression();
void parse_condtion();
void parse_term();

void match(TokenType expected_token) {
  if (current_token.type != expected_token) {
    printf("Expected token %d, but got %d\n", expected_token,
           current_token.type);
    exit(1);
  } else
    current_token = getNextToken();
}

void parse_statement() {
  if (current_token.type == TOKEN_INT)
    parse_declaration();
  else if (current_token.type == TOKEN_IDENTIFIER)
    parse_assignment();
  else if (current_token.type == TOKEN_IF)
    parse_if_statement();
  else if (current_token.type == TOKEN_UNKNOWN) {
    printf("Invalid Token Type");
    exit(1);
  }
}

void parse_declaration() {
  match(TOKEN_INT);
  match(TOKEN_IDENTIFIER);
  match(TOKEN_SEMICOLON);
}

void parse_assignment() {
  match(TOKEN_IDENTIFIER);
  match(TOKEN_ASSIGN);
  parse_expression();
}

void parse_if_statement() {
  match(TOKEN_IF);
  match(TOKEN_LPAREN);
  parse_condtion();
  match(TOKEN_RPAREN);
  match(TOKEN_LBRACE);
  parse_statement();
  match(TOKEN_RBRACE);
}

void parse_expression() {
  parse_term();
  if (current_token.type != TOKEN_SEMICOLON) {
    if (current_token.type == TOKEN_MINUS)
      match(TOKEN_MINUS);
    else if (current_token.type == TOKEN_PLUS)
      match(TOKEN_PLUS);
    else {
      printf("Unexpected token");
    }
    parse_term();
  }
  match(TOKEN_SEMICOLON);
}

void parse_condtion() {
  match(TOKEN_IDENTIFIER);
  match(TOKEN_EQUAL);
  parse_expression();
}

void parse_term() {
  if (current_token.type == TOKEN_IDENTIFIER)
    match(TOKEN_IDENTIFIER);
  else if (current_token.type == TOKEN_NUMBER)
    match(TOKEN_NUMBER);
  else {
    printf("Unexpected Token");
    exit(1);
  }
}

int main() {
  file = fopen("code_input.slang", "r");
  if (!file) {
    perror("Failed to open file");
    return 1;
  }
  current_token = getNextToken();
  parse_statement();

  fclose(file);
  return 0;
}
