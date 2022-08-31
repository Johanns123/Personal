#include "stepper.h"


uint8_t vect_steps_full_step_hard[4] = {0x90, 0x30, 0x60, 0xc0};
uint8_t vect_steps_full_step_easy[4] = {0x10, 0x20, 0x40, 0x80};
uint8_t vect_steps_half_step_hard[8] = {0x10, 0x30, 0x20, 0x60, 0x40, 0xc0, 0x80, 0x90};
uint8_t mode;
uint8_t sentido = 0;
uint16_t contador = 0;
uint16_t local_counter = 0;
bool finish = 0;

void set_stepper_motor_mode(uint8_t mode)
{
  switch (mode)
  {
  case 1:
    passo_completo_alto_torque();
    break;

  case 2:
    passo_completo_baixo_torque();
    break;
  
  case 3:
    meio_passo();
    break;

  default:
    passo_completo_alto_torque();
    break;
  }
}


void passo_completo_alto_torque(void)
{
  mode = 1;
}

void passo_completo_baixo_torque()
{
  mode = 2;
}

void meio_passo(void)
{

  mode = 3;
}

void rotate_CCW()
{
  switch (mode)
  {
  case 1:
    Full_torque();
    sentido = COUNTERCLOCKWISE;
    break;
  

  case 2:
    Half_torque();
    sentido = COUNTERCLOCKWISE;
    break;


  case 3:
    High_precision();
    sentido = COUNTERCLOCKWISE;
    break;

  default:
    break;
  }
}

void rotate_CW()
{
  switch (mode)
  {
  case 1:
    Full_torque();
    sentido = CLOCKWISE;
    break;
  

  case 2:
    Half_torque();
    sentido = CLOCKWISE;
    break;


  case 3:
    High_precision();
    sentido = CLOCKWISE;
    break;

  default:
    break;
  }
}

void Full_torque(void)
{
  static uint8_t step = 1;

  if(!sentido)
  {
    switch (step)
    {
      case 1:
        PORTD = vect_steps_full_step_hard[step - 1];
        step = 2;
        break;

      case 2:
        PORTD = vect_steps_full_step_hard[step - 1];
        step = 3;
        break;

      case 3:
        PORTD = vect_steps_full_step_hard[step - 1];
        step = 4;
        break;

      case 4:
        PORTD = vect_steps_full_step_hard[step - 1];
        step = 1;
        break;
      
      default:
        break;
    }
  }

  else
  {
    switch (step)
    {
      case 1:
        PORTD = vect_steps_full_step_hard[4-step];
        step = 2;
        break;

      case 2:
        PORTD = vect_steps_full_step_hard[4-step];
        step = 3;
        break;

      case 3:
        PORTD = vect_steps_full_step_hard[4-step];
        step = 4;
        break;

      case 4:
        PORTD = vect_steps_full_step_hard[4-step];
        step = 1;
        break;
      
      default:
        break;
    }
  }
  
}

void Half_torque(void)
{
  static uint8_t step = 1;

  if(!sentido)
  {
    switch (step)
      {
        case 1:
          PORTD = vect_steps_full_step_easy[step - 1];
          step = 2;
          break;

        case 2:
          PORTD = vect_steps_full_step_easy[step - 1];
          step = 3;
          break;

        case 3:
          PORTD = vect_steps_full_step_easy[step - 1];
          step = 4;
          break;

        case 4:
          PORTD = vect_steps_full_step_easy[step - 1];
          step = 1;
          break;
        
        default:
          break;
      }
  }

  else
  {
    switch (step)
    {
      case 1:
        PORTD = vect_steps_full_step_easy[4-step];
        step = 2;
        break;

      case 2:
        PORTD = vect_steps_full_step_easy[4-step];
        step = 3;
        break;

      case 3:
        PORTD = vect_steps_full_step_easy[4-step];
        step = 4;
        break;

      case 4:
        PORTD = vect_steps_full_step_easy[4-step];
        step = 1;
        break;
      
      default:
        break;
    }
}
  
}

void High_precision(void)
{
  static uint8_t step = 1;

  if(!sentido)
  {
    switch (step)
    {
      case 1:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 2;
        break;

      case 2:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 3;
        break;

      case 3:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 4;
        break;

      case 4:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 5;
        break;
      
      case 5:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 6;
        break;
      
      case 6:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 7;
        break;
      
      case 7:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 8;
        break;
      
      case 8:
        PORTD = vect_steps_half_step_hard[step-1];
        step = 1;
        break;

      default:
        break;
    }
  }

  else
  {
    switch (step)
    {
      case 1:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 2;
        break;

      case 2:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 3;
        break;

      case 3:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 4;
        break;

      case 4:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 5;
        break;
      
      case 5:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 6;
        break;
      
      case 6:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 7;
        break;
      
      case 7:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 8;
        break;
      
      case 8:
        PORTD = vect_steps_half_step_hard[8-step];
        step = 1;
        break;

      default:
        break;
    }
  }
  
}


void select_angle(uint16_t angle, bool direction)
{
    switch (mode)
    {
        case 1:
        case 2:
            contador = 5.7*angle;
            break;


        case 3:
            contador = 11.4*angle;
            break;

        default:
            break;
  }

    if(direction)
        sentido = COUNTERCLOCKWISE;

    else
        sentido = CLOCKWISE;
}

bool run(void)
{   
    if(local_counter == contador)
    {   
        finish = 1;
        return finish;
    }
    if(sentido == COUNTERCLOCKWISE)
        rotate_CCW();

    else
        rotate_CW();  
    
    local_counter++;

    return finish;
}

void clear()
{
    local_counter = 0;
    finish = 0;
}

void chose_speed(uint8_t time)
{
    speed = time;
}