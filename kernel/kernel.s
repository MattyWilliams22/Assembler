_start:
    movk sp, 0x8000; set the stack pointer
    b main

main:
    bl wait
    
    movk r0, #0x1; move 1 to first register
    bl SetLEDState

    bl wait

    movk r0, #0x0; move 0 to first register
    bl SetLEDState

    b main

wait:
    movz xd, #0xffff, lsl #48
    loop:
        sub r0, #0x1; subtract 1
        cmp r0, #0x0; check if it's zero
        bne loop; if not loop back and keep subtracting
    mov pc, lr

BufferInfo:
    .int BufferInfoEnd - BufferInfo; request size
    .int 0; request code
    .int 0x00038041; ‘Set LED’ Tag ID
    .int 8; value buffer size
    .int 0; request code
    .int 130; power LED pin number
    .int 1; LED status (on)
    .int 0; end tag
BufferInfoEnd:

SetLEDState:
    push {lr}; save the address the function should return to
    mov r2, r0; move state(r0) into temp register
    ldr r0, BufferInfo; load r0 with address of buffer info
    movk r3, #0x0
    str r3, [r0, #0x4];reset request code
    str r3, [r0, #0x10]; reset request code
    movk r1, #0x82
    str r3, [r0, 0x14]; reset pin number
    str r2, [r0, #0x18]; 
    movk r1, #0x8; set the channel to write to mailbox
    bl MailboxWrite; write a message to mailbox

    movk r0, #0x8
    bl MailboxRead; read a message from mailbox

    pop {pc}; pop saved address into program counter

MailboxWrite:
    add r0, r1;
    ldr r2, 0x3f00b880; mailbox address

    bl wait_write

    str r0, [r2, #0x20]; send message to GPU
    mov pc, lr; return from the function

wait_write:
    ldr r3, [r2, #0x38]; load the status of write register
    tst r3, #0x80000000; check the full flag
    b.ne wait_write; keep checking mailbox until it's not full

MailboxRead:
    ldr r1, 0x3f00b880; mailbox address

    bl wait_read

    mov r0, r2; move the mail's address to the function return
    mov pc, lr; return from the function

wait_read:
    ldr r2, [r1, #0x18]; load the status register
    tst r2, #0x40000000; check the empty flag
    bne wait_read; keep checking until mailbox is not empty

    ldr r2, [r1]; load adress of the response
    add r3, r2, #0b1111; extract channel
    teq r3, r0; test if the same channel

    b.ne wait_read; keep checking for message we are interested in
