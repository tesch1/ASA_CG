#include <math.h>
#include <limits.h>
#include <float.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define PRIVATE static
#define ZERO ((double) 0)
#define ONE  ((double) 1)
#define TWO  ((double) 2)
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

typedef struct asa_com_struct /* common variables */
{
    /* parameters computed by the code */
    double          *lo ; /* lower bounds */
    double          *hi ; /* lower bounds */
    double           *x ; /* current estimate for solution */
    double           *d ; /* current search direction */
    double           *g ; /* current gradient */
    double       *xtemp ; /* x + alpha*d */
    double       *gtemp ; /* gradient at x + alpha*d */
    double          *pg ; /* projected gradient */
    double *lastfvalues ; /* previous function values */
    double      minstep ; /* smallest step to reach a bound */
    double      maxstep ; /* step which makes all bounds active */
    double        bdist ; /* closest distance to bounds */
    int         minflag ; /* T (minstep correct), F (minstep is lower bound) */
    int           nfree ; /* number of free variables */
    INT          *ifree ; /* indices of free variables */
    INT               n ; /* problem dimension, saved for reference */
    INT              n5 ; /* n % 5 */
    INT              nf ; /* total number of function evaluations */
    INT              ng ; /* total number of gradient evaluations */
    INT         cbbiter ; /* total number of cbb iterations */
    INT          cgiter ; /* total number of cg iterations */
    INT         cbbfunc ; /* total cbb function evaluations */
    INT         cbbgrad ; /* total cbb gradient evaluations */
    INT          cgfunc ; /* total cg function evaluations */
    INT          cggrad ; /* total cg gradient evaluations */
    INT         cgmaxit ; /* maximum number of iterations in one pass of cg */
    INT         pgmaxit ; /* max iterations in one pass of gradient projection*/
    INT       pgmaxfunc ; /* max function evals/pass of gradient projection */

    double    SmallCost ; /* |f| <= SmallCost => set PertRule = F */
    double        alpha ; /* stepsize along search direction */
    double            f ; /* function value for step alpha */
    double      f_debug ; /* function value at time of debug failure */
    double           df ; /* function derivative for step alpha */
    double        fpert ; /* perturbation is eps*Ck if PertRule is T */
    double          eps ; /* current value of eps */
    double          tol ; /* computing tolerance */
    double           f0 ; /* old function value */
    double          df0 ; /* old derivative */
    double           Ck ; /* average cost as given by the rule:
                             Qk = Qdecay*Qk + 1, Ck += (fabs (f) - Ck)/Qk */
    double     wolfe_hi ; /* upper bound for slope in Wolfe test */
    double     wolfe_lo ; /* lower bound for slope in Wolfe test */
    double    awolfe_hi ; /* upper bound for slope, approximate Wolfe test */
    int          QuadOK ; /* T (quadratic step successful) */
    int        UseCubic ; /* T (use cubic step) F (use secant step) */
    int            neps ; /* number of time eps updated */
    int        PertRule ; /* T => estimated error in function value is eps*Ck,
                            F => estimated error in function value is eps */
    int          QuadF  ; /* T => function appears to be quadratic */
    int          AWolfe ; /* F (use Wolfe line search)
                             T (use approximate Wolfe line search)
                             do not change user's AWolfe, this value can be
                             changed based on AWolfeFac */
    int       DimReduce ;  /*T (compressed problem, nfree < n)
                             F (work in full space, nfree = n)*/
    int         AArmijo ; /* F (use nonmonotone Armijo line search)
                             T (use approximate nonmonotone Armijo line search)
                             do not change user's AArmijo, this value can be
                             changed based on AArmijoFac */
    int           Wolfe ; /* T (means code reached the Wolfe part of cg_line */
    int           nslow ; /* after nslow iterations without strict improvement in cg */
    double          rho ; /* either Parm->rho or Parm->nan_rho */
    double     alphaold ; /* previous value for stepsize alpha */
    double          sts ; /* ||s||^2 */
    double          gtd ; /* g'd */
    double          sty ; /* s'y */
    double      pert_lo ; /* perturbation of lower bounds */
    double      pert_hi ; /* perturbation of upper bounds */
    double         tau1 ; /* if ginorm < tau1*pgnorm, continue gp steps  */
    double         tau2 ; /* ginorm < tau2*pgnorm =>
                             subproblem solved in cgdescent */
    double pgnorm_start ; /* ||Proj (x_0 - g_0) - x_0||_infty */
    double       pgnorm ; /* project gradient norm */
    double       ginorm ; /* norm of inactive gradient components */
    asacg_parm  *cgParm ; /* cg user parameters */
    asa_parm   *asaParm ; /* asa user parameters */
    asa_objective *user ; /* information passed to user when function or
                             gradient must be evaluated */
    double        (*value) (asa_objective *) ; /* evaluate objective function */
    void           (*grad) (asa_objective *) ; /* evaluate objective gradient */
    double      (*valgrad) (asa_objective *) ; /* function & gradient if given*/
} asa_com ;

/* prototypes */

PRIVATE int asa_descent /*  return:
                      -5 (ginorm < tau2*pgnorm without hitting boundary)
                      -4 (ginorm >=tau2*pgnorm, many x components hit boundary)
                      -3 (ginorm >=tau2*pgnorm, one x component hits boundary)
                      -2 (function value became nan)
                      -1 (starting function value is nan)
                       0 (convergence tolerance satisfied)
                       1 (change in func <= feps*|f|)
                       2 (total iterations exceeded maxit)
                       3 (slope always negative in line search)
                       4 (number secant iterations exceed nsecant)
                       5 (search direction not a descent direction)
                       6 (excessive updating of eps)
                       7 (Wolfe conditions never satisfied)
                       8 (debugger is on and the function value increases)
                       9 (no cost or gradient improvement in
                          2n + Parm->nslow iterations)
                      10 (out of memory)
                      11 (function nan or +-INF and could not be repaired)
                      12 (invalid choice for memory parameter) */
(
    asa_com *Com
) ;

/* prototypes */

PRIVATE int asa_Wolfe
(
    double       alpha , /* stepsize */
    double           f , /* function value associated with stepsize alpha */
    double        dphi , /* derivative value associated with stepsize alpha */
    asa_com        *Com
) ;

PRIVATE int asa_tol
(
    double      pgnorm, /* projected gradient sup-norm */
    asa_com       *Com
) ;

PRIVATE int asa_line
(
    asa_com   *Com
) ;

PRIVATE int asacg_contract /* same as cg_contract */
(
    double    *A, /* left side of bracketing interval */
    double   *fA, /* function value at a */
    double   *dA, /* derivative at a */
    double    *B, /* right side of bracketing interval */
    double   *fB, /* function value at b */
    double   *dB, /* derivative at b */
    asa_com  *Com  /* cg com structure */
) ;

PRIVATE double asacg_cubic /* same as cg_cubic */
(
    double  a,
    double fa, /* function value at a */
    double da, /* derivative at a */
    double  b,
    double fb, /* function value at b */
    double db  /* derivative at b */
) ;

PRIVATE void asacg_Yk /* same as cg_Yk */
(
    double    *y, /*output vector */
    double *gold, /* initial vector */
    double *gnew, /* search direction */
    double  *yty, /* y'y */
    INT        n  /* length of the vectors */
) ;

PRIVATE int asacg_evaluate /* same as cg_evaluate */
(
    char    *what, /* fg = evaluate func and grad, g = grad only,f = func only*/
    char     *nan, /* y means check function/derivative values for nan */
    asa_com   *Com
) ;

PRIVATE void asa_matvec
(
    double *y, /* product vector */
    double *A, /* dense matrix */
    double *x, /* input vector */
    int     n, /* number of columns of A */
    INT     m, /* number of rows of A */
    int     w  /* T => y = A*x, F => y = A'*x */
) ;

PRIVATE void asa_trisolve
(
    double *x, /* right side on input, solution on output */
    double *R, /* dense matrix */
    int     m, /* leading dimension of R */
    int     n, /* dimension of triangular system */
    int     w  /* T => Rx = y, F => R'x = y */
) ;

PRIVATE void asa_scale0
(
    double *y, /* output vector */
    double *x, /* input vector */
    double  s, /* scalar */
    int     n /* length of vector */
) ;

PRIVATE void asa_scale
(
    double *y, /* output vector */
    double *x, /* input vector */
    double  s, /* scalar */
    INT     n /* length of vector */
) ;

PRIVATE void asa_daxpy0
(
    double     *x, /* input and output vector */
    double     *d, /* direction */
    double  alpha, /* stepsize */
    int         n  /* length of the vectors */
) ;

PRIVATE void asa_daxpy
(
    double     *x, /* input and output vector */
    double     *d, /* direction */
    double  alpha, /* stepsize */
    INT         n  /* length of the vectors */
) ;

PRIVATE double asa_dot0
(
    double *x, /* first vector */
    double *y, /* second vector */
    int     n /* length of vectors */
) ;


PRIVATE double asa_dot
(
    double *x , /* first vector */
    double *y , /* second vector */
    INT     n   /* length of vectors */
) ;

PRIVATE void asa_copy0
(
    double *y, /* output of copy */
    double *x, /* input of copy */
    int     n  /* length of vectors */
) ;

PRIVATE void asa_copy
(
    double *y, /* output of copy */
    double *x, /* input of copy */
    INT     n  /* length of vectors */
) ;

PRIVATE void asa_update_xy
(
    double *x, /* output of copy */
    double *xnew, /* input of copy */
    double *y, /* output of copy */
    double *ynew, /* input of copy */
    INT     n  /* length of vectors */
) ;

PRIVATE double asa_update_pg /* compute ginorm and pgnorm of g */
(
    double      *x, /* x */
    double      *g, /* gradient */
    double *Ginorm, /* ginorm of g*/ 
    double   bdist, /* distance of x to the boundary is greater than bdist*/
    double     *lo, /* lower bound */
    double     *hi, /* upper bound */
    INT      nfree, /* dimension of subspace */
    INT         n, /* length of vectors */
    asa_com  *Com
) ;

PRIVATE double asa_update_2
(
    double *gold, /* old g */
    double *gnew, /* new g */
    double    *d, /* d */
    INT        n /* length of vectors */
) ;

PRIVATE void asa_update_dg0
(
    double   *gold, /* old g */
    double   *gnew, /* new g */
    double      *d, /* d */
    double *gnorm2, /* 2-norm of g */
    INT          n /* length of vectors */
) ;

PRIVATE double asa_update_dg
(
    double   *gold,
    double   *gnew,
    double      *d,
    double    beta,
    double *gnorm2, /* 2-norm of gnew */
    INT          n /* length of vectors */
) ;

PRIVATE void asa_update_ykyk
(
    double *gold, /* old g */
    double *gnew, /* new g */
    double *Ykyk,
    double *Ykgk,
    INT        n /* length of vectors */
) ;

PRIVATE double asa_update_inf2
(
    double   *gold, /* old g */
    double   *gnew, /* new g */
    double      *d, /* d */
    double *gnorm2, /* 2-norm of g */
    INT          n /* length of vectors */
) ;

PRIVATE void asa_step /* Compute xtemp = x + alpha d */
(
    double *xtemp, /*output vector */
    double     *x, /* initial vector */
    double     *d, /* search direction */
    double  alpha, /* stepsize */
    INT         n  /* length of the vectors */
) ;

PRIVATE void asa_init
(
    double *x, /* input and output vector */
    double  s, /* scalar */
    INT     n /* length of vector */
) ;


PRIVATE double asa_max /* Return max {fabs (x [j]) : 1 <= j < n}*/
(
    double *x,
    INT     n
) ;

PRIVATE void asa_project
(
    double  *xnew,
    double     *x,
    double     *d,
    double  alpha,
    asa_com  *Com   /* cg com structure */
) ;

PRIVATE void asa_maxstep
(
    double       *x, /* current iterate */
    double       *d, /* direction */
    asa_com    *Com
) ;

PRIVATE int asa_grad_proj /*return:
                      -1 (give active constraints to cg routine)
                       0 (convergence tolerance satisfied)
                      14 (number of iterations or function evaluations
                          exceed limit)
                      15 (line search fails)
                      16 (search direction in linesearch is not descent)
                      17 (function value became nan) */
(
    asa_com *Com
) ;

PRIVATE double asa_init_bbstep
(
    asa_com *Com
) ;

PRIVATE double asa_f
(
    double    *x,
    asa_com *Com
) ;

PRIVATE void asa_g
(
    double    *g,
    double    *x,
    asa_com *Com
) ;

PRIVATE double asa_fg
(
    double    *g,
    double    *x,
    asa_com *Com
) ;

PRIVATE int asa_identify
(
   double     *x,
   double     *g,
   double pgnorm,
   asa_com  *Com
) ;

PRIVATE void asa_expandx
(
    double    *x,
    asa_com *Com
) ;

PRIVATE void asa_shrinkx
(
    double    *x,
    asa_com *Com
) ;

PRIVATE void asa_shrinkxg
(
    double    *x,
    double    *g,
    asa_com *Com
) ;

PRIVATE void asa_expand_all
(
    asa_com *Com
) ;

PRIVATE void asa_shrink_all
(
    asa_com *Com
) ;

PRIVATE void asa_printcgParms
(
    asacg_parm  *Parm
) ;

PRIVATE void asa_printParms
(
    asa_parm  *Parm
) ;
