# Métodos Numéricos 2

Esse repositório contém código da disciplina de métodos numéricos 2.

## Estrutura do repositório

```
.
├── exemplos          - pasta de exemplos
│   └── crun            - script para rodar cada exemplo
├── lib               - implementação das estruturas e algoritmos
│   └── matrix.h        - Representação de matriz 2D
└── tests             - diretório de testes unitários
    ├── matrix_test.c   - testes das funções de matrix.h
    ├── test            - script para rodar os testes
    └── testp           - script para rodar os testes em múltiplos compiladores
```

Cada algoritmo vai ser implementado como um arquivo dentro de `lib`. Por exemplo,
`01_subst.h` implementa o algoritmo de substituição e algumas funções auxiliares.
A numeração `01` respeita a ordem do cronograma da disciplina.

O arquivo `matrix.h` deve conter apenas funções básicas,
os arquivos numerados de acordo com o cronograma devem ser bem separados visando
a avaliação de cada um individualmente.

Algoritmos podem depender um do outro livremente. Um exemplo pode incluir múltiplos algoritmos.

## Como testar o código?

Cada exemplo dentro da pasta `exemplos` pode ser rodado com o script `crun`.
Os testes dentro de `tests` podem ser rodados com o script `test` ou `testp`,
o arquivo `testp` testa se o código roda com vários compiladores em vários
niveis de otimização (pra pegar UB, principalmente).

Importante: o script `crun` e os testes rodam com `-fsanitize=address,undefined`
que precisam das libs `libasan` e `libubsan`, se não estiver instalado,
é só remover essa linha antes de rodar.
