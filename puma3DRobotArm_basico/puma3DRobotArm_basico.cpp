// =====================================================
// PRUEBA DE MOTORES - ARDUINO MEGA 2560 + RAMPS 1.4
// =====================================================
//
// Calibracion de corriente de A4988 (con resistencias R100)
//  Motor      Vref   Micropasos
//  SKP42HT47  0.60V  MS1/MS2/MS3 en ON
//  STM042F05  0.35V  MS1/MS2/MS3 en ON
//  28BYJ-48   0.08V  MS1 en ON
//  28BYJ-48 convertido a 2 bobinas: se anula cable medio (rojo)
//  se usan las bobinas A (cables Naranja-Rosa) y B (cables Amarillo-Azul)
//  Aproximadamente 50hm cada bobina con 100mA máximo.
//  Orden estandar al conectar al A4988: naranja/rosa/amarillo/azul 
//
// Compatible con A4988 / DRV8825
// =====================================================

#include <Arduino.h>

#define X_STEP 54
#define X_DIR  55

#define Y_STEP 60
#define Y_DIR  61

#define Z_STEP 46
#define Z_DIR  48

//#define E0_STEP 26
//#define E0_DIR  28
//#define E0_MIN_PIN         20

#define E1_STEP        36
#define E1_DIR         34
#define E1_ENABLE      30
#define E1_MIN_PIN         20

#define X_ENABLE 38
#define Y_ENABLE 56
#define Z_ENABLE 62
//#define E0_ENABLE 24

// Finales de carrera MIN de RAMPS 1.4
#define X_MIN_ENDSTOP 3
#define Y_MIN_ENDSTOP 14
#define Z_MIN_ENDSTOP 18

// Cantidad de pasos por movimiento
int pasos = 500;
int pasos_pinza = 44; 
/* El 48BYJ-18 realiza 4096 pasos para ejecutar 1 vuelta, con 1/2 micropaso. 
  En la práctica, la pinza mejorada tiene una apertura de 44/45mm
  cuando se usa el diente jaw_1 y el avance es de 0.02273 mm/paso.
  Se adopta 44 pasos/mm. Se requieren 1936 pasos para abrir/cerrar la pinza).
*/

// Velocidad: mayor valor = más lento
int velocidad = 1000;
int velocidad_pinza = 750;


void menu(){
  Serial.println();
  Serial.println("Comandos:");
  Serial.println("Motor Lower  :  x = atras     / X = adelante");
  Serial.println("Motor Higher :  y =  adelante / Y = atras");
  Serial.println("Motor Rotate :  z =  derecha  / Z = izquierda");
  Serial.println("Motor Gripper:  p = abrir     / P = cerrar");
  Serial.println("Probar X Y Z :  a");
  Serial.println("Leer MIN FdC :  f");
  Serial.println("Desactivar motores:  s");
}




// =====================================================
// CONFIGURACIÓN
// =====================================================

void setup() {

  pinMode(X_STEP, OUTPUT);
  pinMode(X_DIR, OUTPUT);

  pinMode(Y_STEP, OUTPUT);
  pinMode(Y_DIR, OUTPUT);

  pinMode(Z_STEP, OUTPUT);
  pinMode(Z_DIR, OUTPUT);

  pinMode(E1_STEP, OUTPUT);
  pinMode(E1_DIR, OUTPUT);

  pinMode(X_ENABLE, OUTPUT);
  pinMode(Y_ENABLE, OUTPUT);
  pinMode(Z_ENABLE, OUTPUT);
  pinMode(E1_ENABLE, OUTPUT);

  pinMode(X_MIN_ENDSTOP, INPUT_PULLUP);
  pinMode(Y_MIN_ENDSTOP, INPUT_PULLUP);
  pinMode(Z_MIN_ENDSTOP, INPUT_PULLUP);

  // Activar drivers
  digitalWrite(X_ENABLE, LOW);
  digitalWrite(Y_ENABLE, LOW);
  digitalWrite(Z_ENABLE, LOW);
  digitalWrite(E1_ENABLE, LOW);

  Serial.begin(115200);

  Serial.println();
  Serial.println("=================================");
  Serial.println(" PRUEBA RAMPS 1.4 - MOTORES");
  Serial.println("=================================");
  menu();
  Serial.println();
}


// =====================================================
// FUNCIÓN PARA MOVER UN MOTOR
// =====================================================

void moverMotor(int stepPin, int dirPin, bool direccion, int cantidadPasos) {

  int veloMotor = (stepPin == E1_STEP)?velocidad_pinza:velocidad;
  
  digitalWrite(dirPin, direccion);

  for (int i = 0; i < cantidadPasos; i++) {

    digitalWrite(stepPin, HIGH);
    delayMicroseconds(veloMotor);

    digitalWrite(stepPin, LOW);
    delayMicroseconds(veloMotor);
  }
}


// =====================================================
// LOOP PRINCIPAL
// =====================================================

void loop() {

  if (Serial.available() > 0) {

    char comando = Serial.read();

    if (comando == 'f') {
      Serial.println("Estado de finales de carrera:");
      Serial.println(digitalRead(X_MIN_ENDSTOP) == LOW ? "X_MIN: ACTIVO" : "X_MIN: libre");
      Serial.println(digitalRead(Y_MIN_ENDSTOP) == LOW ? "Y_MIN: ACTIVO" : "Y_MIN: libre");
      Serial.println(digitalRead(Z_MIN_ENDSTOP) == LOW ? "Z_MIN: ACTIVO" : "Z_MIN: libre");
    }

    // -----------------------------
    // MOTOR X
    // -----------------------------

    if (comando == 'x') {

      Serial.println("Motor Lower -> atras");

      moverMotor(X_STEP, X_DIR, HIGH, pasos);
    }

    if (comando == 'X') {

      Serial.println("Motor Lower -> adelante");

      moverMotor(X_STEP, X_DIR, LOW, pasos);
    }


    // -----------------------------
    // MOTOR Y
    // -----------------------------

    if (comando == 'y') {

      Serial.println("Motor Higher -> adelante");

      moverMotor(Y_STEP, Y_DIR, HIGH, pasos);
    }

    if (comando == 'Y') {

      Serial.println("Motor Higher -> atras");

      moverMotor(Y_STEP, Y_DIR, LOW, pasos);
    }


    // -----------------------------
    // MOTOR Z
    // -----------------------------

    if (comando == 'z') {

      Serial.println("Motor Rotate -> derecha");

      moverMotor(Z_STEP, Z_DIR, HIGH, pasos);
    }

    if (comando == 'Z') {

      Serial.println("Motor Rotate -> izquierda");

      moverMotor(Z_STEP, Z_DIR, LOW, pasos);
    }


    // -----------------------------
    // MOTOR P
    // -----------------------------

    if (comando == 'P') {

      Serial.println("Motor Gripper -> abrir");

      moverMotor(E1_STEP, E1_DIR, HIGH, pasos_pinza);
    }

    if (comando == 'p') {

      Serial.println("Motor Gripper -> cerrar");

      moverMotor(E1_STEP, E1_DIR, LOW, pasos_pinza);
    }


    // -----------------------------
    // PROBAR LOS TRES
    // -----------------------------

    if (comando == 'a') {

      Serial.println("Probando X...");
      moverMotor(X_STEP, X_DIR, HIGH, pasos);
      delay(500);

      Serial.println("Probando Y...");
      moverMotor(Y_STEP, Y_DIR, HIGH, pasos);
      delay(500);

      Serial.println("Probando Z...");
      moverMotor(Z_STEP, Z_DIR, HIGH, pasos);

      Serial.println("Prueba terminada.");
    }


    // -----------------------------
    // DESACTIVAR MOTORES
    // -----------------------------

    if (comando == 's') {

      digitalWrite(X_ENABLE, HIGH);
      digitalWrite(Y_ENABLE, HIGH);
      digitalWrite(Z_ENABLE, HIGH);
      digitalWrite(E1_ENABLE, HIGH);

      Serial.println("Motores DESACTIVADOS");
      Serial.println("Enviar cualquier comando de movimiento para activarlos.");

    }


    // -----------------------------
    // REACTIVAR DRIVERS
    // -----------------------------

    if (comando == 'x' ||
        comando == 'X' ||
        comando == 'y' ||
        comando == 'Y' ||
        comando == 'z' ||
        comando == 'Z' ||
        comando == 'p' ||
        comando == 'P' ||
        comando == 'a') {

      digitalWrite(X_ENABLE, LOW);
      digitalWrite(Y_ENABLE, LOW);
      digitalWrite(Z_ENABLE, LOW);
      digitalWrite(E1_ENABLE, LOW);

    }
    menu();
  }
}
