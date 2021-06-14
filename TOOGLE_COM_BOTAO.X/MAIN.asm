.ORG 0X0000
    
    LDI R16, 0B11111111
    OUT DDRD, R16	;CONFIGURAR O PORTD COMO SAÍDA
    
    LDI R16, 0B00000000
    OUT DDRC, R16	;CONFIGURA O PORTC COMO ENTRADA

    LDI R16, 0B00001100
    OUT	PORTC, R16	;ATIVA OS RESISTORES DE PULL-UPS INTERNOS
    
    LDI R17, 0B00000000;
    LDI R18, 0B10000000;
    LDI R19, 0B11000000;
    
    LOOP:
	RJMP MUDA_DE_ESTADO
	
    MUDA_DE_ESTADO:
	SBIC PINC, PC2
	RJMP MUDA_DE_ESTADO
	RCALL ATRASO
	SBIS PINC, PC2
	RJMP MUDA_DE_ESTADO
	SBI PORTD, PD7
	CBI PORTD, PD6
	RJMP MUDA_DE_ESTADO_2

    MUDA_DE_ESTADO_2:
	SBIC PINC, PC2
	RJMP MUDA_DE_ESTADO_2
	RCALL ATRASO
	SBIS PINC, PC2
	RJMP MUDA_DE_ESTADO_2
	SBI PORTD, PD6
	CBI PORTD, PD7
	RJMP MUDA_DE_ESTADO

    ATRASO:                ; Gera atrasos por meio de comandos para ciclos de maquina (por volta de 25ms)
    DEC r3             ; Decrementa um ciclo utilizando o registrador 3
    BRNE ATRASO        ; Faz um loop de volta para atraso caso o valor do registrador não for zero
    DEC r2             ; Decrementa um ciclo utilizando o registrador 2
    BRNE ATRASO        ; Faz um loop de volta para atraso caso o valor do registrador não for zero
    RET                ; Retorna ao lugar de onde "atraso" foi chamado
