# Analisador Léxico em C

Este projeto é um analisador léxico implementado em C. Ele lê um código fonte de uma linguagem hipotética, identifica seus tokens e realiza uma análise básica de balanceamento de símbolos.

## Como Compilar e Executar

1.  **Compilação:**
    Você precisará de um compilador C (como GCC). No diretório raiz do projeto, execute:

    ```bash
    gcc -o analisador main.c lexico.c tokens.c memory_controller.c parse.c -Wall -Wextra -std=c99
    ```
    *(Nota: O arquivo `parse.c` é necessário para a funcionalidade de balanceamento de símbolos. Se ausente, a compilação falhará. Certifique-se de que `parse.c` com as funções de `parser.h` esteja presente, ou ajuste o comando e o `main.c` para compilar sem esta funcionalidade.)*

2.  **Execução:**
    Após a compilação, execute o analisador:

    ```bash
    ./analisador
    ```
    O programa analisará o arquivo `programa1.txt` por padrão.

## Visão Geral do Projeto

-   **Funcionalidades Principais:** Análise léxica (identificação de palavras-chave, variáveis, números, strings, operadores), gerenciamento de memória customizado, verificação de balanceamento de `()`, `[]`, `{}`.
-   **Estrutura:**
    -   `main.c`: Orquestra a execução.
    -   `lexico.c/.h`: Lógica da análise léxica.
    -   `tokens.c/.h`: Definições de tokens.
    -   `parser.c/.h`: Lógica do balanceamento de símbolos.
    -   `memory_controller.c/.h`: Gerenciamento de memória.
    -   `programa1.txt`: Código de exemplo.
-   **Fluxo Básico:** O `main.c` carrega `programa1.txt`, `lexico.c` o tokeniza linha a linha, `parser.c` verifica o balanceamento dos delimitadores, e `memory_controller.c` gerencia alocações.

## Exemplo de Código (`programa1.txt`)

```
principal {
  inteiro !variavelA = 10;
  escreva("O valor é: ", !variavelA);
}
```

**Saída Esperada (resumida):** O programa listará os tokens identificados e informará se os símbolos estão balanceados, seguido por um relatório de uso de memória.

## Participantes

- **Alexandre Arthur Picro Gonçalves** - https://github.com/Zander404
- **Monique Silva Machado** - https://github.com/moniquesmc
```
