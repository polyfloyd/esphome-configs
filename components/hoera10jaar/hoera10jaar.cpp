// ESPHome port of https://github.com/Juerd/hoera10jaar

#include "hoera10jaar.h"
#include <Arduino.h>
#include <esp_task_wdt.h>

namespace esphome {
namespace hoera10jaar {

const int    numrows = 5;
const int    numcols = 6;
const int    rows[numrows] = { 27, 26, 32, 33, 25 };  // +
const int    cols[numcols] = { /* rood */ 4, 17, 18, /* groen */ 16, 5, 19 };  // n fet

const int    numleds = 30;
int          leds[numleds];
int          current[numleds];
const int    OFF = 0;
const int    ON_R = 84;
const int    ON_G = 64;
const int    ON_YR = 84;
const int    ON_YG = 30;
int          fade_interval = 5;
int          waitstep = 10;
int          wait = -14 * waitstep;


bool fade();
void wait_fade();
void all(int ms, bool red, bool green);
void led_matrix_loop(void *param);


void Hoera10JaarComponent::setup() {
    xTaskCreate(
        led_matrix_loop, /* Function to implement the task */
        "hoera10jaar",   /* Name of the task */
        4096,            /* Stack size in words */
        this,            /* Task input parameter */
        1,               /* Priority of the task */
        NULL             /* Task handle. */
    );
}

void Hoera10JaarComponent::set_led(const std::string &name, State state) {
    int lednr = -1;
    if (     name == "leeuwarden") lednr = 0;
    else if (name == "amsterdam")  lednr = 1;
    else if (name == "utrecht")    lednr = 2;
    else if (name == "denhaag")    lednr = 3;
    else if (name == "rotterdam")  lednr = 4;
    else if (name == "zwolle")     lednr = 5;
    else if (name == "amersfoort") lednr = 6;
    else if (name == "arnhem")     lednr = 7;
    else if (name == "wageningen") lednr = 8;
    else if (name == "eindhoven")  lednr = 9;
    // 10 is niet aangesloten
    else if (name == "enschede")   lednr = 11;
    else if (name == "nijmegen")   lednr = 12;
    else if (name == "venlo")      lednr = 13;
    else if (name == "heerlen")    lednr = 14;

    if (lednr == -1) return;

    int minus = 0;
    if (!leds[lednr] && !leds[lednr+15] && wait < 0) { minus = wait; wait += waitstep; }

    if (state == CLOSED) {
        leds[lednr]      = ON_R;
        leds[lednr + 15] = OFF;
    } else if (state == OPEN) {
        leds[lednr]      = OFF;
        leds[lednr + 15] = ON_G;
    } else if (state == DEFUNCT) {
        leds[lednr]      = OFF;
        leds[lednr + 15] = OFF;
    } else {
        leds[lednr]      = ON_YR;
        leds[lednr + 15] = ON_YG;
    }
    current[lednr] += minus;
    current[lednr + 15] += minus;
}

void Hoera10JaarComponent::set_led_from_mqtt(const std::string &name, const std::string &state) {
    if (state == "red") {
        this->set_led(name, CLOSED);
    } else if (state == "green") {
        this->set_led(name, OPEN);
    } else if (state.length() <= 1) {
        this->set_led(name, DEFUNCT);
    } else {
        this->set_led(name, UNKNOWN);
    }
}

void Hoera10JaarComponent::set_off() {
    all(0, false, false);
}

bool fade() {
  static unsigned long previous = 0;
  if (!previous || (millis() - previous > fade_interval)) {
    previous = millis();
  } else {
    return true;
  }

  bool faded = false;
  for (int i = 0; i < 30; i++) {
    if (leds[i] > current[i]) { current[i]++; faded = true; }
    else if (leds[i] < current[i]) { current[i]--; faded = true; }
  }
  return faded;
}

void wait_fade() {
  while (fade());
}

void all(int ms, bool red, bool green) {
  unsigned long start = millis();
  for (int i =  0; i < 15; i++) leds[i] = red * (green ? ON_YR : ON_R);
  for (int i = 15; i < 30; i++) leds[i] = green * (red ? ON_YG : ON_G);
  if (ms) {
    wait_fade();
    while (millis() - start < ms) delay(1);
  }
}


void led_matrix_loop(void *param) {
    Hoera10JaarComponent *self = (Hoera10JaarComponent*)param;

    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);

    const static int levels[] = { 192,40,224,8,24,152,128,104,184,16,88,216,208,240,
        176,232,200,32,160,72,248,80,56,112,136,0,48,144,168,64,96,120};  // ((0..31) »*» 8).pick(*).join(",")
    const uint32_t colgpio[numcols] = {  // calculate at compile time
        (uint32_t)1 << cols[0],
        (uint32_t)1 << cols[1],
        (uint32_t)1 << cols[2],
        (uint32_t)1 << cols[3],
        (uint32_t)1 << cols[4],
        (uint32_t)1 << cols[5]
    };
    const static uint32_t rowgpio[numcols] = {
        rows[0] < 32 ? ((uint32_t)1 << rows[0]) : ((uint32_t)1 << (rows[0] - 32)),
        rows[1] < 32 ? ((uint32_t)1 << rows[1]) : ((uint32_t)1 << (rows[1] - 32)),
        rows[2] < 32 ? ((uint32_t)1 << rows[2]) : ((uint32_t)1 << (rows[2] - 32)),
        rows[3] < 32 ? ((uint32_t)1 << rows[3]) : ((uint32_t)1 << (rows[3] - 32)),
        rows[4] < 32 ? ((uint32_t)1 << rows[4]) : ((uint32_t)1 << (rows[4] - 32)),
    };
    const static int rowregister[numcols] = {
        rows[0] >= 32,
        rows[1] >= 32,
        rows[2] >= 32,
        rows[3] >= 32,
        rows[4] >= 32,
    };

    for (int r = 0; r < numrows; r++) pinMode(rows[r], OUTPUT);
    for (int c = 0; c < numcols; c++) pinMode(cols[c], OUTPUT);

    for (;;) {  // Never hand back control
        for (int s = 0; s < 32; s++) {
            for (int c = 0; c < numcols; c++) {
                bool any = false;
                for (int r = numrows - 1; r >= 0; r--) {
                    bool on = current[c * 5 + r] > levels[s];

                    // digitalWrite(rows[r], on) unrolled:
                    if (on) {
                        any = true;
                        if (rowregister[r]) GPIO.out1_w1ts.val = rowgpio[r];
                        else                GPIO.out_w1ts      = rowgpio[r];
                    } else {
                        if (rowregister[r]) GPIO.out1_w1tc.val = rowgpio[r];
                        else                GPIO.out_w1tc      = rowgpio[r];
                    }
                }
                if (any) GPIO.out_w1ts = colgpio[c];  // digitalWrite(cols[c], HIGH);
                else     GPIO.out_w1tc = colgpio[c];  // digitalWrite(cols[c], LOW);

                ets_delay_us(2);  // more stable than delayMicros()
                GPIO.out_w1tc = colgpio[c];  // digitalWrite(cols[c], LOW);
            }
        }

        fade();
        esp_task_wdt_reset();
        yield();
    }
}

} // esphome hoera10jaar
} // namespace esphome

