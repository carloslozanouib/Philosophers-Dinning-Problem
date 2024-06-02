
//AUTOR:
//CARLOS LOZANO ALEMAÑY

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif
/*************************** Variables Globales y definiciones **************************************/
#define NUM_OF_PHILOSOPHERS (5) //Nombre de filòsofs.
//OPCIONES DEL PROGRAMA
#define DEADLOCK 0              //0 no deadlock / 1 forzamos deadlock.
#define INFINITO 1              //0 ejecución finita / 1 ejecución infinita.
#define ESPERA 200             //intérvalo de espera de vTaskDelay.
#define MENSAJES 1              //0 si no los protegemos / 1 si los protegemos.

//si número filósofos == num permitido de entrada, podrá producirse deadlock.
#if DEADLOCK
#define MAX_NUMBER_ALLOWED (NUM_OF_PHILOSOPHERS)
#else
#define MAX_NUMBER_ALLOWED (NUM_OF_PHILOSOPHERS - 1)
#endif

enum { TASK_STACK_SIZE = 2048 };  // Bytes en ESP32

//DECLARACIÓN SEMÁFOROS
static SemaphoreHandle_t entrarSala; //Para dejar o no entrar a comer a los filósofos.
static SemaphoreHandle_t palillo[NUM_OF_PHILOSOPHERS]; //Array de palillos (tantos com filósofos haya).
static SemaphoreHandle_t mensajes; //Para proteger los mensages(evitar que es superpongan).
static SemaphoreHandle_t done_all; //notificación de que todos ya han comido (caso finito), al infinito no se mostrarà.

//VARIABLES GLOBALS
char bufer[300];
int filosofos[NUM_OF_PHILOSOPHERS];
int k;

//FUNCIÓN
void filosofo(void *parameters) {

  //variables locals
  int idx_1;  //palillo izquierdo
  int idx_2;  //palillo derecho
  int numero = *(int *)parameters; //leemos el valor del paràmetro y lo guardamo a la variable local

  /////ASIGNAMOS PRIORIDAD (primero la izquierda, después derecha)
  idx_1 = numero; //Asignación palillo izquierdo del filósofo
  idx_2 = (numero + 1 ) % NUM_OF_PHILOSOPHERS; //Asignación palillo derecho del filósofo

  randomSeed(analogRead(0)); //Semilla aleatoria

  //**********BUCLE INFINITO**********
  do {
    //ACCIÓN 1: EL FILÓSOFO PIENSA
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i está pensando", numero);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif
    //tiempo que tarda pensando
    vTaskDelay(random(0, ESPERA));

    //ACCIÓN 2: EL FILÓSOFO SOLICITA ENTRAR A COMER
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i quiere entrar a comer", numero);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif

    //ACCIÓN 3: EL FILÓSOFO TIENE PERMISO PARA ENTRAR
    //tiempo que tarda para entrar
    vTaskDelay(random(0, ESPERA));
    //Se cuentan cuantos entran, una vez esté el cupo lleno el resto espera.
    xSemaphoreTake(entrarSala, portMAX_DELAY);
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i se ha sentado a comer", numero);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif

    //ACCIÓN 4: EL FILÓSOFO TOMA EL PALILLO IZQUIERDO O ESPERA A QUE PUEDA TENERLO
    //tiempo aleatorio que tarda para tomar el palillo izquierdo
    vTaskDelay(random(0, ESPERA));
    //toma el tenedor izquierdo si está libre, sino espera
    xSemaphoreTake(palillo[idx_1], portMAX_DELAY);
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i coge palillo izquierdo %i", numero, idx_1);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif

    //ACCIÓN 5: EL FILÓSOFO TOMA EL PALILLO DERECHO O ESPERA A QUE PUEDA TENERLO
    //tiempo aleatorio que tarda para tomar el palillo derecho
    vTaskDelay(random(0, ESPERA));
    //toma el tenedor derecho si está libre, sino espera
    xSemaphoreTake(palillo[idx_2], portMAX_DELAY);
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i coge palillo derecho %i", numero, idx_2);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif

    //ACCIÓN 6: EL FILÓSOFO COMIENZA A COMER
    //tiempo aleatorio que tarda para empezar a comer
    vTaskDelay(random(0, ESPERA));
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i está comiendo", numero);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif

    //ACCIÓN 7: EL FILÓSOFO HA ACABADO DE COMER
    //tiempo aleatorio que tarda en comer
    vTaskDelay(random(0, ESPERA));
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i ha acabado de comer", numero);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif

    //ACCIÓN 8: EL FILÓSOFO DEJA EL PALILLO DERECHO
    //tiempo aleatorio que tarda en dejar el palillo derecho
    vTaskDelay(random(0, ESPERA));
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i deja palillo derecho %i", numero, idx_2);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif
    //Devuelve el palillo derecho
    xSemaphoreGive(palillo[idx_1]);

    //ACCIÓN 9: EL FILÓSOFO DEJA EL PALILLO IZQUIERDO
    //tiempo aleatorio que tarda en dejar el palillo izquierdo
    vTaskDelay(random(0, ESPERA));
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i deja palillo izquierdo %i", numero, idx_1);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif
    //Devuelve el palillo izquierdo
    xSemaphoreGive(palillo[idx_2]);

    //ACCIÓN 10: EL FILÓSOFO SE VA DE LA SALA
    //tiempo aleatorio que tarda en irse de la sala
    vTaskDelay(random(0, ESPERA));
    #if(MENSAJES)
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i se va de la sala", numero);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif
    //sale de la sala
    xSemaphoreGive(entrarSala);
  } while (INFINITO); //si infinito = 1 se hará el bucle infinitas veces, si = 0 lo hará una vez

  xSemaphoreGive(done_all); //en caso de ser finito,para avisar de que todos han comido
  vTaskDelete(NULL); //en caso finito
}

void setup() {
  
  //**********PRESENTACIÓN************
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("--- SOLUCIÓN FreeRTOS COMIDA DE FILÓSOFOS---");

  //**INICIALIZAMOS SEMAFOROS**
  entrarSala = xSemaphoreCreateCounting(MAX_NUMBER_ALLOWED, MAX_NUMBER_ALLOWED); //para controlar que entren como máximo N-1 filósofos
  mensajes = xSemaphoreCreateMutex(); //para evitar que los mensajes se superpongan
  done_all = xSemaphoreCreateCounting(NUM_OF_PHILOSOPHERS, 0); //inicio=0, max = num filosofos;

  //CREACIÓN DE PALILLOS
  for (k = 0; k < NUM_OF_PHILOSOPHERS; k++) {
    palillo[k] = xSemaphoreCreateMutex();
  }
  
  //CREACIÓN DE FILÓSOFOS
  for (k = 0; k < NUM_OF_PHILOSOPHERS; k++) {
    #if(MENSAJES) //para proteger o no los mensajes                          
    xSemaphoreTake(mensajes, portMAX_DELAY);
    #endif
    sprintf(bufer, "Filósofo %i se ha creado", k);
    Serial.println(bufer);
    #if(MENSAJES)
    xSemaphoreGive(mensajes);
    #endif
    filosofos[k] = k; //asignamos con un valor a cada filósofo
    //Comienzo de la actividad de los filósofos
    xTaskCreatePinnedToCore(filosofo, bufer, TASK_STACK_SIZE, (void *)&  filosofos[k],  1, NULL, app_cpu);
  }

  //ESPERAMOS A QUE TODOS LOS FILOSOFOS HAYAN COMIDO
  for (k = 0;  k < NUM_OF_PHILOSOPHERS; k++) {
    xSemaphoreTake(done_all, portMAX_DELAY);
  }
  Serial.println("Todos los filósofos han comido, NO DEADLOCK!");
}

void loop() {

}
