# Computação Escalável

## Trabalho 1: Processamento de Dados de um Simulador de Rodovias

### Integrantes
 - Breno Marques Azevedo
 - Bruno Pereira Fornaro
 - Luis Fernando Laguardia
 - Vanessa Berwanger Wille
 - Vinicius Hedler
 
### Arquivos e Pastas

```
COMPUTACAO_ESCALAVEL_A1
|
└─── etl
|   |   car_classes.cpp                     → Classes auxiliares do ETL
|   |   dashboard_class.cpp                 → Classe Dashboard
|   |   dashboard_with_threads.cpp          → Dashboard com Threads
|   |   etl.cpp                             → ETL
|   |   external_service.cpp                → Serviço Externo
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

A sequência de execução do trabalho é a seguinte: criação do mundo, execução do simulador e execução do ETL. Há uma forma simplificada de fazer tudo isso. Basta executar o script `run.py`, localizado no diretório raiz do projeto:

```bash
python run.py
```

Contudo, caso essa forma não funcione, é possível executar cada parte do trabalho separadamente.

Para executar o simulador, bastar estar no diretório raiz do projeto e executar o script de criação do mundo:

```bash
python world_creator.py
```

Após a criação do mundo, basta executar o simulador:

```bash
python mock/mock.py
```

Por fim, a fim de executar o etl, é preciso estar dentro do diretório `etl`

```bash 
cd etl
g++ etl.cpp -o etl.exe
./etl.exe
```

Com isso, devemos ser capazes de executar o código desenvolvido e testar seu funcionamento, assim como ver os resultados do _dashboard_ impressos no terminal.

Vale observar que deixamos o _mock_ gerando "apenas" mil arquivos, isso é, gerando dados de mil ciclos. Isso foi mais que o suficiente para os testes no geral, mas se for desejado podemos alterar a última linha em `world.loop(1000)` para outro número de parâmetro ou simplesmente deixar `world.loop()`, sem o parâmetro de quantos arquivos gerar no máximo, para que o mock gere os dados dos ciclos até que seu processo seja encerrado (pelo terminal - geralmente com "CTRL + C").


