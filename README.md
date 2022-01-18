## Analiza Algoritmilor 2021 - Tema2

### Structură arhivă
```bash
student@aa:$ tree -L 1
.
├── check
├── check_utils
├── check.zip
├── install.sh
├── Makefile.example_cpp
├── Makefile.exampleJava
├── README.md
├── reclame.cpp
├── Reclame.java
├── registre.cpp
├── Registre.java
├── retele.cpp
├── Retele.java
├── sat_oracle.py
├── task.h
├── Task.java
└── tasks

```


### Structură cod 

Pentru rezolvarea problemelor, recomandam modularizare
în următoarele funcții:

1. o funcție care citește de la `stdin` datele de intrare.
2. o funcție care formulează clauzele către oracol.
3. o funcție care apelează oracolul (deja implementată).
4. o funcție care descifrează (prelucrează) răspunsul oracolului.
5. o funcție care afișează rezultatul la `stdout`.

### Good to know

Pentru fiecare din limbajele `C++` și `Java` există o clasă
ce conține cele 5 metode menționate anterior, din care puteți moșteni câte o clasă pentru fiecare problemă in parte (e.g: puteți avea clasele Task1, Task2, Task3 care moștenesc clasa abstractă Task).

Trebuiesc implementate doar metodele 1, 2, 4, 5. Apelarea oracolului este deja realizată de funcția `ask_oracle` (`askOracle`). Va trebui **doar** să o apelați după ce formulați clauzele SAT corespunzătoare fiecărei probleme.

### Makefile

Pentru fiecare problemă, va trebui să fie o regulă corespunzătoare în Makefile run_`<nume_problema`> (e.g: `run_retele`)

Există câte un exemplu pentru fiecare din limbajele `C++` și `Java`. **Nu este permisă folosirea flag-urilor de optimizare.**

### Rulare checker

Pentru a rula checker-ul, folosiți comanda `./check`

Pentru a rula un anumit task, folosiți comanda `./check --task {nume_task}`. (exemplu: `./check --task reclame`)

Testele pentru fiecare problemă se găsesc în folder-ul `tasks/<nume_problema>/tests`. (exemplu: testele pentru problema reclame se găsesc în folder-ul `tasks/reclame/tasks`)

După rularea checker-ului, pentru fiecare test este generat un fișier de output în folderul (`tasks/<nume_problemă>/tests/<XY-nume_problemă>/<XY-nume_problemă.out>`), unde XY este numărul testului
(exemplu: **după** rularea checker-ului, rezultatul testului 00 pentru problema `reclame`, output-ul se va găsi la ` tasks/reclame/tests/00-reclame/00-reclame.out 
`)
