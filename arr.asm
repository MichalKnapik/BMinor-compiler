Name resolution...
Found 0 error(s) in name resolution.
Typechecking...
Found 0 error(s) while typechecking.
S2 db "\nafter:\n", 0
S1 db " ", 0
S0 db "\nbefore:\n", 0
section .data
bound dq 10
section .data
arr dq 1,2,3,4,5,6,7,8,9,10
section .data
m dq 0
section .text
global main
main:
push rbp
mov rbp, rsp
sub rsp, 0
push rbx
push r12
push r13
push r14
push r15
mov rbx, S0
push rdi
mov rdi, rbx
call print_string
pop rdi
mov rbx, m
mov r10, 0
mov [rbx], r10
L0:
mov rbx, [m]
mov r10, [bound]
cmp rbx, r10
jl L2
mov rbx, 0
jmp L3
L2:
mov rbx, 1
L3:
cmp rbx, 0
je L1
mov rbx, arr
mov r10, [m]
imul r11, r10, 8
add rbx, r11
mov rbx, [rbx]
push rdi
mov rdi, rbx
call print_integer
pop rdi
mov rbx, S1
push rdi
mov rdi, rbx
call print_string
pop rdi
mov rbx, [m]
inc qword [m]
jmp L0
L1:
mov rbx, m
mov r10, 1
mov [rbx], r10
L4:
mov rbx, [m]
mov r10, [bound]
cmp rbx, r10
jl L6
mov rbx, 0
jmp L7
L6:
mov rbx, 1
L7:
cmp rbx, 0
je L5
mov rbx, arr
mov r10, [m]
imul r11, r10, 8
add rbx, r11
mov r10, arr
mov r11, [m]
imul r12, r11, 8
add r10, r12
mov r10, [r10]
mov r11, arr
mov r12, [m]
mov r13, 1
sub r12, r13
imul r13, r12, 8
add r11, r13
mov r11, [r11]
imul r10, r11
mov [rbx], r10
mov rbx, [m]
inc qword [m]
jmp L4
L5:
mov rbx, S2
push rdi
mov rdi, rbx
call print_string
pop rdi
mov rbx, m
mov r10, 0
mov [rbx], r10
L8:
mov rbx, [m]
mov r10, [bound]
cmp rbx, r10
jl L10
mov rbx, 0
jmp L11
L10:
mov rbx, 1
L11:
cmp rbx, 0
je L9
mov rbx, arr
mov r10, [m]
imul r11, r10, 8
add rbx, r11
mov rbx, [rbx]
push rdi
mov rdi, rbx
call print_integer
pop rdi
mov rbx, S1
push rdi
mov rdi, rbx
call print_string
pop rdi
mov rbx, [m]
inc qword [m]
jmp L8
L9:
main_epilogue:
pop r15
pop r14
pop r13
pop r12
pop rbx
mov rsp, rbp
pop rbp
ret
