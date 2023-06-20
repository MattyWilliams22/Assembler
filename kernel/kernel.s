main:
    ldr x14, pc
    b wait
    
    movk r0, #0x1; move 1 to first register
    ldr x14, pc
    b SetLEDState

    ldr x14, pc
    b wait

    movk x0, #0x0; move 0 to first register

    ldr x14, pc
    b SetLEDState

    b main

wait:
    movz x0, #0xffff, lsl #48
    loop:
        sub x0, x0, #0x1; subtract 1
        cmp x0, #0x0; check if it's zero
        b.ne loop; if not loop back and keep subtracting
    mov pc, x14

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
    ldr x13, x14
    mov x2, x0; move state(r0) into temp register
    ldr x0, BufferInfo; load r0 with buffer info
    movk x3, #0x0
    str x3, [x0, #0x4];reset request code
    str x3, [x0, #0x10]; reset request code
    movk x1, #0x82
    str x3, [x0, 0x14]; reset pin number
    str x2, [x0, #0x18]; 
    movk x1, #0x8; set the channel to write to mailbox
    ldr x14, pc
    b MailboxWrite; write a message to mailbox

    movk x0, #0x8
    ldr x14, pc
    b MailboxRead; read a message from mailbox

    ldr pc, x13; pop saved address into program counter

wait_write:
    ldr x3, [x2, #0x38]; load the status of write register
    tst x3, #0x80000000; check the full flag
    b.ne wait_write; keep checking mailbox until it's not full

MailboxWrite:
    add x0, x1
    ldr x2, #0x3f00b880; mailbox address

    ldr x14, pc
    b wait_write

    str x0, [x2, #0x40]; send message to GPU
    mov pc, x14; return from the function

wait_read:
    ldr x2, [x1, #0x18]; load the status register
    tst x2, #0x40000000; check the empty flag
    b.ne wait_read; keep checking until mailbox is not empty

    ldr x2, [x1,]; load adress of the response
    add x3, x2, #0b1111; extract channel
    eor x4, x3, x0; test if the same channel

    b.ne wait_read; keep checking for message we are interested in

MailboxRead:
    ldr x1, #0x3f00b880; mailbox address

    ldr x14, pc
    b wait_read

    mov x0, x2; move the mail's address to the function return
    mov pc, x14; return from the function
