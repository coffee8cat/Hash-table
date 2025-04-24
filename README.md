# Хэш-таблица

## Задача

Написать базовую версию хэш-таблицы, после чего провести серию оптимизаций для повышения производительности операций добавления нового элемента и поиска. Цель (функция) для оптимизации выбиралась на основании профиля программы, полученного с помощью **perf** и его графической оболочки **hotspot**. Для тестирования хэш-таблицы был взят текст романа "Война и мир" Льва Николаевича Толстого на английском и отфильтрован: оставлены только слова (см. [преобразованный текст](text/War-and-Peace-processed.txt)). Для тестирования поиска отобрано около четверти слов (каждое четвертое слово)(см. [преобразованный текст](text/War-and-Peace-test.txt)).

## Хэш таблица как структура данных
Хэш таблица - структура данных, реализующая интерфейс ассоциативного массива, а именно, она позволяет хранить пары (ключ, значение) и выполнять три операции: добавление новой пары, удаление и поиск пары по ключу.
В данной задаче хэш-таблица - массив *H* списков пар.
Выполнение операции в хэш-таблице начинается с вычисления хэш-функции от ключа. Полученное хэш-значение играет роль индекса в массиве *H*, затем соответсвующая операция применяется в соответствующем списке. (Удаление элемента не реализовано в данной задаче)

## Конфигурация

- **CPU**: AMD Ryzen 5 4600H with Radeon Graphics 3.00 GHz
- **OS**: Ubuntu 24.04.2 LTS (GNU/Linux 5.15.167.4-microsoft-standard-WSL2 x86_64)
- **Compiler**: g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0

## Изменение параметров программы

Изменение параметров запуска и тестирования: [params.cpp](src/params.cpp)

## Оптимизации

Сначала посмотрим на профиль программы базовой(начальной) версии:

<details open>
<summary>perf data</summary>

![picture](img/perfdata_basic_ver.png)

</details>

Хотя из данных о работе программы видно, что наиболее используемая функция - это list_search, в качестве первого шага оптимизации было решено выбрать целью вторую по времени работы strncmp.

### Оптимизация strncmp

Примем во внимание, что слова в нашем тексте занимают не более 16 символов каждое, поэтому можно проводить сравнение строк, используя SIMD инструкции с xmm регистрами.


<details open>
<summary>Код str16cmp на ассемблере</summary>
Код оптимизированной функции ([ссылка на исходник](src/my_optimized_funcs.cpp)):

```C
bool my_str16cmp(char word1[STRING_SIZE], char word2[STRING_SIZE]) {
    assert(word1);
    assert(word2);

    int mask;
    __asm__ volatile (
        "movdqu (%1),     %%xmm0 \n\t"    // xmm0 = word1
        "movdqu (%2),     %%xmm1 \n\t"    // xmm1 = word2
        "pcmpeqb  %%xmm1, %%xmm0 \n\t"    // cmpeq: xmm0 == xmm1 → в xmm0
        "pmovmskb %%xmm0, %0 \n\t"        // movemask → mask
        : "=r"(mask)                      // Exit
        : "r"(word1), "r"(word2)          // Entry
        : "xmm0", "xmm1"                  // Dstr
    );

    return mask == 0xFFFF;
}

```

Эквивалент на C без ассемблерных вставок:

```C

bool my_str16cmp(char word1[STRING_SIZE], char word2[STRING_SIZE]) {
    assert(word1);
    assert(word2);

    __m128i word1_xmm = _mm_loadu_si128((__m128i*)word1);
    __m128i word2_xmm = _mm_loadu_si128((__m128i*)word2);

    __m128i cmp = _mm_cmpeq_epi8(word1_xmm, word2_xmm);

    int mask = _mm_movemask_epi8(cmp);

    return mask == 0xFFFF;
}
```
</details>

После оптимизации уменьшилось время работы strcmp относительно других функций, как и время работы функций, ее содержащих(например, list_search),

<details open>
<summary>perf data</summary>

![picture](img/perfdata_basic_ver.png)

</details>

### Оптимизация list_search

В данной оптимизации list_search была полностью переписана на ассемблер x86_64 (синтаксис *NASM*). Основное, что можно сделать для оптимизации поиска в списке - предзагрузка следующего элемента (*prefetching*), что и было сделано.

<details open>
<summary>Код list_search на ассемблере</summary>


Структуры, использующиеся в list_search([ссылка на исходник](headers/my_list.h)):

```C

struct hashtable_elem_t {
    char   buffer[STRING_SIZE];
    size_t hash;
    size_t counter;
};

typedef hashtable_elem_t lst_data_t;
typedef int32_t lst_index_t;

struct list_t
{
    lst_data_t*  data;
    lst_index_t* next;
    lst_index_t* prev;

    lst_index_t free;
    lst_index_t capacity;
};
```
Непосредственно код ([ссылка на исходник](src/asm_funcs.asm)):
```asm
list_search_asm_preload:
        push rbx

        ; rdi is list_t* list
        mov     r8,  qword [rdi + 16]       ; rcx = & (list -> prev)
        movsx   rdx, dword [r8]             ; rdx = list -> prev[0]
        test    edx, edx                    ; if (prev(0) == 0) { end } - means empty list
        je      .return_null

        mov     rdi, qword [rdi]
        mov     rbx, rdx                    ; rbx = list + rdx * sizeof(list_t) <=> rbx = list.data[rdx]
        shl     rbx, 5                      ; sizeof(list_t) = 32
        add     rbx, rdi

        vmovdqu xmm0, [rbx]                 ; xmm0 = list.data[curr].buffer
        vmovdqu xmm1, [rsi]                 ; xmm1 = string

        mov     rax, rbx

        vpcmpeqb    xmm0, xmm0, xmm1        ; xmm0 = _mm_cmpeq_epi8(xmm0, xmm1)
        vpmovmskb   ecx, xmm0               ; ecx  = _mm_movemask_epi8(xmm0)
        cmp         ecx, 65535
        je          .found

        movsx   rdx, dword [r8 + rdx*4]     ; rdx = prev(rdx)
        test    edx, edx
        je      .return_null                ; if (prev(curr) == 0) ==> it was last element to check => end
        mov     rbx, rdx                    ; rbx = list.data[curr]
        shl     rbx, 5
        add     rbx, rdi
                                            ; prefetch next node in xmm2
        vmovdqu xmm2, [rbx]                 ; xmm2 = list.data[curr].buffer

        ;jmp     .check_cond

.while_body:
        ; rdx = curr - index of current elem in list.data

        vmovdqu xmm0, xmm2                  ; xmm0 = list.data[curr].buffer
        mov     rax, rbx

        movsx   rdx, dword [r8 + rdx*4]     ; rdx = prev(rdx)
        test    edx, edx
        je      .final_check                ; if (prev(curr) == 0) ==> it was last element to check => end
        mov     rbx, rdx                    ; rbx = list.data[curr]
        shl     rbx, 5
        add     rbx, rdi
        vmovdqu xmm2, [rbx]                 ; xmm0 = list.data[curr].buffer

.check_cond:
        vpcmpeqb    xmm0, xmm0, xmm1        ; xmm0 = _mm_cmpeq_epi8(xmm0, xmm1)
        vpmovmskb   ecx, xmm0               ; ecx  = _mm_movemask_epi8(xmm0)
        cmp         ecx, 65535
        jne         .while_body

.found:
        pop rbx
        ret                                 ; return list_data[curr]

.final_check:
        vpcmpeqb    xmm0, xmm0, xmm1        ; xmm0 = _mm_cmpeq_epi8(xmm0, xmm1)
        vpmovmskb   ecx, xmm0               ; ecx  = _mm_movemask_epi8(xmm0)
        cmp         ecx, 65535
        je         .found

.return_null:
        xor     eax, eax                    ; return false
        pop rbx
        ret

```
</details>

## Оптимизация CRC32 через SSE

В последней оптимизации все крайне тривиально: снова пользусь тем, что длина слова в тексте не превышает 16 байт, crc32 заменяется на версию crc32_16, которая вычислет хэш с помощью двух использований интринсиков _mm_crc32_u64, вычислющих хэш для 8 байт.


<details open>
<summary>Код crc32_16</summary>

```C
uint32_t crc32_16(const char data[16]) {
    uint32_t crc = 0xFFFFFFFF;

    uint64_t part1 = *(const uint64_t*)(data);
    crc = _mm_crc32_u64(crc, part1);

    uint64_t part2 = *(const uint64_t*)(data + 8);
    crc = _mm_crc32_u64(crc, part2);

    return crc ^ 0xFFFFFFFF;
}
```

</details>

## Тестирование оптимизаций

Для подсчета производительности каждой версии сравнивалось время работы `100` циклов полного заполнения хэш-таблицы (версии insert) и поиска (версии search) через **rdtsc**. Для большей точности и воспроизводимости вычислений программа исполнялась на одном ядре (см. [pin_to_core](src/unit_testing.cpp)) в отсутвие других запущенных приложений (только VS Code).
Исходники тестов: [run_tests](src/unit_testing.cpp)

|   Version              | Insert ticks   | Insert bench | Search ticks   | Search bench |
|------------------------|----------------|--------------|----------------|--------------|
| Basic                         |    16123555018 |  1.000 |     3241929781 |  1.000 |
| + str16cmp                    |    14382870750 |  1.121 |     2849467965 |  1.138 |
| + preload in list search (asm)|    13047913233 |  1.236 |     2532656220 |  1.280 |
| + SSE for crc32_16            |    12750709170 |  1.265 |     2496151515 |  1.299 |

## Анализ равномерности хэширования
Для исследования хэш-функции **CRC32** хэш-таблица полностью заполнялась с расчетом в среднем 10 записей на бакет, после чего рассчитывались следующие зависимости:

<details open>
<summary>Распределение по бакетам (размер)</summary>

![picture](img\crc32_bucket_distribution.png)

</details>

<details open>
<summary>Распределение по размеру бакета</summary>

![picture](img\crc32_sizes_distribution.png)

</details>

Из графиков делаем вывод, что функция хэширования хорошо подходит для хэш-таблицы - хэширование вполне однородное, с небольшим количеством выбросов, распределение по размеру нормальное с большей частью значений в диапазоне от 7 до 13 записей на бакет.
