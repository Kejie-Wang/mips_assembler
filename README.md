# mips_assembler
mips assembler simulator
This is a mips assembler which can assemble the source mips assembly code and disassemble the binary code to source code
and simulate the source code.

//instructions
R-format: add addu sub subu and or xor nor sll srl sra sllv srlv srav slt sltu mul multu div divu jr jalr
I-format: lw lb lbu lh lhu sw sb sh addi addiu andi ori xori lui slti sltiu beq bne blez bgtz
J-format: j jal

//assemble
The assemble class include three key functions translate which translate the source code into binary code, disasm which disassemble
the binary code into source code, simulator which can execute a mips instruction. And it provides a simple data store instruction.
The main is coding by Qt and there is a lose in the GUI design.
