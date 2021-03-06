#ifndef _LIBSVM_H
#define _LIBSVM_H

#define LIBSVM_VERSION 300


#ifdef __cplusplus
extern "C" {
#endif

extern int libsvm_version;

struct svm_node
{
  int index;
  double value;
};

#ifdef _STRING
struct svm_data
{
  void destroy();
  void clone(svm_data *newdata);
  struct svm_node *v;
  char *s;
  unsigned *t; // For tokenized string. if t[0] == n, then t[1]...t[n] are token indicies
  int libsvm_allocated;
};
struct svm_problem
{
  int l;
  double *y;
  struct svm_data *x;
};

#else
struct svm_problem
{
  int l;
  double *y;
  struct svm_node **x;
};
#endif

enum { C_SVC, NU_SVC, ONE_CLASS, EPSILON_SVR, NU_SVR }; /* svm_type */
#ifdef _STRING
enum { VECTOR, STRING, TOKENS }; /* data_type */
enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED, EDIT, SUBSEQ }; /* kernel_type */
#else
enum { LINEAR, POLY, RBF, SIGMOID, PRECOMPUTED }; /* kernel_type */
#endif

struct svm_parameter
{
  int svm_type;
#ifdef _STRING
  int data_type;
#endif
  int kernel_type;
  int degree; /* for poly */
  double gamma; /* for poly/rbf/sigmoid */
  double coef0; /* for poly/sigmoid */

  /* these are for training only */
  double cache_size; /* in MB */
  double eps; /* stopping criteria */
  double C; /* for C_SVC, EPSILON_SVR and NU_SVR */
  int nr_weight;    /* for C_SVC */
  int *weight_label;  /* for C_SVC */
  double* weight;   /* for C_SVC */
  double nu;  /* for NU_SVC, ONE_CLASS, and NU_SVR */
  double p; /* for EPSILON_SVR */
  int shrinking;  /* use the shrinking heuristics */
  int probability; /* do probability estimates */
};

//
// svm_model
// 
struct svm_model
{
  struct svm_parameter param; /* parameter */
  int nr_class;   /* number of classes, = 2 in regression/one class svm */
  int l;      /* total #SV */
#ifdef _STRING
  struct svm_data *SV;    /* SVs (SV[l]) */
#else
  struct svm_node **SV;   /* SVs (SV[l]) */
#endif
  double **sv_coef; /* coefficients for SVs in decision functions (sv_coef[k-1][l]) */
  double *rho;    /* constants in decision functions (rho[k*(k-1)/2]) */
  double *probA;    /* pariwise probability information */
  double *probB;

  /* for classification only */

  int *label;   /* label of each class (label[k]) */
  int *nSV;   /* number of SVs for each class (nSV[k]) */
        /* nSV[0] + nSV[1] + ... + nSV[k-1] = l */
  /* XXX */
  int free_sv;    /* 1 if svm_model is created by svm_load_model*/
        /* 0 if svm_model is created by svm_train */
};

struct svm_model *svm_train(const struct svm_problem *prob, const struct svm_parameter *param);
void svm_cross_validation(const struct svm_problem *prob, const struct svm_parameter *param, int nr_fold, double *target);

int svm_save_model(const char *model_file_name, const struct svm_model *model);
struct svm_model *svm_load_model(const char *model_file_name);

int svm_get_svm_type(const struct svm_model *model);
#ifdef _STRING
int svm_get_data_type(const struct svm_model *model);
#endif
int svm_get_nr_class(const struct svm_model *model);
void svm_get_labels(const struct svm_model *model, int *label);
double svm_get_svr_probability(const struct svm_model *model);

#ifdef _STRING
double svm_predict_values(const struct svm_model *model, const struct svm_data x, double* dec_values);
double svm_predict(const struct svm_model *model, const struct svm_data x);
double svm_predict_p(const struct svm_model *model, const struct svm_data *x);
double svm_predict_probability(const struct svm_model *model, const struct svm_data x, double* prob_estimates);

#else
double svm_predict_values(const struct svm_model *model, const struct svm_node *x, double* dec_values);
double svm_predict(const struct svm_model *model, const struct svm_node *x);
double svm_predict_probability(const struct svm_model *model, const struct svm_node *x, double* prob_estimates);
#endif

void svm_free_model_content(struct svm_model *model_ptr);
void svm_free_and_destroy_model(struct svm_model **model_ptr_ptr);
void svm_destroy_param(struct svm_parameter *param);

const char *svm_check_parameter(const struct svm_problem *prob, const struct svm_parameter *param);
int svm_check_probability_model(const struct svm_model *model);

void svm_set_print_string_function(void (*print_func)(const char *));

// deprecated
// this function will be removed in future release
void svm_destroy_model(struct svm_model *model_ptr); 

#ifdef __cplusplus
}
#endif

#endif /* _LIBSVM_H */
