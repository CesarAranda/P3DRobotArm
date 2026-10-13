# Puma 3D Robot Arm

Proyecto educativo de un Brazo Robot con impresora 3D
por Cesar Aranda [Puma3D]
Más información: http://www.thingiverse.com/thing:

Hardware:
  Modificación del trabajo de Florin Tobler [ftobler]
  (info http://www.thingiverse.com/thing:1718984)
  Con ideas tomadas de Francis Ng [Francis128]
  (info http://www.thingiverse.com/thing:2040526)

Firmware:
* Version V1.1 (02Nov2026)
  - Adaptada para usar un tipo de Gripper con sensores de presion
    basado en un Servo 28BYJ-48 modificado para operar como bipolar.
Cambios:
  - Agregado de módulo/entidad separada para realizar el control del
    Gripper Sensible a Presión.
- Agregado de Control de Limites del Espacio de Trabajo previos a la
    ejecución de los desplazamientos (uso de planificador).
        
* Version V1.0 (10Oct2026)
  - Adaptada para usar un tipo de Gripper basico, para sostener lapiz,
    basado en un Servo 28BYJ-48 modificado para operar como bipolar.
    Luego de un tiempo de mínima actividad por otras obligaciones,
    en 2026 se retoma el proyecto y se lanza la version 1.0 de producción.

  Cambios:
  - Estandarización de nombres de entidades (camelCase).
  - Estandarización de los identificadores usados, omitiendo referirlos
    a ejes X, Y, Z, Extrusor, para asociarlos más fuertemente
    a esta implementación particular.
    Cambio de nombres en los archivos para evitar confusiones,
    si bien, algunos módulos coinciden con los originales.
  - Eliminación del Modo de funcionamiento "Simulation". 
    Desarrollo de aplicativo independiente para realizar las tareas
    asociadas a prototipos de Interfaces de Usuario de Control
    sin necesidad de tener el hardware montado, compatible con este 
    firmware.
  - Agregado de funciones para apertura/cierre variables del gripper
    controlado mediante A4988.
  - Redefinicion de la clase gripper.
  - Procedimiento manual para Homing de la pinza.
  - Control de disponibilidad para permitir activación/desactivación de
    dispositivos complementarios (fan, pump, laser, led). 
  - Ajuste de mensajes e información devuelta por cada servicio.
  - Corrección de algoritmos para Homing
  - Agregado de servicios adicionales para homing (lower shank, upper shank, rotate base)
  - Sólo se deja código para la implementación con Arduino MEGA 2560
  - Si bien no se implementa montaje con Rail, se deja las instrucciones
    asociadas, por si se agregara a futuro
  - Reemplazo de respuesta OK , por identificación de comando específico ejecutado
  - Para el control de la respuesta anterior se cambia al mecanismo de LOGGER
  - Cada pseudo método definido en command.h se mueve como miembro a la clase correspondiente.
  - Unificación de los mensajes de salida de activación a enabled/disabled.

* Version V0.9 (07-Oct-2024)
  - Adaptada para usar un tipo de Gripper basico, para sostener lapiz,
    basado en un Servo 28BYJ-48 modificado para operar como bipolar.
  Cambios:
  - Agregado de un Modo de funcionamiento "Simulation" para usar el
    firmware en tareas de desarrollo de Interfaces de Usuario de Control
    sin necesidad de tener el hardware montado.

* Versiones beta 0.62, 0.63, 0.7 y 0.8 (desde 05-2020).
  Derivación de la version V0.61 del trabajo de Leo Yeung [20sffactory]
  (más información en https://www.20sffactory.com)
  Se adopta el esquema de objetos con los módulos:
      byj_gripper.h, command.h, config.h, endstop.h, equipment.h,
      fanControl.h, gripper.h, interpolation.h, logger.h, pinout.h, queue.h,
      RampsStepper.h, RobotGeometry.h, servo_gripper.h
  Se hicieron cambios menores, sin registrarlos.

* Archivos STL agregados o actualizados:
  - Bandeja interna central para guía de cables 
  - Bandeja de cola para guía de cables 
  - Final de carrera en Eje Z
  - Brazo superior con Orificios para pasaje de cables
  - Anillo de suplemento para cada mordaza de la pinza
  . Pinza adicinal con forma cónica
  - Pinza adicional con sensores de presión y fin de carrera
  - Gabinete principal para la alojar la electrónica de control
  - Mini gabinete para alojar electrónica complementaria de la pinza
  

