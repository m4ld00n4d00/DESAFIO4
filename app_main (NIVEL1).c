//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       _              //
//               _    _       _      _        _     _   _   _    _   _   _        _   _  _   _          //
//           |  | |  |_| |\| |_| |\ |_|   |\ |_|   |_| |_| | |  |   |_| |_| |\/| |_| |  |_| | |   /|    //    
//         |_|  |_|  |\  | | | | |/ | |   |/ | |   |   |\  |_|  |_| |\  | | |  | | | |_ | | |_|   _|_   //
//                                                                                       /              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   Programa básico para controle da placa durante a Jornada da Programação 1
*   Permite o controle das entradas e saídas digitais, entradas analógicas, display LCD e teclado. 
*   Cada biblioteca pode ser consultada na respectiva pasta em componentes
*   Existem algumas imagens e outros documentos na pasta Recursos
*   O código principal pode ser escrito a partir da linha 86
*/

// Área de inclusão das bibliotecas
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <string.h>

#define RELE_GPIO GPIO_NUM_2
#define PULSO_TEMPO 500 // Tempo do pulso em milissegundos

void abrirPorta();

void app_main() {
    char input[10];
    printf("IHM: Aperte =\n");

    gpio_set_direction(RELE_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_level(RELE_GPIO, 0);

    while (1) {
        fgets(input, sizeof(input), stdin); // Lê entrada do usuário

        if (input[0] == '=') {
            abrirPorta();
        } else {
            printf("Tecla inválida! Aperte '=' para abrir a porta.\n");
        }
    }
}

void abrirPorta() {
    printf("Abrindo porta...\n");
    gpio_set_level(RELE_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(PULSO_TEMPO));
    gpio_set_level(RELE_GPIO, 0);
    printf("Porta abrida\n");
}