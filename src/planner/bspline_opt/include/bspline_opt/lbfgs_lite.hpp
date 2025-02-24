#ifndef LBFGS_LITE_HPP
#define LBFGS_LITE_HPP

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

namespace lbfgs_lite
{
// ----------------------- Data Type Part -----------------------

/**
 * L-BFGS optimization parameters.
 *  Call lbfgs_load_default_parameters() function to initialize parameters to the
 *  default values.
 */
struct lbfgs_parameter_t
{
    /**
     * The number of corrections to approximate the inverse hessian matrix.
     *  The L-BFGS routine stores the computation results of previous m
     *  iterations to approximate the inverse hessian matrix of the current
     *  iteration. This parameter controls the size of the limited memories
     *  (corrections). The default value is 8. Values less than 3 are
     *  not recommended. Large values will result in excessive computing time.
     */
    int mem_size;

    /**
     * Epsilon for grad norm convergence test.
     *  This parameter determines the accuracy with which the solution is to
     *  be found. A minimization terminates when
     *      ||g|| < g_epsilon * max(1, ||x||),
     *  where ||.|| denotes the Euclidean (L2) norm. The default value is 1e-5.
     */
    double g_epsilon;

    /**
     * Distance for delta-based convergence test.
     *  This parameter determines the distance, in iterations, to compute
     *  the rate of decrease of the objective function. If the value of this
     *  parameter is zero, the library does not perform the delta-based
     *  convergence test. The default value is 0.
     */
    int past;

    /**
     * Delta for convergence test.
     *  This parameter determines the minimum rate of decrease of the
     *  objective function. The library stops iterations when the
     *  following condition is met:
     *      (f' - f) / f < delta,
     *  where f' is the objective value of past iterations ago, and f is
     *  the objective value of the current iteration.
     *  The default value is 1e-5.
     */
    double delta;

    /**
     * The maximum number of iterations.
     *  The lbfgs_optimize() function terminates an optimization process with
     *  ::LBFGSERR_MAXIMUMITERATION status code when the iteration count
     *  exceedes this parameter. Setting this parameter to zero continues an
     *  optimization process until a convergence or error. The default value
     *  is 0.
     */
    int max_iterations;

    /**
     * The maximum number of trials for the line search.
     *  This parameter controls the number of function and gradients evaluations
     *  per iteration for the line search routine. The default value is 40.
     */
    int max_linesearch;

    /**
     * The minimum step of the line search routine.
     *  The default value is 1e-20. This value need not be modified unless
     *  the exponents are too large for the machine being used, or unless the
     *  problem is extremely badly scaled (in which case the exponents should
     *  be increased).
     */
    double min_step;

    /**
     * The maximum step of the line search.
     *  The default value is 1e+20. This value need not be modified unless
     *  the exponents are too large for the machine being used, or unless the
     *  problem is extremely badly scaled (in which case the exponents should
     *  be increased).
     */
    double max_step;

    /**
     * A parameter to control the accuracy of the line search routine.
     *  The default value is 1e-4. This parameter should be greater
     *  than zero and smaller than 0.5.
     */
    double f_dec_coeff;

    /**
     * A parameter to control the accuracy of the line search routine.
     *  The default value is 0.9. If the function and gradient
     *  evaluations are inexpensive with respect to the cost of the
     *  iteration (which is sometimes the case when solving very large
     *  problems) it may be advantageous to set this parameter to a small
     *  value. A typical small value is 0.1. This parameter shuold be
     *  greater than the f_dec_coeff parameter (1e-4) 
     *  and smaller than 1.0.
     */
    double s_curv_coeff;

    /**
     * The machine precision for floating-point values. The default is 1e-16. 
     *  This parameter must be a positive value set by a client program to
     *  estimate the machine precision. The line search routine will terminate
     *  with the status code (::LBFGSERR_ROUNDING_ERROR) if the relative width
     *  of the interval of uncertainty is less than this parameter.
     */
    double xtol;
};

/**
 * Default L-BFGS parameters.
 */
static const lbfgs_parameter_t _default_param = {
    8,
    1e-5,
    0,
    1e-5,
    0,
    40,
    1e-20,
    1e20,
    1e-4,
    0.9,
    1.0e-16,
};

/**
 *  Return values of lbfgs_optimize().
 * 
 * Roughly speaking, a negative value indicates an error.
 */

enum
{
    /** L-BFGS reaches convergence. */
    LBFGS_CONVERGENCE = 0,
    /** L-BFGS satisfies stopping criteria. */
    LBFGS_STOP,
    /** The initial variables already minimize the objective function. */
    LBFGS_ALREADY_MINIMIZED,

    /** Unknown error. */
    LBFGSERR_UNKNOWNERROR = -1024,
    /** Logic error. */
    LBFGSERR_LOGICERROR,
    /** The minimization process has been canceled. */
    LBFGSERR_CANCELED,
    /** Invalid number of variables specified. */
    LBFGSERR_INVALID_N,
    /** Invalid parameter lbfgs_parameter_t::mem_size specified. */
    LBFGSERR_INVALID_MEMSIZE,
    /** Invalid parameter lbfgs_parameter_t::g_epsilon specified. */
    LBFGSERR_INVALID_GEPSILON,
    /** Invalid parameter lbfgs_parameter_t::past specified. */
    LBFGSERR_INVALID_TESTPERIOD,
    /** Invalid parameter lbfgs_parameter_t::delta specified. */
    LBFGSERR_INVALID_DELTA,
    /** Invalid parameter lbfgs_parameter_t::min_step specified. */
    LBFGSERR_INVALID_MINSTEP,
    /** Invalid parameter lbfgs_parameter_t::max_step specified. */
    LBFGSERR_INVALID_MAXSTEP,
    /** Invalid parameter lbfgs_parameter_t::f_dec_coeff specified. */
    LBFGSERR_INVALID_FDECCOEFF,
    /** Invalid parameter lbfgs_parameter_t::wolfe specified. */
    LBFGSERR_INVALID_SCURVCOEFF,
    /** Invalid parameter lbfgs_parameter_t::xtol specified. */
    LBFGSERR_INVALID_XTOL,
    /** Invalid parameter lbfgs_parameter_t::max_linesearch specified. */
    LBFGSERR_INVALID_MAXLINESEARCH,
    /** The line-search step went out of the interval of uncertainty. */
    LBFGSERR_OUTOFINTERVAL,
    /** A logic error occurred; alternatively, the interval of uncertainty became too small. */
    LBFGSERR_INCORRECT_TMINMAX,
    /** A rounding error occurred; alternatively, no line-search step satisfies the sufficient decrease and curvature conditions. */
    LBFGSERR_ROUNDING_ERROR,
    /** The line-search step became smaller than lbfgs_parameter_t::min_step. */
    LBFGSERR_MINIMUMSTEP,
    /** The line-search step became larger than lbfgs_parameter_t::max_step. */
    LBFGSERR_MAXIMUMSTEP,
    /** The line-search routine reaches the maximum number of evaluations. */
    LBFGSERR_MAXIMUMLINESEARCH,
    /** The algorithm routine reaches the maximum number of iterations. */
    LBFGSERR_MAXIMUMITERATION,
    /** Relative width of the interval of uncertainty is at most lbfgs_parameter_t::xtol. */
    LBFGSERR_WIDTHTOOSMALL,
    /** A logic error (negative line-search step) occurred. */
    LBFGSERR_INVALIDPARAMETERS,
    /** The current search direction increases the objective function value. */
    LBFGSERR_INCREASEGRADIENT,
};

/**
 * Callback interface to provide objective function and gradient evaluations.
 *
 *  The lbfgs_optimize() function call this function to obtain the values of objective
 *  function and its gradients when needed. A client program must implement
 *  this function to evaluate the values of the objective function and its
 *  gradients, given current values of variables.
 *  
 *  @param  instance    The user data sent for lbfgs_optimize() function by the client.
 *  @param  x           The current values of variables.
 *  @param  g           The gradient vector. The callback function must compute
 *                      the gradient values for the current variables.
 *  @param  n           The number of variables.
 *  @retval double      The value of the objective function for the current
 *                          variables.
 */
typedef double (*lbfgs_evaluate_t)(void *instance,
                                   const double *x,
                                   double *g,
                                   const int n);

/**
 * Callback interface to provide an upper bound at the beginning of current linear search.
 *
 *  The lbfgs_optimize() function call this function to obtain the values of the
 *  upperbound of the stepsize to search in, provided with the beginning values of
 *  variables before the linear search, and the current step vector (can be descent direction). 
 *  A client program can implement this function for more efficient linesearch. 
 *  If it is not used, just set it NULL or nullptr.
 *  
 *  @param  instance    The user data sent for lbfgs_optimize() function by the client.
 *  @param  xp          The values of variables before current line search.
 *  @param  d           The step vector. It can be the descent direction.
 *  @param  n           The number of variables.
 *  @retval double      The upperboud of the step in current line search routine,
 *                      such that stpbound*d is the maximum reasonable step.
 */
typedef double (*lbfgs_stepbound_t)(void *instance,
                                    const double *xp,
                                    const double *d,
                                    const int n);

/**
 * Callback interface to receive the progress of the optimization process.
 *
 *  The lbfgs_optimize() function call this function for each iteration. Implementing
 *  this function, a client program can store or display the current progress
 *  of the optimization process. If it is not used, just set it NULL or nullptr.
 *
 *  @param  instance    The user data sent for lbfgs_optimize() function by the client.
 *  @param  x           The current values of variables.
 *  @param  g           The current gradient values of variables.
 *  @param  fx          The current value of the objective function.
 *  @param  xnorm       The Euclidean norm of the variables.
 *  @param  gnorm       The Euclidean norm of the gradients.
 *  @param  step        The line-search step used for this iteration.
 *  @param  n           The number of variables.
 *  @param  k           The iteration count.
 *  @param  ls          The number of evaluations called for this iteration.
 *  @retval int         Zero to continue the optimization process. Returning a
 *                      non-zero value will cancel the optimization process.
 */
typedef int (*lbfgs_progress_t)(void *instance,
                                const double *x,
                                const double *g,
                                const double fx,
                                const double xnorm,
                                const double gnorm,
                                const double step,
                                int n,
                                int k,
                                int ls);

/**
 * Callback data struct
 */

struct callback_data_t
{
    int n;
    void *instance;
    lbfgs_evaluate_t proc_evaluate;
    lbfgs_stepbound_t proc_stepbound;
    lbfgs_progress_t proc_progress;
};

/**
 * Iteration data struct
 */
struct iteration_data_t
{
    double alpha;
    double *s; /* [n] */
    double *y; /* [n] */
    double ys; /* vecdot(y, s) */
};

// ----------------------- Arithmetic Part -----------------------

/**
 * Define the local variables for computing minimizers.
 */
#define USES_MINIMIZER_LBFGS \
    double a, d, gamm, theta, p, q, r, s;

/**
 * Find a minimizer of an interpolated cubic function.
 *  @param  cm      The minimizer of the interpolated cubic.
 *  @param  u       The value of one point, u.
 *  @param  fu      The value of f(u).
 *  @param  du      The value of f'(u).
 *  @param  v       The value of another point, v.
 *  @param  fv      The value of f(v).
 *  @param  du      The value of f'(v).
 */
#define CUBIC_MINIMIZER_LBFGS(cm, u, fu, du, v, fv, dv) \
    d = (v) - (u);                                      \
    theta = ((fu) - (fv)) * 3 / d + (du) + (dv);        \
    p = fabs(theta);                                    \
    q = fabs(du);                                       \
    r = fabs(dv);                                       \
    s = p >= q ? p : q;                                 \
    s = s >= r ? s : r;                                 \
    /* gamm = s*sqrt((theta/s)**2 - (du/s) * (dv/s)) */ \
    a = theta / s;                                      \
    gamm = s * sqrt(a * a - ((du) / s) * ((dv) / s));   \
    if ((v) < (u))                                      \
        gamm = -gamm;                                   \
    p = gamm - (du) + theta;                            \
    q = gamm - (du) + gamm + (dv);                      \
    r = p / q;                                          \
    (cm) = (u) + r * d;

/**
 * Find a minimizer of an interpolated cubic function.
 *  @param  cm      The minimizer of the interpolated cubic.
 *  @param  u       The value of one point, u.
 *  @param  fu      The value of f(u).
 *  @param  du      The value of f'(u).
 *  @param  v       The value of another point, v.
 *  @param  fv      The value of f(v).
 *  @param  du      The value of f'(v).
 *  @param  xmin    The maximum value.
 *  @param  xmin    The minimum value.
 */
#define CUBIC_MINIMIZER2_LBFGS(cm, u, fu, du, v, fv, dv, xmin, xmax) \
    d = (v) - (u);                                                   \
    theta = ((fu) - (fv)) * 3 / d + (du) + (dv);                     \
    p = fabs(theta);                                                 \
    q = fabs(du);                                                    \
    r = fabs(dv);                                                    \
    s = p >= q ? p : q;                                              \
    s = s >= r ? s : r;                                              \
    /* gamm = s*sqrt((theta/s)**2 - (du/s) * (dv/s)) */              \
    a = theta / s;                                                   \
    gamm = a * a - ((du) / s) * ((dv) / s);                          \
    gamm = gamm > 0 ? s * sqrt(gamm) : 0;                            \
    if ((u) < (v))                                                   \
        gamm = -gamm;                                                \
    p = gamm - (dv) + theta;                                         \
    q = gamm - (dv) + gamm + (du);                                   \
    r = p / q;                                                       \
    if (r < 0. && gamm != 0.)                                        \
    {                                                                \
        (cm) = (v)-r * d;                                            \
    }                                                                \
    else if (a < 0)                                                  \
    {                                                                \
        (cm) = (xmax);                                               \
    }                                                                \
    else                                                             \
    {                                                                \
        (cm) = (xmin);                                               \
    }

/**
 * Find a minimizer of an interpolated quadratic function.
 *  @param  qm      The minimizer of the interpolated quadratic.
 *  @param  u       The value of one point, u.
 *  @param  fu      The value of f(u).
 *  @param  du      The value of f'(u).
 *  @param  v       The value of another point, v.
 *  @param  fv      The value of f(v).
 */
#define QUARD_MINIMIZER_LBFGS(qm, u, fu, du, v, fv) \
    a = (v) - (u);                                  \
    (qm) = (u) + (du) / (((fu) - (fv)) / a + (du)) / 2 * a;

/**
 * Find a minimizer of an interpolated quadratic function.
 *  @param  qm      The minimizer of the interpolated quadratic.
 *  @param  u       The value of one point, u.
 *  @param  du      The value of f'(u).
 *  @param  v       The value of another point, v.
 *  @param  dv      The value of f'(v).
 */
#define QUARD_MINIMIZER2_LBFGS(qm, u, du, v, dv) \
    a = (u) - (v);                               \
    (qm) = (v) + (dv) / ((dv) - (du)) * a;

inline void *vecalloc(size_t size)
{
    void *memblock = malloc(size);
    if (memblock)
    {
        memset(memblock, 0, size);
    }
    return memblock;
}

inline void vecfree(void *memblock)
{
    free(memblock);
}

inline void veccpy(double *y, const double *x, const int n)
{
    memcpy(y, x, sizeof(double) * n);
}

inline void vecncpy(double *y, const double *x, const int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        y[i] = -x[i];
    }
}

inline void vecadd(double *y, const double *x, const double c, const int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        y[i] += c * x[i];
    }
}

inline void vecdiff(double *z, const double *x, const double *y, const int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        z[i] = x[i] - y[i];
    }
}

inline void vecscale(double *y, const double c, const int n)
{
    int i;

    for (i = 0; i < n; ++i)
    {
        y[i] *= c;
    }
}

inline void vecdot(double *s, const double *x, const double *y, const int n)
{
    int i;
    *s = 0.;
    for (i = 0; i < n; ++i)
    {
        *s += x[i] * y[i];
    }
}

inline void vec2norm(double *s, const double *x, const int n)
{
    vecdot(s, x, x, n);
    *s = (double)sqrt(*s);
}

inline void vec2norminv(double *s, const double *x, const int n)
{
    vec2norm(s, x, n);
    *s = (double)(1.0 / *s);
}

// ----------------------- L-BFGS Part -----------------------

inline int line_search_backtracking(
    int n,
    double *x,
    double *f,
    double *g,
    double *stp,
    const double *s,
    const double *xp,
    const double *gp,
    const double *stpmin,
    const double *stpmax,
    callback_data_t *cd,
    const lbfgs_parameter_t *param)
{
    int count = 0;
    double width;
    double finit, dginit = 0., dgtest;
    const double dec = 0.5;

    /* Check the input parameters for errors. */
    if (*stp <= 0.)
    {
        return LBFGSERR_INVALIDPARAMETERS;
    }

    /* Compute the initial gradient in the search direction. */
    vecdot(&dginit, gp, s, n);

    /* Make sure that s points to a descent direction. */
    if (0 < dginit)
    {
        return LBFGSERR_INCREASEGRADIENT;
    }

    /* The initial value of the objective function. */
    finit = *f;
    dgtest = param->f_dec_coeff * dginit;

    for (;;)
    {
        veccpy(x, xp, n);
        vecadd(x, s, *stp, n);

        /* Evaluate the function and gradient values. */
        *f = cd->proc_evaluate(cd->instance, x, g, cd->n);

        ++count;

        if (*f > finit + *stp * dgtest)
        {
            width = dec;
        }
        else
        {
            return count;
        }

        if (*stp < *stpmin)
        {
            /* The step is the minimum value. */
            return LBFGSERR_MINIMUMSTEP;
        }
        if (*stp > *stpmax)
        {
            /* The step is the maximum value. */
            return LBFGSERR_MAXIMUMSTEP;
        }
        if (param->max_linesearch <= count)
        {
            /* Maximum number of iteration. */
            return LBFGSERR_MAXIMUMLINESEARCH;
        }

        (*stp) *= width;
    }
}

/**
 * Initialize L-BFGS parameters to the default values.
 *
 *  Call this function to fill a parameter structure with the default values
 *  and overwrite parameter values if necessary.
 *
 *  @param  param       The pointer to the parameter structure.
 */
inline void lbfgs_load_default_parameters(lbfgs_parameter_t *param)
{
    memcpy(param, &_default_param, sizeof(*param));
}

/**
 * Start a L-BFGS optimization.
 * A user must implement a function compatible with ::lbfgs_evaluate_t (evaluation
 * callback) and pass the pointer to the callback function to lbfgs_optimize() 
 * arguments. Similarly, a user can implement a function compatible with 
 * ::lbfgs_stepbound_t to provide an external upper bound for stepsize, and 
 * ::lbfgs_progress_t (progress callback) to obtain the current progress 
 * (e.g., variables, function value, ||G||, etc) and to cancel the iteration 
 * process if necessary. Implementation of the stepbound and the progress callback 
 * is optional: a user can pass NULL if progress notification is not necessary.
 * 
 * This algorithm terminates an optimization
 * when:
 *
 *   ||G|| < g_epsilon \cdot \max(1, ||x||) .
 * 
 * In this formula, ||.|| denotes the Euclidean norm.
 *
 *  @param  n           The number of variables.
 *  @param  x           The array of variables. A client program can set
 *                      default values for the optimization and receive the
 *                      optimization result through this array.
 *  @param  ptr_fx      The pointer to the variable that receives the final
 *                      value of the objective function for the variables.
 *                      This argument can be set to NULL if the final
 *                      value of the objective function is unnecessary.
 *  @param  proc_evaluate   The callback function to provide function and
 *                          gradient evaluations given a current values of
 *                          variables. A client program must implement a
 *                          callback function compatible with 
 *                          lbfgs_evaluate_t and pass the pointer to the
 *                          callback function.
 *  @param  proc_stepbound  The callback function to provide values of the
 *                          upperbound of the stepsize to search in, provided
 *                          with the beginning values of variables before the 
 *                          linear search, and the current step vector (can 
 *                          be negative gradient). A client program can implement
 *                          this function for more efficient linesearch. If it is
 *                          not used, just set it NULL or nullptr.
 *  @param  proc_progress   The callback function to receive the progress
 *                          (the number of iterations, the current value of
 *                          the objective function) of the minimization
 *                          process. This argument can be set to NULL if
 *                          a progress report is unnecessary.
 *  @param  instance    A user data for the client program. The callback
 *                      functions will receive the value of this argument.
 *  @param  param       The pointer to a structure representing parameters for
 *                      L-BFGS optimization. A client program can set this
 *                      parameter to NULL to use the default parameters.
 *                      Call lbfgs_load_default_parameters() function to 
 *                      fill a structure with the default values.
 *  @retval int         The status code. This function returns zero if the
 *                      minimization process terminates without an error. A
 *                      non-zero value indicates an error.
 */
inline int lbfgs_optimize(int n,
                          double *x,
                          double *ptr_fx,
                          lbfgs_evaluate_t proc_evaluate,
                          lbfgs_stepbound_t proc_stepbound,
                          lbfgs_progress_t proc_progress,
                          void *instance,
                          lbfgs_parameter_t *_param)
{
    int ret;
    int i, j, k, ls, end, bound;
    double step;
    int loop;
    double step_min, step_max;

    /* Constant parameters and their default values. */
    lbfgs_parameter_t param = (_param != NULL) ? (*_param) : _default_param;
    const int m = param.mem_size;

    double *xp = NULL;
    double *g = NULL, *gp = NULL;
    double *d = NULL, *pf = NULL;
    iteration_data_t *lm = NULL, *it = NULL;
    double ys, yy;
    double xnorm, gnorm, beta;
    double fx = 0.;
    double rate = 0.;

    /* Construct a callback data. */
    callback_data_t cd;
    cd.n = n;
    cd.instance = instance;
    cd.proc_evaluate = proc_evaluate;
    cd.proc_stepbound = proc_stepbound;
    cd.proc_progress = proc_progress;

    /* Check the input parameters for errors. */
    if (n <= 0)
    {
        return LBFGSERR_INVALID_N;
    }
    if (m <= 0)
    {
        return LBFGSERR_INVALID_MEMSIZE;
    }
    if (param.g_epsilon < 0.)
    {
        return LBFGSERR_INVALID_GEPSILON;
    }
    if (param.past < 0)
    {
        return LBFGSERR_INVALID_TESTPERIOD;
    }
    if (param.delta < 0.)
    {
        return LBFGSERR_INVALID_DELTA;
    }
    if (param.min_step < 0.)
    {
        return LBFGSERR_INVALID_MINSTEP;
    }
    if (param.max_step < param.min_step)
    {
        return LBFGSERR_INVALID_MAXSTEP;
    }
    if (param.f_dec_coeff < 0.)
    {
        return LBFGSERR_INVALID_FDECCOEFF;
    }
    if (param.s_curv_coeff <= param.f_dec_coeff || 1. <= param.s_curv_coeff)
    {
        return LBFGSERR_INVALID_SCURVCOEFF;
    }
    if (param.xtol < 0.)
    {
        return LBFGSERR_INVALID_XTOL;
    }
    if (param.max_linesearch <= 0)
    {
        return LBFGSERR_INVALID_MAXLINESEARCH;
    }

    /* Allocate working space. */
    xp = (double *)vecalloc(n * sizeof(double));
    g = (double *)vecalloc(n * sizeof(double));
    gp = (double *)vecalloc(n * sizeof(double));
    d = (double *)vecalloc(n * sizeof(double));

    /* Allocate limited memory storage. */
    lm = (iteration_data_t *)vecalloc(m * sizeof(iteration_data_t));

    /* Initialize the limited memory. */
    for (i = 0; i < m; ++i)
    {
        it = &lm[i];
        it->alpha = 0;
        it->ys = 0;
        it->s = (double *)vecalloc(n * sizeof(double));
        it->y = (double *)vecalloc(n * sizeof(double));
    }

    /* Allocate an array for storing previous values of the objective function. */
    if (0 < param.past)
    {
        pf = (double *)vecalloc(param.past * sizeof(double));
    }

    /* Evaluate the function value and its gradient. */
    fx = cd.proc_evaluate(cd.instance, x, g, cd.n);

    /* Store the initial value of the objective function. */
    if (pf != NULL)
    {
        pf[0] = fx;
    }

    /*
    Compute the direction;
    we assume the initial hessian matrix H_0 as the identity matrix.
    */
    vecncpy(d, g, n);

    /*
    Make sure that the initial variables are not a minimizer.
    */
    vec2norm(&xnorm, x, n);
    vec2norm(&gnorm, g, n);

    if (xnorm < 1.0)
        xnorm = 1.0;
    if (gnorm / xnorm <= param.g_epsilon)
    {
        ret = LBFGS_ALREADY_MINIMIZED;
    }
    else
    {
        /* Compute the initial step:
        step = 1.0 / sqrt(vecdot(d, d, n))
        */
        vec2norminv(&step, d, n);

        k = 1;
        end = 0;
        loop = 1;

        while (loop == 1)
        {
            /* If the step bound can be provied dynamically, then apply it. */
            step_min = param.min_step;
            step_max = param.max_step;
            if (cd.proc_stepbound)
            {
                step_max = cd.proc_stepbound(cd.instance, xp, d, cd.n);
                step_max = step_max < param.max_step ? step_max : param.max_step;
                if (step >= step_max)
                    step = step_max / 2.0;
            }

            /* Store the current position, gradient vectors and initial step. */
            veccpy(xp, x, n);
            veccpy(gp, g, n);

            /* Search for an optimal step. */
            ls = line_search_backtracking(n, x, &fx, g, &step, d, xp, gp, &step_min, &step_max, &cd, &param);

            if (ls < 0)
            {
                /* Revert to the previous point. */
                veccpy(x, xp, n);
                veccpy(g, gp, n);
                ret = ls;
                loop = 0;
                continue;
            }

            /* Compute x and g norms. */
            vec2norm(&xnorm, x, n);
            vec2norm(&gnorm, g, n);

            /* Report the progress. */
            if (cd.proc_progress)
            {
                if ((ret = cd.proc_progress(cd.instance, x, g, fx, xnorm, gnorm, step, cd.n, k, ls)))
                {
                    loop = 0;
                    continue;
                }
            }

            /*
            Convergence test.
            The criterion is given by the following formula:
            |g(x)| / \max(1, |x|) < g_epsilon
            */
            if (xnorm < 1.0)
                xnorm = 1.0;
            if (gnorm / xnorm <= param.g_epsilon)
            {
                /* Convergence. */
                ret = LBFGS_CONVERGENCE;
                break;
            }

            /*
            Test for stopping criterion.
            The criterion is given by the following formula:
            |(f(past_x) - f(x))| / f(x) < \delta
            */
            if (pf != NULL)
            {
                /* We don't test the stopping criterion while k < past. */
                if (param.past <= k)
                {
                    /* Compute the relative improvement from the past. */
                    rate = (pf[k % param.past] - fx) / fx;

                    /* The stopping criterion. */
                    if (fabs(rate) < param.delta)
                    {
                        ret = LBFGS_STOP;
                        break;
                    }
                }

                /* Store the current value of the objective function. */
                pf[k % param.past] = fx;
            }

            if (param.max_iterations != 0 && param.max_iterations < k + 1)
            {
                /* Maximum number of iterations. */
                ret = LBFGSERR_MAXIMUMITERATION;
                break;
            }

            /*
            Update vectors s and y:
            s_{k+1} = x_{k+1} - x_{k} = \step * d_{k}.
            y_{k+1} = g_{k+1} - g_{k}.
            */
            it = &lm[end];
            vecdiff(it->s, x, xp, n);
            vecdiff(it->y, g, gp, n);

            /*
            Compute scalars ys and yy:
            ys = y^t \cdot s = 1 / \rho.
            yy = y^t \cdot y.
            Notice that yy is used for scaling the hessian matrix H_0 (Cholesky factor).
            */
            vecdot(&ys, it->y, it->s, n);
            vecdot(&yy, it->y, it->y, n);
            it->ys = ys;

            /*
            Recursive formula to compute dir = -(H \cdot g).
            This is described in page 779 of:
            Jorge Nocedal.
            Updating Quasi-Newton Matrices with Limited Storage.
            Mathematics of Computation, Vol. 35, No. 151,
            pp. 773--782, 1980.
            */
            bound = (m <= k) ? m : k;
            ++k;
            end = (end + 1) % m;

            /* Compute the negative of gradients. */
            vecncpy(d, g, n);

            j = end;
            for (i = 0; i < bound; ++i)
            {
                j = (j + m - 1) % m; /* if (--j == -1) j = m-1; */
                it = &lm[j];
                /* \alpha_{j} = \rho_{j} s^{t}_{j} \cdot q_{k+1}. */
                vecdot(&it->alpha, it->s, d, n);
                it->alpha /= it->ys;
                /* q_{i} = q_{i+1} - \alpha_{i} y_{i}. */
                vecadd(d, it->y, -it->alpha, n);
            }

            vecscale(d, ys / yy, n);

            for (i = 0; i < bound; ++i)
            {
                it = &lm[j];
                /* \beta_{j} = \rho_{j} y^t_{j} \cdot \gamm_{i}. */
                vecdot(&beta, it->y, d, n);
                beta /= it->ys;
                /* \gamm_{i+1} = \gamm_{i} + (\alpha_{j} - \beta_{j}) s_{j}. */
                vecadd(d, it->s, it->alpha - beta, n);
                j = (j + 1) % m; /* if (++j == m) j = 0; */
            }

            /*
            Now the search direction d is ready. We try step = 1 first.
            */
            step = 1.0;
        }
    }

    /* Return the final value of the objective function. */
    if (ptr_fx != NULL)
    {
        *ptr_fx = fx;
    }

    vecfree(pf);

    /* Free memory blocks used by this function. */
    if (lm != NULL)
    {
        for (i = 0; i < m; ++i)
        {
            vecfree(lm[i].s);
            vecfree(lm[i].y);
        }
        vecfree(lm);
    }
    vecfree(d);
    vecfree(gp);
    vecfree(g);
    vecfree(xp);

    return ret;
}

/**
 * Get string description of an lbfgs_optimize() return code.
 *
 *  @param err          A value returned by lbfgs_optimize().
 */
inline const char *lbfgs_strerror(int err)
{
    switch (err)
    {
    case LBFGS_CONVERGENCE:
        return "Success: reached convergence (g_epsilon).";

    case LBFGS_STOP:
        return "Success: met stopping criteria (past f decrease less than delta).";

    case LBFGS_ALREADY_MINIMIZED:
        return "The initial variables already minimize the objective function.";

    case LBFGSERR_UNKNOWNERROR:
        return "Unknown error.";

    case LBFGSERR_LOGICERROR:
        return "Logic error.";

    case LBFGSERR_CANCELED:
        return "The minimization process has been canceled.";

    case LBFGSERR_INVALID_N:
        return "Invalid number of variables specified.";

    case LBFGSERR_INVALID_MEMSIZE:
        return "Invalid parameter lbfgs_parameter_t::mem_size specified.";

    case LBFGSERR_INVALID_GEPSILON:
        return "Invalid parameter lbfgs_parameter_t::g_epsilon specified.";

    case LBFGSERR_INVALID_TESTPERIOD:
        return "Invalid parameter lbfgs_parameter_t::past specified.";

    case LBFGSERR_INVALID_DELTA:
        return "Invalid parameter lbfgs_parameter_t::delta specified.";

    case LBFGSERR_INVALID_MINSTEP:
        return "Invalid parameter lbfgs_parameter_t::min_step specified.";

    case LBFGSERR_INVALID_MAXSTEP:
        return "Invalid parameter lbfgs_parameter_t::max_step specified.";

    case LBFGSERR_INVALID_FDECCOEFF:
        return "Invalid parameter lbfgs_parameter_t::f_dec_coeff specified.";

    case LBFGSERR_INVALID_SCURVCOEFF:
        return "Invalid parameter lbfgs_parameter_t::s_curv_coeff specified.";

    case LBFGSERR_INVALID_XTOL:
        return "Invalid parameter lbfgs_parameter_t::xtol specified.";

    case LBFGSERR_INVALID_MAXLINESEARCH:
        return "Invalid parameter lbfgs_parameter_t::max_linesearch specified.";

    case LBFGSERR_OUTOFINTERVAL:
        return "The line-search step went out of the interval of uncertainty.";

    case LBFGSERR_INCORRECT_TMINMAX:
        return "A logic error occurred; alternatively, the interval of uncertainty"
               " became too small.";

    case LBFGSERR_ROUNDING_ERROR:
        return "A rounding error occurred; alternatively, no line-search step"
               " satisfies the sufficient decrease and curvature conditions.";

    case LBFGSERR_MINIMUMSTEP:
        return "The line-search step became smaller than lbfgs_parameter_t::min_step.";

    case LBFGSERR_MAXIMUMSTEP:
        return "The line-search step became larger than lbfgs_parameter_t::max_step.";

    case LBFGSERR_MAXIMUMLINESEARCH:
        return "The line-search routine reaches the maximum number of evaluations.";

    case LBFGSERR_MAXIMUMITERATION:
        return "The algorithm routine reaches the maximum number of iterations.";

    case LBFGSERR_WIDTHTOOSMALL:
        return "Relative width of the interval of uncertainty is at most"
               " lbfgs_parameter_t::xtol.";

    case LBFGSERR_INVALIDPARAMETERS:
        return "A logic error (negative line-search step) occurred.";

    case LBFGSERR_INCREASEGRADIENT:
        return "The current search direction increases the objective function value.";

    default:
        return "(unknown)";
    }
}

} // namespace lbfgs_lite

#endif