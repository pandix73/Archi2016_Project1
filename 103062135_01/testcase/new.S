addi $5, $0, 2
addi $4, $4, 1
beq $4, $5, 1 #1085 0001 
jr 4 #0800 0001
lw $1, 0($0)
add $1, $1, $1
lw $1, 0($0)  
addiu $1, $1, -30000 #2421 8ad0
lw $2, 4($0) #8c02 0004
lw $0, -1($2)
