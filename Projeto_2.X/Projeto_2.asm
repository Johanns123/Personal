.ORG 0X0000
    
    
    LDI R16, 0B11111111
    OUT DDRB, R16

    LDI R20, 230    ;255
    SUBI R20, 45    ;185 = 1011 1001	
    OUT PORTB, R20
			    
LOOP:
    RJMP    LOOP


