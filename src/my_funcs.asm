global list_search_asm

section .text

;=======================================================================================
; Entry: rdi: list_t* lst (pointer to a list for search)
;        rsi: size_t key (hash to compare)
; Exit:  rax: pointer to a list element with the hash == key or
;             NULL if not such element met
;=======================================================================================

;struct hashtable_elem_t {
;    char   buffer[16];
;    size_t hash;
;    size_t counter;
;};

;typedef hashtable_elem_t lst_data_t;
;typedef int32_t lst_index_t;

;struct list_t
;{
;    lst_data_t*  data;
;    lst_index_t* next;
;    lst_index_t* prev;
;
;    lst_index_t free;
;    lst_index_t capacity;
;};

list_search_asm:

    mov     rcx, [rdi+16]                   ; rcx = prev
    movsx   rax, dword [rcx]                ; rax = prev[0]
    test    eax, eax                        ; check rax == pivot element (0)
    je      .return_null
    mov     rdi, [rdi]                      ; rdi = data[0]
    jmp     .while_body

.while_cond_check:
    movsx   rax, dword [rcx + rax*4]        ; rax = prev[rax], 4 bc of int32
    test    eax, eax                        ; check rax == pivot element (0)
    je      .return_null

.while_body:
    ; temp_data = lst -> data[curr_elem]; curr_elem = rax
    mov     rdx, rax
    shl     rdx, 5                          ; sizeof(lst_data_t) = 32
    add     rdx, rdi                        ; rdx = rdi + sizeof(lst_data_t) * rax
    cmp     qword [rdx + 16], rsi           ; (temp_data.hash == key)
    jne     .while_cond_check

    mov     rax, rdx
    ret

.return_null:
    xor     rax, rax
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
