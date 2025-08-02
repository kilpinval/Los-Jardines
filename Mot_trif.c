#include <XC164CS.h>

// Configuración del sistema
#define F_OSC 40000000UL  // Frecuencia del oscilador: 40 MHz
#define PWM_FREQ 20000    // Frecuencia PWM objetivo: 20 kHz
#define DEAD_TIME 100     // Tiempo muerto en ns (ajustar según drivers)

// Cálculo de valores del período PWM
#define T_PWM (1.0 / PWM_FREQ)                                // Período en segundos
#define PWM_PERIOD ((unsigned int)(F_OSC * T_PWM))            // Valor del período
#define DEAD_TIME_TICKS ((unsigned int)(DEAD_TIME * F_OSC / 1e9)) // Tiempo muerto en ticks

// Registros de control
__sfr __at (0xFF00) CC6IC;    // Registro de control de interrupciones CAPCOM6
__sfr __at (0xFF10) CC6MCON;  // Modo de control CAPCOM6
__sfr __at (0xFF1A) CC6MSEL;  // Selección de modo
__sfr __at (0xFF50) CC6EIC;   // Control de interrupción de error

// Registros de comparación
__sfr16 __at (0xFF30) CC60;   // Canal 0 (fase U alta)
__sfr16 __at (0xFF32) CC61;   // Canal 1 (fase U baja)
__sfr16 __at (0xFF34) CC62;   // Canal 2 (fase V alta)
__sfr16 __at (0xFF36) CC63;   // Canal 3 (fase V baja)
__sfr16 __at (0xFF38) CC64;   // Canal 4 (fase W alta)
__sfr16 __at (0xFF3A) CC65;   // Canal 5 (fase W baja)

void CAPCOM6_Init(void) {
    // 1. Configurar pines GPIO (alternate function)
    P1_DIR |= 0x003F;     // P1.0 a P1.5 como salidas
    P1_ALTSEL0 |= 0x003F; // Función alternativa CAPCOM6
    
    // 2. Deshabilitar CAPCOM6 durante configuración
    CC6MCON = 0x0000;
    
    // 3. Configurar modo de operación
    CC6MSEL = 0x0000;     // Modo PWM centro-alineado
    
    // 4. Configurar tiempos muertos
    T12DT = (DEAD_TIME_TICKS << 8) | DEAD_TIME_TICKS; // Tiempo muerto igual para ambos flancos
    
    // 5. Configurar período PWM
    CC6_PERIOD = PWM_PERIOD;
    
    // 6. Configurar ciclo de trabajo inicial (50%)
    CC60 = PWM_PERIOD / 2;  // Fase U alta
    CC61 = PWM_PERIOD / 2;  // Fase U baja
    CC62 = PWM_PERIOD / 2;  // Fase V alta
    CC63 = PWM_PERIOD / 2;  // Fase V baja
    CC64 = PWM_PERIOD / 2;  // Fase W alta
    CC65 = PWM_PERIOD / 2;  // Fase W baja
    
    // 7. Habilitar salidas PWM
    CC6MCON = 0x00CF;      // Habilitar caneles 0-5
    CC6MCON |= 0x0100;     // Habilitar generación PWM
    
    // 8. Configurar protección
    CC6EIC = 0x0000;       // Deshabilitar interrupciones de error
}

void setPWM_DutyCycle(unsigned int duty, unsigned char phase) {
    // Asegurar límites del duty cycle (10%-90%)
    duty = (duty < 100) ? 100 : duty;
    duty = (duty > 900) ? 900 : duty;
    
    unsigned int duty_val = (PWM_PERIOD * duty) / 1000;
    
    switch(phase) {
        case 0: // Fase U
            CC60 = duty_val;
            CC61 = PWM_PERIOD - duty_val;
            break;
        case 1: // Fase V
            CC62 = duty_val;
            CC63 = PWM_PERIOD - duty_val;
            break;
        case 2: // Fase W
            CC64 = duty_val;
            CC65 = PWM_PERIOD - duty_val;
            break;
    }
}

void main(void) {
    // Configuración del reloj
    SYS_CLKCON = 0x0000;  // Usar oscilador interno
    
    // Inicializar CAPCOM6
    CAPCOM6_Init();
    
    // Ejemplo: Secuencia de arranque
    setPWM_DutyCycle(150, 0);  // 15% duty fase U
    setPWM_DutyCycle(150, 1);  // 15% duty fase V
    setPWM_DutyCycle(150, 2);  // 15% duty fase W
    
    while(1) {
        // Control principal del motor aquí
        // (Implementar lógica de conmutación según posición del rotor)
    }
}
