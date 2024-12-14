UNIVERSIDADE DE BRASÍLIA
DEPARTAMENTO DE CIÊNCIA DA COMPUTAÇÃO
SISTEMAS OPERACIONAIS - 02/2015

--= Gerente de execução postergada de processos =--

[ARQUIVOS]

-> servidor.cpp :
    Servidor de execução postergada de processos.
    Utiliza uma fila de mensagens para receber comandos dos outros executáveis.
    Guarda processos postergados numa estrutura do tipo Vector.
    Guarda processos em execução em uma estrutura do tipo Queue.

    Compilar usando: "g++ servidor.cpp -o servidor"
    Executar usando: "./servidor &"

-> executa_postergado.cpp:
    Manda ao servidor comandos para executar processos via fila de mensagens.
    Mostra o job id, caminho do executável, hora da execução e número de execuções que serão realizadas.

    Compilar usando: "g++ executa_postergado.cpp -o executa_postergado"
    Executar usando: "./executa_postergado <hora:min> <vezes> <arq_exec>"
    * Favor usar o caminho absoluto em <arq_exec>

-> shutdown_postergado.cpp
    Manda uma mensagem para o servidor parar a execução e mostrar o relatório de processos
    executados e que não serão executados.

    Compilar usando: "g++ shutdown_postergado.cpp -o shutdown_postergado"
    Executar usando: "./shutdown_postergado"

-> lista_postergados.cpp
    Manda uma mensagem para o servidor listar os processos postergados ainda não executados.

    Compilar usando: "g++ lista_postergados.cpp -o lista_postergados"
    Executar usando: "./lista_postergados"

-> remove_postergado.cpp
    Manda uma mensagem com um id de um job para que o servidor remova esse job da lista de postergados.

    Compilar usando: "g++ remove_postergado.cpp -o remove_postergado"
    Executar usando: "./remove_postergado <job_id>"

-> hello.cpp
    Hello World para uso com o servidor de execução postergada.

    Compilar usando: "g++ hello.cpp -o hello"
    Utilizar como argumento para o "executa_postergado"
