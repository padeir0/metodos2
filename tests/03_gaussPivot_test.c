/* Testes unitários pra `03_gaussPivot.h` gerado com Claude Sonnet 5.

   Também inclui um teste comparando o erro numérico entre a eliminação
   SEM pivoteamento (02_gauss.h) e as duas variantes COM pivoteamento,
   pra verificar que o pivoteamento realmente reduz o erro.

   Recomendo que rode com:
      gcc -Wall -fsanitize=address,undefined -Wextra -Werror -pedantic -Wconversion -Wstrict-prototypes -std=c99 -O2 03_gaussPivot_test.c -o out_gausspivottest
*/

#include "common.h"
#include "../lib/01_subst.h"
#include "../lib/02_gauss.h"
#include "../lib/03_gaussPivot.h"

/* BEGIN: testing i_matrix_findLargestRow */

// diferente de i_matrix_findFirstNonNullRow (02_gauss.h), essa busca é por
// MAGNITUDE, não pela primeira entrada não-nula -- por isso o -5 (não o 1,
// que aparece primeiro) é o escolhido.
bool test_findLargestRow_picksLargestMagnitudeNotFirstNonzero(void) {
  Matrix* a = matrix_new(3, 1);
  double vals[] = {1, -5, 3};
  matrix_set(vals, 3, 1, a);

  int row = i_matrix_findLargestRow(a, 0, 0, 1e-9);
  bool ok = row == 1;

  matrix_free(&a);
  return ok;
}

// busca começa em `row_start`, ignorando entradas antes dele -- mesmo que
// a entrada de maior módulo (9, linha 0) esteja fora do intervalo.
bool test_findLargestRow_respectsRowStart(void) {
  Matrix* a = matrix_new(4, 1);
  double vals[] = {9, -2, 4, 1};
  matrix_set(vals, 4, 1, a);

  int row = i_matrix_findLargestRow(a, 1, 0, 1e-9);
  bool ok = row == 2;

  matrix_free(&a);
  return ok;
}

// coluna inteira (a partir de row_start) é zero dentro da tolerância:
// retorna -1, sinalizando matriz não-invertível.
bool test_findLargestRow_returnsMinusOneWhenAllZero(void) {
  Matrix* a = matrix_new(3, 1);
  double vals[] = {0, 0, 0};
  matrix_set(vals, 3, 1, a);

  int row = i_matrix_findLargestRow(a, 0, 0, 1e-9);
  bool ok = row == -1;

  matrix_free(&a);
  return ok;
}

// só olha a coluna pedida, não a linha inteira.
bool test_findLargestRow_onlyChecksGivenColumn(void) {
  Matrix* a = matrix_new(2, 2);
  double vals[] = {
    0, 8, // coluna 0 é zero nas duas linhas, coluna 1 não
    0, 8,
  };
  matrix_set(vals, 2, 2, a);

  int row = i_matrix_findLargestRow(a, 0, 0, 1e-9);
  bool ok = row == -1;

  matrix_free(&a);
  return ok;
}
/* END: testing i_matrix_findLargestRow */

/* BEGIN: testing matrix_sqGaussianEliminationPivot_1 */

// 2x2 sem necessidade de pivoteamento -- mesmo caso de
// test_sqGaussianElimination_noPivotingNeeded (02_gauss_test.c), serve pra
// conferir que Pivot_1 concorda com a eliminação simples quando o pivô já
// é o maior da coluna.
bool test_sqGaussianEliminationPivot_1_noPivotingNeeded(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    2, 1,
    1, 3,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {5, 10};
  matrix_set(bvals, 2, 1, b);

  bool eliminated = matrix_sqGaussianEliminationPivot_1(a, b, 1e-9);

  double expectedA[] = {
    2, 1,
    0, 2.5,
  };
  double expectedB[] = {5, 7.5};
  bool ok = eliminated &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// pivô da diagonal (1) é NÃO-NULO, então i_matrix_findFirstNonNullRow (Gauss
// sem pivoteamento) não trocaria de linha -- mas 5 tem módulo maior, então
// Pivot_1 troca mesmo assim. É essa troca por magnitude que diferencia
// Pivot_1 do Gauss simples.
// Cálculo à mão: swap(0,1): A=[[5,1],[1,2]], B=[12,5];
// multiple=-(1/5)=-0.2; row1 <- row1+(-0.2)*row0 = [1-1, 2-0.2] = [0,1.8];
// B1 <- 5+(-0.2)*12 = 2.6.
bool test_sqGaussianEliminationPivot_1_swapsOnLargerMagnitudeEvenWithNonzeroDiagonal(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    1, 2,
    5, 1,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {5, 12};
  matrix_set(bvals, 2, 1, b);

  bool eliminated = matrix_sqGaussianEliminationPivot_1(a, b, 1e-9);

  double expectedA[] = {
    5, 1,
    0, 1.8,
  };
  double expectedB[] = {12, 2.6};
  bool ok = eliminated &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// matriz singular (linha 1 é múltipla da linha 0): depois de eliminar a
// coluna 0, a linha 1 fica inteiramente zero -> retorna false.
bool test_sqGaussianEliminationPivot_1_falseForSingularMatrix(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    1, 2,
    2, 4,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {1, 1};
  matrix_set(bvals, 2, 1, b);

  bool ok = matrix_sqGaussianEliminationPivot_1(a, b, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// integração: elimina, resolve por substituição, e confere a solução contra
// o sistema ORIGINAL. Mesma matriz de test_sqGaussianElimination_endToEndSolveAndVerify
// (02_gauss_test.c) e de exemplos/gauss_01.c -- útil pra comparar com o Gauss
// sem pivoteamento na mesma entrada.
bool test_sqGaussianEliminationPivot_1_endToEndSolveAndVerify(void) {
  Matrix* a = matrix_new(4, 4);
  double avals[] = {
    1.0, 3.0, 2.0, 1,
      0, 3.0,   1, 1,
      0,   0,   1, 0,
     -1,   0,   0, 4,
  };
  matrix_set(avals, 4, 4, a);
  Matrix* b = matrix_new(4, 1);
  double bvals[] = {1.0, 2.0, 0, 3.0};
  matrix_set(bvals, 4, 1, b);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* bOrig = matrix_newCopy(b);
  Matrix* x = matrix_new(4, 1);

  bool eliminated = matrix_sqGaussianEliminationPivot_1(a, b, 1e-9);
  bool upper = eliminated && matrix_isUpperTriangular(a, 1e-9);
  bool solved = upper && matrix_solveBySubstitution(a, x, b, 1e-9);
  bool verified = solved && matrix_verifySolution(aOrig, x, bOrig, 1e-9);

  double expectedX[] = {-1, 0.5, 0, 0.5};
  bool ok = eliminated && upper && solved && verified &&
            expectMatrix(x, expectedX, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&aOrig);
  matrix_free(&bOrig);
  matrix_free(&x);
  return ok;
}
/* END: testing matrix_sqGaussianEliminationPivot_1 */

/* BEGIN: testing i_matrix_createMaxArray */

// regressão do bug do `fabs` ausente: uma linha só-negativa precisa que o
// maior módulo (8) seja encontrado mesmo sem nenhum valor positivo na linha.
bool test_createMaxArray_usesAbsoluteValue(void) {
  Matrix* a = matrix_new(1, 3);
  double avals[] = {-5, -2, -8};
  matrix_set(avals, 1, 3, a);

  Matrix* s = i_matrix_createMaxArray(a);
  double expected[] = {8};
  bool ok = expectMatrix(s, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&s);
  return ok;
}

// regressão do bug do índice `j` não resetado entre linhas: cada linha
// precisa do seu PRÓPRIO máximo, não do máximo travado na primeira linha
// (ou em DBL_MIN, como acontecia com o bug).
bool test_createMaxArray_computesIndependentlyPerRow(void) {
  Matrix* a = matrix_new(3, 2);
  double avals[] = {
    3,  1,
    2,  9,
    5,  4,
  };
  matrix_set(avals, 3, 2, a);

  Matrix* s = i_matrix_createMaxArray(a);
  double expected[] = {3, 9, 5};
  bool ok = expectMatrix(s, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&s);
  return ok;
}

bool test_createMaxArray_singleRow(void) {
  Matrix* a = matrix_new(1, 3);
  double avals[] = {-2, 7, -9};
  matrix_set(avals, 1, 3, a);

  Matrix* s = i_matrix_createMaxArray(a);
  double expected[] = {9};
  bool ok = expectMatrix(s, expected, 1e-9);

  matrix_free(&a);
  matrix_free(&s);
  return ok;
}
/* END: testing i_matrix_createMaxArray */

/* BEGIN: testing i_matrix_findLargestRelativeRow */

// O ponto central do pivoteamento escalonado: a linha 0 tem a MAIOR entrada
// bruta na coluna (2 > 1), mas também é a de escala MAIOR (1000), então sua
// entrada é quase irrelevante perto do resto da própria linha (razão
// 2/1000 = 0.002). A linha 1 tem escala 1 e razão 1/1 = 1.0, então é ela a
// escolhida -- diferente de i_matrix_findLargestRow, que olharia só a
// magnitude bruta e escolheria a linha 0.
bool test_findLargestRelativeRow_differsFromPlainMagnitudePivoting(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    2, 1000,
    1,    1,
  };
  matrix_set(avals, 2, 2, a);

  Matrix* s = i_matrix_createMaxArray(a);
  int plainRow = i_matrix_findLargestRow(a, 0, 0, 1e-9);
  int scaledRow = i_matrix_findLargestRelativeRow(a, s, 0, 0, 1e-9);

  bool ok = plainRow == 0 && scaledRow == 1;

  matrix_free(&a);
  matrix_free(&s);
  return ok;
}

// busca começa em `row_start`: a linha 0 tem a maior razão (100/1 = 100),
// mas é ignorada porque row_start=1; entre as linhas 1 e 2, a linha 2 vence
// (50/1 = 50 > 1/1 = 1).
bool test_findLargestRelativeRow_respectsRowStart(void) {
  Matrix* a = matrix_new(3, 1);
  double avals[] = {100, 1, 50};
  matrix_set(avals, 3, 1, a);
  Matrix* s = matrix_new(3, 1);
  double svals[] = {1, 1, 1};
  matrix_set(svals, 3, 1, s);

  int row = i_matrix_findLargestRelativeRow(a, s, 1, 0, 1e-9);
  bool ok = row == 2;

  matrix_free(&a);
  matrix_free(&s);
  return ok;
}

// coluna inteira (a partir de row_start) é zero: todas as razões ficam em
// 0, então retorna -1.
bool test_findLargestRelativeRow_returnsMinusOneWhenAllZeroFromRowStart(void) {
  Matrix* a = matrix_new(3, 2);
  double avals[] = {
    9, 9,
    0, 9, // coluna 0 zerada a partir daqui
    0, 9,
  };
  matrix_set(avals, 3, 2, a);
  Matrix* s = i_matrix_createMaxArray(a);

  int row = i_matrix_findLargestRelativeRow(a, s, 1, 0, 1e-9);
  bool ok = row == -1;

  matrix_free(&a);
  matrix_free(&s);
  return ok;
}
/* END: testing i_matrix_findLargestRelativeRow */

/* BEGIN: testing matrix_sqGaussianEliminationPivot_2 */

// 2x2 sem necessidade de pivoteamento -- mesmo caso de
// test_sqGaussianEliminationPivot_1_noPivotingNeeded, serve pra conferir que
// Pivot_2 concorda com as outras duas eliminações quando não há troca.
bool test_sqGaussianEliminationPivot_2_noPivotingNeeded(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    2, 1,
    1, 3,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {5, 10};
  matrix_set(bvals, 2, 1, b);

  bool eliminated = matrix_sqGaussianEliminationPivot_2(a, b, 1e-9);

  double expectedA[] = {
    2, 1,
    0, 2.5,
  };
  double expectedB[] = {5, 7.5};
  bool ok = eliminated &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// mesma matriz de test_findLargestRelativeRow_differsFromPlainMagnitudePivoting,
// agora rodando a eliminação completa nas duas variantes a partir do MESMO
// A e B originais: Pivot_1 (magnitude bruta) mantém a linha 0 como pivô
// (2 > 1, sem troca); Pivot_2 (escalonado) troca pra linha 1, porque a
// escala da linha 0 (1000) faz sua entrada de coluna valer pouco (razão
// 0.002) perto da razão 1.0 da linha 1. `x*=[1,1]` em ambos os casos --
// aqui o interesse é qual pivô cada um escolhe, não a solução final.
// Cálculo à mão do Pivot_2: swap(0,1): A=[[1,1],[2,1000]], B=[2,1002];
// multiple=-(2/1)=-2; row1 <- row1+(-2)*row0 = [2-2,1000-2] = [0,998];
// B1 <- 1002+(-2)*2 = 998.
bool test_sqGaussianEliminationPivot_2_swapsOnScaleWherePivot_1DoesNot(void) {
  Matrix* aOriginal = matrix_new(2, 2);
  double avals[] = {
    2, 1000,
    1,    1,
  };
  matrix_set(avals, 2, 2, aOriginal);
  Matrix* bOriginal = matrix_new(2, 1);
  double bvals[] = {1002, 2};
  matrix_set(bvals, 2, 1, bOriginal);

  Matrix* a1 = matrix_newCopy(aOriginal);
  Matrix* b1 = matrix_newCopy(bOriginal);
  bool eliminated1 = matrix_sqGaussianEliminationPivot_1(a1, b1, 1e-9);
  bool pivot1KeptRow0 = closeEnough(matrix_at(a1, 0, 0), 2, 1e-9);

  Matrix* a2 = matrix_newCopy(aOriginal);
  Matrix* b2 = matrix_newCopy(bOriginal);
  bool eliminated2 = matrix_sqGaussianEliminationPivot_2(a2, b2, 1e-9);

  double expectedA2[] = {
    1, 1,
    0, 998,
  };
  double expectedB2[] = {2, 998};
  bool ok = eliminated1 && pivot1KeptRow0 &&
            eliminated2 &&
            expectMatrix(a2, expectedA2, 1e-9) &&
            expectMatrix(b2, expectedB2, 1e-9);

  matrix_free(&aOriginal);
  matrix_free(&bOriginal);
  matrix_free(&a1);
  matrix_free(&b1);
  matrix_free(&a2);
  matrix_free(&b2);
  return ok;
}

// matriz singular (linha 1 é múltipla da linha 0) -> retorna false.
bool test_sqGaussianEliminationPivot_2_falseForSingularMatrix(void) {
  Matrix* a = matrix_new(2, 2);
  double avals[] = {
    1, 2,
    2, 4,
  };
  matrix_set(avals, 2, 2, a);
  Matrix* b = matrix_new(2, 1);
  double bvals[] = {1, 1};
  matrix_set(bvals, 2, 1, b);

  bool ok = matrix_sqGaussianEliminationPivot_2(a, b, 1e-9) == false;

  matrix_free(&a);
  matrix_free(&b);
  return ok;
}

// REGRESSÃO do bug do maxMatrix (s_i) não acompanhar as trocas de linha:
// esse 3x3 só passa se `s_i` continuar associado à linha certa depois de
// CADA troca -- inclusive na segunda troca, que depende da primeira ter
// sido propagada corretamente pro maxMatrix.
//
// s original (por linha): linha0=[1,5,1000]->1000; linha1=[1,1,1]->1;
// linha2=[2,40,3]->40.
//
// k=0, coluna 0: razões = 1/1000=0.001, 1/1=1.0, 2/40=0.05 -> linha 1 vence,
// swap(0,1). A=[[1,1,1],[1,5,1000],[2,40,3]], s=[1,1000,40] (s ACOMPANHA a
// troca: a posição 1 agora tem a linha 0 original, escala 1000).
// Eliminação: multiple(pos1)=-(1/1)=-1 -> [0,4,999]; multiple(pos2)=-(2/1)=-2 -> [0,38,1].
//
// k=1, coluna 1, posições 1 e 2: se `s` tivesse ficado parado (bug), a
// posição 1 ainda carregaria a escala antiga (1, da linha 1 original), e a
// razão 4/1=4.0 venceria erroneamente a razão certa da posição 2 (38/40=0.95)
// -- nenhuma troca aconteceria e o pivô ficaria em 4 (mal condicionado).
// Com `s` correto (s[1]=1000, s[2]=40): razões = 4/1000=0.004, 38/40=0.95 ->
// posição 2 vence, swap(1,2). A=[[1,1,1],[0,38,1],[0,0,?]], onde
// multiple=-(4/38)=-2/19 e ? = 999-(2/19)*1 = 18979/19.
bool test_sqGaussianEliminationPivot_2_maxArrayTracksRowIdentityAcrossSwaps(void) {
  Matrix* a = matrix_new(3, 3);
  double avals[] = {
    1,  5, 1000,
    1,  1,    1,
    2, 40,    3,
  };
  matrix_set(avals, 3, 3, a);
  Matrix* b = matrix_new(3, 1);
  double bvals[] = {1006, 3, 45}; // = A * [1,1,1]^T
  matrix_set(bvals, 3, 1, b);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* bOrig = matrix_newCopy(b);
  Matrix* x = matrix_new(3, 1);

  bool eliminated = matrix_sqGaussianEliminationPivot_2(a, b, 1e-9);
  bool upper = eliminated && matrix_isUpperTriangular(a, 1e-9);
  bool solved = upper && matrix_solveBySubstitution(a, x, b, 1e-9);
  bool verified = solved && matrix_verifySolution(aOrig, x, bOrig, 1e-9);

  double lastPivot = 18979.0 / 19.0;
  double expectedA[] = {
    1,  1, 1,
    0, 38, 1,
    0,  0, lastPivot,
  };
  double expectedB[] = {3, 39, lastPivot};
  double expectedX[] = {1, 1, 1};

  bool ok = eliminated && upper && solved && verified &&
            expectMatrix(a, expectedA, 1e-9) &&
            expectMatrix(b, expectedB, 1e-9) &&
            expectMatrix(x, expectedX, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&aOrig);
  matrix_free(&bOrig);
  matrix_free(&x);
  return ok;
}

// integração: mesma matriz 4x4 de test_sqGaussianEliminationPivot_1_endToEndSolveAndVerify
// e de test_sqGaussianElimination_endToEndSolveAndVerify (02_gauss_test.c).
bool test_sqGaussianEliminationPivot_2_endToEndSolveAndVerify(void) {
  Matrix* a = matrix_new(4, 4);
  double avals[] = {
    1.0, 3.0, 2.0, 1,
      0, 3.0,   1, 1,
      0,   0,   1, 0,
     -1,   0,   0, 4,
  };
  matrix_set(avals, 4, 4, a);
  Matrix* b = matrix_new(4, 1);
  double bvals[] = {1.0, 2.0, 0, 3.0};
  matrix_set(bvals, 4, 1, b);

  Matrix* aOrig = matrix_newCopy(a);
  Matrix* bOrig = matrix_newCopy(b);
  Matrix* x = matrix_new(4, 1);

  bool eliminated = matrix_sqGaussianEliminationPivot_2(a, b, 1e-9);
  bool upper = eliminated && matrix_isUpperTriangular(a, 1e-9);
  bool solved = upper && matrix_solveBySubstitution(a, x, b, 1e-9);
  bool verified = solved && matrix_verifySolution(aOrig, x, bOrig, 1e-9);

  double expectedX[] = {-1, 0.5, 0, 0.5};
  bool ok = eliminated && upper && solved && verified &&
            expectMatrix(x, expectedX, 1e-9);

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&aOrig);
  matrix_free(&bOrig);
  matrix_free(&x);
  return ok;
}
/* END: testing matrix_sqGaussianEliminationPivot_2 */

/* BEGIN: testing numerical error, pivoted vs. naive (02_gauss.h) */

// O sistema é BEM-CONDICIONADO (A = matriz de 1's com uma diagonal ínfima;
// autovalores O(1) -- ver discussão na revisão) e tem solução exata
// conhecida x*=[1,1,1,1]. O problema não é a matriz, é o MÉTODO: a
// eliminação sem pivoteamento (i_matrix_findFirstNonNullRow) aceita o
// pivô ínfimo da diagonal só por ele ser não-nulo, gerando um multiplicador
// gigante (~1/eps) que estoura a precisão de double e apaga o "1" de termos
// como `1 - 1/eps`. As duas variantes com pivoteamento trocam pra um pivô
// de ordem 1 em cada coluna e não sofrem disso.
//
// error tolerance: erro do ingênuo fica em ~1e-8 (bem acima de 1e-9), erro
// de ambos os pivoteados fica em ~4e-16 (grandeza do épsilon de máquina).
// Os limiares abaixo têm folga de sobra pros dois lados.
bool test_pivotedGaussHasLessErrorThanNaiveGauss(void) {
  double eps = 1e-8;
  Matrix* a = matrix_new(4, 4);
  double avals[] = {
    eps,   1,   1,   1,
      1, eps,   1,   1,
      1,   1, eps,   1,
      1,   1,   1, eps,
  };
  matrix_set(avals, 4, 4, a);

  Matrix* xStar = matrix_new(4, 1);
  double xStarVals[] = {1, 1, 1, 1};
  matrix_set(xStarVals, 4, 1, xStar);

  Matrix* b = matrix_new(4, 1);
  matrix_mult(a, xStar, b); // B = A*x*, então a solução exata é conhecida

  Matrix* aNaive = matrix_newCopy(a);
  Matrix* bNaive = matrix_newCopy(b);
  Matrix* xNaive = matrix_new(4, 1);
  bool okNaive = matrix_sqGaussianElimination(aNaive, bNaive, 1e-9) &&
                 matrix_solveBySubstitution(aNaive, xNaive, bNaive, 1e-9);

  Matrix* aPivot1 = matrix_newCopy(a);
  Matrix* bPivot1 = matrix_newCopy(b);
  Matrix* xPivot1 = matrix_new(4, 1);
  bool okPivot1 = matrix_sqGaussianEliminationPivot_1(aPivot1, bPivot1, 1e-9) &&
                  matrix_solveBySubstitution(aPivot1, xPivot1, bPivot1, 1e-9);

  Matrix* aPivot2 = matrix_newCopy(a);
  Matrix* bPivot2 = matrix_newCopy(b);
  Matrix* xPivot2 = matrix_new(4, 1);
  bool okPivot2 = matrix_sqGaussianEliminationPivot_2(aPivot2, bPivot2, 1e-9) &&
                  matrix_solveBySubstitution(aPivot2, xPivot2, bPivot2, 1e-9);

  double errNaive = 0, errPivot1 = 0, errPivot2 = 0;
  int i = 0;
  while (i < 4) {
    double eN = fabs(matrix_at(xNaive, i, 0) - matrix_at(xStar, i, 0));
    double e1 = fabs(matrix_at(xPivot1, i, 0) - matrix_at(xStar, i, 0));
    double e2 = fabs(matrix_at(xPivot2, i, 0) - matrix_at(xStar, i, 0));
    if (eN > errNaive) errNaive = eN;
    if (e1 > errPivot1) errPivot1 = e1;
    if (e2 > errPivot2) errPivot2 = e2;
    i++;
  }

  bool naiveHasVisibleError = errNaive > 1e-9;
  bool pivot1IsNearMachineEpsilon = errPivot1 < 1e-12;
  bool pivot2IsNearMachineEpsilon = errPivot2 < 1e-12;
  // pivoteado precisa ser pelo menos 1000x mais preciso (na prática é ~2e7x)
  bool pivot1MuchBetter = errNaive > 1000 * errPivot1;
  bool pivot2MuchBetter = errNaive > 1000 * errPivot2;

  bool ok = okNaive && okPivot1 && okPivot2 &&
            naiveHasVisibleError &&
            pivot1IsNearMachineEpsilon && pivot2IsNearMachineEpsilon &&
            pivot1MuchBetter && pivot2MuchBetter;

  if (!ok) {
    fprintf(stderr, "  errNaive=%e errPivot1=%e errPivot2=%e\n",
            errNaive, errPivot1, errPivot2);
  }

  matrix_free(&a);
  matrix_free(&b);
  matrix_free(&xStar);
  matrix_free(&aNaive);
  matrix_free(&bNaive);
  matrix_free(&xNaive);
  matrix_free(&aPivot1);
  matrix_free(&bPivot1);
  matrix_free(&xPivot1);
  matrix_free(&aPivot2);
  matrix_free(&bPivot2);
  matrix_free(&xPivot2);
  return ok;
}
/* END: testing numerical error, pivoted vs. naive (02_gauss.h) */

/* BEGIN: DRIVER CODE */
Tester tests[] = {
  {"test_findLargestRow_picksLargestMagnitudeNotFirstNonzero", test_findLargestRow_picksLargestMagnitudeNotFirstNonzero},
  {"test_findLargestRow_respectsRowStart", test_findLargestRow_respectsRowStart},
  {"test_findLargestRow_returnsMinusOneWhenAllZero", test_findLargestRow_returnsMinusOneWhenAllZero},
  {"test_findLargestRow_onlyChecksGivenColumn", test_findLargestRow_onlyChecksGivenColumn},

  {"test_sqGaussianEliminationPivot_1_noPivotingNeeded", test_sqGaussianEliminationPivot_1_noPivotingNeeded},
  {"test_sqGaussianEliminationPivot_1_swapsOnLargerMagnitudeEvenWithNonzeroDiagonal", test_sqGaussianEliminationPivot_1_swapsOnLargerMagnitudeEvenWithNonzeroDiagonal},
  {"test_sqGaussianEliminationPivot_1_falseForSingularMatrix", test_sqGaussianEliminationPivot_1_falseForSingularMatrix},
  {"test_sqGaussianEliminationPivot_1_endToEndSolveAndVerify", test_sqGaussianEliminationPivot_1_endToEndSolveAndVerify},

  {"test_createMaxArray_usesAbsoluteValue", test_createMaxArray_usesAbsoluteValue},
  {"test_createMaxArray_computesIndependentlyPerRow", test_createMaxArray_computesIndependentlyPerRow},
  {"test_createMaxArray_singleRow", test_createMaxArray_singleRow},

  {"test_findLargestRelativeRow_differsFromPlainMagnitudePivoting", test_findLargestRelativeRow_differsFromPlainMagnitudePivoting},
  {"test_findLargestRelativeRow_respectsRowStart", test_findLargestRelativeRow_respectsRowStart},
  {"test_findLargestRelativeRow_returnsMinusOneWhenAllZeroFromRowStart", test_findLargestRelativeRow_returnsMinusOneWhenAllZeroFromRowStart},

  {"test_sqGaussianEliminationPivot_2_noPivotingNeeded", test_sqGaussianEliminationPivot_2_noPivotingNeeded},
  {"test_sqGaussianEliminationPivot_2_swapsOnScaleWherePivot_1DoesNot", test_sqGaussianEliminationPivot_2_swapsOnScaleWherePivot_1DoesNot},
  {"test_sqGaussianEliminationPivot_2_falseForSingularMatrix", test_sqGaussianEliminationPivot_2_falseForSingularMatrix},
  {"test_sqGaussianEliminationPivot_2_maxArrayTracksRowIdentityAcrossSwaps", test_sqGaussianEliminationPivot_2_maxArrayTracksRowIdentityAcrossSwaps},
  {"test_sqGaussianEliminationPivot_2_endToEndSolveAndVerify", test_sqGaussianEliminationPivot_2_endToEndSolveAndVerify},

  {"test_pivotedGaussHasLessErrorThanNaiveGauss", test_pivotedGaussHasLessErrorThanNaiveGauss},
};
#define TEST_LEN (int)(sizeof(tests) / sizeof(tests[0]))

int main(void) {
  int failed = run_tests("03_gaussPivot", tests, TEST_LEN);
  return failed == 0 ? 0 : 1;
}
/* END: DRIVER CODE */
