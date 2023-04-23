# Computação Escalável

## Trabalho 1: Processamento de Dados de um Simulador de Rodovias

### Arquivos e Pastas

```
COMPUTACAO_ESCALAVEL_A1
|
└─── etl
|   |   car_classes.cpp                     → (?)
|   |   dashboard_class.cpp                 → (?)
|   |   dashboard_with_threadschpp          → (?)
|   |   etl.cpp                             → (?)
|   |   external_service.cpp                → (?)
|
└─── report
|   |   report.pdf                          → Relatório do trabalho
|   |   report.tex                          → Arquivo fonte do relatório
|   |   ***(arquivos auxiliares do LaTeX)
|
└─── mock.py                                → Simulador de Rodovias
|   |   ansi.py                             → Classe de cores
|   |   mock.py                             → Scrip do Simulador
|   |   parameters.py                       → Parâmetros do simulador
|
└─ dashboard_test.cpp                       → Teste do Dashboard
|
└─ world_creator.py                         → Criação do mundo
```

### Execução

Para executar o simulador, bastar estar no diretório raiz do projeto e executar o script de criação do mundo:

```bash
python world_creator.py
```

Após a criação do mundo, basta executar o simulador:

```bash
python mock/mock.py
```


### Avaliação
 - Modelagem geral do sistema.
 - Identificação e solução dos problemas de concorrência.
 - Relatório contendo a modelagem, os problemas/soluções e demais decisões de projeto.
 - Eficiência do ETL considerando as características de paralelismo e concorrência.
 - Solução para o acesso ao serviço externo legado.
 - Solução para a diferença de prioridade entre análises.
 - Projeto e implementação dos mocks.
 - Organização do projeto e qualidade do código.
 - Manual de instruções.
### Relatório
 - [X] Modelagem do Trabalho
 - [X] Mock
 - [X] ETL
 - [] Dashboard
 - [-] Relatório
 - [-] Problemas e Soluções
 - [] Conclusão


### Integrantes
 - Breno Marques Azevedo
 - Bruno Pereira Fornaro
 - Luis Fernando Laguardia
 - Vanessa Berwanger Wille
 - Vinicius Hedler
