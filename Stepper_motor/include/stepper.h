#include <avr/io.h>
#include <stdbool.h>

#define CLOCKWISE           1
#define COUNTERCLOCKWISE    0

uint8_t speed;

void passo_completo_alto_torque(void);
void passo_completo_baixo_torque(void);
void meio_passo(void);
void set_stepper_motor_mode(uint8_t mode);
void rotate_CW(void);
void rotate_CCW(void);
void Full_torque(void);
void Half_torque(void);
void High_precision(void);
void select_angle(uint16_t angle, bool direction);
bool run(void);
void clear(void);
void chose_speed(uint8_t time);