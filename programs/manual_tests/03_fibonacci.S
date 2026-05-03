# Test 03 — Suite de Fibonacci
#
# Calcule fib(10) = 55. Convention : fib(0) = 0, fib(1) = 1.
#
# Algorithme itératif :
#   prev = 0, curr = 1, i = 1
#   tant que i != n :
#       next = prev + curr
#       prev = curr
#       curr = next
#       i++
#   retourne curr (qui est fib(n))
#
# Exerce : addi, add, beq, mv (pseudo = add x, y, zero), boucle conditionnelle.

.section .text
.global _start

_start:
    li   t0, 10       # n = 10 (fib à calculer)
    li   a0, 0        # cas spécial : si n == 0, retourner 0
    beq  t0, zero, end

    li   t1, 0        # prev = 0
    li   a0, 1        # curr = 1 (a0 = registre de résultat)
    li   t3, 1        # i = 1

loop:
    beq  t3, t0, end  # si i == n, terminé (a0 contient fib(n))
    add  t4, t1, a0   # next = prev + curr
    mv   t1, a0       # prev = curr
    mv   a0, t4       # curr = next
    addi t3, t3, 1    # i++
    j    loop

end:
    ecall
