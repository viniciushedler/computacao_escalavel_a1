# Computação Escalável

## Trabalho 1: Processamento de Dados de um Simulador de Rodovias

### Arquivos e Pastas

```
COMPTACAO_ESCALAVEL_A1
|
└─── etl
|   |   car_classes.cpp                     → (?)
|   |   dashboard_class.cpp                 → (?)
|   |   etl.cpp                             → (?)
|   |   external_service.cpp                → (?)
|
└─── report
|   |   report.pdf                          → Relatório do trabalho
|   |   report.tex                          → Arquivo fonte do relatório
|   |   ***(arquivos auxiliares do LaTeX)
|
└─ mock.py                                  → Simulador de Rodovias
|
└─ parameters.py                            → Parâmetros do simulador
|
└─ progressive.py                           → (?)
|
└─ world_creator.py                         → (?)
```

### Execução

Para executar o simulador, bastar estar no diretório raiz do projeto e executar o comando:

```bash
python mock.py
```

Para atualizar o ciclo do simulador, basta apertar a tecla ENTER. Para sair do simulador, basta apertar a tecla ESC (?).

Além disso, é possível definir a quantidade de ciclos e carros via CLI, como no exemplo abaixo:

(?????)
```bash
python mock.py --cycles 100 --cars 1000
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
 - [] ETL
 - [] Dashboard
 - [-] Problemas e Soluções
 - [] Conclusão


### Integrantes
 - Breno Marques Azevedo
 - Bruno Pereira Fornaro
 - Luis Fernando Laguardia
 - Vanessa Berwanger Wille
 - Vinicius Hedler
