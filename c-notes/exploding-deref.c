/**
 * Compile C to LLVM IR
 *
 *     clang -Os -S -emit-llvm runtime/c-notes/exploding-deref.c -o runtime/c-notes/exploding-deref.ll
 *
 * Compile the IR to assembly
 *
 *    llvm-gcc -S -march=x86-64 runtime/c-notes/exploding-deref.ll -o runtime/c-notes/exploding-deref.s
 *
 * The IR makes what happens easy to read:
 *
 *     %3 = load i32**, i32*** %2, align 8
 *     %4 = load i32*, i32** %3, align 8, !tbaa !2
 *     %5 = load i32, i32* %4, align 4, !tbaa !6
 *
 * Since we're running load on stuff that isn't a pointer, things will explode.
 *
 * Digging into the x86 assembler:
 *
 *     movl	$7, -8(%rbp)
 *     movq	-8(%rbp), %rax
 *     movq	(%rax), %rax
 *     movl	(%rax), %esi
 *     movb	$0, %al
 *
 * Reading Intel's manual (3-10 Vol. 1) on memory exceptions:
 *
 *    In most cases, a general-protection exception (#GP) is generated
 *
 * Reading about the MOV instruction (Vol. 2B 4-37), we see the following algorithm:
 *
 *    IF SS is loaded
 *      THEN
 *      IF segment selector is NULL
 *        THEN #GP(0); FI;
 *      IF segment selector index is outside descriptor table limits
 *      OR segment selector's RPL ≠ CPL
 *      OR segment is not a writable data segment
 *        THEN #GP(0); FI;
 *
 * #GP(0) means a general protection error - see Intel manual (6-10 Vol. 1)
 *
 * References:
 *   https://software.intel.com/en-us/articles/intel-sdm
 */
#include <stdio.h>

int main() {
    int hi = 7;
    int*** explodey = (void*)&hi;
    printf("%i\n", ***explodey);
}
