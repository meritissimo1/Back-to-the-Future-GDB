# De Volta para o Futuro com GDB

## Por que voltar no tempo?
Pelo mesmo motivo que Marty Mcfly em De Volta pra o Futuro 2, CONSERTAR O FUTURO!

Digamos que estamos depurando um laço `while` que percorre um ponteiro para um array de ponteiros (matriz) e que ocorre um problema de lógica em uma das iterações. Não sabemos em qual iteração o erro ocorre, e seria custoso percorrer iteração por iteração com o GDB até chegar ao problema, levaria muito tempo. Nesse caso, podemos definir um breakpoint onde o fluxo do código leva ao erro e voltar no tempo para ver onde o problema ocorreu.

Neste breve artigo, eu vou mostrar como nós podemos fazer isso usando alguns comandos poderosos do GDB:
-   `record`
-   `record stop`
-   `record save`
-   `record full restore`
-   `reverse-next` (`rn`)
-   `reverse-step` (`rs`)
-   `set exec-direction`
-   `reverse-continue` (`rc`)


Vamos usar o código abaixo como exemplo. Ele pode ser executado usando `make` no terminal.   

 ``` c
  1 │ #include <stdio.h>
  2 │ #include <stdlib.h>
  3 │ int bubble_sort(int *vec, int size) {
  4 │ 	for (int i = 0; i < size; i++) {
  5 │ 	  for (int j = i; j < size-1; j++) {
  6 │     	if (vec[j] >= vec[j+1]) {
  7 │     	  int tmp = vec[j];
  8 │     	  vec[j] = vec[j+1];
  9 │     	  vec[j+1] = tmp;
10 │     	}
11 │ 	  }
12 │ 	}
13 │ }
14 │
15 │ int sorted(int* vec, int size) {
16 │ 	for (int i = 0; i < size-1; i++) {
17 │ 	  if (vec[i] > vec[i+1])
18 │     	return 0;
19 │ 	}
20 │ 	return 1;
21 │ }
22 │
23 │ int main() {
24 │ 	int a[6] = {0, 3, 1, 5, 4, 2};
25 │ 	int b[6] = {5, 0, 3, 1, 2, 4};
26 │ 	int c[6] = {5, 4, 3, 2, 1, 0};
27 │ 	int d[6] = {3, 0, 4, 1, 2, 5};
28 │ 	int e[6] = {0, 4, 5, 1, 2, 3};
29 │ 	int** v = malloc (sizeof(int *) * 6);
30 │ 	int size = 6;
31 │ 	int fail = 0;
32 │ 	v[0] = a;
33 │ 	v[1] = b;
34 │ 	v[2] = c;
35 │ 	v[3] = d;
36 │ 	v[4] = e;
37 │ 	v[5] = NULL;
38 │
39 │ 	while (*v != NULL) {
40 │ 	  bubble_sort (*v, size);
41 │ 	  if (!sorted(*v, size))
42 │     	fail++;
43 │ 	  v++;
44 │ 	}
45 │
46 │ 	if (fail)
47 │ 	printf ("Oh no! there were errors!\n");
48 │
49 │ 	return 0;
50 │ } 
```
 Executando:
 ```
 $ ./sort
Oh no! there were errors!
```
BUGS!
Nós temos um problema de lógica no algoritmo de ordenação `bubble_sort`, pois, em uma das iterações nosso programa pensa que o array não está ordenado, incrementando +1 na variável `Fail`. Vamo ver isso:

`make gdb`
```
(gdb) b 42
Breakpoint 1 at 0x40141f: file sort.c, line 42.
(gdb) r
Starting program: /home/meritissimo1/m1/gdb-time-travel/sort 

This GDB supports auto-downloading debuginfo from the following URLs:
  <https://debuginfod.fedoraproject.org/>
Enable debuginfod for this session? (y or [n]) y
Debuginfod has been enabled.
To make this setting permanent, add 'set debuginfod enabled on' to .gdbinit.
[Thread debugging using libthread_db enabled]                                                                                                                                
Using host libthread_db library "/lib64/libthread_db.so.1".

Breakpoint 1, main () at sort.c:42
42              fail++;
(gdb) print **v@6
$1 = {4, 2, 0, 1, 3, 5}
```

Podemos ver que `bubble_sort` realmente não está ordenando corretamente os arrays, e é por isso que o fluxo do código cai em `fail`. Para evitar percorrer todas as iterações, podemos manter o breakpoint na linha 42, onde `fail` é incrementado, reiniciar o programa do início usando `start`, e então usar os recursos de viagem no tempo do GDB.

## Como viajar no tempo?

```
(gdb) start
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Temporary breakpoint 2 at 0x401289: file sort.c, line 24.
Starting program: /home/meritissimo1/m1/gdb-time-travel/sort 
[Thread debugging using libthread_db enabled]                                                                                                                                
Using host libthread_db library "/lib64/libthread_db.so.1".

Temporary breakpoint 2, main () at sort.c:24
24              int a[6] = {0, 3, 1, 5, 4, 2};
(gdb) record
(gdb) cont
Continuing.

Breakpoint 1, main () at sort.c:42
42              fail++;
(gdb)
```
Observe que temos que iniciar o processo de depuração antes de usar o `record`, isso porque o GDB redefine o estado de gravação entre as execuções.

Estamos de volta onde estávamos antes, mas agora podemos voltar ao final do bubble_sort. Para isso, vejamos os próximos dois comandos interessantes: `reverse-next (rn)` e `reverse-step (rs)`. Ambos respectivamente revertem a última instrução executada e revertem a execução da última instrução também, mas se a última instrução foi uma chamada de função, entram na função e a revertem também.

```
(gdb) list
37              v[5] = NULL;
38
39              while (*v != NULL) {
40                bubble_sort (*v, size);
41                if (!sorted(*v, size))
42              fail++;
43                v++;
44              }
45
46              if (fail)
(gdb) reverse-next
41                if (!sorted(*v, size))
(gdb) rs
bubble_sort (vec=0x7fffffffdcb0, size=6) at sort.c:13
13       }
(gdb)
```
Neste caso, estamos a um passo da chamada de `sorted`, portando, precisamos continuar avançando na direção contrária para chegar até `bubble_sort`. Para isso usamos `set exec-direction reverse`

```
(gdb) set exec-direction reverse
(gdb) b bubble_sort 
Breakpoint 3 at 0x401141: file sort.c, line 4.
(gdb) cont
Continuing.

Breakpoint 3, bubble_sort (vec=0x7fffffffdcb0, size=6) at sort.c:4
4         for (int i = 0; i < size; i++) {
(gdb) set exec-direction forward
(gdb)
```

Observe que conduzimos para o passado o fluxo do código para chegamos até o breapoint em `bubble_sort` e depois voltamos o fluxo para o futuro para agora podermos percorrer a função e encontrar o erro. *Fazendo uma analogia, esse comando funciona como se fosse a cartinha "reverse" do uno...* SIMPLESMENTE SENSACIONAL!!

A partir desse ponto podemos percorrer por `bubble_sort` usando `next (n)` e ver que o problema é resolvido resetando j para 0 na linha 5 -> `(int j = 0)`.

## Agradecimentos
Este artigo é uma breve demonstração em português do [artigo original](https://developers.redhat.com/articles/2024/08/08/using-gdb-time-travel# ) feito pela [Guinevere Larsen](https://developers.redhat.com/author/guinevere-larsen).

Muito obrigado Guinevere por compartilhar seu conhecimento de maneira tão clara e descontraída. se ou soubesse disso em outros tempos teria sido muito útil !! (ainda me lembro do dia que comentei com um amigo sobre isso... "O gdb podia voltar no tempo..." haha).

Fica aqui todo meu agradecimento a ela :)

esse post pode ser encontrado aqui também [tabnews](https://www.tabnews.com.br/meritissimo1/usando-gdb-para-viajar-no-tempo)
