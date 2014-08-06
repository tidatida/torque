/*
*         OpenPBS (Portable Batch System) v2.3 Software License
*
* Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
* All rights reserved.
*
* ---------------------------------------------------------------------------
* For a license to use or redistribute the OpenPBS software under conditions
* other than those described below, or to purchase support for this software,
* please contact Veridian Systems, PBS Products Department ("Licensor") at:
*
*    www.OpenPBS.org  +1 650 967-4675                  sales@OpenPBS.org
*                        877 902-4PBS (US toll-free)
* ---------------------------------------------------------------------------
*
* This license covers use of the OpenPBS v2.3 software (the "Software") at
* your site or location, and, for certain users, redistribution of the
* Software to other sites and locations.  Use and redistribution of
* OpenPBS v2.3 in source and binary forms, with or without modification,
* are permitted provided that all of the following conditions are met.
* After December 31, 2001, only conditions 3-6 must be met:
*
* 1. Commercial and/or non-commercial use of the Software is permitted
*    provided a current software registration is on file at www.OpenPBS.org.
*    If use of this software contributes to a publication, product, or
*    service, proper attribution must be given; see www.OpenPBS.org/credit.html
*
* 2. Redistribution in any form is only permitted for non-commercial,
*    non-profit purposes.  There can be no charge for the Software or any
*    software incorporating the Software.  Further, there can be no
*    expectation of revenue generated as a consequence of redistributing
*    the Software.
*
* 3. Any Redistribution of source code must retain the above copyright notice
*    and the acknowledgment contained in paragraph 6, this list of conditions
*    and the disclaimer contained in paragraph 7.
*
* 4. Any Redistribution in binary form must reproduce the above copyright
*    notice and the acknowledgment contained in paragraph 6, this list of
*    conditions and the disclaimer contained in paragraph 7 in the
*    documentation and/or other materials provided with the distribution.
*
* 5. Redistributions in any form must be accompanied by information on how to
*    obtain complete source code for the OpenPBS software and any
*    modifications and/or additions to the OpenPBS software.  The source code
*    must either be included in the distribution or be available for no more
*    than the cost of distribution plus a nominal fee, and all modifications
*    and additions to the Software must be freely redistributable by any party
*    (including Licensor) without restriction.
*
* 6. All advertising materials mentioning features or use of the Software must
*    display the following acknowledgment:
*
*     "This product includes software developed by NASA Ames Research Center,
*     Lawrence Livermore National Laboratory, and Veridian Information
*     Solutions, Inc.
*     Visit www.OpenPBS.org for OpenPBS software support,
*     products, and information."
*
* 7. DISCLAIMER OF WARRANTY
*
* THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND. ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
* ARE EXPRESSLY DISCLAIMED.
*
* IN NO EVENT SHALL VERIDIAN CORPORATION, ITS AFFILIATED COMPANIES, OR THE
* U.S. GOVERNMENT OR ANY OF ITS AGENCIES BE LIABLE FOR ANY DIRECT OR INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* This license will be governed by the laws of the Commonwealth of Virginia,
* without reference to its choice of law rules.
*/
/*
 * Functions which provide basic operation on the job structure
 *
 * Included public functions are:
 *
 *   job_alloc    allocate job struct and initialize defaults
 *   mom_job_free   free space allocated to the job structure and its
 *    childern structures.
 *   mom_job_purge   purge job from server
 *
 *   job_unlink_file() unlinks a given file using job credentials
 *
 * Include private function:
 *   job_init_wattr() initialize job working attribute array to "unspecified"
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

#ifndef SIGKILL
#include <signal.h>
#endif
#if __STDC__ != 1
#include <memory.h>
#endif

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "pbs_ifl.h"
#include "list_link.h"
#include "work_task.h"
#include "attribute.h"
#include "resource.h"
#include "server_limits.h"
#include "server.h"
#include "queue.h"
#include "pbs_job.h"
#include "log.h"
#include "../lib/Liblog/pbs_log.h"
#include "../lib/Liblog/log_event.h"
#include "pbs_error.h"
#include "svrfunc.h"
#include "acct.h"
#include "net_connect.h"
#include "portability.h"
#include "threadpool.h"
#include "alps_functions.h"
#include "alps_constants.h"
#include "dis.h"
#ifdef PENABLE_LINUX26_CPUSETS
#include "pbs_cpuset.h"
#endif
#include "utils.h"
#include "mom_job_cleanup.h"

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

int conn_qsub(char *, long, char *);

void mom_job_purge(job *);

/* External functions */
extern void mom_checkpoint_delete_files(job_file_delete_info *);

#if IBM_SP2==2  /* IBM SP PSSP 3.1 */
void unload_sp_switch(job *pjob);
#endif   /* IBM SP */

#ifdef PENABLE_LINUX26_CPUSETS
extern int use_cpusets(job *);
#endif /* PENABLE_LINUX26_CPUSETS */
/* Local Private Functions */

static void job_init_wattr(job *);

/* Global Data items */

extern char   *apbasil_path;
extern char   *apbasil_protocol;
extern char    tmpdir_basename[];	/* for TMPDIR */
extern gid_t   pbsgroup;
extern uid_t   pbsuser;
extern char   *msg_abt_err;
extern char   *path_jobs;
extern char   *path_spool;
extern char   *path_aux;
extern char   *msg_err_purgejob;
extern char    server_name[];
extern time_t  time_now;
extern int     LOGLEVEL;
extern int     is_login_node;

extern tlist_head svr_newjobs;
extern tlist_head svr_alljobs;

void nodes_free(job *);
extern int thread_unlink_calls;

extern void MOMCheckRestart(void);
void       send_update_soon();


extern int multi_mom;
extern int pbs_rm_port;

void tasks_free(

  job *pj)

  {
  task            *tp = (task *)GET_NEXT(pj->ji_tasks);
  obitent         *op;
  infoent         *ip;
  resizable_array *freed_chans = initialize_resizable_array(30);

  while (tp != NULL)
    {
    op = (obitent *)GET_NEXT(tp->ti_obits);

    while (op != NULL)
      {
      delete_link(&op->oe_next);

      free(op);

      op = (obitent *)GET_NEXT(tp->ti_obits);
      }  /* END while (op != NULL) */

    ip = (infoent *)GET_NEXT(tp->ti_info);

    while (ip != NULL)
      {
      delete_link(&ip->ie_next);

      free(ip->ie_name);
      free(ip->ie_info);
      free(ip);

      ip = (infoent *)GET_NEXT(tp->ti_info);
      }

    if (tp->ti_chan != NULL)
      {
      if (is_present(freed_chans, tp->ti_chan) == FALSE)
        {
        insert_thing(freed_chans, tp->ti_chan);
        close_conn(tp->ti_chan->sock, FALSE);
        DIS_tcp_cleanup(tp->ti_chan);
        }
        
      tp->ti_chan = NULL;
      }

    delete_link(&tp->ti_jobtask);

    free(tp);

    tp = (task *)GET_NEXT(pj->ji_tasks);
    }  /* END while (tp != NULL) */

  free_resizable_array(freed_chans);

  return;
  }  /* END tasks_free() */





/*
 * remtree - remove a tree (or single file)
 *
 * returns  0 on success
 *  -1 on failure
 */

int remtree(

  char *dirname)

  {
  DIR           *dir;

  struct dirent *pdir;
  char           namebuf[MAXPATHLEN];
  int            len;
  int            rtnv = 0;
#if defined(HAVE_STRUCT_STAT64) && defined(HAVE_STAT64) && defined(LARGEFILE_WORKS)

  struct stat64  sb;
#else

  struct stat    sb;
#endif

#if defined(HAVE_STRUCT_STAT64) && defined(HAVE_STAT64) && defined(LARGEFILE_WORKS)

  if (lstat64(dirname, &sb) == -1)
#else
  if (lstat(dirname, &sb) == -1)
#endif
    {

    if (errno != ENOENT)
      log_err(errno, __func__, (char *)"stat");

    return(-1);
    }

  if (S_ISDIR(sb.st_mode))
    {
    if ((dir = opendir(dirname)) == NULL)
      {
      if (errno != ENOENT)
        log_err(errno, __func__, (char *)"opendir");

      return(-1);
      }

    snprintf(namebuf, sizeof(namebuf), "%s/", dirname);

    len = strlen(namebuf);

    while ((pdir = readdir(dir)) != NULL)
      {
      if ((pdir->d_name[0] == '.') &&
          ((pdir->d_name[1] == '\0') || (pdir->d_name[1] == '.')))
        continue;

      snprintf(namebuf + len, sizeof(namebuf) - len, "%s", pdir->d_name);

#if defined(HAVE_STRUCT_STAT64) && defined(HAVE_STAT64) && defined(LARGEFILE_WORKS)
      if (lstat64(namebuf, &sb) == -1)
#else
      if (lstat(namebuf, &sb) == -1)
#endif
        {
        log_err(errno, __func__, (char *)"stat");

        rtnv = -1;

        continue;
        }

      if (S_ISDIR(sb.st_mode))
        {
        rtnv = remtree(namebuf);
        }
      else if (unlink(namebuf) < 0)
        {
        if (errno != ENOENT)
          {
          sprintf(log_buffer, "unlink failed on %s", namebuf);
          log_err(errno, __func__, log_buffer);
          
          rtnv = -1;
          }
        }
      else if (LOGLEVEL >= 7)
        {
        sprintf(log_buffer, "unlink(1) succeeded on %s", namebuf);

        log_ext(-1, __func__, log_buffer, LOG_DEBUG);
        }
      }    /* END while ((pdir = readdir(dir)) != NULL) */

    closedir(dir);

    if (rmdir(dirname) < 0)
      {
      if ((errno != ENOENT) && (errno != EINVAL))
        {
        sprintf(log_buffer, "rmdir failed on %s",
                dirname);

        log_err(errno, __func__, log_buffer);

        rtnv = -1;
        }
      }
    else if (LOGLEVEL >= 7)
      {
      sprintf(log_buffer, "rmdir succeeded on %s", dirname);

      log_ext(-1, __func__, log_buffer, LOG_DEBUG);
      }
    }
  else if (unlink(dirname) < 0)
    {
    snprintf(log_buffer,sizeof(log_buffer),"unlink failed on %s",dirname);
    log_err(errno,__func__,log_buffer);

    rtnv = -1;
    }
  else if (LOGLEVEL >= 7)
    {
    sprintf(log_buffer, "unlink(2) succeeded on %s", dirname);

    log_ext(-1, __func__, log_buffer, LOG_DEBUG);
    }

  return(rtnv);
  }  /* END remtree() */



/*
 * conn_qsub - connect to the qsub that submitted this interactive job
 * return >= 0 on SUCCESS, < 0 on FAILURE
 * (this was moved from resmom/mom_inter.c)
 */



int conn_qsub(

  char *hostname,  /* I */
  long  port,      /* I */
  char *EMsg)      /* O (optional,minsize=1024) */

  {
  pbs_net_t hostaddr;
  
  int       s;
  int       local_errno = 0;
  int flags;


  if (EMsg != NULL)
    EMsg[0] = '\0';

  if ((hostaddr = get_hostaddr(&local_errno, hostname)) == (pbs_net_t)0)
    {
#if !defined(H_ERRNO_DECLARED) && !defined(_AIX)
    extern int h_errno;
#endif

    /* FAILURE */

    if (EMsg != NULL)
      {
      snprintf(EMsg, 1024, "cannot get address for host '%s', h_errno=%d",
               hostname,
               h_errno);
      }

    return(-1);
    }

  s = client_to_svr(hostaddr, (unsigned int)port, 0, EMsg);

  /* NOTE:  client_to_svr() can return 0 for SUCCESS */

  /* assume SUCCESS requires s > 0 (USC) was 'if (s >= 0)' */
  /* above comment not enabled */

  if (s < 0)
    {
    /* FAILURE */

    return(-1);
    }

  /* SUCCESS */

  /* this socket should be blocking */

  flags = fcntl(s, F_GETFL);

  flags &= ~O_NONBLOCK;

  fcntl(s, F_SETFL, flags);

  return(s);
  }  /* END conn_qsub() */




/*
 * job_alloc - allocate space for a job structure and initialize working
 * attribute to "unset"
 *
 * Returns: pointer to structure or null is space not available.
 */

job *job_alloc(void)

  {
  job *pj;

  pj = (job *)calloc(1, sizeof(job));

  if (pj == NULL)
    {
    log_err(errno, "job_alloc", (char *)"no memory");

    return(NULL);
    }

  pj->ji_qs.qs_version = PBS_QS_VERSION;

  CLEAR_LINK(pj->ji_alljobs);
  CLEAR_LINK(pj->ji_jobque);

  CLEAR_HEAD(pj->ji_tasks);
  pj->ji_taskid = TM_NULL_TASK + 1;
  pj->ji_obit = TM_NULL_EVENT;
  pj->ji_nodekill = TM_ERROR_NODE;
  pj->ji_stats_done = false;

  pj->ji_momhandle = -1;  /* mark mom connection invalid */

  /* set the working attributes to "unspecified" */
  job_init_wattr(pj);

  return(pj);
  }  /* END job_alloc() */





/*
 * mom_job_free - free job structure and its various sub-structures
 */

void mom_job_free(

  job *pj)  /* I (modified) */

  {
  int    i;

  if (LOGLEVEL >= 8)
    {
    sprintf(log_buffer, "freeing job");

    log_record(PBSEVENT_DEBUG,
               PBS_EVENTCLASS_JOB,
               pj->ji_qs.ji_jobid,
               log_buffer);
    }

  /* remove any calloc working attribute space */

  for (i = 0;i < JOB_ATR_LAST;i++)
    {
    job_attr_def[i].at_free(&pj->ji_wattr[i]);
    }

  if (pj->ji_grpcache)
    free(pj->ji_grpcache);

  assert(pj->ji_preq == NULL);

  nodes_free(pj);

  tasks_free(pj);

  if (pj->ji_resources)
    {
    free(pj->ji_resources);
    pj->ji_resources = NULL;
    }

  if (pj->ji_sister_vnods)
    {
    free(pj->ji_sister_vnods);
    pj->ji_sister_vnods = NULL;
    }

  /* now free the main structure */

  free((char *)pj);

  return;
  }  /* END mom_job_free() */


 /*
 * job_unlink_file - unlink file, but drop root credentials before
 * doing this to avoid removing objects that aren't belong to the user.
 */
int job_unlink_file(

  job        *pjob,  /* I */
  const char *name)	 /* I */

  {
  int   saved_errno = 0;
  int   result = 0;
  uid_t uid = geteuid();
  gid_t gid = getegid();

  if (uid != 0)
    return unlink(name);

  if ((setegid(pjob->ji_qs.ji_un.ji_momt.ji_exgid) == -1))
    return -1;
  if ((setuid_ext(pjob->ji_qs.ji_un.ji_momt.ji_exuid, TRUE) == -1))
    {
    saved_errno = errno;
    setegid(gid);
    errno = saved_errno;
    return -1;
    }
  result = unlink(name);
  saved_errno = errno;

  setuid_ext(uid, TRUE);
  setegid(gid);

  errno = saved_errno;
  return(result);
  }  /* END job_unlink_file() */




/*
 * job_init_wattr - initialize job working attribute array
 * set the types and the "unspecified value" flag
 */

static void job_init_wattr(

  job *pj)

  {
  int i;

  for (i = 0; i < JOB_ATR_LAST; i++)
    {
    clear_attr(&pj->ji_wattr[i], &job_attr_def[i]);
    }

  return;
  }   /* END job_init_wattr() */





void *delete_job_files(

  void *vp)

  {
  job_file_delete_info *jfdi = (job_file_delete_info *)vp;
  char                  namebuf[MAXPATHLEN];
  int                   rc = 0;

  if (jfdi->has_temp_dir == TRUE)
    {
    if (tmpdir_basename[0] == '/')
      {
      snprintf(namebuf, sizeof(namebuf), "%s/%s", tmpdir_basename, jfdi->jobid);
      sprintf(log_buffer, "removing transient job directory %s",
        namebuf);

      log_record(PBSEVENT_DEBUG,PBS_EVENTCLASS_JOB,jfdi->jobid,log_buffer);

      if ((setegid(jfdi->gid) == -1) ||
          (setuid_ext(jfdi->uid, TRUE) == -1))
        {
        /* FAILURE */
        rc = -1;;
        }
      else
        {
        rc = remtree(namebuf);
        
        setuid_ext(pbsuser, TRUE);
        setegid(pbsgroup);
        }
      
      if ((rc != 0) && 
          (LOGLEVEL >= 5))
        {
        sprintf(log_buffer,
          "recursive remove of job transient tmpdir %s failed",
          namebuf);
        
        log_err(errno, "recursive (r)rmdir", log_buffer);
        }
      }
    } /* END code to remove temp dir */

#ifdef PENABLE_LINUX26_CPUSETS
  /* Delete the cpuset for the job. */
  delete_cpuset(jfdi->jobid, true);
#endif /* PENABLE_LINUX26_CPUSETS */

  /* delete the node file and gpu file */
  sprintf(namebuf,"%s/%s", path_aux, jfdi->jobid);
  unlink(namebuf);
  
  sprintf(namebuf, "%s/%sgpu", path_aux, jfdi->jobid);
  unlink(namebuf);
  
  sprintf(namebuf, "%s/%smic", path_aux, jfdi->jobid);
  unlink(namebuf);

  /* delete script file */
  if (multi_mom)
    {
    snprintf(namebuf, sizeof(namebuf), "%s%s%d%s",
      path_jobs,
      jfdi->prefix,
      pbs_rm_port,
      JOB_SCRIPT_SUFFIX);
    }
  else
    {
    snprintf(namebuf, sizeof(namebuf), "%s%s%s",
      path_jobs,
      jfdi->prefix,
      JOB_SCRIPT_SUFFIX);
    }

  if (unlink(namebuf) < 0)
    {
    if (errno != ENOENT)
      log_err(errno,__func__,msg_err_purgejob);
    }
  else
    {
    snprintf(log_buffer,sizeof(log_buffer),"removed job script");
    log_record(PBSEVENT_DEBUG,PBS_EVENTCLASS_JOB,jfdi->jobid,log_buffer);
    }

  /* delete job task directory */
  if (multi_mom)
    {
    snprintf(namebuf,sizeof(namebuf),"%s%s%d%s",
      path_jobs,
      jfdi->prefix,
      pbs_rm_port,
      JOB_TASKDIR_SUFFIX);
    }
  else
    {
    snprintf(namebuf,sizeof(namebuf),"%s%s%s",
      path_jobs,
      jfdi->prefix,
      JOB_TASKDIR_SUFFIX);
    }

  remtree(namebuf);

  mom_checkpoint_delete_files(jfdi);

  /* delete job file */
  if (multi_mom)
    {
    snprintf(namebuf,sizeof(namebuf),"%s%s%d%s",
      path_jobs,
      jfdi->prefix,
      pbs_rm_port,
      JOB_FILE_SUFFIX);
    }
  else
    {
    snprintf(namebuf,sizeof(namebuf),"%s%s%s",
      path_jobs,
      jfdi->prefix,
      JOB_FILE_SUFFIX);
    }

  if (unlink(namebuf) < 0)
    {
    if (errno != ENOENT)
      log_err(errno,__func__,msg_err_purgejob);
    }
  else if (LOGLEVEL >= 6)
    {
    snprintf(log_buffer,sizeof(log_buffer),"remove job file");
    log_record(PBSEVENT_DEBUG,PBS_EVENTCLASS_JOB,jfdi->jobid,log_buffer);
    }

  free(jfdi);

  return(NULL);
  } /* END delete_job_files() */





int release_job_reservation(

  job *pjob)

  {
  int   rc = PBSE_NONE;
  char *rsv_id;

  /* release this job's reservation */
  if ((pjob->ji_wattr[JOB_ATR_reservation_id].at_flags & ATR_VFLAG_SET) &&
      (pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str != NULL))
    {
    rsv_id = pjob->ji_wattr[JOB_ATR_reservation_id].at_val.at_str;

    if ((rc = destroy_alps_reservation(rsv_id, apbasil_path, apbasil_protocol, APBASIL_RETRIES)) != PBSE_NONE)
      {
      snprintf(log_buffer, sizeof(log_buffer), "Couldn't release reservation for job %s",
        pjob->ji_qs.ji_jobid);
      log_err(-1, __func__, log_buffer);
      }
    }

  return(rc);
  } /* END release_job_reservation() */


/* 
 * remove the job from the exiting_job_list if its there
 *
 * @param pjob - the job to remove from the exiting_job_list
 *
 */
void remove_from_exiting_list(

  job *pjob)

  {
  int               iter = -1;
  int               prev_index = -1;
  exiting_job_info *eji;

  while ((eji = (exiting_job_info *)next_thing(exiting_job_list, &iter)) != NULL)
    {
    if (!strcmp(eji->jobid, pjob->ji_qs.ji_jobid))
      {
      if (prev_index == -1)
        pop_thing(exiting_job_list);
      else
        remove_thing_from_index(exiting_job_list, prev_index);
        
      free(eji);
      break;
      }

    prev_index = iter;
    }

  } /* END remove_from_exiting_list() */



void mom_job_purge(

  job *pjob)  /* I (modified) */

  {
  job_file_delete_info *jfdi;

  jfdi = (job_file_delete_info *)calloc(1, sizeof(job_file_delete_info));

  if (jfdi == NULL)
    {
    log_err(ENOMEM,__func__, (char *)"No space to allocate info for job file deletion");
    return;
    }

#ifdef NVIDIA_GPUS
  /*
   * Did this job have a gpuid assigned?
   * if so, then update gpu status
   */
  if (((pjob->ji_wattr[JOB_ATR_exec_gpus].at_flags & ATR_VFLAG_SET) != 0) &&
      (pjob->ji_wattr[JOB_ATR_exec_gpus].at_val.at_str != NULL))
    {
    send_update_soon();
    }
#endif  /* NVIDIA_GPUS */

  /* initialize struct information */
  if (pjob->ji_flags & MOM_HAS_TMPDIR)
    {
    jfdi->has_temp_dir = TRUE;
    pjob->ji_flags &= ~MOM_HAS_TMPDIR;
    }
  else
    jfdi->has_temp_dir = FALSE;

  strcpy(jfdi->jobid,pjob->ji_qs.ji_jobid);
  strcpy(jfdi->prefix,pjob->ji_qs.ji_fileprefix);

  if ((pjob->ji_wattr[JOB_ATR_checkpoint_dir].at_flags & ATR_VFLAG_SET) &&
      (pjob->ji_wattr[JOB_ATR_checkpoint_name].at_flags & ATR_VFLAG_SET))
    jfdi->checkpoint_dir = strdup(pjob->ji_wattr[JOB_ATR_checkpoint_dir].at_val.at_str);

  jfdi->gid = pjob->ji_qs.ji_un.ji_momt.ji_exgid;
  jfdi->uid = pjob->ji_qs.ji_un.ji_momt.ji_exuid;

  if (thread_unlink_calls == TRUE)
    enqueue_threadpool_request(delete_job_files,jfdi);
  else
    delete_job_files(jfdi);

  /* remove this job from the global queue */
  delete_link(&pjob->ji_jobque);
  delete_link(&pjob->ji_alljobs);

  remove_from_exiting_list(pjob);

  if (LOGLEVEL >= 6)
    {
    sprintf(log_buffer,"removing job");

    log_record(PBSEVENT_DEBUG, PBS_EVENTCLASS_JOB, pjob->ji_qs.ji_jobid, log_buffer);
    }

#if IBM_SP2==2        /* IBM SP PSSP 3.1 */
  unload_sp_switch(pjob);

#endif   /* IBM SP */

  mom_job_free(pjob);

  /* if no jobs are left, check if MOM should be restarted */

  if (((job *)GET_NEXT(svr_alljobs)) == NULL)
    MOMCheckRestart();

  return;
  }  /* END mom_job_purge() */





/*
 * mom_find_job() - find job by jobid
 *
 * Search list of all server jobs for one with same jobid
 * Return NULL if not found or pointer to job struct if found
 */

job *mom_find_job(

  char *jobid)

  {
  char *at;
  job  *pj;

  if ((at = strchr(jobid, (int)'@')) != NULL)
    * at = '\0'; /* strip off @server_name */

  pj = (job *)GET_NEXT(svr_alljobs);

  while (pj != NULL)
    {
    if (!strcmp(jobid, pj->ji_qs.ji_jobid))
      break;

    pj = (job *)GET_NEXT(pj->ji_alljobs);
    }

  if (at)
    *at = '@'; /* restore @server_name */

  return(pj);  /* may be NULL */
  }   /* END mom_find_job() */



/*
 * am_i_mother_superior()
 *
 * @return true if I am this job's mother superior, else false
 */
bool am_i_mother_superior(

  const job &pjob)

  {
  bool mother_superior = ((pjob.ji_nodeid == 0) && ((pjob.ji_qs.ji_svrflags & JOB_SVFLG_HERE) != 0));
    
  return(mother_superior);
  }

/* END job_func.c */

