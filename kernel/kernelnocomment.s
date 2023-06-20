main:
    ldr x14, pc
    b wait
    
    movk r0, #0x1
    ldr x14, pc
    b SetLEDState

    ldr x14, pc
    b wait

    movk x0, #0x0

    ldr x14, pc
    b SetLEDState

    b main

wait:
    movz x0, #0xffff, lsl #48
    b loop
    mov pc, x14

loop:
    sub x0, x0, #0x1
    cmp x0, #0x0
    b.ne loop

BufferInfo:
    .int BufferInfoEnd - BufferInfo
    .int 0
    .int 0x00038041
    .int 8
    .int 0
    .int 130
    .int 1
    .int 0
BufferInfoEnd:

SetLEDState:
    ldr x13, x14
    mov x2, x0
    ldr x0, BufferInfo
    movk x3, #0x0
    str x3, [x0, #0x4]
    str x3, [x0, #0x10]
    movk x1, #0x82
    str x3, [x0, 0x14]
    str x2, [x0, #0x18]
    movk x1, #0x8
    ldr x14, pc
    b MailboxWrite

    movk x0, #0x8
    ldr x14, pc
    b MailboxRead

    ldr pc, x13

wait_write:
    ldr x3, [x2, #0x38]
    tst x3, #0x80000000
    b.ne wait_write

MailboxWrite:
    add x0, x0, x1
    ldr x2, #0x3f00b880

    ldr x14, pc
    b wait_write

    str x0, [x2, #0x40]
    mov pc, x14

wait_read:
    ldr x2, [x1, #0x18]
    tst x2, #0x40000000
    b.ne wait_read

    ldr x2, [x1,]
    add x3, x2, #0b1111
    eor x4, x3, x0

    b.ne wait_read

MailboxRead:
    ldr x1, #0x3f00b880

    ldr x14, pc
    b wait_read

    mov x0, x2
    mov pc, x14
