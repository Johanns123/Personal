.ORG 0X0000
    
    INICIO:
	
	NOP	;NO OPERATION - GASTA UM CICLO DE M�QUINA QUE ATUA MAS N�O FAZ NADA
	NOP
	RCALL	SUB_ROTINA1	;CHAMA UMA SUB_ROTINA
	NOP
	RJMP INICIO
	
SUB_ROTINA1:
    NOP
    NOP
    RCALL SUB_ROTINA2
    RET	    ;RETORNA PARA LINHA ABAIXO DO RECALL

SUB_ROTINA2:
    NOP
    NOP
    RCALL SUB_ROTINA3
    RET
    
SUB_ROTINA3:
    NOP
    NOP
    RET