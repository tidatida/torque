#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "attribute.h" /* attribute_def, pbs_attribute, svrattrl */
#include "pbs_job.h" /* all_jobs, job */
#include "resource.h" /* resource_def */
#include "pbs_ifl.h" /* PBS_MAXSERVERNAME */
#include "server.h" /* server */
#include "log.h" /* LOG_BUF_SIZE */
#include "queue.h" /* pbs_queue */
#include "batch_request.h" /* batch_request */
#include "work_task.h" /* work_task, work_type */
#include "dynamic_string.h"
#include "user_info.h"
#include "mutex_mgr.hpp"
#include "threadpool.h"
#include "id_map.hpp"

bool exit_called = false;
const char *PJobSubState[10];
char *path_jobs;
const char *msg_script_write = "Unable to write script file";
int svr_resc_size = 0;
attribute_def job_attr_def[10];
const char *msg_script_open = "Unable to open script file";
const char *msg_jobnew = "Job Queued at request of %s@%s, owner = %s, job name = %s, queue = %s";
all_jobs newjobs;
char server_name[PBS_MAXSERVERNAME + 1];
const char *pbs_o_host = "PBS_O_HOST";
resource_def *svr_resc_def;
int queue_rank = 0;
char *path_spool;
struct server server;
const char *msg_daemonname = "unset";
int LOGLEVEL = 7; /* force logging code to be exercised as tests run */
user_info_holder users;
threadpool_t *task_pool;
char str_to_set[1024];
long long_to_set = -1;
bool default_queue = false;
bool mem_fail = false;


int setup_array_struct(job *pjob)
  {
  fprintf(stderr, "The call to setup_array_struct to be mocked!!\n");
  exit(1);
  }

pbs_queue *find_queuebyname(const char *quename)
  {
  if (!strcmp(quename, "batch"))
    return((pbs_queue *)calloc(1, sizeof(pbs_queue)));

  return(NULL);
  }

int job_save(job *pjob, int updatetype, int mom_port)
  {
  fprintf(stderr, "The call to job_save to be mocked!!\n");
  exit(1);
  }

int svr_job_purge(job *pjob, int leaveSpoolFiles)
  {
  fprintf(stderr, "The call to job_purge to be mocked!!\n");
  exit(1);
  }

void set_chkpt_deflt(job *pjob, pbs_queue *pque)
  {
  fprintf(stderr, "The call to set_chkpt_deflt to be mocked!!\n");
  exit(1);
  }

void clear_attr(pbs_attribute *pattr, attribute_def *pdef)
  {
  fprintf(stderr, "The call to clear_attr to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_jobs_queue(job **pjob)
  {
  fprintf(stderr, "The call to get_jobs_queue to be mocked!!\n");
  exit(1);
  }

void reply_ack(struct batch_request *preq)
  {
  }

int svr_authorize_jobreq(struct batch_request *preq, job *pjob)
  {
  return 0;
  }

char *get_variable(job *pjob, const char *variable)
  {
  fprintf(stderr, "The call to get_variable to be mocked!!\n");
  exit(1);
  }

void job_clone_wt(void *cloned_id)
  {
  fprintf(stderr, "The call to job_clone_wt to be mocked!!\n");
  exit(1);
  }

struct work_task *set_task(enum work_type type, long event_id, void (*func)(), void *parm, int get_lock)
  {
  fprintf(stderr, "The call to set_task to be mocked!!\n");
  exit(1);
  }

char * csv_find_value(const char *csv_str, const char *search_str)
  {
  fprintf(stderr, "The call to csv_find_value to be mocked!!\n");
  exit(1);
  }

ssize_t write_nonblocking_socket(int fd, const void *buf, ssize_t count)
  {
  fprintf(stderr, "The call to write_nonblocking_socket to be mocked!!\n");
  exit(1);
  }

int insert_job(all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to insert_job to be mocked!!\n");
  exit(1);
  }

int find_attr(struct attribute_def *attr_def, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_attr to be mocked!!\n");
  exit(1);
  }

int decode_resc(struct pbs_attribute *patr, const char *name, const char *rescn, const char *val, int perm)
  {
  fprintf(stderr, "The call to decode_resc to be mocked!!\n");
  exit(1);
  }

void req_reject(int code, int aux, struct batch_request *preq, const char *HostName, const char *Msg)
  {
  }

job *next_job(all_jobs *aj, all_jobs_iterator *iter)
  {
  return(iter->get_next_item());
  }

void *get_next(list_link pl, char *file, int line)
  {
  fprintf(stderr, "The call to get_next to be mocked!!\n");
  exit(1);
  }

int job_route(job *jobp)
  {
  fprintf(stderr, "The call to job_route to be mocked!!\n");
  exit(1);
  }

int svr_enquejob(job *pjob, int has_sv_qs_mutex, char *prev_id, bool reservation)
  {
  fprintf(stderr, "The call to svr_enquejob to be mocked!!\n");
  exit(1);
  }

resource_def *find_resc_def(resource_def *rscdf, const char *name, int limit)
  {
  fprintf(stderr, "The call to find_resc_def to be mocked!!\n");
  exit(1);
  }

int svr_chkque(job *pjob, pbs_queue *pque, char *hostname, int mtype, char *EMsg)
  {
  fprintf(stderr, "The call to svr_chkque to be mocked!!\n");
  exit(1);
  }

int get_fullhostname(char *shortname, char *namebuf, int bufsize, char *EMsg)
  {
  strcpy(namebuf, shortname);
  return(PBSE_NONE);
  }

int remove_job(all_jobs *aj, job *pjob)
  {
  fprintf(stderr, "The call to remove_job to be mocked!!\n");
  exit(1);
  }

int reply_jobid(struct batch_request *preq, char *jobid, int which)
  {
  fprintf(stderr, "The call to reply_jobid to be mocked!!\n");
  exit(1);
  }

void issue_track(job *pjob)
  {
  fprintf(stderr, "The call to issue_track to be mocked!!\n");
  exit(1);
  }

void close_conn(int sd, int has_mutex)
  {
  fprintf(stderr, "The call to close_conn to be mocked!!\n");
  exit(1);
  }

int svr_setjobstate(job *pjob, int newstate, int newsubstate, int  has_queue_mute)
  {
  fprintf(stderr, "The call to svr_setjobstate to be mocked!!\n");
  exit(1);
  }

resource *find_resc_entry(pbs_attribute *pattr, resource_def *rscdf)
  {
  fprintf(stderr, "The call to find_resc_entry to be mocked!!\n");
  exit(1);
  }

job *svr_find_job(const char *jobid, int get_subjob)
  {
  return(NULL);
  }

int svr_save(struct server *ps, int mode)
  {
  return(0);
  }

void replace_attr_string(struct pbs_attribute *attr, char *newval)
  {
  fprintf(stderr, "The call to replace_attr_string to be mocked!!\n");
  exit(1);
  }

job *job_alloc(void)
  {
  if (mem_fail == true)
    return(NULL);
  else
    return((job *)calloc(1, sizeof(job)));
  }

int unlock_queue(struct pbs_queue *the_queue, const char *method_name, const char *msg, int logging)
  {
  return(0);
  }

void svr_evaljobstate(job &pjob, int &newstate, int &newsub, int forceeval)
  {
  fprintf(stderr, "The call to svr_evaljobstate to be mocked!!\n");
  exit(1);
  }

pbs_queue *get_dfltque(void)
  {
  static pbs_queue pque;

  if (default_queue == true)
    return(&pque);

  return(NULL);
  }

void reply_badattr(int code, int aux, svrattrl *pal, struct batch_request *preq)
  {
  fprintf(stderr, "The call to reply_badattr to be mocked!!\n");
  exit(1);
  }

char *pbs_default(void)
  {
  return(server_name);
  }

pbs_net_t get_connectaddr(int sock, int mutex)
  {
  fprintf(stderr, "The call to get_connectaddr to be mocked!!\n");
  exit(1);
  }

int get_svr_attr_l(int index, long *l)
  {
  if (long_to_set != -1)
    *l = long_to_set;

  return(0);
  }

int get_svr_attr_str(int index, char **str)
  {
  if (str_to_set[0] != '\0')
    *str = str_to_set;

  return(0);
  }

dynamic_string *get_dynamic_string(int initial_size, const char *str)
  {
  return(NULL);
  }

int enqueue_threadpool_request(

  void *(*func)(void *),
  void *arg,
  threadpool_t *tp)

  {
  return(0);
  }

int unlock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

int lock_sv_qs_mutex(pthread_mutex_t *sv_qs_mutex, const char *msg_string)
  {
  return(0);
  }

struct pbsnode *find_nodebyname(

  const char *nodename) /* I */

  {
  return(NULL);
  }

void unlock_node(struct pbsnode *pnode, const char *func_id, const char *msg, int logging) {} 

int lock_queue(

  struct pbs_queue *the_queue,
  const char       *id,
  const char       *msg,
  int               logging)

  {
  return(0);
  }

int lock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int unlock_ji_mutex(job *pjob, const char *id, const char *msg, int logging)
  {
  return(0);
  }

int  can_queue_new_job(char *user_name, job *pjob)
  {
  return(0);
  }

int  increment_queued_jobs(user_info_holder *uih, char *user_name, job *pjob)
  {
  return(0);
  }

int  decrement_queued_jobs(user_info_holder *uih, char *user_name)
  {
  return(0);
  }

ssize_t write_ac_socket(int fd, const void *buf, ssize_t count)
  {
  return(0);
  }

void log_err(int errnum, const char *routine, const char *text) {}
void log_record(int eventtype, int objclass, const char *objname, const char *text) {}
void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

mutex_mgr::mutex_mgr(pthread_mutex_t *mutex, bool locked) {}

int mutex_mgr::unlock() 
  {
  return(0);
  }

mutex_mgr::~mutex_mgr() {}

int safe_strncat(char *str, const char *to_append, size_t space_remaining)
  {
  return(0);
  }

const char *prefix_std_file(
  job            *pjob,
  std::string&    ds,
  int             key)
  {
  return "prefix";
  }

const char *add_std_filename(
  job            *pjob,
  char           *path,
  int             key,
  std::string&    ds)
  {
  return "stdfilename";
  }

job *find_job_by_array(all_jobs *aj, const char *jobid, int get_subjob, bool locked)
{
  if (!strcmp(jobid, "1.napali"))
    {
    job *pj = (job *)calloc(1, sizeof(job));
    strcpy(pj->ji_qs.ji_fileprefix, "1.napali");
    return(pj);
    }

  return(NULL);
  }

id_map::id_map() {}

id_map::~id_map() {}

int id_map::get_new_id(const char *job_name)
  {
  static int i = 0;
  return(i++);
  }

id_map job_mapper;

void log_ext(int i, char const* s, char const* s2, int i2)
  {
  }

