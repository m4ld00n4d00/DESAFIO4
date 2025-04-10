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
#include "esp_log.h"
#include <string.h> // Necessário para strcmp()

#include "HCF_IOTEC.h"
#include "HCF_LCD.h"
#include "HCF_ADC.h"

#define TEMPO_PULSO (500 / portTICK_PERIOD_MS) // 0.5s
#define SAIDA_PORTA 0x01 // Definição da saída para abertura da porta
#define SENHA_CORRETA "1407" // Senha para abrir a porta
#define MAX_ERROS 3 // Número máximo de erros seguidos antes do bloqueio

static const char *TAG = "Sistema_Abertura_Porta";
static uint8_t saidas = 0;
char senha_digitada[5] = "";
int indice_senha = 0;
int aberturas = 0; // Contagem de aberturas bem-sucedidas
int erros_seguidos = 0; // Contador de erros seguidos
bool bloqueado = false; // Indica se o sistema está bloqueado
char escrever[40];

void limpar_display_senha()
{
    escreve_lcd(1, 7, "    "); // Apaga os caracteres da senha
}

void app_main(void)
{
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    iniciar_iotec();
    saidas = 0;
    io_le_escreve(saidas); // Garantir que o relé inicia desligado

    iniciar_lcd();
    escreve_lcd(1, 0, "Digite a senha:");
    sprintf(escrever, "Aberturas: %d", aberturas);
    escreve_lcd(2, 0, escrever);

    while (1)
    {
        if (bloqueado)
        {
            escreve_lcd(1, 0, "SISTEMA BLOQUEADO!");
            ESP_LOGI(TAG, "Sistema bloqueado por excesso de erros seguidos!");
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue;
        }

        char tecla = le_teclado();
       
        if (tecla >= '0' && tecla <= '9' && indice_senha < 4)
        {
            senha_digitada[indice_senha++] = tecla;
            senha_digitada[indice_senha] = '\0';

            memset(escrever, '*', indice_senha);
            escrever[indice_senha] = '\0';
           
            escreve_lcd(1, 0, "Senha:");
            escreve_lcd(1, 7, escrever);
        }
        else if (tecla == '=' && indice_senha == 4)
        {
            if (strcmp(senha_digitada, SENHA_CORRETA) == 0)
            {
                aberturas++;
                ESP_LOGI(TAG, "Senha correta! Abrindo as portas...");
                escreve_lcd(1, 0, "Portas Abertas");

                saidas = SAIDA_PORTA;
                io_le_escreve(saidas);

                vTaskDelay(TEMPO_PULSO);

                saidas = 0;
                io_le_escreve(saidas);

                erros_seguidos = 0; // Reseta a contagem de erros após sucesso
            }
            else
            {
                erros_seguidos++;
                ESP_LOGI(TAG, "Senha incorreta!");
                escreve_lcd(1, 0, "Senha Errada");
                vTaskDelay(1000 / portTICK_PERIOD_MS);

                if (erros_seguidos >= MAX_ERROS)
                {
                    bloqueado = true;
                    continue;
                }
            }
           
            sprintf(escrever, "Aberturas: %d", aberturas);
            escreve_lcd(2, 0, escrever);

            indice_senha = 0;
            senha_digitada[0] = '\0';
            escreve_lcd(1, 0, "Digite a senha:");
        }
        else if (tecla == 'C' && indice_senha > 0)
        {
            indice_senha--;
            senha_digitada[indice_senha] = '\0';

            limpar_display_senha();

            memset(escrever, '*', indice_senha);
            escrever[indice_senha] = '\0';

            escreve_lcd(1, 0, "Senha:");
            escreve_lcd(1, 7, escrever);
        }

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}