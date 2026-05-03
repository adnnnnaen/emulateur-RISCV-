# Tests manuels — programmes RISC-V à exécuter sur l'émulateur

Petits programmes assembleur pour valider l'émulateur en bout-en-bout.
Chacun calcule une valeur et la pose dans `a0`, puis fait `ecall`.

## Contenu

| Fichier | Calcul | Résultat attendu (a0) |
|---|---|---|
| `01_set_a0.S` | Met simplement 42 dans a0 | 42 (0x2A) |
| `02_multiply.S` | 5 × 7 par addition itérative | 35 (0x23) |
| `03_fibonacci.S` | Fibonacci(10) | 55 (0x37) |
| `04_factorial.S` | 5! = 120 (multiply imbriqué) | 120 (0x78) |
| `05_max_of_three.S` | max(15, 7, 22) | 22 (0x16) |
| `06_popcount.S` | Nombre de bits à 1 dans 0xFF00FF00 | 16 (0x10) |

## Comment lancer

Depuis WSL Ubuntu, à la racine du projet :

```bash
# 1. (Une fois) Compile l'émulateur si pas déjà fait
cmake -B build -G Ninja && cmake --build build

# 2. Va dans le dossier des tests manuels
cd programs/manual_tests

# 3. Compile tous les tests assembleur en .bin
make

# 4a. Lance un test isolé
../../build/src/riscv_emu 03_fibonacci.bin

# 4b. OU lance tous les tests d'un coup, avec vérification automatique
./run_all.sh
```

## Format des tests

Tous les fichiers `.S` suivent ce squelette :

```asm
.section .text
.global _start
_start:
    # ... ton code ici ...
    ecall            # halte l'émulateur, a0 inspecté par le main
```

Convention : si `a0 == 0` à la fin, c'est un succès (exit code 0). Sinon
exit code 1 avec la valeur de `a0` affichée.

## Pour aller plus loin

Désassemble un binaire pour voir le code généré exact :
```bash
riscv64-unknown-elf-objdump -d 03_fibonacci.o
```

Modifie un `.S`, recompile (`make`), relance — boucle d'expérimentation rapide.
