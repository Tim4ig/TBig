.intel_syntax noprefix

.text

// uint64_t adcx_u64(uint64_t a, uint64_t b, uint64_t *carry);
.globl adcx_u64
adcx_u64:
     xor rax, rax       // reset CF
     mov rax, rdi
     adcx rax, rsi
     setc byte ptr [rdx]
     ret

// uint64_t adox_u64(uint64_t a, uint64_t b, uint64_t *carry);
.globl adox_u64
adox_u64:
     xor rax, rax       // reset CF
     mov rax, rdi
     adcx rax, rsi
     seto byte ptr [rdx]
     ret

// uint64_t sbb_u64(uint64_t a, uint64_t b, uint64_t *borrow);
.global sbb_u64
sbb_u64:
    mov rax, rdi
    sub rax, [rdx]
    sbb rax, rsi
    setc byte ptr [rdx]
    ret

// void mulx_u64(uint64_t *h, uint64_t *l, uint64_t a, uint64_t b);
.globl mulx_u64
mulx_u64:
    mulx r8, r9, rcx    // r8(hi), r9(lo) <- rdx * rcx
    mov [rdi], r8
    mov [rsi], r9
    ret
