#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/ssd1306.h"
#include "lib/font.h"
#include "lib/matrix.h"
#include "lib/buzzer.h"
#include "lib/button.h"
#include "lib/leds.h"
#include "lib/joystick.h"

#include "hardware/i2c.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "ws2812.pio.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

#include <cyw43_ll.h>
#include "pico/cyw43_arch.h"       
#include "lwip/pbuf.h"           
#include "lwip/tcp.h"            
#include "lwip/netif.h" 
#include <locale.h>

#define WIFI_SSID "A35 de Lucas"
#define WIFI_PASSWORD "lucaslucas"

char html[2048]; // Buffer para armazenar a resposta HTML
ssd1306_t ssd; // Variável para o display OLED SSD1306
QueueHandle_t xQueueMatrixData;
QueueHandle_t xQueueLedData;
QueueHandle_t xQueueDisplayData;
QueueHandle_t xQueueWebServerData;
uint16_t water_level_html = 0; // Nível de água
uint16_t rain_level_html = 0; // Nível de chuva

// Tarefa para ler os dados do joystick e enviar para as filas
void vJoystickTask(void *params){
    joystick_init();
    joystick_data_t data;
    while(true){
        data = joystick_read();
        xQueueSend(xQueueMatrixData, &data, 0);
        xQueueSend(xQueueLedData, &data, 0);
        xQueueSend(xQueueDisplayData, &data, 0);
        xQueueSend(xQueueWebServerData, &data, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa para controlar os LEDs RGB
void vLEDsTask(void *params){
    led_init_all();
    joystick_data_t data;
    while(true){
        if(xQueueReceive(xQueueLedData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);

            if(water_level >= 70 || rain_level >= 80){
                led_rgb_red_color();
            } else if (water_level >= 55 || rain_level >= 60){
                led_rgb_yellow_color();
            } else {
                led_rgb_green_color();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

// Tarefa para controlar o display OLED
void vDisplayTask(void *params){
    i2c_init(I2C_PORT, 400000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ADRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    joystick_data_t data;
    char water_level_str[20];
    char rain_level_str[20];
    while(true){
        if(xQueueReceive(xQueueDisplayData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);
            sprintf(water_level_str, "Water Level:%d%%", water_level);
            sprintf(rain_level_str, "Rain Level:%d%%", rain_level);
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, "Flood Station", 10, 15);
            ssd1306_draw_string(&ssd, water_level_str, 0, 25);
            ssd1306_draw_string(&ssd, rain_level_str, 0, 35);
            if(water_level >= 70 || rain_level >= 80){
                ssd1306_draw_string(&ssd, "DANGER", 35, 45);
            } else if (water_level >= 55 || rain_level >= 60){
                ssd1306_draw_string(&ssd, "WARNING", 30, 45);
            } else {
                ssd1306_draw_string(&ssd, "Safe", 40, 45);
            }
            ssd1306_send_data(&ssd);
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }   

}

// Tarefa para controlar a matriz de LEDs RGB
void vMatrixTask(void *params){
    matrix_init();
    joystick_data_t data;
    while(true){
        if(xQueueReceive(xQueueMatrixData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);
            update_matrix(water_level, rain_level);
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

// Tarefa para controlar os buzzers
void vBuzzerTask(void *params){
    buzzer_init_all();
    joystick_data_t data;
    while(true){
        if(xQueueReceive(xQueueMatrixData, &data, portMAX_DELAY)){
            uint16_t water_level = get_percentage(data.x_pos);
            uint16_t rain_level = get_percentage(data.y_pos);
            if(water_level >= 70 || rain_level >= 80){
                for(int i = 0; i < 8; i++){
                    play_buzzers();
                    vTaskDelay(pdMS_TO_TICKS(50));
                    stop_buzzers();
                    vTaskDelay(pdMS_TO_TICKS(50));
                };
                vTaskDelay(pdMS_TO_TICKS(500));
            } else {
                stop_buzzers();
            }
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

// Função de callback para o botão
void gpio_irq_handler(uint gpio, uint32_t events){
    clear_matrix();
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    reset_usb_boot(0, 0);
}

// Função para gerar o HTML da página
void user_request(char *html, size_t html_size) {
    const char *status_msg;
    const char *color;
    const char *recommendation;
    const char *reason;
    const char *image_url;

    if (water_level_html >= 70 || rain_level_html >= 80) {
        status_msg = "Perigo";
        color = "#D32F2F";
        image_url = "https://i.imgur.com/rJb8Ti0.png";
        recommendation = "Evacue a area imediatamente e procure um abrigo.";
        if (water_level_html >= 70 && rain_level_html >= 80) {
            reason = "Alertamos para risco de enchente e volume excessivo de chuva.";
        } else if (water_level_html >= 70) {
            reason = "Alertamos para risco de enchente devido ao alto nivel da agua.";
        } else {
            reason = "Alertamos para risco devido ao alto volume de chuva.";
        }
    } else if (water_level_html >= 55 || rain_level_html >= 60) {
        status_msg = "Atencao";
        color = "#FBC02D";
        image_url = "https://i.imgur.com/wfjgl8f.png";
        recommendation = "Fique atento aos alertas e preparado para abrigar-se.";
        if (water_level_html >= 55 && rain_level_html >= 60) {
            reason = "Possivel risco por aumento do nivel da agua e chuvas fortes.";
        } else if (water_level_html >= 55) {
            reason = "Possivel risco por aumento do nivel da agua.";
        } else {
            reason = "Possivel risco devido ao volume elevado de chuva.";
        }
    } else {
        status_msg = "Seguro";
        color = "#388E3C";
        image_url = "https://i.imgur.com/Hbmsm4U.png";
        recommendation = "Condicoes normais. Mantenha-se informado.";
        reason = "Sem indicativos de risco de enchente ou chuva intensa no momento.";
    }

    snprintf(html, html_size,
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
        "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<style>"
        "body{font-family:sans-serif;text-align:center;background:#e3f2fd;margin:10px;color:#0D47A1;}"
        "h1{color:#1565C0;}"
        "div.status{margin:20px auto;padding:15px;border-radius:10px;width:90%%;max-width:400px;background:white;box-shadow:0 0 10px rgba(0,0,0,0.1);}"
        ".level{font-size:1.2em;margin:10px 0;}"
        ".alert{font-weight:bold;font-size:1.5em;margin:15px 0;}"
        ".rec{font-size:1em;margin-top:10px;}"
        ".reason{font-size:0.95em;margin-top:10px;color:#0277BD;}"
        ".alert-img{width:140px;height:auto;margin:20px auto;display:block;"
        "animation: pulse 0.8s infinite alternate ease-in-out;}"
        "@keyframes pulse {"
        "  from { transform: scale(1); }"
        "  to { transform: scale(1.25); }"
        "}"
        ".footer-img{width:100%%;max-width:300px;margin:30px auto;display:block;}"
        "</style>"
        "<script>setInterval(function(){location.href='/';},1600);</script>"
        "</head><body>"

        "<div class='status'>"
        "<h1>Estacao de Alerta de Enchente</h1>"
        "<div class='level'>Nivel da Agua: %d%%</div>"
        "<div class='level'>Volume de Chuva: %d%%</div>"
        "<div class='alert' style='color:%s;'>%s</div>"
        "<img class='alert-img' src='%s' alt='Imagem de Alerta'>"
        "<div class='reason'>%s</div>"
        "<div class='rec'>%s</div>"
         "<img class='footer-img' src='https://i.imgur.com/HiwsnXV.png' alt='Imagem Extra'>"
        "</div>"

        "</body></html>",
        water_level_html, rain_level_html, color, status_msg, image_url, reason, recommendation);
}

// Função de callback para receber dados TCP
err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }
    char *request = (char *)p->payload;
    user_request(html, sizeof(html));
    
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);
    pbuf_free(p);
    return ERR_OK;
}

// Função de callback ao aceitar conexões TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err){
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

// Função para inicializar o servidor TCP
int server_init(void) {
    //Inicializa a arquitetura do cyw43
    while (cyw43_arch_init()){
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    // Ativa o Wi-Fi no modo Station, de modo a que possam ser feitas ligações a outros pontos de acesso Wi-Fi.
    cyw43_arch_enable_sta_mode();

    // Conectar à rede WiFI - fazer um loop até que esteja conectado
    printf("Conectando ao Wi-Fi...\n");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 180000)){
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
    printf("Conectado ao Wi-Fi\n");

    // Caso seja a interface de rede padrão - imprimir o IP do dispositivo.
    if (netif_default){
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Configura o servidor TCP - cria novos PCBs TCP. É o primeiro passo para estabelecer uma conexão TCP.
    struct tcp_pcb *server = tcp_new();
    if (!server){
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    //vincula um PCB (Protocol Control Block) TCP a um endereço IP e porta específicos.
    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK){
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    // Coloca um PCB (Protocol Control Block) TCP em modo de escuta, permitindo que ele aceite conexões de entrada.
    server = tcp_listen(server);

    // Define uma função de callback para aceitar conexões TCP de entrada. É um passo importante na configuração de servidores TCP.
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");
}

// Tarefa para o servidor web
void vWebServerTask(void *params){
    server_init();
    joystick_data_t data;
    while(true){
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo  
        if(xQueueReceive(xQueueWebServerData, &data, portMAX_DELAY)){
            water_level_html = get_percentage(data.x_pos);
            rain_level_html = get_percentage(data.y_pos);
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

int main(){
    stdio_init_all();
    button_init(BUTTON_A);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    srand(to_ms_since_boot(get_absolute_time()));

    xQueueMatrixData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueLedData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueDisplayData = xQueueCreate(5, sizeof(joystick_data_t));
    xQueueWebServerData = xQueueCreate(5, sizeof(joystick_data_t));

    xTaskCreate(vJoystickTask, "Joystick Task", 256, NULL, 1, NULL);
    xTaskCreate(vDisplayTask, "Display Task", 512, NULL, 1, NULL);
    xTaskCreate(vMatrixTask, "Matrix Task", 512, NULL, 1, NULL);
    xTaskCreate(vLEDsTask, "LED Task", 256, NULL, 1, NULL);
    xTaskCreate(vBuzzerTask, "Buzzer Task", 256, NULL, 1, NULL);
    xTaskCreate(vWebServerTask, "Web Server Task", 512, NULL, 1, NULL);

    vTaskStartScheduler();
    panic_unsupported();
}
