.ORG 0X0000
    
    LDI R16, 0B11111111
    OUT DDRD, R16	;CONFIGURAR O PORTD COMO SA�DA
    
    LDI R16, 0B00000000
    OUT DDRC, R16	;CONFIGURA O PORTC COMO ENTRADA

    LDI R16, 0B00001100
    OUT	PORTC, R16	;ATIVA OS RESISTORES DE PULL-UPS INTERNOS
    
    LOOP:
	;SBIS - PULA SE ESTIVER SETADO - BOT�O N�O PRESSIONADO ~SBIC (INVERSO DE)
	SBIC	PINC, PC2	    ;VERIFICA SE O BOT�O EST� PRESSIONADO (PINX L� O VALOR)
	RJMP	TESTA_BOTAO2    ;V� PARA A FLAG SE O BOT�O FOR PRESSIONADO, VALOR 0
	RJMP	LIGA_LED

    DESLIGA_LED:
	CBI	PORTD, PD5
	RJMP	LOOP

    LIGA_LED:
	SBI	PORTD, PD5
	RJMP	LOOP

    TESTA_BOTAO2:
	SBIC	PINC, PC3
	RJMP	LOOP
	RJMP	DESLIGA_LED