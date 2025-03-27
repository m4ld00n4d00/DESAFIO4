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
/*#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "HCF_IOTEC.h"   // Vai se tornar HCF_IOTEC
#include "HCF_LCD.h" // Vai se tornar HCF_LCD
#include "HCF_ADC.h"   // Vai se tornar HCF_ADC
#include "HCF_MP.h"   // Vai se tornar HCF_MP
// Incluir HCF_IOT HCF_BT HCF_DHT HCF_ULTRA HCF_RFID HCF_ZMPT HCF_ACS HCF_SERVO HCF_OLED HCF_CAM HCF_SD HCF_LORA


// Área das macros
//-----------------------------------------------------------------------------------------------------------------------

#define IN(x) (entradas>>x)&1

// Área de declaração de variáveis e protótipos de funções
//-----------------------------------------------------------------------------------------------------------------------

static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0; //variáveis de controle de entradas e saídas
static char tecla = '-' ;
char escrever[40];

// Funções e ramos auxiliares
//-----------------------------------------------------------------------------------------------------------------------


// Programa Principal
//-----------------------------------------------------------------------------------------------------------------------

void app_main(void)
{
    /////////////////////////////////////////////////////////////////////////////////////   Programa principal
    escrever[39] = '\0';

    // a seguir, apenas informações de console, aquelas notas verdes no início da execução
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    /////////////////////////////////////////////////////////////////////////////////////   Inicializações de periféricos (manter assim)
    
    // inicializar os IOs e teclado da placa
    iniciar_iotec();      
    entradas = io_le_escreve(saidas); // Limpa as saídas e lê o estado das entradas

    // inicializar o display LCD 
    iniciar_lcd();
    escreve_lcd(1,0,"    Jornada 1   ");
    escreve_lcd(2,0," Programa Basico");
    
    // Inicializar o componente de leitura de entrada analógica
    esp_err_t init_result = iniciar_adc_CHX(0);
    if (init_result != ESP_OK) {
        ESP_LOGE("MAIN", "Erro ao inicializar o componente ADC personalizado");
    }

    //delay inicial
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    limpar_lcd();

    /////////////////////////////////////////////////////////////////////////////////////   Periféricos inicializados

 

    /////////////////////////////////////////////////////////////////////////////////////   Início do ramo principal                    
    while (1)                                                                                                                         
    {                                                                                                                                 
        //_______________________________________________________________________________________________________________________________________________________ //
        //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - -  -  -  -  -  -  -  -  -  -  Escreva seu código aqui!!! //
        tecla = le_teclado();     
        if(tecla > '0' && tecla <= '8')
        {
            saidas = 1 << (tecla - '0' - 1);
        }            
        else if (tecla == '9')
        {
            saidas = 0xFF;
        }                                  
        else if (tecla == '0')
        {
            saidas = 0x00;
        }                                                                                                      
        
        entradas = io_le_escreve(0b00000001);

        tecla = le_teclado();     
        if(tecla = '=' )
        {
          
        
        //-  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  - -  -  -  -  -  -  -  -  -  -  Escreva seu só até aqui!!! //
        //________________________________________________________________________________________________________________________________________________________//
        vTaskDelay(10 / portTICK_PERIOD_MS);    // delay mínimo obrigatório, se retirar, pode causar reset do ESP
    }
    
    // caso erro no programa, desliga o módulo ADC
    adc_limpar();

    /////////////////////////////////////////////////////////////////////////////////////   Fim do ramo principal
    
}
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <string.h> // Necessário para strcmp()

#include "HCF_IOTEC.h"
#include "HCF_LCD.h"
#include "HCF_ADC.h"

#define TEMPO_PULSO 500 / portTICK_PERIOD_MS // 0.5s
#define SAIDA_PORTA 0x01 // Definição da saída para abertura da porta
#define SENHA_CORRETA "2222" // Senha para abrir a porta

static const char *TAG = "Sistema_Abertura_Porta";
static uint8_t saidas = 0;
char senha_digitada[5] = "";
int indice_senha = 0;
int tentativas = 0;
char escrever[40];

void app_main(void)
{
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    iniciar_iotec();
    saidas = 0;
    io_le_escreve(saidas); // Garantir que o relé inicia desligado

    iniciar_lcd();
    escreve_lcd(1, 0, "Digite a senha:");
    escreve_lcd(2, 0, "Aberturas: 0");

    while (1)
    {
        char tecla = le_teclado();
        if (tecla >= '0' && tecla <= '9' && indice_senha < 4)
        {
            senha_digitada[indice_senha++] = tecla;
            senha_digitada[indice_senha] = '\0';

            // Exibir * para cada número digitado sem sobrescrever "Tentativas"
            memset(escrever, '*', indice_senha);
            escrever[indice_senha] = '\0'; // Finaliza a string corretamente
            
            escreve_lcd(1, 0, "Senha:"); // Mantém o título fixo
            escreve_lcd(1, 7, escrever);  // Exibe os asteriscos a partir da posição 7 (evita sobrescrever "Senha:")
        }
        else if (tecla == '=' && indice_senha == 4)
        {
            tentativas++;
            sprintf(escrever, "Aberturas: %d", tentativas);
            escreve_lcd(2, 0, escrever);

            if (strcmp(senha_digitada, SENHA_CORRETA) == 0)
            {
                ESP_LOGI(TAG, "Senha correta! Abrindo as portas...");
                escreve_lcd(1, 0, "Aberturas");

                saidas = SAIDA_PORTA;
                io_le_escreve(saidas);

                vTaskDelay(TEMPO_PULSO);

                saidas = 0;
                io_le_escreve(saidas);
                escreve_lcd(1, 0, "Digite a senha:");
                escreve_lcd(2, 0, escrever); // Mantém a contagem de tentativas visível
            }
            else
            {
                ESP_LOGI(TAG, "Senha incorreta!");
                escreve_lcd(1, 0, "Senha Errada");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                escreve_lcd(1, 0, "Digite a senha:");
                escreve_lcd(2, 0, escrever); // Mantém a contagem de tentativas visível
            }
            
            indice_senha = 0;
            senha_digitada[0] = '\0';
        }
        else if (tecla == 'C')
        {
            indice_senha = 0;
            senha_digitada[0] = '\0';
            escreve_lcd(1, 0, "Digite a senha:");
            escreve_lcd(2, 0, "Tentativas: 0");
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
